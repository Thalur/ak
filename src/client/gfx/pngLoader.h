/**
 *
 */
#ifndef AK_PNGLOADER_H_INCLUDED
#define AK_PNGLOADER_H_INCLUDED

#include "Texture.h"
#include "common/cabinet/File.h"


namespace Client
{

TTexturePtr LoadFromMemory(TFilePtr& aFile, const char* aLogdata);

} // namespace Client

#endif // AK_PNGLOADER_H_INCLUDED
