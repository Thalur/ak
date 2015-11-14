/**
 *
 */
#ifndef AK_POSIXFILE_H_INCLUDED
#define AK_POSIXFILE_H_INCLUDED

#include "common/cabinet/File.h"
#include <memory>
#include <fstream>

class CPosixFile;

typedef std::shared_ptr<CPosixFile> TPosixFilePtr;

class CPosixFile : public CWriteableFile
{
public:
   typedef std::unique_ptr<std::fstream> TFStreamPtr;

   static TPosixFilePtr OpenExistingFile(const std::string& aFilename, bool bAllowWrite = true);
   static TPosixFilePtr CreateEmptyFile(const std::string& aFilename, bool bOverwrite = true);
   static TPosixFilePtr OpenOrCreate(const std::string& aFilename);
   CPosixFile(CPosixFile&& aSrc);
   virtual ~CPosixFile() {}

   // CFile implementation
   virtual bool Open() override;
   virtual bool IsOpen() const override;
   virtual void Close() override;
   virtual TSize Size() const override { return iSize; }
   virtual bool Read(TFileData& aResult, TSize aPosition = 0, TSize aSize = 0) override;

   // CWriteableFile implementation
   virtual bool Insert(const TFileData& aData, TSize aFileOffset, TSize aDataOffset = 0, TSize aDataSize = 0) override;
   virtual bool Overwrite(const TFileData& aData, TSize aFileOffset, TSize aDataOffset = 0, TSize aDataSize = 0) override;
   virtual bool Clear() override;

protected:
   explicit CPosixFile(const std::string& aFilename, std::ios_base::openmode aMode,
                       TFStreamPtr& aStream, bool aCanWrite);
   CPosixFile(const CPosixFile&) = delete;

   std::string iFilename;
   std::ios_base::openmode iMode;
   TFStreamPtr iStream;
   TSize iSize;
   bool iCanWrite;

private:
   static TPosixFilePtr MakeFile(const std::string& aFilename, std::ios_base::openmode aMode, bool aAllowWrite);
   void MoveChunk(TSize aSourcePos, TSize aSize, TSize aDestPos);
};

#endif // AK_POSIXFILE_H_INCLUDED
