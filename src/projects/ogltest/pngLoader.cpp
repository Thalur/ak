/**
 *
 */
#include "pngLoader.h"
#include "common/log/log.h"
#include "png.h"
#include <cstring>

#ifdef AK_SYSTEM_ANDROID
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#elif defined AK_SYSTEM_OSX
#include "OpenGL/gl.h"
#include "OpenGL/glext.h"
#else
#define FREEGLUT_LIB_PRAGMAS 0
#include "GL/glew.h"
#include "GL/freeglut.h"
#endif


namespace {

const TSize PNGSIGSIZE = 8;
TFileData dataBuffer;

struct TInputFile
{
   TInputFile(CFile* aFile, TSize aPos) : iFile(aFile), iPos(aPos) {}

   CFile* iFile; // not owned
   TSize iPos;
};

bool ValidateFileHeader(TFilePtr& aFile)
{
   // Let LibPNG check the sig. If this function returns 0, everything is OK.
   if ((aFile->Size() >= PNGSIGSIZE) && aFile->Read(dataBuffer, 0, PNGSIGSIZE)) {
      return png_sig_cmp((png_byte*)&dataBuffer[0], 0, PNGSIGSIZE) == 0;
   }
   return false;
}

void userReadData(png_structp aPngPtr, png_bytep aData, png_size_t aLength)
{
   TInputFile* file = (TInputFile*)png_get_io_ptr(aPngPtr);
   if (file->iPos + aLength > file->iFile->Size()) {
      LOG_ERROR("Unexpected file ending - file size %" PRIuS ", expected at least %" PRIuS, file->iFile->Size(), file->iPos + aLength);
   } else if (file->iFile->Read(dataBuffer, file->iPos, aLength)) {
      file->iPos += aLength;
      std::memcpy(aData, &dataBuffer[0], aLength);
   } else {
      LOG_ERROR("Could not read from file");
   }
}

// Return color type and format
/*std::pair<GLenum, GLint> GetTextureType(int aColorType)
{
   switch (aColorType)
   {
   case PNG_COLOR_TYPE_GRAY:
      return std::make_pair(GL_LUMINANCE, 1);
   case PNG_COLOR_TYPE_GRAY_ALPHA:
      return std::make_pair(GL_LUMINANCE_ALPHA, 2);
   case PNG_COLOR_TYPE_RGB:
      return std::make_pair(GL_RGB, 3);
   case PNG_COLOR_TYPE_RGB_ALPHA:
      return std::make_pair(GL_RGBA, 4);
   default:
      LOG_ERROR("Invalid color type: %d", aColorType);
      return std::make_pair(0, 0);
   }
}*/

int GetTextureType(int aColorType)
{
   switch (aColorType)
   {
   case PNG_COLOR_TYPE_GRAY:
      return GL_LUMINANCE;
   case PNG_COLOR_TYPE_GRAY_ALPHA:
      return GL_LUMINANCE_ALPHA;
   case PNG_COLOR_TYPE_RGB:
      return GL_RGB;
   case PNG_COLOR_TYPE_RGB_ALPHA:
      return GL_RGBA;
   default:
      LOG_ERROR("Invalid color type: %d", aColorType);
      return 0;
   }
}

uint16_t Get2Fold(const uint16_t aFold)
{
   uint16_t ret = 2;
   while (ret < aFold) {
      ret *= 2;
   }
   return ret;
}

} // anonymous namespace

TTexturePtr CTexture::LoadFromMemory(TFilePtr& aFile, const std::string& aLogdata)
{
   LOG_PARAMS(aLogdata.c_str());
   if (!ValidateFileHeader(aFile)) {
      LOG_ERROR("Could not load image %s: Invalid file header", aLogdata.c_str());
      return TTexturePtr();
   }
   png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (pngPtr == nullptr) {
      LOG_ERROR("Could not initialize png read struct for %s", aLogdata.c_str());
      return TTexturePtr();
   }
   TTexturePtr result = LoadPNG(aFile, aLogdata, pngPtr);
   png_destroy_read_struct(&pngPtr, nullptr, nullptr);
   return result;
}

const char* CTexture::GetGLErrorText(int32_t aErrorCode)
{
   switch (aErrorCode) {
   case GL_NO_ERROR:
      return "GL_NO_ERROR";
   case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";
   case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";
   case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";
   //case GL_INVALID_FRAMEBUFFER_OPERATION: // not in GL1
   //   return "GL_INVALID_FRAMEBUFFER_OPERATION";
   case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";
   case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";
   case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";
   default:
      return "(unknown error)";
   }
}

TTexturePtr CTexture::LoadPNG(TFilePtr& aFile, const std::string& aLogdata, void* aPngPtr)
{
   LOG_PARAMS(aLogdata.c_str());
   png_structp pngPtr = (png_structp)aPngPtr;
   png_infop infoPtr = png_create_info_struct(pngPtr);
   if (infoPtr == nullptr) {
      LOG_ERROR("Could not initialize png info struct for %s", aLogdata.c_str());
      return TTexturePtr();
   }

   TInputFile file(&(*aFile), 8); // first 8 bytes were the file header
   png_bytep* rowPtrs = nullptr;
   char* data = nullptr;
   if (setjmp(png_jmpbuf(pngPtr))) {
      LOG_ERROR("An error occurred while reading PNG file %s", aLogdata.c_str());
      if (rowPtrs != nullptr) delete[] rowPtrs;
      if (data != nullptr) delete[] data;
      return TTexturePtr();
   }

   // Simply execute all commands, it will jump to the error code on error
   png_set_read_fn(pngPtr, (png_voidp)&file, userReadData);
   png_set_sig_bytes(pngPtr, PNGSIGSIZE);
   png_read_info(pngPtr, infoPtr);
   int bitdepth = png_get_bit_depth(pngPtr, infoPtr);
   int color_type = png_get_color_type(pngPtr, infoPtr);
   png_uint_32 channels = png_get_channels(pngPtr, infoPtr);
   if (color_type == PNG_COLOR_TYPE_PALETTE) {
      png_set_palette_to_rgb(pngPtr);
      channels = 3;
   }
   if (color_type == PNG_COLOR_TYPE_GRAY && bitdepth < 8) {
      png_set_expand_gray_1_2_4_to_8(pngPtr);
   }
   if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) {
      png_set_tRNS_to_alpha(pngPtr);
      channels++;
   }
   if (bitdepth == 16) {
      png_set_strip_16(pngPtr);
   } else if (bitdepth < 8) {
      png_set_packing(pngPtr);
   }
   png_read_update_info(pngPtr, infoPtr);
   png_uint_32 w=0, h=0;
   png_get_IHDR(pngPtr, infoPtr, &w, &h, &bitdepth, &color_type, NULL, NULL, NULL);   

   // Allocate the buffers and read the data
   rowPtrs = new png_bytep[h];
   data = new char[w * h * bitdepth * channels / 8];
   const uint32_t stride = w * bitdepth * channels / 8;
   for (png_uint_32 i = 0; i < h; i++) {
      png_uint_32 q = (h - i - 1) * stride;
      rowPtrs[i] = (png_bytep)data + q;
   }
   png_read_image(pngPtr, rowPtrs);
   png_read_end(pngPtr, nullptr);
   delete[] rowPtrs;

   // And send it over to OpenGL
   TTexturePtr texture = CreateTexture(data, w, h, GetTextureType(color_type));
   //LOG_DEBUG("Data: %d %d %d %d %d %d %d %d", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
   delete[] data;
   LOG_DEBUG("PNG file loaded: %s (%ux%u, bpp %d, type %d)", aLogdata.c_str(), w, h, bitdepth, GetTextureType(color_type));
   return texture;
}

/**
 * Create a OpenGL texture from the given image data and return the id of the new texture.
 * Due to early Android version requirements, the underlying texture always has dimensions
 * of powers of two.
 */
TTexturePtr CTexture::CreateTexture(char* aData, uint16_t aWidth, uint16_t aHeight, int aType)
{
   LOG_PARAMS("aWidth=%d, aHeight=%d", aWidth, aHeight);
   GLuint id(0);
   glGenTextures(1, &id);
   CHECK_GL_ERROR("glGenTextures");
   glBindTexture(GL_TEXTURE_2D, id);
   CHECK_GL_ERROR("glBindTexture");
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   CHECK_GL_ERROR("glTexParameteri@minFilter");
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   CHECK_GL_ERROR("glTexParameteri@magFilter");

   uint16_t texWidth = Get2Fold(aWidth);
   uint16_t texHeight = Get2Fold(aHeight);
   //gluBuild2DMipmaps (GL_TEXTURE_2D, aType.second, aWidth, aHeight, aType.first, GL_UNSIGNED_BYTE, aData);
   //LOG_DEBUG("Calling glTexImage2D: %d x %d, type %d", texWidth, texHeight, aType);
   //glTexImage2D(GL_TEXTURE_2D, 0, aType.second, aWidth, aHeight, 0, aType.first, GL_UNSIGNED_BYTE, aData);
   glTexImage2D(GL_TEXTURE_2D, 0, aType, texWidth, texHeight, 0, aType, GL_UNSIGNED_BYTE, aData);
   CHECK_GL_ERROR("glTexImage2D");

#ifdef AK_SYSTEM_ANDROID
   // Cropping is required only on Android
   int32_t crop[4];
   crop[0] = 0;
   crop[1] = 0;
   crop[2] = texWidth; // width
   crop[3] = texHeight; // -height
   glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);
   CHECK_GL_ERROR("glTexParameteriv@textureCrop");
#endif
   return TTexturePtr(new CTexture(id, texWidth, texHeight, aWidth, aHeight));
}

