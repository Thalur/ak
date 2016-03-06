/**
 * Font class for drawing bitmap-type text with OpenGL.
 */
#ifndef AK_FONT_H_INCLUDED
#define AK_FONT_H_INCLUDED

#include "common/types.h"
#include <string>
#include <memory>
#include <vector>
#include <map>

class CMemoryFile;

enum class EHorizontal : uint8_t {
   LEFT, RIGHT, CENTER
};

enum class EVertical : uint8_t {
   TOP, BOTTOM, CENTER
};

enum class ELines : uint8_t {
   NOCLIP,     // single line, ignore bounding box
   SINGLELINE, // single line, clip
   ELLIPSIS,   // single line, clip with ellipsis
   MULTILINE   // multi line, clip
};

struct TFontStyle
{
   EHorizontal iHorizontal;
   EVertical iVertical;
   ELines iLines;

   TFontStyle(EHorizontal aHorizontal = EHorizontal::LEFT, EVertical aVertical = EVertical::TOP, ELines aLines = ELines::NOCLIP)
    : iHorizontal(aHorizontal), iVertical(aVertical), iLines(aLines)
   {}
};

namespace Client
{

class CGraphicsComponent;
class CResourceManager;
class CFont;
using TFontPtr = std::unique_ptr<CFont>;

class CFont
{
public:
   static TFontPtr FromFile(CMemoryFile& aFile, CResourceManager& aResourceManager, CGraphicsComponent* aGraphics);
   static std::vector<std::string> GetFontVariants(CMemoryFile& aFile);

   void Draw(const std::string& aLine, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
             TFontStyle aStyle, uint32_t aVariant) const;

   int32_t GetCharWidth(int32_t aChar) const
   {
      return GetCharData(aChar).iWidth;
   }
private:
   struct TCharData {
      uint16_t iX;
      uint16_t iY;
      uint16_t iWidth;
      uint16_t iHeight;
   };
   using TCharMap = std::map<uint32_t, TCharData>;

   CFont(CGraphicsComponent* aGraphicsComponent, std::vector<uint32_t>&& aFontVariants, TCharMap&& aChars)
    : iGraphics(aGraphicsComponent), iFontVariants(std::move(aFontVariants)), iChars(std::move(aChars))
   {}

   void DrawLine(const char* aChars, TSize aStartIndex, TSize aEndIndex, int32_t x, int32_t y, int32_t aVariant) const;

   const TCharData& GetCharData(uint32_t aChar) const {
      auto it = iChars.find(aChar);
      if (it != iChars.end()) {
         return it->second;
      }
      return iChars.rbegin()->second;
   }

   CGraphicsComponent* iGraphics;
   std::vector<uint32_t> iFontVariants;
   TCharMap iChars;
};

} // namespace Client

#endif // AK_FONT_H_INCLUDED
