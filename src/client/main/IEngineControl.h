/**
 * Interface for the game classes to control the engine.
 */
#ifndef AK_IENGINECONTROL_H_INCLUDED
#define AK_IENGINECONTROL_H_INCLUDED

#include "ResourceManager.h"
#include "client/gfx/Font.h"

namespace Client
{

class IEngineControl
{
public:
   virtual ~IEngineControl() {}

   /// General information

   virtual int32_t Width() = 0;
   virtual int32_t Height() = 0;

   /// Graphics control

   virtual void Draw(TResourceFileId aTexture, int32_t x, int32_t y) = 0;
   virtual void Draw(TResourceFileId aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight) = 0;
   virtual void Draw(TResourceFileId aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
                     int32_t aTexLeft, int32_t aTexTop, int32_t aTexRight, int32_t aTexBottom) = 0;

   virtual void Text(TResourceFileId aFont, const std::string& aLine, int32_t x, int32_t y,
                     TFontStyle aStyle = TFontStyle(), int32_t aScale = 1, uint32_t aVariant = 0) = 0;
   virtual void Text(TResourceFileId aFont, const std::string& aLine, int32_t x, int32_t y,
                     int32_t aWidth, int32_t aHeight, TFontStyle aStyle = TFontStyle(),
                     int32_t aScale = 1, uint32_t aVariant = 0) = 0;
};

} // namespace Client

#endif // AK_IENGINECONTROL_H_INCLUDED
