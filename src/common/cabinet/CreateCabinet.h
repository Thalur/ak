/**
 *
 */
#ifndef AK_CREATECABINET_H_INCLUDED
#define AK_CREATECABINET_H_INCLUDED

#include "common/cabinet/Cabinet.h"


class CCreateCabinet
{
public:
   CCreateCabinet();
   CCreateCabinet(const TCabinetPtr& aSourceCabinet);

   void Clear();
   bool RemoveFile(TSize aIndex);
   bool RemoveFile(const std::string& aFilename);
   bool AddFile(const std::string& aFilename, const TFilePtr& aSourceFile, bool aEncrypted=true, bool aCompressed=true);
   bool AddFile(const std::string& aFilename, const TCabinetPtr& aSourceCabinet, TSize aSourceIndex); // use source entry comp/enc
   bool AddFile(const std::string& aFilename, const TCabinetPtr& aSourceCabinet, TSize aSourceIndex, bool aEncrypted, bool aCompressed);

   bool RenameFile(const std::string& aOriginalFilename, const std::string& aNewFilename);
   bool SetFileProperties(const std::string& aFilename, bool aEncrypt, bool aCompress);

   void SetDestFile(const TWriteableFilePtr& aFile) { iFile = aFile; }
   void SetEncryptionKey(int32_t aEncryptionKey) { iEncryptionKey = aEncryptionKey; }
   void SetEncryption(CCabinet::TEncryption aEncryption) { iEncryption = aEncryption; }
   void SetCompression(CCabinet::TCompression aCompression) { iCompression = aCompression; }
   void SetPatchCycle(uint16_t aPatchCycle) { iPatchCycle = aPatchCycle; }
   void SetCabinetVersion(uint16_t aCabinetVersion) { iCabinetVersion = aCabinetVersion; }

   bool WriteToDisk();
   bool WriteToDisk(const TWriteableFilePtr& aFile) { SetDestFile(aFile); return WriteToDisk(); }

   TSize GetNumFiles() const { return iEntries.size(); }
   int32_t GetEncryptionKey() const { return iEncryptionKey; }
   bool IsEncrypted() const { return iEncryption != CCabinet::ENCRYPT_NONE; }
   CCabinet::TEncryption GetEncryption() const { return iEncryption; }
   bool IsCompressed() const { return iCompression != CCabinet::COMPRESS_NONE; }
   CCabinet::TCompression GetCompression() const { return iCompression; }
   uint16_t GetCabinetVersion() const { return iCabinetVersion; }
   uint16_t GetPatchCycle() const { return iPatchCycle; }

   void PrintFileIndex() const;

   struct TCreatedEntry
   {
      TCreatedEntry(const std::string& aFilename, bool aEncrypted, bool aCompressed, const TFilePtr& aSourceFile)
       : iFilename(aFilename), iFileSize(aSourceFile->Size()), iEncrypted(aEncrypted)
       , iCompressed(aCompressed), iInCabinet(false), iSourceFile(aSourceFile), iSourceIndex(0)
      {}
      TCreatedEntry(const std::string& aFilename, bool aEncrypted, bool aCompressed,
                    const TCabinetPtr& aSourceCabinet, TSize aSourceIndex)
       : iFilename(aFilename), iEncrypted(aEncrypted), iCompressed(aCompressed)
       , iInCabinet(true), iSourceCabinet(aSourceCabinet), iSourceIndex(aSourceIndex)
      {
         iFileSize = iSourceCabinet->iEntries[iSourceIndex].iFileSize;
      }

      std::string iFilename;
      TSize iFileSize;
      bool iEncrypted;
      bool iCompressed;
      bool iInCabinet;
      TFilePtr iSourceFile; // if !iInCabinet
      TCabinetPtr iSourceCabinet; // if iInCabinet
      TSize iSourceIndex;         // if iInCabinet
   };

private:
   std::vector<TCreatedEntry>::iterator FindEntryByName(const std::string& aFilename);
   bool EncryptData(TFileData& aData, uint64_t aKeyOffset);
   bool DeflateData(TFileData& aData, bool aForceDeflation=false);

   bool iCreatedFromCabinet;
   TCabinetPtr iSourceCabinet; // if iCreatedFromCabinet
   TWriteableFilePtr iFile; // File to save to
   int32_t iEncryptionKey;
   uint16_t iPatchCycle;
   uint16_t iCabinetVersion; // 0: invalid
   CCabinet::TEncryption iEncryption;
   CCabinet::TCompression iCompression;
   std::vector<TCreatedEntry> iEntries;
};


#endif // AK_CREATECABINET_H_INCLUDED
