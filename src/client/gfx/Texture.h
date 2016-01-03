/**
 * CTexture - OpenGL texture representation class
 */
#ifndef AK_TEXTURE_H_INCLUDED
#define AK_TEXTURE_H_INCLUDED

#include <memory>

namespace Client
{

class CTexture
{
public:
   CTexture(int32_t aGLindex, uint16_t aWidth, uint16_t aHeight, uint16_t aTexWidth, uint16_t aTexHeight, bool aTransparent)
    : iGLindex(aGLindex), iWidth(aWidth), iHeight(aHeight), iTexWidth(aTexWidth), iTexHeight(aTexHeight), iTransparent(aTransparent)
   {}

   int32_t ID() const { return iGLindex; }
   uint16_t Width() const { return iWidth; }
   uint16_t Height() const { return iHeight; }
   uint16_t TexWidth() const { return iTexWidth; }
   uint16_t TexHeight() const { return iTexHeight; }
   float CropX() const { return static_cast<float>(iWidth) / static_cast<float>(iTexWidth); }
   float CropY() const { return static_cast<float>(iHeight) / static_cast<float>(iTexHeight); }
   bool HasTransparency() const { return iTransparent; }

private:
   int32_t iGLindex;
   uint16_t iWidth; // image size; raw texture may be bigger (next power of 2)
   uint16_t iHeight;
   uint16_t iTexWidth; // texture size (always a power of 2)
   uint16_t iTexHeight;
   bool iTransparent; // Texture has an alpha component
};

typedef std::unique_ptr<CTexture> TTexturePtr;

} // namespace Client

#endif // AK_TEXTURE_H_INCLUDED
