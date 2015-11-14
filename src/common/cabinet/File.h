/**
 * Abstract file interface definition
 */
#ifndef AK_FILE_H_INCLUDED
#define AK_FILE_H_INCLUDED

#include "common/types.h"
#include <vector>
#include <memory>

class CFile;
class CWriteableFile;

typedef std::vector<char> TFileData;
typedef std::shared_ptr<CFile> TFilePtr;
typedef std::shared_ptr<CWriteableFile> TWriteableFilePtr;

class CFile
{
public:
   CFile() : iPos(0) {}
   virtual bool Open() = 0;
   virtual bool IsOpen() const = 0;
   virtual void Close() = 0;
   virtual TSize Size() const = 0;

   virtual bool Read(TFileData& aResult, TSize aPosition, TSize aSize) = 0;

   virtual ~CFile() {}

   // Convenience functions to read from text files using the internal stream
   bool AtEOF() { return iPos >= Size(); }
   void SetPosition(TSize aPos) { iPos = aPos; }
   std::string ReadLine();
   std::string GetNextConfigLine(const std::string& aCommentSymbols = DEFAULT_COMMENT_SYMBOLS);

protected:
   TSize iPos; // Simulate a stream position for the string reading functions
   const static std::string DEFAULT_COMMENT_SYMBOLS;

   bool CheckOffsets(TSize& aDataOffset, TSize& aDataSize, TSize aRealDataSize);
   bool CheckOffsets(TSize& aFileOffset, TSize& aDataOffset, TSize& aDataSize, TSize aRealDataSize);
};

class CWriteableFile : virtual public CFile
{
public:
   virtual bool Insert(const TFileData& aData, TSize aFileOffset, TSize aDataOffset = 0, TSize aDataSize = 0) = 0;
   virtual bool Overwrite(const TFileData& aData, TSize aFileOffset, TSize aDataOffset = 0, TSize aDataSize = 0) = 0;
   virtual bool Clear() = 0;

   virtual bool WriteComplete(const TFileData& aData, TSize aDataOffset = 0, TSize aDataSize = 0);
   virtual bool Append(const TFileData& aData, TSize aDataOffset = 0, TSize aDataSize = 0);

   // Convenience functions for copying data between files
   bool WriteComplete(CFile& aSourceFile, TSize aSourceOffset = 0, TSize aSourceSize = 0);
   bool Append(CFile& aSourceFile, TSize aSourceOffset = 0, TSize aSourceSize = 0);
   bool Insert(CFile& aSourceFile, TSize aDestinationOffset, TSize aSourceOffset = 0, TSize aSourceSize = 0);

protected:
   const static TSize kChunkSizeBytes;
};

#endif // AK_FILE_H_INCLUDED
