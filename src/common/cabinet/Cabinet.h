/**
 * AK Cabinet File - Multi-platform storage container implementation
 * featuring compression and encryption on a per-file basis.
 * 
 * Copyright (C) 2015 by André Koschmieder
 *
 * AKCabinet file definition Version 3:
 * The files are expected to be sorted by filename.
 * HEADER (24 bytes) - all numbers are unsigned and stored as big-endian
 *  4 - (4*char) AK File prefix ( ?? 'A' 'K' ?? )
 *  2 - (TUint16) patch cycle (basically: 2 bytes user data)
 *  2 - (TUint16) cabinet version (currently supported: 2..3)
 *  4 - (TUint32) header size (size of FILEHEADERS section)
 *  1 - (TUint8)  encryption method (0: no encryption, or ENCRYPT_*)
 *  6 - (TUint48) file size (to detect corrupted archives), max: 256 TB
 *  1 - (TUint8)  compression method (0: no compression; or COMPRESS_*)
 *  4 - (TUint32) number of files (max: 2^31-1 for Java compatibility)
 * FILEHEADERS ((12 + name) bytes per file) -- compression and encryption starts here
 *  X - (TUint16+x*char) file name (java style storage: 2 bytes length + string)
 *  6 - (TUint48) file data offset in the cabinet (after headers, so 0 for first file)
 *  5 - (TUint40) file data size (max: 1 TB; files > 2 GB cannot be compressed, encrypted, or opened as MemoryFile)
 *  1 - (TUint8)  storage flags (compressed,encrypted)
 * BINARY DATA
 *  Y - file data size bytes
 *      (for compressed files, the first 4 bytes are the uncompressed file size)</pre>
 */
#ifndef AK_CABINET_H_INCLUDED
#define AK_CABINET_H_INCLUDED

#include "common/types.h"
#include "common/cabinet/MemoryFile.h"
#include <string>

class CCabinet;
typedef std::shared_ptr<CCabinet> TCabinetPtr;
typedef std::pair<const std::string&, TSize> TEntryIndex;


class CCabinetEntry
{
public:
   std::string Filename() const { return iFilename; }
   std::string Prefix() const { return iPrefix; }

protected:
   CCabinetEntry(const std::string& aFilename, uint64_t aOffset, uint64_t aSize, bool aCompressed, bool aEncrypted);

   std::string iFilename;   // complete file name as stored in the cabinet
   std::string iPrefix;     // file name part: [prefix].[extension]
   std::string iExtension;
   uint64_t iFileOffset;     // position of the data in the cabinet
   uint64_t iFileSize;       // file size "on disk"
   bool iCompressed;
   bool iEncrypted;

   friend class CCabinet;
   friend class CCreateCabinet;
};


class CCabinet
{
public:
   enum TEncryption { ENCRYPT_NONE = 0, ENCRYPT_AKV2 = 1 };
   enum TCompression { COMPRESS_NONE = 0, COMPRESS_ZIP = 1 };

   static TCabinetPtr Open(TFilePtr& aFile, int32_t aKey = 0);

   TSize GetNumFiles() const { return iEntries.size(); }
   bool IsEncrypted() const { return iEncryption != ENCRYPT_NONE; }
   TEncryption GetEncryption() const { return iEncryption; }
   bool IsCompressed() const { return iCompression != COMPRESS_NONE; }
   TCompression GetCompression() const { return iCompression; }
   uint16_t GetPatchCycle() const { return iPatchCycle; }
   uint16_t GetVersion() const { return iCabinetVersion; }
   uint64_t GetFileSize() const { return iFileSize; }

   void PrintFileIndex() const;
   TMemoryFilePtr ReadFileByName(const std::string& aFilename);
   TMemoryFilePtr ReadFileByPrefix(const std::string& aPrefix);
   TMemoryFilePtr ReadFileByIndex(TSize aIndex);
   bool GetFileIndex(TSize& outIndex, const std::string& aFilename) const;
   std::vector<TEntryIndex> GetFileList() const;
   std::vector<TEntryIndex> GetFilePrefixList() const;

protected:
   CCabinet(TFilePtr& aFile, int32_t aKey);

   bool ReadHeader();
   bool ReadHeaderV2();
   bool ReadHeaderV3();
   bool ReadFilePart(TFileData& aResult, uint64_t aOffset, uint64_t aSize, uint64_t aKeyOffset,
                     bool aEncrypted, bool aCompressed);
   bool DecryptData(TFileData& aData, uint64_t aKeyOffset);
   bool InflateData(TFileData& aData);
   TMemoryFilePtr ReadFile(const std::string& aName, bool aFullName);

   TFilePtr iFile;
   int32_t iDecryptionKey;
   uint16_t iPatchCycle;
   uint16_t iCabinetVersion; // 0: invalid
   uint64_t iFileSize;
   TEncryption iEncryption;
   TCompression iCompression;
   std::vector<CCabinetEntry> iEntries;
   uint64_t iDataOffset;

   friend class CCreateCabinet;
};

#endif // AK_CABINET_H_INCLUDED
