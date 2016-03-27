/**
 * Custom UTF-8 string implementation
 */
#ifndef AK_UTF8STRING_H_INCLUDED
#define AK_UTF8STRING_H_INCLUDED

#include <common/types.h>
#include <string>


namespace utf8
{

/**
 * The iterator does not own the string it operates on.
 * That object must stay in scope while the iterator exists.
 */
class CIterator
 //: public std::iterator<std::random_access_iterator_tag, uint32_t>
{
public:
   CIterator()
      : iString(nullptr), iLength(0), iStart(0), iPos(iStart) {}
   CIterator(const std::string& aSrc)
      : iString(aSrc.c_str()), iLength(aSrc.length()), iStart(GetStartPos()), iPos(iStart) {}
   CIterator(const char* aSrc)
      : iString(aSrc), iLength(std::strlen(aSrc)), iStart(GetStartPos()), iPos(iStart) {}
   CIterator(const char* aSrc, TSize aLength)
      : iString(aSrc), iLength(aLength), iStart(GetStartPos()), iPos(iStart) {}
   CIterator(const CIterator& aSrc)
      : iString(aSrc.iString), iLength(aSrc.iLength), iStart(aSrc.iStart), iPos(aSrc.iPos) {}
   CIterator(CIterator&& aSrc)
      : iString(aSrc.iString), iLength(aSrc.iLength), iStart(aSrc.iStart), iPos(aSrc.iPos) {}

   CIterator& operator=(const CIterator aSrc) {
      iString = aSrc.iString;
      iLength = aSrc.iLength;
      iStart = aSrc.iStart;
      iPos = aSrc.iPos;
      return *this;
   }

   operator bool() const { return iPos >= iStart && iPos < iLength; } // is iterator in a valid position
   uint32_t operator*() const; // "dereference": get character at current position
   int32_t length() const { return end() - begin(); }
   void reset() { iPos = iStart; }

   CIterator& operator++() { // preincrement
      while (iPos < iLength) {
         if ((iString[++iPos] & 0xC0) != 0x80) break;
      }
      return *this;
   }
   CIterator operator++(int) { // postincrement
      CIterator tmp { *this };
      ++*this;
      return tmp;
   }
   CIterator& operator--() { // predecrement
      while (iPos > iStart) {
         if ((iString[--iPos] & 0xC0) != 0x80) break;
      }
      return *this;
   }
   CIterator operator--(int) { // postdecrement
      CIterator tmp { *this };
      --*this;
      return tmp;
   }
   CIterator& operator+=(TSize aOffset) { // increment by integer
      for (TSize i = 0; i < aOffset; i++) {
         ++*this;
      }
      return *this;
   }
   CIterator operator+(TSize aOffset) const { // return this + integer
      CIterator tmp { *this };
      return (tmp += aOffset);
   }
   CIterator& operator-=(TSize aOffset) { // decrement by integer
      for (TSize i = 0; i < aOffset; i++) {
         --*this;
      }
      return *this;
   }
   CIterator operator-(TSize aOffset) const { // return this - integer
      CIterator tmp { *this };
      return (tmp -= aOffset);
   }
   int32_t operator-(const CIterator& aRight) const; // return difference of iterators
   uint32_t operator[](TSize aOffset) const { // subscript
      return (*(*this + aOffset));
   }
   bool operator==(const CIterator& aRight) const { // test for iterator equality
      return (this->iPos == aRight.iPos);
   }
   bool operator!=(const CIterator& aRight) const { // test for iterator inequality
      return (!(*this == aRight));
   }
   bool operator<(const CIterator& aRight) const { // test if this < aRight
      return (this->iPos < aRight.iPos);
   }
   bool operator>(const CIterator& aRight) const { // test if this > aRight
      return (aRight < *this);
   }
   bool operator<=(const CIterator& aRight) const { // test if this <= aRight
      return (!(aRight < *this));
   }
   bool operator>=(const CIterator& aRight) const { // test if this >= aRight
      return (!(*this < aRight));
   }

   CIterator begin() const {
      CIterator tmp { *this };
      tmp.iPos = tmp.iStart;
      return tmp;
   }
   CIterator end() const {
      CIterator tmp { *this };
      tmp.iPos = tmp.iLength;
      return tmp;
   }

private:
   TSize GetStartPos() const { // Skip UTF-8 BOM if present at the beginning of the string
      if ((iLength >= 3) && (iString[0] == 0xEF) && (iString[1] == 0xBB) && (iString[2] == 0xBF)) {
         return 3;
      }
      return 0;
   }

   const char* iString;
   TSize iLength;
   TSize iStart;
   TSize iPos;
};

} // namespace utf8

#endif // AK_UTF8STRING_H_INCLUDED
