/**
 *
 */
#include "PosixFile.h"
#include "common/log/log.h"


namespace {

inline bool FileExists(const std::string& aFilename)
{
   LOG_METHOD();
   std::ifstream f(aFilename.c_str());
   return f.good();
}

}

TPosixFilePtr CPosixFile::MakeFile(const std::string& aFilename, std::ios_base::openmode aMode, bool aAllowWrite)
{
   LOG_METHOD();
   std::unique_ptr<std::fstream> stream = make_unique<std::fstream>(aFilename.c_str(), aMode);
   if (stream->good()) {
      return TPosixFilePtr(new CPosixFile(aFilename, aMode&(~std::ios_base::trunc), stream, aAllowWrite));
   }
   return TPosixFilePtr();
}

TPosixFilePtr CPosixFile::OpenExistingFile(const std::string& aFilename, bool aAllowWrite)
{
   LOG_METHOD();
   if (FileExists(aFilename)) {
      std::ios_base::openmode mode = std::ios_base::in | std::ios_base::binary;
      if (aAllowWrite) {
         mode |= std::ios_base::out;
      }
      return MakeFile(aFilename, mode, aAllowWrite);
   }
   return TPosixFilePtr();
}

TPosixFilePtr CPosixFile::CreateEmptyFile(const std::string& aFilename, bool aOverwrite)
{
   LOG_METHOD();
   if (aOverwrite || !FileExists(aFilename)) {
      std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc;
      return MakeFile(aFilename, mode, true);
   }
   return TPosixFilePtr();
}

TPosixFilePtr CPosixFile::OpenOrCreate(const std::string& aFilename)
{
   LOG_METHOD();
   std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
   if (!FileExists(aFilename)) {
      mode |= std::ios_base::trunc;
   }
   return MakeFile(aFilename, mode, true);
}

CPosixFile::CPosixFile(CPosixFile&& aSrc)
 : iFilename(std::move(aSrc.iFilename)), iMode(aSrc.iMode), iStream(std::move(aSrc.iStream))
 , iSize(aSrc.iSize), iCanWrite(aSrc.iCanWrite)
{
   LOG_METHOD();
}

CPosixFile::CPosixFile(const std::string& aFilename, std::ios_base::openmode aMode, TFStreamPtr& aStream, bool aCanWrite)
 : iFilename(aFilename), iMode(aMode), iStream(std::move(aStream)), iSize(0), iCanWrite(aCanWrite) 
{
   LOG_METHOD();
   if (Open()) {
      std::streampos fsize = iStream->tellg();
      iStream->seekg(0, std::ios::end);
      fsize = iStream->tellg() - fsize;
      iSize = fsize;
   }
}

bool CPosixFile::Open()
{
   LOG_METHOD();
   if (!IsOpen()) {
      iStream->open(iFilename, iMode);
      return !iStream->fail();
   }
   return true;
}

bool CPosixFile::IsOpen() const
{
   LOG_METHOD();
   return iStream->is_open();
}

void CPosixFile::Close()
{
   LOG_METHOD();
   iStream->close();
}

bool CPosixFile::Read(TFileData& aResult, TSize aPosition, TSize aSize)
{
   LOG_METHOD();
   aResult.clear();
   bool wasOpen = IsOpen();
   if (!wasOpen) {
      if (!Open()) {
         return false;
      }
   }
   if (aPosition+aSize > Size()) {
      return false;
   }
   if (aSize == 0) {
      aSize = Size() - aPosition;
   }
   aResult.resize(aSize);
   iStream->seekg(aPosition);
   iStream->read(&aResult[0], aSize);
   bool retval = !iStream->fail();
   if (!wasOpen) {
      Close();
   }
   return retval;
}

bool CPosixFile::Insert(const TFileData& aData, TSize aFileOffset, TSize aDataOffset, TSize aDataSize)
{
   LOG_METHOD();
   if (iCanWrite && CheckOffsets(aFileOffset, aDataOffset, aDataSize, aData.size())) {
      bool wasOpen = IsOpen();
      if (!wasOpen) {
         if (!Open()) {
            return false;
         }
      }
      TSize oldEnd = Size();
      iSize += aDataSize;
      if (aFileOffset < oldEnd) {
         TSize pos = oldEnd;
         while ((pos > kChunkSizeBytes) && (pos-kChunkSizeBytes >= aFileOffset)) {
            MoveChunk(pos-kChunkSizeBytes, kChunkSizeBytes, iSize-kChunkSizeBytes-(oldEnd-pos));
            pos -= kChunkSizeBytes;
         }
         if (pos > aFileOffset) {
            MoveChunk(aFileOffset, pos-aFileOffset, aFileOffset + aDataSize);
         }
      }
      bool retval = Overwrite(aData, aFileOffset, aDataOffset, aDataSize);
      if (!wasOpen) {
         Close();
      }
      return retval;
   }
   return false;
}

bool CPosixFile::Overwrite(const TFileData& aData, TSize aFileOffset, TSize aDataOffset, TSize aDataSize)
{
   LOG_METHOD();
   if (iCanWrite && CheckOffsets(aFileOffset, aDataOffset, aDataSize, aData.size())) {
      bool wasOpen = IsOpen();
      if (!wasOpen) {
         if (!Open()) {
            return false;
         }
      }
      iStream->seekp(aFileOffset);
      iStream->write(&aData[aDataOffset], aDataSize);
      bool retval = !iStream->fail();
      if (aFileOffset + aDataSize > Size()) {
         iSize = aFileOffset + aDataSize;
      }
      if (!wasOpen) {
         Close();
      }
      return retval;
   }
   return false;
}

bool CPosixFile::Clear()
{
   LOG_METHOD();
   if (iCanWrite) {
      bool wasOpen = IsOpen();
      if (wasOpen) {
         Close();
      }
      iStream->open(iFilename, iMode | std::ios_base::trunc);
      iSize = 0;
      bool retval = !iStream->fail();
      if (!wasOpen) {
         Close();
      }
      return retval;
   }
   return false;
}

void CPosixFile::MoveChunk(TSize aSourcePos, TSize aSize, TSize aDestPos)
{
   LOG_METHOD();
   TFileData data;
   Read(data, aSourcePos, aSize);
   Overwrite(data, aDestPos, 0, aSize);
}

