/**
 *
 */
#include "GraphicsComponent.h"
#include "pngLoader.h"
#include "common/log/log.h"
#include "common/cabinet/CabManager.h"


namespace Client
{

CGraphicsComponent::CGraphicsComponent(CCabManager& aCabinets, int32_t aWidth, int32_t aHeight)
 : iCabinets(aCabinets), iWidth(aWidth), iHeight(aHeight), iCurrentTexture(-1), iBlending(false)
{
   LOG_METHOD();
}

CGraphicsComponent::~CGraphicsComponent()
{
   LOG_METHOD();
}

void CGraphicsComponent::InitOpenGL()
{
   LOG_METHOD();

   // Initialize OpenGL drawing parameters
   glEnable(GL_TEXTURE_2D);
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glViewport(0, 0, iWidth, iHeight);
   glMatrixMode(GL_MODELVIEW);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
#ifdef AK_SYSTEM_ANDROID
   glOrthof(0, iWidth, iHeight, 0, 1, -1);
#else // For some reason, glOrthof() crashes on Windows
   glOrtho(0, iWidth, iHeight, 0, 1, -1);
#endif
   glMatrixMode(GL_MODELVIEW);
}

void CGraphicsComponent::LoadGraphics(const TFileList &aFiles)
{
   LOG_METHOD();
   for (const auto& file : aFiles) {
      if (iTextures.find(file.first) == iTextures.end()) { // Texture not yet loaded
         LoadTexture(file.first, file.second);
      }
   }
}

const CTexture* CGraphicsComponent::LoadTexture(const TResourceFileId aId, const std::string& aFilename)
{
   TFilePtr memFile = iCabinets.GetFile(aId);
   if (!memFile) { // Try loading by filename if by index did not work
      memFile = iCabinets.GetFile(aFilename);
   }
   if (memFile) {
      TTexturePtr texture = LoadFromMemory(memFile, aFilename.c_str());
      if (texture) {
         iTextures[aId] = std::move(texture);
         return &*iTextures[aId];
      }
   }
   iTextures[aId] = TTexturePtr(); // Store the failure to load
   return nullptr;
}

const CTexture* CGraphicsComponent::LoadLazy(TResourceFileId aId)
{
   LOG_PARAMS("Id: %u", aId);
   std::string filename { "" };
   return LoadTexture(aId, filename);
}

void CGraphicsComponent::LoadFonts(const TFileList &aFiles, CResourceManager& aResourceManager)
{
   LOG_METHOD();
   for (const auto& file : aFiles) {
      if (iFonts.find(file.first) == iFonts.end()) { // Font not yet loaded
         TMemoryFilePtr memFile = iCabinets.GetFile(file.first);
         if (memFile) {
            TFontPtr font = CFont::FromFile(*memFile, aResourceManager, this);
            if (font) {
               iFonts[file.first] = std::move(font);
            }
         }
      }
   }
}


#ifdef AK_SYSTEM_ANDROID
void CGraphicsComponent::Crop(int32_t aLeft, int32_t aTop, int32_t aRight, int32_t aBottom)
{
   // Set the cropping rectangle to only draw part of the source bitmap
   const int32_t crop[4] { aLeft, aTop, aRight - aLeft, aBottom - aTop };
   glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);
   if (glGetError() != 0) LOG_ERROR("Error!");
}
void CGraphicsComponent::NativeBlitAndroid(int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
    int32_t cropLeft, int32_t cropTop, int32_t cropRight, int32_t cropBottom, int32_t orgWidth, int32_t orgHeight)
{
   const bool bCrop = (cropLeft != 0) || (cropTop != 0) || (cropRight != orgWidth) || (cropBottom != orgHeight);
   if (bCrop) {
      Crop(cropLeft, cropTop, cropRight, cropBottom);
   }
   glDrawTexiOES(x, y, 0, aWidth, aHeight);
   if (glGetError() != 0) LOG_ERROR("Error!");
   if (bCrop) {
      Crop(0, 0, orgWidth, orgHeight);
   }
}
void CGraphicsComponent::NativeBlitAndroid(int32_t x, int32_t y, int32_t aWidth, int32_t aHeight)
{
   glDrawTexiOES(x, y, 0, aWidth, aHeight);
   if (glGetError() != 0) LOG_ERROR("Error!");
}
#else
void CGraphicsComponent::NativeBlitPosix(float x, float y, float aWidth, float aHeight,
                                         float cropLeft, float cropTop, float cropRight, float cropBottom)
{
   const float right = x + aWidth;
   const float bottom = y + aHeight;
   glBegin(GL_QUADS);
   glTexCoord2f(cropLeft, cropBottom);
   glVertex2f(x, y);
   glTexCoord2f(cropRight, cropBottom);
   glVertex2f(right, y);
   glTexCoord2f(cropRight, cropTop);
   glVertex2f(right, bottom);
   glTexCoord2f(cropLeft, cropTop);
   glVertex2f(x, bottom);
   glEnd();
}
#endif

} // namespace Client
