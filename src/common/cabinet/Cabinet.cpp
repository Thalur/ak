/**
 *
 */
#include "common/cabinet/Cabinet.h"
#include "common/cabinet/JavaRandom.h"
#include "common/log/log.h"
#include "common/util/typeconversions.h"
#include <algorithm>
#include <iostream>
#include "zlib.h"


CCabinetEntry::CCabinetEntry(const std::string& aFilename, uint64_t aOffset, uint64_t aSize, bool aCompressed, bool aEncrypted)
 :	iFilename(aFilename), iFileOffset(aOffset), iFileSize(aSize), iCompressed(aCompressed), iEncrypted(aEncrypted)
{
   // Get file prefix and extension
   TSize pos = iFilename.rfind('.');
   if (pos == std::string::npos) {
      iPrefix = iFilename;
      iExtension = "";
   } else {
      iPrefix = iFilename.substr(0,pos);
      iExtension = iFilename.substr(pos+1);
   }
}


namespace {

static const uint32_t kFilePrefix = 0xA9414BB1;
static const uint64_t kFirstHeaderSize = 24;
static const uint8_t kEncrypted = 1;
static const uint8_t kCompressed = 2;

template <typename T>
inline T GetUnsigned(const TFileData& aData, TSize aPosition, TSize aSize)
{
   T result = 0;
   for (TSize i = 0; i < aSize; i++) {
      result += (static_cast<uint8_t>(aData[aPosition+i]) << (8*(aSize-i-1)));
   }
   return result;
}

std::string GetString(const TFileData& aData, TSize aPosition, TSize aMaxSize, TSize& aResultPosition)
{
   if (2 <= aMaxSize) {
      uint16_t size = GetUnsigned<uint16_t>(aData, aPosition, 2);
      if (size+2 <= aMaxSize) {
         aResultPosition = aPosition + 2 + size;
         if (size > 0) {
            return std::string(&aData[aPosition+2], size);
         }
      } else LOG_ERROR("Invalid String data (%u)", size);
   } else LOG_ERROR("Invalid data size");
   return std::string();
}

bool CompareEntries(const CCabinetEntry& a1, const CCabinetEntry& a2)
{
   return a1.Filename() <= a2.Filename();
}

typedef std::pair<std::string, bool> TSearchParam; // filename, bComplete (false: prefix)

bool CompareEntryForSearch(const CCabinetEntry& e1, const TSearchParam& a2)
{
   if (a2.second) {
      return e1.Filename() < a2.first;
   }
   return e1.Prefix() < a2.first;
}

}

CCabinet::CCabinet(TFilePtr& aFile, int32_t aKey)
 : iFile(aFile), iDecryptionKey(aKey), iCabinetVersion(0), iFileSize(aFile->Size())
{
   LOG_METHOD();
}

TCabinetPtr CCabinet::Open(TFilePtr& aFile, int32_t aKey)
{
   LOG_METHOD();
   TCabinetPtr cab = std::make_shared<CCabinet>(CCabinet(aFile, aKey));
   aFile->Open();
   if (cab->ReadHeader()) {
      cab->PrintFileIndex();
      return cab;
   }
   aFile->Close();
   return TCabinetPtr();
}

bool CCabinet::ReadHeader()
{
   LOG_METHOD();
   TFileData data;
   if (iFile->Read(data, 0, 8)) {
      uint32_t prefix = GetUnsigned<uint32_t>(data, 0, 4);
      if (prefix == kFilePrefix) {
         iPatchCycle = GetUnsigned<uint16_t>(data, 4, 2);
         iCabinetVersion = GetUnsigned<uint16_t>(data, 6, 2);
         if (iCabinetVersion == 3) {
            return ReadHeaderV3(); // current version
         } else if (iCabinetVersion == 2) {
            return ReadHeaderV2(); // DuneDroid version
         } else LOG_ERROR("Invalid header version %u", iCabinetVersion);
      } else LOG_ERROR("Invalid cabinet prefix");
   } else LOG_ERROR("Could not read from cabinet file");
   return false;
}

bool CCabinet::ReadHeaderV2()
{
   LOG_METHOD();
   if (iPatchCycle != 0) {
      LOG_WARN("Patch cycle is not supported in V2 and is required to be 0 for backwards compatibility");
   }
   TFileData data;
   if (iFile->Read(data, 8, 12)) {
      uint32_t encryptHeaderSize = GetUnsigned<uint32_t>(data, 0, 4);
      iEncryption = (encryptHeaderSize != 0 ? ENCRYPT_AKV2 : ENCRYPT_NONE);
      uint32_t fileSize = GetUnsigned<uint32_t>(data, 4, 4);
      if (fileSize != iFileSize) {
         LOG_WARN("File size mismatch, expected %u, actual %" PRIu64, fileSize, iFileSize);
      }
      uint32_t numFiles = GetUnsigned<uint32_t>(data, 8, 4);
      if (numFiles >= 16777216) {
         numFiles -= 16777216;
         iCompression = COMPRESS_ZIP;
      } else iCompression = COMPRESS_NONE;

      iDataOffset = 0;
      if (numFiles == 0) {
         return true; // empty file
      }

      // We don't know how long the file headers are, so just read the whole file
      if (!IsEncrypted()) {
         encryptHeaderSize = fileSize-20;
      }
      // Disable compression for reading the header as it's not compressed in V2
      TCompression oldCompression = iCompression;
      iCompression = COMPRESS_NONE;
      if (ReadFilePart(data, 20, encryptHeaderSize, 99, IsEncrypted(), false))
      {
         iCompression = oldCompression;
         TSize pos = 0;
         for (uint32_t i = 0; i < numFiles; i++) {
            std::string fName = GetString(data, pos, encryptHeaderSize-pos, pos);
            if (fName.empty() || (pos+9 > encryptHeaderSize)) {
               LOG_ERROR("Error reading file header data");
               return false;
            }
            uint32_t offset = GetUnsigned<uint32_t>(data, pos, 4);
            uint32_t size = GetUnsigned<uint32_t>(data, pos+4, 4);
            uint8_t flags = static_cast<uint8_t>(data[pos+8]);
            pos += 9;
            iEntries.push_back(std::move(CCabinetEntry(fName, offset, size,
               (flags&kCompressed) > 0 && IsCompressed(), (flags&kEncrypted) > 0 &&IsEncrypted())));
         }
         if (IsEncrypted() && (pos != encryptHeaderSize)) {
            LOG_ERROR("Invalid file header data");
            return false;
         }
         std::sort(iEntries.begin(), iEntries.end(), CompareEntries);
         return true;
      } else LOG_ERROR("Error reading the file headers");
   } else LOG_ERROR("Error reading the cabinet header");
   return false;
}

bool CCabinet::ReadHeaderV3()
{
   LOG_METHOD();
   TFileData data;
   if (iFile->Read(data, 8, 16)) {
      uint32_t headerSize = GetUnsigned<uint32_t>(data, 0, 4);
      iEncryption = static_cast<TEncryption>(data[4]);
      uint64_t fileSize = GetUnsigned<uint64_t>(data, 5, 6);
      iCompression = static_cast<TCompression>(data[11]);
      uint32_t numFiles = GetUnsigned<uint32_t>(data, 12, 4);
      if ((headerSize+kFirstHeaderSize <= fileSize) && (fileSize == iFileSize) &&
          (iEncryption <= ENCRYPT_AKV2) && (iCompression <= COMPRESS_ZIP))
      {
         iDataOffset = kFirstHeaderSize + headerSize;
         if (numFiles == 0) {
            return true; // empty file
         }
         if (ReadFilePart(data, kFirstHeaderSize, headerSize, 99, IsEncrypted(), IsCompressed()))
         {
            TSize dataSize = data.size();
            TSize pos = 0;
            std::string lastName;
            bool bSorted = true;
            for (uint32_t i = 0; i < numFiles; i++) {
               std::string fName = GetString(data, pos, dataSize-pos, pos);
               if (fName.empty() || (pos+12 > dataSize)) {
                  LOG_ERROR("Error reading file header data");
                  return false;
               }
               if (!lastName.empty() && (lastName >= fName)) {
                  bSorted = false;
               }
               uint64_t offset = GetUnsigned<uint64_t>(data, pos, 6);
               uint64_t size = GetUnsigned<uint64_t>(data, pos+6, 5);
               uint8_t flags = static_cast<uint8_t>(data[pos+11]);
               pos += 12;
               iEntries.push_back(std::move(CCabinetEntry(fName, offset, size,
                  (flags&kCompressed) > 0 && IsCompressed(), (flags&kEncrypted) > 0 &&IsEncrypted())));
               lastName = std::move(fName);
            }
            if (pos != dataSize) {
               LOG_ERROR("Invalid file header data");
               return false;
            }
            if (!bSorted) {
               LOG_WARN("File header is not sorted");
               std::sort(iEntries.begin(), iEntries.end(), CompareEntries);
            }
            return true;
         } else LOG_ERROR("Error reading the file headers");
      } else LOG_ERROR("Invalid cabinet properties");
   } else LOG_ERROR("Error reading the cabinet header");
   return false;
}

bool CCabinet::ReadFilePart(TFileData& aResult, uint64_t aOffset, uint64_t aSize, uint64_t aKeyOffset,
                            bool aEncrypted, bool aCompressed)
{
   bool bOK = iFile->Read(aResult, aOffset, aSize);
   bOK = bOK && (!aEncrypted || !IsEncrypted() || DecryptData(aResult, aKeyOffset));
   bOK = bOK && (!aCompressed || !IsCompressed() || InflateData(aResult));
   return bOK;
}

bool CCabinet::DecryptData(TFileData& aData, uint64_t aKeyOffset)
{
   TSize length = aData.size();
   CJavaRandom jr{iDecryptionKey + aKeyOffset};
   for (TSize i = 0; i < length; i++) {
      aData[i] = (char)(((uint32_t)aData[i] + 512 - jr.nextInt(256)) % 256);
   }
   return true;
}

bool CCabinet::InflateData(TFileData& aData)
{
   TSize size = aData.size();
   if (size < 4) return false;
   uint32_t originalSize = GetUnsigned<uint32_t>(aData, 0, 4);
   LOG_DEBUG("Inflating entry from size %" PRIuS " to %u", size, originalSize);

   TFileData inflateBuffer;
   inflateBuffer.resize(originalSize);
   z_stream strm;
   strm.zalloc = Z_NULL;
   strm.zfree  = Z_NULL;
   strm.opaque = Z_NULL;
   strm.next_in = (unsigned char *)(&aData[4]);
   strm.avail_in = static_cast<uint32_t>(size-4);
   strm.avail_out = originalSize;
   strm.next_out = (unsigned char *)(&inflateBuffer[0]);
   int result = inflateInit2(&strm, -15); // zlib encoding without headers
   if (result != Z_OK) {
      LOG_ERROR("Error in inflateInit2(): %d", result);
      return false;
   }
   result = inflate(&strm, Z_FINISH);
   inflateEnd(&strm);
   if (result < Z_OK) {
      LOG_ERROR("Error in inflate(): %d", result);
      return false;
   }
   if (strm.total_out != originalSize) {
      LOG_WARN("Inflated size %u does not match expected size %u", strm.total_out, originalSize);
   }
   aData.swap(inflateBuffer);
   return true;
}

void CCabinet::PrintFileIndex() const
{
   LOG_METHOD();
   for (const CCabinetEntry& e: iEntries) {
      char mod = ' ';
      if (e.iCompressed)
         if (e.iEncrypted) mod = '@';
         else mod = '*';
      else if (e.iEncrypted) mod = '~';
      std::cout << FormatSize(e.iFileSize) << mod << " " << e.iFilename << std::endl;
   }
   std::cout << "========================" << std::endl;
   std::cout << FormatSize(iFileSize) << ", " << iEntries.size() << " files" << (IsCompressed()? ", compressed" : "") << (IsEncrypted()? ", encrypted" : "") << ", V" << iCabinetVersion << "." << std::endl;
}

TMemoryFilePtr CCabinet::ReadFileByName(const std::string& aFilename)
{
   LOG_PARAMS("aFilename=%s", aFilename.c_str());
   return ReadFile(aFilename, true);
}

TMemoryFilePtr CCabinet::ReadFileByPrefix(const std::string& aPrefix)
{
   LOG_PARAMS("aPrefix=%s", aPrefix.c_str());
   return ReadFile(aPrefix, false);
}

bool CCabinet::GetFileIndex(TSize& outIndex, const std::string& aFilename) const
{
   LOG_PARAMS("aFilename=%s", aFilename.c_str());
   auto it = std::lower_bound(iEntries.begin(), iEntries.end(), TSearchParam(aFilename, true), CompareEntryForSearch);
   if (it != iEntries.end()) {
      outIndex = it - iEntries.begin();
      return true;
   }
   return false;
}

TMemoryFilePtr CCabinet::ReadFile(const std::string& aName, bool aFullName)
{
   LOG_PARAMS("aName=%s, aFullName=%d", aName.c_str(), aFullName);
   std::vector<CCabinetEntry>::iterator it = std::lower_bound(iEntries.begin(), iEntries.end(),
      TSearchParam(aName, aFullName), CompareEntryForSearch);
   if (it != iEntries.end()) {
      if ((aFullName && (aName == it->iFilename)) || (!aFullName && (aName == it->iPrefix))) {
         return ReadFileByIndex(it - iEntries.begin());
      }
   }
   LOG_WARN("File not found in cabinet: %s", aName.c_str());
   return TMemoryFilePtr();
}

TMemoryFilePtr CCabinet::ReadFileByIndex(TSize aIndex)
{
   LOG_METHOD();
   if (aIndex < iEntries.size()) {
      const CCabinetEntry& entry = iEntries[aIndex];
      TSize size = entry.iFileSize;
      //LOG_DEBUG("Entry: %s %u %u", entry.iFilename.c_str(), entry.iFileOffset, entry.iFileSize);
      if (size < (TSize(1) << 31)) {
         TFileData data;
         if (ReadFilePart(data, iDataOffset+entry.iFileOffset, size, 2*aIndex+1, entry.iEncrypted, entry.iCompressed))
         {
            return std::make_shared<CMemoryFile>(std::move(data));
         } else LOG_ERROR("Could not read file %" PRIuS " from cabinet", aIndex);
      } else LOG_ERROR("File %" PRIuS " is too large to store in memory", aIndex);
   } else LOG_ERROR("Invalid file index %" PRIuS, aIndex);
   return TMemoryFilePtr();
}

std::vector<TEntryIndex> CCabinet::GetFileList() const
{
   LOG_METHOD();
   const TSize size = iEntries.size();
   std::vector<TEntryIndex> result;
   result.reserve(size);
   for (TSize i = 0; i < size; i++) {
      result.emplace_back(iEntries[i].iFilename, i);
   }
   return result;
}

std::vector<TEntryIndex> CCabinet::GetFilePrefixList() const
{
   LOG_METHOD();
   const TSize size = iEntries.size();
   std::vector<TEntryIndex> result;
   result.reserve(size);
   for (TSize i = 0; i < size; i++) {
      result.emplace_back(iEntries[i].iPrefix, i);
   }
   return result;
}
