/**
 *
 */
#include "AndroidAssetFile.h"
#include "common/log/log.h"


TAndroidAssetFilePtr CAndroidAssetFile::OpenAsset(AAssetManager* aManager, const std::string& aFilename)
{
   LOG_METHOD();
   if (aManager != nullptr) {
      AAsset* assetFile = AAssetManager_open(aManager, aFilename.c_str(), AASSET_MODE_RANDOM);
      if (assetFile != nullptr) {
         TSize fileSize = AAsset_getLength(assetFile);
         if (fileSize > 0) {
            return TAndroidAssetFilePtr(new CAndroidAssetFile(aManager, aFilename, fileSize, assetFile));
         } else {
            LOG_ERROR("Empty Asset file %s", aFilename.c_str());
         }
      } else {
        LOG_ERROR("Could not open asset file %s", aFilename.c_str());
      }
   } else {
      LOG_ERROR("Asset manager is required");
   }
   return TAndroidAssetFilePtr();
}

CAndroidAssetFile::CAndroidAssetFile(CAndroidAssetFile&& aSrc)
 : iManager(aSrc.iManager), iFilename(std::move(aSrc.iFilename))
 , iSize(aSrc.iSize), iOpen(aSrc.iOpen), iFile(aSrc.iFile)
{
   LOG_METHOD();
}

CAndroidAssetFile::CAndroidAssetFile(AAssetManager* aManager, const std::string& aFilename, TSize aSize, AAsset* aFile)
 : iManager(aManager), iFilename(aFilename), iSize(aSize), iOpen(true), iFile(aFile)
{
   LOG_METHOD();
   
}

CAndroidAssetFile::~CAndroidAssetFile()
{
   LOG_METHOD();
   if (IsOpen()) {
      Close();
   }
}

bool CAndroidAssetFile::Open()
{
   LOG_METHOD();
   if (!IsOpen()) {
      iFile = AAssetManager_open(iManager, iFilename.c_str(), AASSET_MODE_RANDOM);
      return (iFile != nullptr);
   }
   return true;
}

void CAndroidAssetFile::Close()
{
   LOG_METHOD();
   if (IsOpen()) {
      iOpen = false;
      AAsset_close(iFile);
      iFile = nullptr;
   }
}

bool CAndroidAssetFile::Read(TFileData& aResult, TSize aPosition, TSize aSize)
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
   bool retval = AAsset_seek(iFile, aPosition, SEEK_SET) != -1;
   if (retval) {
      retval = AAsset_read(iFile, &aResult[0], aSize) == aSize;
   }
   if (!wasOpen) {
      Close();
   }
   return retval;
}

