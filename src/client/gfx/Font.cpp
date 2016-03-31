/**
 *
 */
#include "Font.h"
#include "client/gfx/GraphicsComponent.h"
#include "client/main/ResourceManager.h"
#include "common/cabinet/MemoryFile.h"
#include "common/log/log.h"
#include <tuple>
#include <limits>


namespace Client
{

TFontPtr CFont::FromFile(CMemoryFile &aFile, CResourceManager &aResourceManager, CGraphicsComponent *aGraphics)
{
   LOG_METHOD();
   std::vector<std::string> variants = GetFontVariants(aFile);
   if (!variants.empty()) {
      std::vector<uint32_t> variantIds;
      for (const std::string& variant : variants) {
         variantIds.emplace_back(aResourceManager.GetResourceId(variant));
      }
      const std::string startCharLine = aFile.GetNextConfigLine();
      const std::string numCharsLine = aFile.GetNextConfigLine();
      const std::string defaultSymbolLine = aFile.GetNextConfigLine();
      const std::string numExtraLine = aFile.GetNextConfigLine();
      const std::string defaultScalingLine = aFile.GetNextConfigLine();
      const std::string horizontalSpaceLine = aFile.GetNextConfigLine();
      const std::string verticalSpaceLine = aFile.GetNextConfigLine();
      if (!verticalSpaceLine.empty()) {
         const int32_t startChar = std::atoi(startCharLine.c_str());
         ASSERT(startChar >= 0 && startChar < 255);
         const int32_t numChars = std::atoi(numCharsLine.c_str());
         ASSERT(numChars >= 0 && numChars < 1000000);
         const int32_t defaultSymbol = std::atoi(defaultSymbolLine.c_str());
         ASSERT(defaultSymbol >= 0);
         const int32_t numExtra = std::atoi(numExtraLine.c_str());
         ASSERT(numExtra >= 0);
         const int32_t defaultScaling = std::atoi(defaultScalingLine.c_str());
         ASSERT(defaultScaling > 0 && defaultScaling <= 100);
         const int32_t horizontalSpace = std::atoi(horizontalSpaceLine.c_str());
         ASSERT(horizontalSpace >= -100 && horizontalSpace <= 100);
         const int32_t verticalSpace = std::atoi(verticalSpaceLine.c_str());
         ASSERT(verticalSpace >= -100 && verticalSpace <= 100);
         TCharMap chars;
         for (int i = 0; i < numChars; i++) {
            chars[startChar + i] = ReadCharData(aFile);
         }
         for (int i = 0; i < numExtra; i++) {
            const std::string charIdLine = aFile.GetNextConfigLine();
            if (charIdLine.empty()) break;
            const int32_t charId = std::atoi(charIdLine.c_str());
            ASSERT(charId >= 0);
            ASSERT(chars.find(charId) == chars.end());
            chars[charId] = ReadCharData(aFile);
         }
         ASSERT(numChars + numExtra == chars.size());
         return TFontPtr(new CFont(aGraphics, std::move(variantIds), std::move(chars),
                                   defaultSymbol, static_cast<int16_t>(defaultScaling),
                                   static_cast<int16_t>(horizontalSpace),
                                   static_cast<int16_t>(verticalSpace)));
      }
   }
   LOG_ERROR("Font specification file truncated");
   return TFontPtr();
}

std::vector<std::string> CFont::GetFontVariants(CMemoryFile &aFile)
{
   LOG_METHOD();
   std::vector<std::string> result;
   const std::string headerLine = aFile.GetNextConfigLine();
   if (headerLine == "AKFONTDESC1") {
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
      LOG_ERROR("Invalid font identifier: %s", headerLine.c_str());
   }
   return result;
}

CFont::TCharData CFont::ReadCharData(CMemoryFile &aFile)
{
   const std::string xLine = aFile.GetNextConfigLine();
   const std::string yLine = aFile.GetNextConfigLine();
   const std::string widthLine = aFile.GetNextConfigLine();
   const std::string heightLine = aFile.GetNextConfigLine();
   if (heightLine.empty()) {
      LOG_ERROR("Font file truncated.");
      return TCharData();
   }
   const int32_t x = std::atoi(xLine.c_str());
   ASSERT(x >= 0 && x < std::numeric_limits<uint16_t>::max());
   const int32_t y = std::atoi(yLine.c_str());
   ASSERT(y >= 0 && y < std::numeric_limits<uint16_t>::max());
   const int32_t width = std::atoi(widthLine.c_str());
   ASSERT(width > 0 && width < std::numeric_limits<uint16_t>::max());
   const int32_t height = std::atoi(heightLine.c_str());
   ASSERT(height > 0 && height < std::numeric_limits<uint16_t>::max());
   return TCharData {
      static_cast<uint16_t>(x), static_cast<uint16_t>(y), static_cast<uint16_t>(width), static_cast<uint16_t>(height) };
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
      return (2 * y + aHeight - aOverallHeight) / 2;
   }
}

inline int32_t GetXPos(const EHorizontal aHorizontal, const int32_t x, const int32_t aWidth, const int32_t aOverallWidth)
{
   if (aHorizontal == EHorizontal::LEFT) {
      return x;
   } else if (aHorizontal == EHorizontal::RIGHT) {
      return x + aWidth - aOverallWidth;
   } else {
      return (2 * x + aWidth - aOverallWidth) / 2;
   }
}

inline bool IsWhitespace(const uint32_t aChar)
{
   return (aChar == ' ') || (aChar == '\t') || (aChar == '\n');
}

inline bool IsLineBreakChar(const uint32_t aChar)
{
   return (aChar == '-') || (aChar == '.') || (aChar == ',') || (aChar == ';');
}

} // anonymous namespace


void CFont::DrawScaled(const std::string &aLine, const int32_t x, const int32_t y,
                       const int32_t aWidth, const int32_t aHeight, const TFontStyle aStyle,
                       const int32_t aScale, const uint32_t aVariant) const
{
   if (aLine.empty()) return; // empty string
   TString text { aLine };
   ASSERT(aVariant < iFontVariants.size());

   if (aStyle.iLines == ELines::MULTILINE) {
      DrawMultiline(text, x, y, aWidth, aHeight, aStyle, aVariant, aScale);
   } else { // Single line drawing
      const int32_t scale = aScale * iDefaultScaling;
      int32_t overallLength = 0;
      int32_t overallHeight = 0;
      for (auto c : text) {
         const TCharData& data = GetCharData(c);
         overallLength += data.iWidth * scale + iHorizontalDistance * aScale;
         if (data.iHeight * scale + iVerticalDistance * aScale > overallHeight) {
            overallHeight = data.iHeight * scale + iVerticalDistance * aScale;
         }
      }
      overallLength -= iHorizontalDistance * aScale;
      overallHeight -= iVerticalDistance * aScale;

      switch (aStyle.iLines) {
      case ELines::NOCLIP: // Just draw the whole string without any checks
         DrawLine(text, GetXPos(aStyle.iHorizontal, x, aWidth, overallLength),
                  GetYPos(aStyle.iVertical, y, aHeight, overallHeight), aVariant, aScale);
         break;
      case ELines::SINGLELINE:
      {
         TStringIt startIt { text.begin() }, endIt { text.end() };
         while ((overallLength > aWidth) && startIt && endIt.rvalid()) {
            if (aStyle.iHorizontal != EHorizontal::RIGHT) {
               overallLength -= GetCharData(*--endIt).iWidth * scale + iHorizontalDistance * aScale;
            } else {
               overallLength -= GetCharData(*startIt++).iWidth * scale + iHorizontalDistance * aScale;
            }
         }
         if (startIt.bytePos() < endIt.bytePos()) {
            DrawLine(TString(aLine, startIt.bytePos(), endIt.bytePos()), GetXPos(aStyle.iHorizontal, x, aWidth, overallLength),
                     GetYPos(aStyle.iVertical, y, aHeight, overallHeight), aVariant, aScale);
         }
         break;
      }
      case ELines::ELLIPSIS:
         TStringIt endIt { text.end() };
         int32_t numEll = 0;
         const int32_t ellLength = GetCharData('.').iWidth * scale + iHorizontalDistance * aScale;
         if (overallLength > aWidth) {
            numEll = 3;
            while (overallLength + numEll * ellLength > aWidth && endIt.rvalid()) {
               overallLength -= GetCharData(*--endIt).iWidth * scale + iHorizontalDistance * aScale;
            }
            if ((endIt.pos() == 0) && (numEll * ellLength - 1 > aWidth)) {
               overallLength = 0;
               numEll = aWidth / ellLength;
            }
         }
         const int32_t xPos = GetXPos(aStyle.iHorizontal, x, aWidth, overallLength + numEll * ellLength);
         const int32_t yPos = GetYPos(aStyle.iVertical, y, aHeight, overallHeight);
         if (endIt.pos() > 0) {
            DrawLine(TString(aLine, 0, endIt.bytePos()), xPos, yPos, aVariant, aScale);
         }
         if (numEll > 0) {
            DrawLine(TString("...", numEll), xPos + overallLength + iHorizontalDistance * aScale,
                     yPos, aVariant, aScale);
         }
         break;
      }
   }
}

void CFont::DrawMultiline(TString aText, const int32_t x, const int32_t y,
                          const int32_t aWidth, const int32_t aHeight, const TFontStyle aStyle,
                          const int32_t aVariant, const int32_t aScale) const
{
   //LOG_PARAMS("'%s' (%d) x:%d y:%d w:%d h:%d scale: %d", aText.c_str(), aText.size(), x, y, aWidth, aHeight, aScale);
   const int32_t scale = iDefaultScaling * aScale;
   TStringIt it { aText.begin() };
   TSize lineStart = 0, lastWhitespace = 0;
   int32_t overallHeight = 0, lineHeight = 0, lineWidth = 0, widthToLastWhitespace = 0;
   std::vector<std::tuple<TString, int32_t, int32_t>> lines; // <start index, end index, width, height>
   while (it) {
      uint32_t c = *it;
      int32_t nextCharWidth = GetCharData(c).iWidth * scale + iHorizontalDistance * aScale;
      int32_t nextCharHeight = GetCharData(c).iHeight * scale + iVerticalDistance * aScale;
      if (IsWhitespace(c)) {
         lastWhitespace = it.pos();
         widthToLastWhitespace = lineWidth;
      }
      if (lineWidth + nextCharWidth > aWidth) { // line is full
         //LOG_VERBOSE("Adding line from %d to %d, size %dx%d", (int)lineStart, (int)lastWhitespace, widthToLastWhitespace, lineHeight);
         lines.emplace_back(TString(aText, lineStart, lastWhitespace), widthToLastWhitespace, lineHeight);
         it -= (it.pos() - lastWhitespace);
         for (; IsWhitespace(*it); ++it) {} // Skip over spaces after line break
         lineStart = lastWhitespace = it.pos();
         overallHeight += lineHeight;
         lineWidth = lineHeight = 0;
      } else {
         lineWidth += nextCharWidth;
         //LOG_VERBOSE("Adding char %c: %dx%d", c, nextCharWidth, nextCharHeight);
         if (nextCharHeight > lineHeight) {
            lineHeight = nextCharHeight;
            if (overallHeight + lineHeight > aHeight) break; // no space for more lines
         }
         if (IsLineBreakChar(c)) {
            lastWhitespace = it.pos() + 1;
            widthToLastWhitespace = lineWidth;
         }
         ++it;
      }
   }
   if (!lines.empty()) {
      int32_t yPos = GetYPos(aStyle.iVertical, y, aHeight, overallHeight);
      for (const auto& line : lines) {
         DrawLine(std::get<0>(line),
                  GetXPos(aStyle.iHorizontal, x, aWidth, std::get<1>(line)),
                  yPos, aVariant, aScale);
         yPos += std::get<2>(line);
      }
   }
}

void CFont::DrawLine(TString aText, const int32_t x, const int32_t y,
                     const int32_t aVariant, const int32_t aScale) const
{
   const int32_t scale = iDefaultScaling * aScale;
   int32_t pos = x;
   for (auto c : aText) {
      const TCharData data = GetCharData(c);
      iGraphics->Draw(iFontVariants[aVariant], pos, y, data.iWidth * scale, data.iHeight * scale,
                      data.iX, data.iY, data.iX + data.iWidth, data.iY + data.iHeight);
      pos += data.iWidth * scale + iHorizontalDistance * aScale;
   }
}

} // namespace Client
