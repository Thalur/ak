/**
 *
 */
#ifndef AK_GRAPHICSCOMPONENT_H_INCLUDED
#define AK_GRAPHICSCOMPONENT_H_INCLUDED

#include "Font.h"
#include "Texture.h"
#include "oglincludes.h"
#include "client/main/ResourceManager.h"
#include "common/log/log.h"


class CCabManager;

namespace Client
{

class CGraphicsComponent
{
public:
   CGraphicsComponent(int32_t aWidth, int32_t aHeight);
   ~CGraphicsComponent();

   void InitOpenGL();
   void LoadGraphics(CCabManager& aCabinets, const TFileList& aFiles);
   void LoadFonts(CCabManager& aCabinets, const TFileList& aFiles, CResourceManager &aResourceManager);

   // Most drawing methods are inline for performance reasons
   void StartFrame() const
   {
      glClearColor(0.2f, 0.2f, 0.2f, 1);
      glClear(GL_COLOR_BUFFER_BIT);
      iCurrentTexture = -1;
   }

   void Draw(TResourceFileId aTexture, int32_t x, int32_t y) const
   {
      const CTexture* texture = GetTexture(aTexture);
      if (texture != nullptr) {
         DrawTexture(*texture, x, y, texture->Width(), texture->Height());
      }
   }

   void Draw(TResourceFileId aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight) const
   {
      const CTexture* texture = GetTexture(aTexture);
      if (texture != nullptr) {
         DrawTexture(*texture, x, y, aWidth, aHeight);
      }
   }

   void Draw(TResourceFileId aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
             int32_t aTexLeft, int32_t aTexTop, int32_t aTexRight, int32_t aTexBottom) const
   {
      const CTexture* texture = GetTexture(aTexture);
      if (texture != nullptr) {
         DrawTexture(*texture, x, y, aWidth, aHeight, aTexLeft, aTexTop, aTexRight, aTexBottom);
      }
   }

   void DrawText(TResourceFileId aFont, const std::string& aLine, int32_t x, int32_t y, int32_t aWidth,
                 int32_t aHeight, TFontStyle aStyle, int32_t aScale, uint32_t aVariant) const
   {
      auto it = iFonts.find(aFont);
      if (it != iFonts.end()) {
         it->second->DrawScaled(aLine, x, y, aWidth, aHeight, aStyle, aScale, aVariant);
      } else {
         LOG_ERROR("Invalid font %d", aFont);
      }
   }

private:
   void DrawTexture(const CTexture& aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight) const
   {
      BindTexture(aTexture);
#ifdef AK_SYSTEM_ANDROID
      NativeBlitAndroid(x, iHeight-y-aHeight, aWidth, aHeight);
#else
      NativeBlitPosix(static_cast<float>(x), static_cast<float>(y), static_cast<float>(aWidth), static_cast<float>(aHeight),
         0, 0, aTexture.CropX(), aTexture.CropY());
#endif
   }
   void DrawTexture(const CTexture& aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
                    int32_t aTexLeft, int32_t aTexTop, int32_t aTexRight, int32_t aTexBottom) const
   {
      BindTexture(aTexture);
#ifdef AK_SYSTEM_ANDROID
      NativeBlitAndroid(x, iHeight-y-aHeight, aWidth, aHeight, aTexLeft, aTexture.TexHeight() - aTexBottom,
                        aTexRight, aTexture.TexHeight() - aTexTop, aTexture.Width(), aTexture.Height());
#else
      float texWidth = static_cast<float>(aTexture.TexWidth());
      float texHeight = static_cast<float>(aTexture.TexHeight());
      NativeBlitPosix(static_cast<float>(x), static_cast<float>(y), static_cast<float>(aWidth), static_cast<float>(aHeight),
         static_cast<float>(aTexLeft) / texWidth, (texHeight - static_cast<float>(aTexBottom)) / texHeight,
         static_cast<float>(aTexRight) / texWidth, (texHeight - static_cast<float>(aTexTop)) / texHeight);
#endif
   }

   const CTexture* GetTexture(const TResourceFileId aTexture) const
   {
      auto texture = iTextures.find(aTexture);
      if (texture != iTextures.end()) {
         return &(*texture->second);
      }
      return nullptr;
   }
   void BindTexture(const CTexture& aTexture) const
   {
      if (iCurrentTexture != aTexture.ID()) {
         iCurrentTexture = aTexture.ID();
         glBindTexture(GL_TEXTURE_2D, iCurrentTexture);
         if (iBlending != aTexture.HasTransparency()) {
            iBlending = !iBlending;
            if (iBlending) {
               glEnable(GL_BLEND);
            } else {
               glDisable(GL_BLEND);
            }
         }
      }
   }

   // Platform-dependent blit methods
#ifdef AK_SYSTEM_ANDROID
   static void Crop(int32_t aLeft, int32_t aTop, int32_t aRight, int32_t aBottom);
   static void NativeBlitAndroid(int32_t x, int32_t y, int32_t aWidth, int32_t aHeight, int32_t cropLeft,
      int32_t cropTop, int32_t cropRight, int32_t cropBottom, int32_t orgWidth, int32_t orgHeight);
   static void NativeBlitAndroid(int32_t x, int32_t y, int32_t aWidth, int32_t aHeight);
#else
   static void NativeBlitPosix(float x, float y, float aWidth, float aHeight,
                               float cropLeft, float cropTop, float cropRight, float cropBottom);
#endif

   // Drawing state
   int32_t iWidth;
   int32_t iHeight;
   mutable int32_t iCurrentTexture;
   mutable bool iBlending;

   // Resources state
   TRequiredResources iLoadedResources;
   std::map<TResourceFileId, TTexturePtr> iTextures;
   std::map<TResourceFileId, TFontPtr> iFonts;
};

} // namespace Client

#endif // AK_GRAPHICSCOMPONENT_H_INCLUDED
