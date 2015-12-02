/**
 *
 */
#ifndef AK_ANDROIDASSETFILE_H_INCLUDED
#define AK_ANDROIDASSETFILE_H_INCLUDED

#include "common/cabinet/File.h"
#include <android/asset_manager.h>

class CAndroidAssetFile;

typedef std::shared_ptr<CAndroidAssetFile> TAndroidAssetFilePtr;

class CAndroidAssetFile : public CFile
{
public:
   static TAndroidAssetFilePtr OpenAsset(AAssetManager* aManager, const std::string& aFilename);
   CAndroidAssetFile(CAndroidAssetFile&& aSrc);
   virtual ~CAndroidAssetFile();

   // CFile implementation
   virtual bool Open() override;
   virtual bool IsOpen() const override { return iOpen; }
   virtual void Close() override;
   virtual TSize Size() const override { return iSize; }
   virtual bool Read(TFileData& aResult, TSize aPosition = 0, TSize aSize = 0) override;

protected:
   explicit CAndroidAssetFile(AAssetManager* aManager, const std::string& aFilename, TSize aSize, AAsset* aFile);
   CAndroidAssetFile(const CAndroidAssetFile&) = delete;

   AAssetManager* iManager; // not owned
   std::string iFilename;
   TSize iSize;
   bool iOpen;
   AAsset* iFile; // owned
};

#endif // AK_ANDROIDASSETFILE_H_INCLUDED

