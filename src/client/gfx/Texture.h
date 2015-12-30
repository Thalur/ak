/**
 * CTexture - OpenGL texture representation class
 */
#ifndef AK_TEXTURE_H_INCLUDED
#define AK_TEXTURE_H_INCLUDED

#include <memory>

namespace Client
{

class CTexture;
typedef std::unique_ptr<CTexture> TTexturePtr;

class CTexture
{
public:
   CTexture(int32_t aGLindex, uint16_t aWidth, uint16_t aHeight, float aCropX, float aCropY)
    : iGLindex(aGLindex), iWidth(aWidth), iHeight(aHeight), iCropX(aCropX), iCropY(aCropY)
   {}

   int32_t ID() { return iGLindex; }
   uint16_t Width() { return iWidth; }
   uint16_t Height() { return iHeight; }
   // cmath rounding functions are not available in Android NDK
   uint16_t TexWidth() { return static_cast<uint16_t>(static_cast<float>(iWidth) / iCropX + 0.5); }
   uint16_t TexHeight() { return static_cast<uint16_t>(static_cast<float>(iHeight) / iCropY + 0.5); }
   float CropX() { return iCropX; }
   float CropY() { return iCropY; }

private:
   int32_t iGLindex;
   uint16_t iWidth; // image size; raw texture may be bigger (next power of 2)
   uint16_t iHeight;
   float iCropX; // cropped part of the image defined by iWidth [0,1]
   float iCropY;
};

} // namespace Client

#endif // AK_TEXTURE_H_INCLUDED
