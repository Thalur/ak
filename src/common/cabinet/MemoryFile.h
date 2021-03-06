/**
 *
 */
#ifndef AK_MEMORYFILE_H_INCLUDED
#define AK_MEMORYFILE_H_INCLUDED

#include "common/cabinet/File.h"
#include <sstream>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4250) // MSVC bug: diamond inheritance with virtual methods generates warning
#endif

class CMemoryFile;
typedef std::shared_ptr<CMemoryFile> TMemoryFilePtr;

class CMemoryFile : virtual public CFile
{
public:
   typedef std::unique_ptr<std::stringstream> TStringStreamPtr;

   CMemoryFile(const TFileData& aData);
   CMemoryFile(TFileData&& aData);
   CMemoryFile(CMemoryFile&& aSrc);
   CMemoryFile(const CMemoryFile& aSrc);
   static TMemoryFilePtr FromFile(CFile& aFile);
   virtual ~CMemoryFile() {}

   // CFile implementation
   virtual bool Open() override { return true; }
   virtual bool IsOpen() const override { return true; }
   virtual void Close() override {}
   virtual TSize Size() const override { return iBuffer.size(); }
   virtual bool Read(TFileData& aResult, TSize aPosition = 0, TSize aSize = 0) override;

   // Additional memory file functionality
   TFileData GetBuffer() const { return iBuffer; }
   TStringStreamPtr CreateStream();

   char operator[] (TSize i) const { if (i < Size()) return iBuffer[i]; return -1; }

protected:
   TFileData iBuffer;
};

class CWriteableMemoryFile : public CMemoryFile, public CWriteableFile
{
public:
   CWriteableMemoryFile(TSize aSize = 0);
   CWriteableMemoryFile(CMemoryFile&& aSource);
   virtual ~CWriteableMemoryFile() {}

   // CWriteableFile implementation
   virtual bool Insert(const TFileData& aData, TSize aFileOffset, TSize aDataOffset = 0, TSize aDataSize = 0) override;
   virtual bool Overwrite(const TFileData& aData, TSize aFileOffset, TSize aDataOffset = 0, TSize aDataSize = 0) override;
   virtual bool Clear() override { return Remove(); }
   
   bool Remove(TSize aDataOffset = 0, TSize aDataSize = 0);
   
   using CWriteableFile::WriteComplete;
   using CWriteableFile::Append;
   using CWriteableFile::Insert;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // AK_MEMORYFILE_H_INCLUDED
