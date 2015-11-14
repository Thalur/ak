/**
 *
 */
#include "common/cabinet/MemoryFile.h"
#include "common/log/log.h"
#include <fstream>

// Construct a memory file with given data, copying the backing data vector
CMemoryFile::CMemoryFile(const TFileData& aData)
 : iBuffer(aData)
{
   LOG_DEBUG("TFileData constructor.");
}

// Construct a memory file with given data, moving the backing data vector
CMemoryFile::CMemoryFile(TFileData&& aData)
 : iBuffer(aData) // move construction
{
   //LOG_DEBUG("TFileData move constructor.");
}

// Move constructor: move the data but create a new stream
CMemoryFile::CMemoryFile(CMemoryFile&& aSrc)
 : iBuffer(std::move(aSrc.iBuffer))
{
   LOG_DEBUG("Move constructor.");
}

// Copy constructor
CMemoryFile::CMemoryFile(const CMemoryFile& aSrc)
 : iBuffer(aSrc.iBuffer)
{
   LOG_DEBUG("Copy constructor.");
}

TMemoryFilePtr CMemoryFile::FromFile(CFile& aFile)
{
   TFileData result;
   TSize size = aFile.Size();
   TSize maxSize = result.max_size();
   if (size > maxSize) {
      LOG_WARN("File size %" PRIuS " exceeds maximum memory file size. cropping to %" PRIuS, size, maxSize);
      size = maxSize;
   }
   TSize readSize = size;
   if (aFile.Read(result, 0, readSize)) {
      if (result.size() != readSize) {
         LOG_WARN("Could not read the complete file. Expected %d but got %d", readSize, result.size());
      }
      if ((result.size() > 0) || (readSize == 0)) {
         return std::make_shared<CMemoryFile>(std::move(result));
      }
   }
   return TMemoryFilePtr();
}

bool CMemoryFile::Read(TFileData& aResult, TSize aPosition, TSize aSize)
{
   aResult.clear();
   if (aPosition+aSize > Size()) {
      return false;
   }
   if (aSize == 0) {
      aSize = Size() - aPosition;
   }
   aResult.reserve(aSize);
   std::copy(iBuffer.begin() + aPosition, iBuffer.begin() + aPosition + aSize, std::back_inserter(aResult));
   return true;
}

std::stringstream CMemoryFile::CreateStream()
{
#ifdef _WIN32 // MSVC does not support replacing the stringstream back buffer
   return std::stringstream(std::string(&iBuffer[0], Size()), std::stringstream::in);
#else
   std::stringstream stream;
   stream.rdbuf()->pubsetbuf(&iBuffer[0], Size());
   return stream;
#endif
}


CWriteableMemoryFile::CWriteableMemoryFile(TSize aSize) : CMemoryFile(TFileData(aSize))
{
}

bool CWriteableMemoryFile::Insert(const TFileData& aData, TSize aFileOffset, TSize aDataOffset, TSize aDataSize)
{
   if (aFileOffset > Size()) {
      return false;
   }
   if (!CheckOffsets(aFileOffset, aDataOffset, aDataSize, aData.size())) {
      return false;
   }
   std::copy(aData.begin() + aDataOffset, aData.begin() + aDataOffset + aDataSize,
             std::inserter(iBuffer, iBuffer.begin() + aFileOffset));
   return true;
}

bool CWriteableMemoryFile::Overwrite(const TFileData& aData, TSize aFileOffset, TSize aDataOffset, TSize aDataSize)
{
   if (aFileOffset > Size()) {
      return false;
   }
   if (!CheckOffsets(aFileOffset, aDataOffset, aDataSize, aData.size())) {
      return false;
   }
   if (aFileOffset + aDataSize > Size()) {
      iBuffer.resize(aFileOffset + aDataSize);
   }
   std::copy(aData.begin() + aDataOffset, aData.begin() + aDataOffset + aDataSize,
             iBuffer.begin() + aFileOffset);
   return true;
}

bool CWriteableMemoryFile::Remove(TSize aDataOffset, TSize aDataSize)
{
   if (aDataOffset < Size()) {
      return false;
   }
   if (aDataSize == 0) {
      if (aDataOffset == 0) {
         iBuffer.clear();
      } else {
         iBuffer.erase(iBuffer.begin() + aDataOffset, iBuffer.end());
      }
   } else {
      if (aDataOffset + aDataSize > Size()) {
         aDataSize = Size() - aDataOffset;
      }
      iBuffer.erase(iBuffer.begin() + aDataOffset, iBuffer.begin() + aDataOffset + aDataSize);
   }
   return true;
}
