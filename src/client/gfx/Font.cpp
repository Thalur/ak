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
#include <cstdlib>

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

// UTF-8 definition from http://www.ietf.org/rfc/rfc3629.txt
// More checks for invalid characters should be added.
CFont::TSymbols CFont::ParseUTF8String(const char* const aChars, const TSize length)
{
   TSymbols symbols;
   for (TSize pos = 0; pos < length; pos++) {
      const uint32_t byte1 = aChars[pos];
      if ((byte1 & 0x80) == 0) { // simple ASCII char
         symbols.emplace_back(byte1);
      } else if ((byte1 & 0x40) != 0) { // First byte is valid
         if (++pos < length) { // At least two bytes
            const uint32_t byte2 = aChars[pos];
            if (((byte2 & 0xC0) == 0x80)) { // Second byte is valid
               if ((byte1 & 0x20) == 0) { // 2-byte char
                  symbols.emplace_back(((byte1 & 0x1F) << 6) | (byte2 & 0x3F));
               } else if (++pos < length) { // At least 3 bytes
                  const uint32_t byte3 = aChars[pos];
                  if ((byte3 & 0xC0) == 0x80) { // Third byte is valid
                     if ((byte1 & 0x10) == 0) { // 3-byte char
                        symbols.emplace_back(((byte1 & 0xF) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F));
                     } else if (++pos < length) { // 4 bytes
                        const uint32_t byte4 = aChars[pos];
                        if (((byte1 & 0x8) == 0) && ((byte4 & 0xC0) == 0x80)) { // Forth byte is valid
                           symbols.emplace_back(((byte1 & 0x7) << 18) | ((byte2 & 0x3F) << 12)
                                                | ((byte3 & 0x3F) << 6) | (byte4 & 0x3F));
                        } else {
                           LOG_WARN("Invalid UTF-8 sequence: %X %X %X %X", byte1, byte2, byte3, byte4);
                        }
                     } else {
                        LOG_WARN("Truncated UTF-8 string: %X %X %X", byte1, byte2, byte3);
                     }
                  } else {
                     LOG_WARN("Invalid UTF-8 sequence: %X %X %X", byte1, byte2, byte3);
                  }
               } else {
                  LOG_WARN("Truncated UTF-8 string: %X %X", byte1, byte2);
               }
            } else {
               LOG_WARN("Invalid UTF-8 sequence: %X %X", byte1, byte2);
            }
         } else {
            LOG_WARN("Truncated UTF-8 string: %X", byte1);
         }
      } else {
         LOG_WARN("Invalid UTF-8 sequence start: %X", byte1);
      }
   }
   return symbols;
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

} // anonymous namespace

void CFont::DrawScaled(const std::string &aLine, const int32_t x, const int32_t y,
                       const int32_t aWidth, const int32_t aHeight, const TFontStyle aStyle,
                       const int32_t aScale, const uint32_t aVariant) const
{
   const TSymbols chars = ParseUTF8String(aLine.c_str(), aLine.length());
   const TSize len = chars.size();
   if (len == 0) return;
   ASSERT(aVariant < iFontVariants.size());
   const int32_t scale = aScale * iDefaultScaling;

   if (aStyle.iLines == ELines::MULTILINE) {
      DrawMultiline(chars, len, x, y, aWidth, aHeight, aStyle, aVariant, aScale);
   } else { // Single line drawing
      int32_t overallLength = 0;
      int32_t overallHeight = 0;
      for (TSize i = 0; i < len; i++) {
         const TCharData& data = GetCharData(chars[i]);
         overallLength += data.iWidth * scale + iHorizontalDistance * aScale;
         if (data.iHeight * scale + iVerticalDistance * aScale > overallHeight) {
            overallHeight = data.iHeight * scale + iVerticalDistance * aScale;
         }
      }
      overallLength -= iHorizontalDistance * aScale;
      overallHeight -= iVerticalDistance * aScale;

      switch (aStyle.iLines) {
      case ELines::NOCLIP:
         DrawLine(chars, 0, len, GetXPos(aStyle.iHorizontal, x, aWidth, overallLength),
                  GetYPos(aStyle.iVertical, y, aHeight, overallHeight), aVariant, aScale);
         break;
      case ELines::SINGLELINE:
      {
         TSize startIndex = 0, endIndex = len;
         while ((overallLength > aWidth) && (startIndex < endIndex)) {
            if (aStyle.iHorizontal != EHorizontal::RIGHT) {
               overallLength -= GetCharData(chars[--endIndex]).iWidth * scale + iHorizontalDistance * aScale;
            }
            if (aStyle.iHorizontal == EHorizontal::LEFT || ((aStyle.iHorizontal == EHorizontal::CENTER)
                && (startIndex < endIndex) && (overallLength > aWidth))) {
               overallLength -= GetCharData(chars[startIndex++]).iWidth * scale + iHorizontalDistance * aScale;
            }
         }
         if (startIndex < endIndex) {
            DrawLine(chars, startIndex, endIndex, GetXPos(aStyle.iHorizontal, x, aWidth, overallLength),
                     GetYPos(aStyle.iVertical, y, aHeight, overallHeight), aVariant, aScale);
         }
         break;
      }
      case ELines::ELLIPSIS:
         TSize endIndex = len;
         int32_t numEll = 0;
         const int32_t ellLength = GetCharData('.').iWidth * scale + iHorizontalDistance * aScale;
         if (overallLength > aWidth) {
            numEll = 3;
            while (overallLength + numEll * ellLength > aWidth && endIndex > 0) {
               overallLength -= GetCharData(chars[--endIndex]).iWidth * scale + iHorizontalDistance * aScale;
            }
            if ((endIndex == 0) && (numEll * ellLength - 1 > aWidth)) {
               overallLength = 0;
               numEll = aWidth / ellLength;
            }
         }
         const int32_t xPos = GetXPos(aStyle.iHorizontal, x, aWidth, overallLength + numEll * ellLength);
         const int32_t yPos = GetYPos(aStyle.iVertical, y, aHeight, overallHeight);
         if (endIndex > 0) {
            DrawLine(chars, 0, endIndex, xPos, yPos, aVariant, aScale);
         }
         if (numEll > 0) {
            DrawLine(ParseUTF8String("...", numEll), 0, numEll,
                     xPos + overallLength + iHorizontalDistance * aScale, yPos, aVariant, aScale);
         }
         break;
      }
   }
}

void CFont::DrawMultiline(const TSymbols aChars, const TSize aLength, const int32_t x, const int32_t y,
                          const int32_t aWidth, const int32_t aHeight, const TFontStyle aStyle,
                          const int32_t aVariant, const int32_t aScale) const
{
   //LOG_PARAMS("'%s' (%d) x:%d y:%d w:%d h:%d scale: %d", aChars, (int)aLength, x, y, aWidth, aHeight, aScale);
   const int32_t scale = iDefaultScaling * aScale;
   TSize lineStart = 0, pos = 0, lastWhitespace = 0;
   int32_t overallHeight = 0, lineHeight = 0, lineWidth = 0, widthToLastWhitespace = 0;
   int32_t nextCharWidth = 0, nextCharHeight = 0;
   std::vector<std::tuple<TSize, TSize, int32_t, int32_t>> lines; // <start index, end index, width, height>
   while (pos <= aLength) {
      if (pos < aLength) { // Get dimensions for the next character
         nextCharWidth = GetCharData(aChars[pos]).iWidth * scale + iHorizontalDistance * aScale;
         nextCharHeight = GetCharData(aChars[pos]).iHeight * scale + iVerticalDistance * aScale;
      }
      if ((pos == aLength) || (aChars[pos] == ' ')) { // can break here
         lastWhitespace = pos;
         widthToLastWhitespace = lineWidth;
      }
      if ((pos == aLength) || (lineWidth + nextCharWidth > aWidth)) { // line is full
         if (pos == lineStart) break; // not enough space for one char, or finished
         if (lastWhitespace == lineStart) { // no space found, break word
            lastWhitespace = pos;
            widthToLastWhitespace = lineWidth;
         }
         //LOG_VERBOSE("Adding line from %d to %d, size %dx%d", (int)lineStart, (int)lastWhitespace, widthToLastWhitespace, lineHeight);
         lines.emplace_back(lineStart, lastWhitespace, widthToLastWhitespace, lineHeight);
         pos = lastWhitespace;
         while ((pos < aLength) && (aChars[pos] == ' ')) pos++; // Skip over spaces after line break
         if (pos == aLength) break;
         lineStart = lastWhitespace = pos;
         overallHeight += lineHeight;
         lineWidth = lineHeight = 0;
      } else { // Add current symbol to the current line
         lineWidth += nextCharWidth;
         //LOG_VERBOSE("Adding char %c: %dx%d", aChars[pos], nextCharWidth, nextCharHeight);
         if (nextCharHeight > lineHeight) {
            lineHeight = nextCharHeight;
            if (overallHeight + lineHeight > aHeight) break; // no space for more lines
         }
         if ((aChars[pos] == '-') || (aChars[pos] == '.') || (aChars[pos] == ',') || (aChars[pos] == ';')) {
            lastWhitespace = pos+1;
            widthToLastWhitespace = lineWidth;
         }
         pos++;
      }
   }
   if (!lines.empty()) {
      int32_t yPos = GetYPos(aStyle.iVertical, y, aHeight, overallHeight);
      for (const auto& line : lines) {
         DrawLine(aChars, std::get<0>(line), std::get<1>(line),
                  GetXPos(aStyle.iHorizontal, x, aWidth, std::get<2>(line)),
                  yPos, aVariant, aScale);
         yPos += std::get<3>(line);
      }
   }
}

void CFont::DrawLine(const TSymbols aChars, const TSize aStartIndex, const TSize aEndIndex,
                     const int32_t x, const int32_t y, const int32_t aVariant, const int32_t aScale) const
{
   const int32_t scale = iDefaultScaling * aScale;
   int32_t pos = x;
   for (TSize i = aStartIndex; i < aEndIndex; i++) {
      const TCharData data = GetCharData(aChars[i]);
      iGraphics->Draw(iFontVariants[aVariant], pos, y, data.iWidth * scale, data.iHeight * scale,
                      data.iX, data.iY, data.iX + data.iWidth, data.iY + data.iHeight);
      pos += data.iWidth * scale + iHorizontalDistance * aScale;
   }
}

} // namespace Client
