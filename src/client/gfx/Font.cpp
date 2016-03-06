/**
 *
 */
#include "Font.h"
#include "client/gfx/GraphicsComponent.h"
#include "client/main/ResourceManager.h"
#include "common/cabinet/MemoryFile.h"
#include "common/log/log.h"

namespace Client
{

TFontPtr CFont::FromFile(CMemoryFile &aFile, CResourceManager &aResourceManager, CGraphicsComponent *aGraphics)
{
   std::vector<std::string> variants = GetFontVariants(aFile);
   if (!variants.empty()) {
      std::vector<uint32_t> variantIds;
      for (const std::string& variant : variants) {
         variantIds.emplace_back(aResourceManager.GetResourceId(variant));
      }
      const std::string startCharLine = aFile.GetNextConfigLine();
      const std::string numCharsLine = aFile.GetNextConfigLine();
      const std::string numExtraLine = aFile.GetNextConfigLine();
      if (!numExtraLine.empty()) {
         const int32_t startChar = std::atoi(startCharLine.c_str());
         ASSERT(startChar >= 0 && startChar < 255);
         const int32_t numChars = std::atoi(numCharsLine.c_str());
         ASSERT(numChars >= 0 && numChars < 1000000);
         const int32_t numExtra = std::atoi(numExtraLine.c_str());
         ASSERT(numExtra == 0);
         TCharMap chars;
         for (int i = 0; i < numChars; i++) {
            const std::string xLine = aFile.GetNextConfigLine();
            const std::string yLine = aFile.GetNextConfigLine();
            const std::string widthLine = aFile.GetNextConfigLine();
            const std::string heightLine = aFile.GetNextConfigLine();
            if (heightLine.empty()) break;
            const int32_t x = std::atoi(xLine.c_str());
            ASSERT(x >= 0 && x < std::numeric_limits<uint16_t>::max());
            const int32_t y = std::atoi(yLine.c_str());
            ASSERT(y >= 0 && y < std::numeric_limits<uint16_t>::max());
            const int32_t width = std::atoi(widthLine.c_str());
            ASSERT(width > 0 && width < std::numeric_limits<uint16_t>::max());
            const int32_t height = std::atoi(heightLine.c_str());
            ASSERT(height > 0 && height < std::numeric_limits<uint16_t>::max());
            chars[startChar + i] = TCharData {
               static_cast<uint16_t>(x), static_cast<uint16_t>(y), static_cast<uint16_t>(width), static_cast<uint16_t>(height) };
         }
         ASSERT(numChars + numExtra == chars.size());
         return TFontPtr(new CFont(aGraphics, std::move(variantIds), std::move(chars)));
      }
   }
   LOG_ERROR("Font specification file truncated");
   return TFontPtr();
}

std::vector<std::string> CFont::GetFontVariants(CMemoryFile &aFile)
{
   LOG_METHOD();
   std::vector<std::string> result;
   const std::string headeLine = aFile.GetNextConfigLine();
   if (headeLine == "AKFONTDESC1") {
      const std::string numberLine = aFile.GetNextConfigLine();
      if (!numberLine.empty()) {
         const int32_t num = std::atoi(numberLine.c_str());
         if ((num > 0) && (num < 1000)) {
            for (int i = 0; i < num; i++) {
               std::string fileNameLine = aFile.GetNextConfigLine();
               if (fileNameLine.empty()) {
                  LOG_ERROR("Found insufficient font bitmap file entries");
                  break;
               }
               result.emplace_back(std::move(fileNameLine));
            }
         }
      }
      if (result.empty()) {
         LOG_ERROR("Invalid font specification file");
      }
   } else {
      LOG_ERROR("Invalid font identifier: %s", headeLine.c_str());
   }
   return result;
}

namespace
{

inline int32_t GetYPos(const EVertical aVertical, const int32_t y, const int32_t aHeight, const int32_t aOverallHeight)
{
   if (aVertical == EVertical::TOP) {
      return y;
   } else if (aVertical == EVertical::BOTTOM) {
      return y + aHeight - aOverallHeight;
   } else {
      return (2 * y + aHeight) / 2 - aOverallHeight / 2;
   }
}

inline int32_t GetXPos(const EHorizontal aHorizontal, const int32_t x, const int32_t aWidth, const int32_t aOverallWidth)
{
   if (aHorizontal == EHorizontal::LEFT) {
      return x;
   } else if (aHorizontal == EHorizontal::RIGHT) {
      return x + aWidth - aOverallWidth;
   } else {
      return (2 * x + aWidth) / 2 - aOverallWidth / 2;
   }
}

} // anonymous namespace

void CFont::Draw(const std::string &aLine, const int32_t x, const int32_t y, const int32_t aWidth,
                 const int32_t aHeight, const TFontStyle aStyle, const uint32_t aVariant) const
{
   const TSize len = aLine.length();
   if (len == 0) return;
   const char* const chars = aLine.c_str();
   ASSERT(aVariant < iFontVariants.size());

   if (aStyle.iLines == ELines::MULTILINE) {
      LOG_ERROR("Multiline text drawing is NYI");
   } else { // Single line drawing
      int32_t overallLength = 0;
      int32_t overallHeight = 0;
      for (TSize i = 0; i < len; i++) {
         const TCharData& data = GetCharData(chars[i]);
         overallLength += data.iWidth;
         if (data.iHeight > overallHeight) {
            overallHeight = data.iHeight;
         }
      }
      overallHeight *= 2;
      overallLength = overallLength * 2 - len - 1;

      switch (aStyle.iLines) {
      case ELines::NOCLIP:
         DrawLine(chars, 0, len, GetXPos(aStyle.iHorizontal, x, aWidth, overallLength),
                  GetYPos(aStyle.iVertical, y, aHeight, overallHeight), aVariant);
         break;
      case ELines::SINGLELINE:
         //DrawSingleLine(chars, len, x, y, aWidth, aHeight, aStyle, aVariant);
         break;
      case ELines::ELLIPSIS:
         //DrawEllipsis(chars, len, x, y, aWidth, aHeight, aStyle, aVariant);
         break;
      }
   }
}

void CFont::DrawLine(const char* const aChars, const TSize aStartIndex, const TSize aEndIndex,
                     const int32_t x, const int32_t y, const int32_t aVariant) const
{
   LOG_VERBOSE("%s (%d, %d) --> ImageId %d", aChars, aStartIndex, aEndIndex, iFontVariants[aVariant]);
   int32_t pos = x;
   for (TSize i = aStartIndex; i < aEndIndex; i++) {
      const TCharData data = GetCharData(aChars[i]);
      iGraphics->Draw(iFontVariants[aVariant], pos, y, data.iWidth*2, data.iHeight*2,
                      data.iX, data.iY, data.iX + data.iWidth, data.iY + data.iHeight);
      if (i == 0) {
         LOG_DEBUG("Drawing char '%c' (%d) with parameters %u, %d, %d, %u, %u, %u, %u, %u, %u",
                   aChars[i], (int)aChars[i], iFontVariants[aVariant], pos, y, data.iWidth, data.iHeight, data.iX, data.iY, data.iWidth, data.iHeight);
      }
      pos += data.iWidth*2 - 1;
   }
}

} // namespace Client
