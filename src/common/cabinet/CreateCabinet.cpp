/**
 *
 */
#include "common/cabinet/CreateCabinet.h"
#include "common/cabinet/JavaRandom.h"
#include "common/log/log.h"
#include "common/util/typeconversions.h"
#include "zlib.h"
#include <algorithm>
#include <iostream>


namespace {

static const uint32_t kFilePrefix = 0xA9414BB1;
static const uint8_t kEncrypted = 1;
static const uint8_t kCompressed = 2;

bool CompareEntries(const CCreateCabinet::TCreatedEntry& a1, const CCreateCabinet::TCreatedEntry& a2)
{
   return a1.iFilename <= a2.iFilename;
}

inline void AppendUnsigned(TFileData& aData, uint64_t aValue, TSize aBytes)
{
   for (int i = 0; i < aBytes; i++) {
      aData.emplace_back(static_cast<char>(aValue >> 8*(aBytes-i-1)));
   }
}

inline void AppendString(TFileData& aData, const std::string& aString)
{
   TSize size = aString.length();
   AppendUnsigned(aData, size, 2);
   std::copy(aString.begin(), aString.end(), std::back_inserter(aData));
}

}

CCreateCabinet::CCreateCabinet()
 : iCreatedFromCabinet(false), iEncryptionKey(0), iPatchCycle(0), iCabinetVersion(3)
 , iEncryption(CCabinet::ENCRYPT_NONE), iCompression(CCabinet::COMPRESS_NONE)
{
   LOG_METHOD();
}

CCreateCabinet::CCreateCabinet(const TCabinetPtr& aSourceCabinet)
 : iCreatedFromCabinet(true), iSourceCabinet(aSourceCabinet), iEncryptionKey(iSourceCabinet->iDecryptionKey)
 , iPatchCycle(iSourceCabinet->iPatchCycle), iCabinetVersion(iSourceCabinet->iCabinetVersion)
 , iEncryption(iSourceCabinet->iEncryption), iCompression(iSourceCabinet->iCompression)
{
   TSize size = iSourceCabinet->iEntries.size();
   iEntries.reserve(size);
   LOG_DEBUG("Adding %" PRIuS " cabinet files", size);
   for (TSize i = 0; i < size; i++) {
      const CCabinetEntry& entry = iSourceCabinet->iEntries[i];
      iEntries.emplace_back(entry.iFilename, entry.iEncrypted, entry.iCompressed, iSourceCabinet, i);
   }
}

std::vector<CCreateCabinet::TCreatedEntry>::iterator CCreateCabinet::FindEntryByName(const std::string& aFilename)
{
   LOG_METHOD();
   std::vector<TCreatedEntry>::iterator it = iEntries.begin();
   const std::vector<TCreatedEntry>::iterator itEnd = iEntries.end();
   while (it != itEnd) {
      if (aFilename == it->iFilename) {
         return it;
      }
      it++;
   }
   return itEnd;
}

void CCreateCabinet::Clear()
{
   LOG_METHOD();
   iEntries.clear();
}

bool CCreateCabinet::RemoveFile(TSize aIndex)
{
   LOG_METHOD();
   if (aIndex < iEntries.size()) {
      iEntries.erase(iEntries.begin() + aIndex);
      return true;
   }
   return false;
}

bool CCreateCabinet::RemoveFile(const std::string& aFilename)
{
   LOG_METHOD();
   std::vector<TCreatedEntry>::iterator it = FindEntryByName(aFilename);
   if (it != iEntries.end()) {
      iEntries.erase(it);
      return true;
   }
   return false;
}

bool CCreateCabinet::AddFile(const std::string& aFilename, const TFilePtr& aSourceFile, bool aEncrypted, bool aCompressed)
{
   if (FindEntryByName(aFilename) == iEntries.end()) {
      LOG_DEBUG("Adding file %s", aFilename.c_str());
      iEntries.emplace_back(aFilename, aEncrypted, aCompressed, aSourceFile);
      return true;
   }
   LOG_ERROR("Duplicate filename %s", aFilename.c_str());
   return false;
}

bool CCreateCabinet::AddFile(const std::string& aFilename, const TCabinetPtr& aSourceCabinet, TSize aSourceIndex)
{
   if (FindEntryByName(aFilename) == iEntries.end()) {
      LOG_DEBUG("Adding cabinet file %s", aFilename.c_str());
      const CCabinetEntry& entry = aSourceCabinet->iEntries[aSourceIndex];
      iEntries.emplace_back(aFilename, entry.iEncrypted, entry.iCompressed, aSourceCabinet, aSourceIndex);
      return true;
   }
   LOG_ERROR("Duplicate filename %s", aFilename.c_str());
   return false;
}

bool CCreateCabinet::AddFile(const std::string& aFilename, const TCabinetPtr& aSourceCabinet, TSize aSourceIndex, bool aEncrypted, bool aCompressed)
{
   if (FindEntryByName(aFilename) == iEntries.end()) {
      LOG_DEBUG("Adding cabinet file %s", aFilename.c_str());
      iEntries.emplace_back(aFilename, aEncrypted, aCompressed, aSourceCabinet, aSourceIndex);
      return true;
   }
   LOG_ERROR("Duplicate filename %s", aFilename.c_str());
   return false;
}

bool CCreateCabinet::RenameFile(const std::string& aOriginalFilename, const std::string& aNewFilename)
{
   auto it = FindEntryByName(aOriginalFilename);
   if (it != iEntries.end()) {
      LOG_DEBUG("Renaming file %s to %s", aOriginalFilename.c_str(), aNewFilename.c_str());
      it->iFilename = aNewFilename;
      return true;
   }
   LOG_ERROR("File not found in cabinet: %s", aOriginalFilename.c_str());
   return false;
}

bool CCreateCabinet::SetFileProperties(const std::string& aFilename, bool aEncrypt, bool aCompress)
{
   auto it = FindEntryByName(aFilename);
   if (it != iEntries.end()) {
      LOG_DEBUG("Setting properties for file %s", aFilename.c_str());
      it->iEncrypted = aEncrypt;
      it->iCompressed = aCompress;
      return true;
   }
   LOG_ERROR("File not found in cabinet: %s", aFilename.c_str());
   return false;
}

bool CCreateCabinet::WriteToDisk()
{
   LOG_METHOD();
   if (!iFile) {
      LOG_ERROR("Output file needs to be set first");
      return false;
   }
   if (iCabinetVersion != 3) {
      LOG_ERROR("Unsupported cabinet version %u", iCabinetVersion);
      return false;
   }
   std::sort(iEntries.begin(), iEntries.end(), CompareEntries);
   TFileData header;
   iFile->Open();
   iFile->Clear();

   // Write the header to memory and the files to disk
   LOG_DEBUG("Writing content part to disk");
   TSize currentPosition = 0;
   uint64_t i = 0;
   for (TCreatedEntry& entry : iEntries) {
      TSize curSize = entry.iFileSize;
      bool bEnc = (iEncryption != CCabinet::ENCRYPT_NONE) && entry.iEncrypted;
      bool bComp = (iCompression != CCabinet::COMPRESS_NONE) && entry.iCompressed;

      TFileData content;
      if (entry.iInCabinet) {
         TMemoryFilePtr memFile = entry.iSourceCabinet->ReadFileByIndex(entry.iSourceIndex);
         if (memFile) {
            content = std::move(memFile->GetBuffer());
            curSize = content.size();
         } else {
            LOG_ERROR("Could not read file %s from cabinet", entry.iFilename.c_str());
            return false;
         }
      } else if (!entry.iSourceFile->Read(content, 0, curSize)) {
         LOG_ERROR("Could not read from file %s", entry.iFilename.c_str());
         return false;
      }

      if (bComp) {
         // Determine if it is beneficial to compress the file
         if (DeflateData(content)) {
            curSize = content.size();
         } else {
            bComp = false;
         }
      }
      if (bEnc) {
         EncryptData(content, 2*i+1);
      }
      if (!iFile->Append(content)) {
         LOG_ERROR("Could not write to output file");
         return false;
      }

      uint64_t flags = (bEnc ? kEncrypted : 0) | (bComp ? kCompressed : 0);
      AppendString(header, entry.iFilename);
      AppendUnsigned(header, currentPosition, 6);
      AppendUnsigned(header, curSize, 5);
      AppendUnsigned(header, flags, 1);
      currentPosition += curSize;
      i++;
   }

   // Then put the header before the files
   LOG_DEBUG("Writing cabinet header section");
   if (iCompression != CCabinet::COMPRESS_NONE) {
      DeflateData(header, true); // Force deflation as we have no flag for that in the file
   }
   if (iEncryption != CCabinet::ENCRYPT_NONE) {
      EncryptData(header, 99);
   }
   iFile->Insert(header, 0);

   // And last, the overall cabinet header to the front
   TFileData firstHeader;
   AppendUnsigned(firstHeader, kFilePrefix, 4);
   AppendUnsigned(firstHeader, iPatchCycle, 2);
   AppendUnsigned(firstHeader, iCabinetVersion, 2);
   AppendUnsigned(firstHeader, header.size(), 4);
   AppendUnsigned(firstHeader, static_cast<uint64_t>(iEncryption), 1);
   AppendUnsigned(firstHeader, 24 + header.size() + currentPosition, 6);
   AppendUnsigned(firstHeader, static_cast<uint64_t>(iCompression), 1);
   AppendUnsigned(firstHeader, iEntries.size(), 4);
   iFile->Insert(firstHeader, 0);
   iFile->Close();

   LOG_DEBUG("Written cabinet to disk. File size: %" PRIuS ", written %" PRIuS, iFile->Size(), 24 + header.size() + currentPosition);
   return true;
}

bool CCreateCabinet::EncryptData(TFileData& aData, uint64_t aKeyOffset)
{
   LOG_METHOD();
   TSize length = aData.size();
   CJavaRandom jr{iEncryptionKey + aKeyOffset};
   for (TSize i = 0; i < length; i++) {
      aData[i] = (char)(((uint32_t)aData[i] + jr.nextInt(256)) % 256);
   }
   return true;
}

bool CCreateCabinet::DeflateData(TFileData& aData, bool aForceDeflation)
{
   LOG_METHOD();
   TSize size = aData.size();
   if (size < 100 && !aForceDeflation) { // Do not compress tiny files
      LOG_DEBUG("Not deflating %" PRIuS " bytes", size);
      return false;
   }

   TFileData deflateBuffer;
   AppendUnsigned(deflateBuffer, size, 4);
   deflateBuffer.resize(size);
   z_stream strm;
   strm.zalloc = Z_NULL;
   strm.zfree  = Z_NULL;
   strm.opaque = Z_NULL;
   strm.next_in = (unsigned char *)(&aData[0]);
   strm.avail_in = static_cast<uint32_t>(size);
   strm.avail_out = static_cast<uint32_t>(size-4);
   strm.next_out = (unsigned char *)(&deflateBuffer[4]);
   int result = deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY); // zlib encoding without headers
   if (result != Z_OK) {
      LOG_ERROR("Error in deflateInit2(): %d", result);
      return false;
   }
   result = deflate(&strm, Z_FINISH);
   deflateEnd(&strm);
   if (result < Z_OK) {
      LOG_ERROR("Error in deflate(): %d", result);
      return false;
   }

   TSize small = strm.total_out;
   if (aForceDeflation || ((small+100 < size) && (static_cast<double>(small)/static_cast<double>(size) <= 0.9))) {
      LOG_DEBUG("Deflated data from %" PRIuS " to %" PRIuS, size, small);
      deflateBuffer.resize(small+4);
      aData.swap(deflateBuffer);
      return true;
   }
   LOG_DEBUG("Not deflating for too little gain: from %" PRIuS " to %" PRIuS, size, small);
   return false;
}

void CCreateCabinet::PrintFileIndex() const
{
   LOG_METHOD();
   for (const TCreatedEntry& e: iEntries) {
      char mod = ' ';
      if (e.iCompressed)
         if (e.iEncrypted) mod = '@';
         else mod = '*';
      else if (e.iEncrypted) mod = '~';
      std::cout << FormatSize(e.iFileSize) << mod << " " << e.iFilename << std::endl;
   }
   std::cout << "========================" << std::endl;
   std::cout << iEntries.size() << " files" << (IsCompressed()? ", compressed" : "") << (IsEncrypted()? ", encrypted" : "") << ", V" << iCabinetVersion << "." << std::endl;
}

