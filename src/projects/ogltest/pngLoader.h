/**
 *
 */
#ifndef AK_PNGLOADER_H_INCLUDED
#define AK_PNGLOADER_H_INCLUDED

#include <vector>
#include <string>
#include <memory>
#include "common/cabinet/File.h"


class CTexture;
typedef std::unique_ptr<CTexture> TTexturePtr;

class CTexture
{
public:
   static TTexturePtr LoadFromMemory(TFilePtr& aFile, const std::string& aLogdata);

   int32_t ID() { return iGLindex; }
   uint16_t Width() { return iImageWidth; }
   uint16_t Height() { return iImageHeight; }

private:
   CTexture(int32_t aGLindex, uint16_t aTextureWidth, uint16_t aTextureHeight, uint16_t aImageWidth, uint16_t aImageHeight)
    : iGLindex(aGLindex), iTextureWidth(aTextureWidth), iTextureHeight(aTextureHeight)
    , iImageWidth(aImageWidth), iImageHeight(aImageHeight)
   {}

   static TTexturePtr LoadPNG(TFilePtr& aFile, const std::string& aLogdata, void* aPngPtr);

   int32_t iGLindex;
   uint16_t iTextureWidth;
   uint16_t iTextureHeight;
   uint16_t iImageWidth;
   uint16_t iImageHeight;
};

#endif // AK_PNGLOADER_H_INCLUDED
