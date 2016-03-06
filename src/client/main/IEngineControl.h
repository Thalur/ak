/**
 * Interface for the game classes to control the engine.
 */
#ifndef AK_IENGINECONTROL_H_INCLUDED
#define AK_IENGINECONTROL_H_INCLUDED

#include "ResourceManager.h"

namespace Client
{

class IEngineControl
{
public:

   /// Graphics control

   virtual void Draw(TResourceFileId aTexture, int32_t x, int32_t y) = 0;
   virtual void Draw(TResourceFileId aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight) = 0;
   virtual void Draw(TResourceFileId aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
                     int32_t aTexLeft, int32_t aTexTop, int32_t aTexRight, int32_t aTexBottom) = 0;
};

} // namespace Client

#endif AK_IENGINECONTROL_H_INCLUDED
