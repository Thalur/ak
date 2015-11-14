/**
 *
 */
#include "common/cabinet/File.h"
#include "common/log/log.h"
#include <sstream>

const std::string CFile::DEFAULT_COMMENT_SYMBOLS = {"#%;\r"};
const TSize CWriteableFile::kChunkSizeBytes = 1024*1024; // Write in chunks of 1 MB

namespace {
const TSize MAX_LINE_LENGTH = 1050; // longest supported text file line
}

std::string CFile::ReadLine()
{
   TSize end = Size();
   if (iPos < end) {
      TSize length = MAX_LINE_LENGTH;
      if (iPos + length > end) {
         length = end - iPos;
      }
      TFileData data(length);
      Read(data, iPos, length);
      std::string s(&data[0], data.size());
      TSize pos = s.find('\n');
      if (pos != std::string::npos) {
         iPos += pos+1;
         if ((pos > 0) && (s[pos-1] == '\r')) {
            pos--;
         }
         s.resize(pos);
      }
      return s;
   }
   return std::string();
}

std::string CFile::GetNextConfigLine(const std::string& aCommentSymbols)
{
	std::string line;
	while (!AtEOF()) {
      line = ReadLine();
		if (!line.empty()) { // ignore empty lines
			TSize pos = line.find_first_not_of(" \t");
			if (pos != std::string::npos) { // ignore whitespace-only lines
				if (pos > 0) {
               line.substr(pos).swap(line);
            }
				pos = line.find_first_of(aCommentSymbols);
				if (pos > 0) { // ignore comment-only lines
					if (pos != std::string::npos) {
                  line.substr(0,pos).swap(line);
               }
					pos = line.find_last_not_of(" \t");
					if (pos != std::string::npos) {
                  line.substr(0,pos+1).swap(line);
               }
					break;
				}
			}
		}
	}
	return line;
}

bool CFile::CheckOffsets(TSize& aDataOffset, TSize& aDataSize, TSize aRealDataSize)
{
   if (aDataOffset+aDataSize > aRealDataSize) {
      return false;
   }
   if (aDataSize == 0) {
      aDataSize = aRealDataSize - aDataOffset;
   }
   return true;
}

bool CFile::CheckOffsets(TSize& aFileOffset, TSize& aDataOffset, TSize& aDataSize, TSize aRealDataSize)
{
   if (aFileOffset > Size()) {
      aFileOffset = Size();
   }
   return CheckOffsets(aDataOffset, aDataSize, aRealDataSize);
}



bool CWriteableFile::WriteComplete(const TFileData& aData, TSize aDataOffset, TSize aDataSize)
{
   if (!CheckOffsets(aDataOffset, aDataSize, aData.size())) {
      return false; // Double-check to not clear on error
   }
   Clear();
   return Insert(aData, 0, aDataOffset, aDataSize);
}

bool CWriteableFile::Append(const TFileData& aData, TSize aDataOffset, TSize aDataSize)
{
   return Insert(aData, Size(), aDataOffset, aDataSize);
}


bool CWriteableFile::WriteComplete(CFile& aSourceFile, TSize aSourceOffset, TSize aSourceSize)
{
   if (!CheckOffsets(aSourceOffset, aSourceSize, aSourceFile.Size())) {
      return false; // Double-check to not clear on error
   }
   Clear();
   return Insert(aSourceFile, 0, aSourceOffset, aSourceSize);
}

bool CWriteableFile::Append(CFile& aSourceFile, TSize aSourceOffset, TSize aSourceSize)
{
   return Insert(aSourceFile, Size(), aSourceOffset, aSourceSize);
}

bool CWriteableFile::Insert(CFile& aSourceFile, TSize aDestinationOffset, TSize aSourceOffset, TSize aSourceSize)
{
   if (!CheckOffsets(aDestinationOffset, aSourceOffset, aSourceSize, aSourceFile.Size())) {
      return false;
   }

   // Copy the file in chunks
   bool bOK = true;
   TSize pos = aSourceOffset;
   TSize end = aSourceOffset + aSourceSize;
   TFileData buffer(kChunkSizeBytes);
   while (bOK && pos < end) {
      TSize bytesToRead = kChunkSizeBytes;
      if (pos + kChunkSizeBytes > end) {
         bytesToRead = end - pos;
      }
      if (aSourceFile.Read(buffer, pos, bytesToRead)) {
         bOK = Insert(buffer, aDestinationOffset);
         pos += bytesToRead;
         aDestinationOffset += bytesToRead;
      } else bOK = false;
   }
   return bOK;
}
