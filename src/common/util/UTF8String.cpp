/**
 * UTF-8 iterator implementation.
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

uint32_t CIterator::operator*() const
{
   if (*this) {
      const uint32_t byte1 = iString[iPos];
      if ((byte1 & 0x80) == 0) { // simple ASCII char
         return byte1;
      }
      if ((iPos+1 < iLength) && (byte1 & 0x40) != 0) { // first byte is valid
         const uint32_t byte2 = iString[iPos+1];
         if (((byte2 & 0xC0) == 0x80)) { // second byte is valid
            if ((byte1 & 0x20) == 0) { // 2-byte char
               return ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
            }
            if (iPos+2 < iLength) { // more than 2 bytes possible
               const uint32_t byte3 = iString[iPos+2];
               if ((byte3 & 0xC0) == 0x80) { // third byte is valid
                  if ((byte1 & 0x10) == 0) { // 3-byte char
                     return ((byte1 & 0xF) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
                  }
                  if (iPos+3 < iLength) { // more than 3 bytes possible
                     const uint32_t byte4 = iString[iPos+3];
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
   return 0; // Invalid character or premature ending
}

// return the difference between the two iterators in characters
int32_t CIterator::operator-(const CIterator &aRight) const
{
   ASSERT(this->iString == aRight.iString);
   ASSERT(this->iLength == aRight.iLength);
   ASSERT(this->iStart == aRight.iStart);
   ASSERT(this->iPos >= this->iStart);
   ASSERT(this->iPos <= this->iLength);
   ASSERT(aRight.iPos >= aRight.iStart);
   ASSERT(aRight.iPos <= aRight.iLength);
   int32_t result = 0;
   CIterator it { *this };
   if (it.iPos <= aRight.iPos) {
      while ((it.iPos < aRight.iPos) && (it.iPos >= iStart) && (it.iPos < iLength)) {
         result--;
         ++it;
      }
   } else {
      while ((it.iPos > aRight.iPos) && (it.iPos > iStart) && (it.iPos <= iLength)) {
         result++;
         --it;
      }
   }
   return result;
}


}
