/**
 * Font class for drawing bitmap-type text with OpenGL.
 */
#ifndef AK_FONT_H_INCLUDED
#define AK_FONT_H_INCLUDED

#include "common/types.h"
#include "common/log/log.h"
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
             TFontStyle aStyle, uint32_t aVariant) const {
      DrawScaled(aLine, x, y, aWidth, aHeight, aStyle, 1, aVariant);
   }
   void DrawScaled(const std::string& aLine, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
                   TFontStyle aStyle, int32_t aScale, uint32_t aVariant) const;

   int32_t GetCharWidth(uint32_t aChar) const
   {
      return GetCharData(aChar).iWidth * iDefaultScaling;
   }
private:
   struct TCharData {
      uint16_t iX;
      uint16_t iY;
      uint16_t iWidth;
      uint16_t iHeight;
   };
   using TCharMap = std::map<uint32_t, TCharData>;
   //using TSymbols = std::vector<uint32_t>; // For UTF-8 support

   CFont(CGraphicsComponent* aGraphicsComponent, std::vector<uint32_t>&& aFontVariants, TCharMap&& aChars,
         uint32_t aDefaultChar, int16_t aDefaultScaling, int16_t aHorizontalDistance, int16_t aVerticalDistance)
    : iGraphics(aGraphicsComponent), iFontVariants(std::move(aFontVariants)), iChars(std::move(aChars))
    , iDefaultChar(aDefaultChar), iDefaultScaling(aDefaultScaling)
    , iHorizontalDistance(aHorizontalDistance), iVerticalDistance(aVerticalDistance)
   {}

   CFont(const CFont&) = delete;
   CFont& operator=(const CFont&) = delete;

   static TCharData ReadCharData(CMemoryFile& aFile);
   //static TSymbols ParseUTF8String(const char* aChars, TSize length);

   void DrawMultiline(TString aText, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
                      TFontStyle aStyle, int32_t aVariant, int32_t aScale) const;

   void DrawLine(TString aText, int32_t x, int32_t y, int32_t aVariant, int32_t aScale) const;

   const TCharData& GetCharData(uint32_t aChar) const {
      const auto it = iChars.find(aChar);
      if (it != iChars.end()) {
         return it->second;
      }
      LOG_WARN("Char not found in font table: %u", aChar);
      const auto def = iChars.find(iDefaultChar);
      if (def != iChars.end()) {
         return def->second;
      }
      return iChars.rbegin()->second;
   }

   const CGraphicsComponent* iGraphics;
   const std::vector<uint32_t> iFontVariants;
   const TCharMap iChars;
   const uint32_t iDefaultChar;
   const int16_t iDefaultScaling;
   const int16_t iHorizontalDistance;
   const int16_t iVerticalDistance;
};

} // namespace Client

#endif // AK_FONT_H_INCLUDED
