/**
 * UTF-8 string iterator implementation.
 */
#include "UTF8String.h"
#include "common/log/log.h"

/*
 * Byte order marks
 * UTF-8: 0xEF 0xBB 0xBF
 * UTF-16: 0xFE 0xFF
 * UTF-16 reversed: 0xFF 0xFE
 */

namespace utf8 {

// Get the UTF-8 character at the current position
// UTF-8 definition from http://www.ietf.org/rfc/rfc3629.txt
uint32_t CIterator::operator*() const
{
   if (*this) {
      const uint32_t byte1 = iString[iBytePos];
      if ((byte1 & 0x80) == 0) { // simple ASCII char
         return byte1;
      }
      if ((iBytePos+1 < iByteEnd) && (byte1 & 0x40) != 0) { // first byte is valid
         const uint32_t byte2 = iString[iBytePos+1];
         if (((byte2 & 0xC0) == 0x80)) { // second byte is valid
            if ((byte1 & 0x20) == 0) { // 2-byte char
               return ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
            }
            if (iBytePos+2 < iByteEnd) { // more than 2 bytes possible
               const uint32_t byte3 = iString[iBytePos+2];
               if ((byte3 & 0xC0) == 0x80) { // third byte is valid
                  if ((byte1 & 0x10) == 0) { // 3-byte char
                     return ((byte1 & 0xF) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
                  }
                  if (iBytePos+3 < iByteEnd) { // more than 3 bytes possible
                     const uint32_t byte4 = iString[iBytePos+3];
                     if (((byte1 & 0x8) == 0) && ((byte4 & 0xC0) == 0x80)) { // forth byte is valid
                        return ((byte1 & 0x7) << 18) | ((byte2 & 0x3F) << 12) |
                               ((byte3 & 0x3F) << 6) | (byte4 & 0x3F);
                     }
                  }
               }
            }
         }
      }
   }
   return 0; // Invalid character or premature end of string
}

// return the difference between the two iterators in characters
int32_t CIterator::operator-(const CIterator &aRight) const
{
   ASSERT(this->iString == aRight.iString);
   ASSERT(this->iByteEnd == aRight.iByteEnd);
   ASSERT(this->iByteStart == aRight.iByteStart);
   ASSERT(this->iBytePos >= this->iByteStart);
   ASSERT(this->iBytePos <= this->iByteEnd);
   ASSERT(aRight.iBytePos >= aRight.iByteStart);
   ASSERT(aRight.iBytePos <= aRight.iByteEnd);
   if (iBytePos <= aRight.iBytePos) {
      return - static_cast<int32_t>(GetCharDistance(iString, iByteEnd, iBytePos, aRight.iBytePos));
   } else {
      return static_cast<int32_t>(GetCharDistance(iString, iByteEnd, aRight.iBytePos, iBytePos));
   }
}

TSize CIterator::GetCharDistance(const char* aString, TSize aEnd, TSize aPos1, TSize aPos2)
{
   LOG_METHOD();
   ASSERT(aPos1 <= aEnd);
   ASSERT(aPos1 <= aPos2);
   TSize result = 0;
   CIterator it { aString, aPos1, aEnd };
   while ((it.bytePos() < aEnd) && (it.bytePos() < aPos2)) {
      result++;
      ++it;
   }
   return result;
}

} // namespace utf8
