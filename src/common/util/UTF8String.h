/**
 * Custom UTF-8 string implementation
 */
#ifndef AK_UTF8STRING_H_INCLUDED
#define AK_UTF8STRING_H_INCLUDED

#include <string>
#include <cstring>
#include <cstdint>


namespace utf8
{

/**
 * The iterator does not own the string it operates on.
 * That object must stay in scope while the iterator exists.
 */
class CIterator
 //: public std::iterator<std::random_access_iterator_tag, uint32_t>
{
   using TSize = std::size_t;
public:
   CIterator()
      : iString(nullptr), iByteEnd(0) {}
   CIterator(const std::string& aString)
      : iString(aString.c_str()), iByteEnd(aString.length()), iByteStart(GetStartPos()), iBytePos(iByteStart) {}
   CIterator(const std::string& aString, TSize aStart, TSize aEnd)
      : iString(aString.c_str()), iByteEnd(aEnd), iByteStart(aStart), iBytePos(iByteStart) {}
   CIterator(const char* aString)
      : iString(aString), iByteEnd(std::strlen(aString)), iByteStart(GetStartPos()), iBytePos(iByteStart) {}
   CIterator(const char* aString, TSize aLength)
      : iString(aString), iByteEnd(aLength), iByteStart(GetStartPos()), iBytePos(iByteStart) {}
   CIterator(const char* aString, TSize aStart, TSize aEnd)
      : iString(aString), iByteEnd(aEnd), iByteStart(aStart), iBytePos(aStart) {}
   CIterator(const CIterator& aSrc) // copy constructor
      : iString(aSrc.iString), iByteEnd(aSrc.iByteEnd), iByteStart(aSrc.iByteStart)
      , iBytePos(aSrc.iBytePos), iCharPos(aSrc.iCharPos), iCharLength(aSrc.iCharLength) {}
   CIterator(CIterator&& aSrc) // move constructor
      : iString(aSrc.iString), iByteEnd(aSrc.iByteEnd), iByteStart(aSrc.iByteStart)
      , iBytePos(aSrc.iBytePos), iCharPos(aSrc.iCharPos), iCharLength(aSrc.iCharLength) {}

   CIterator& operator=(const CIterator& aSrc) { // copy assignment
      iString = aSrc.iString;
      iByteEnd = aSrc.iByteEnd;
      iByteStart = aSrc.iByteStart;
      iBytePos = aSrc.iBytePos;
      iCharPos = aSrc.iCharPos;
      iCharLength = aSrc.iCharLength;
      return *this;
   }

   operator bool() const { // is iterator in a valid position
      return iBytePos >= iByteStart && iBytePos < iByteEnd;
   }
   bool rvalid() const { // backwards iterator position valid
      return iBytePos > iByteStart && iBytePos <= iByteEnd;
   }
   uint32_t operator*() const; // "dereference": get character at current position
   TSize length() const { // determine the number of characters in O(n)
      if (iCharLength != std::string::npos) {
         return iCharLength;
      } else {
         return CalculateCharLength();
      }
   }
   TSize pos() const { // get current character offset
      if (iCharPos != std::string::npos) {
         return iCharPos;
      } else {
         iCharPos = GetCharDistance(iString, iByteEnd, iByteStart, iBytePos);
         return iCharPos;
      }
   }
   TSize bytePos() const { return iBytePos; } // get current position in bytes

   CIterator& reset() { // reset iterator position
      iBytePos = iByteStart;
      iCharPos = 0;
      return *this;
   }

   CIterator& operator++() { // preincrement
      while (iBytePos < iByteEnd) {
         if ((iString[++iBytePos] & 0xC0) != 0x80) {
            iCharPos++;
            break;
         }
      }
      return *this;
   }
   CIterator operator++(int) { // postincrement
      CIterator tmp { *this };
      ++*this;
      return tmp;
   }
   CIterator& operator--() { // predecrement
      while (iBytePos > iByteStart) {
         if ((iString[--iBytePos] & 0xC0) != 0x80) {
            if (iCharPos != std::string::npos) {
               iCharPos--;
            }
            break;
         }
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
      return (this->iBytePos == aRight.iBytePos);
   }
   bool operator!=(const CIterator& aRight) const { // test for iterator inequality
      return (!(*this == aRight));
   }
   bool operator<(const CIterator& aRight) const { // test if this < aRight
      return (this->iBytePos < aRight.iBytePos);
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

private:
   TSize GetStartPos() const { // Skip UTF-8 BOM if present at the beginning of the string
      if ((iByteEnd >= 3) && (static_cast<uint8_t>(iString[0]) == 0xEF) && 
          (static_cast<uint8_t>(iString[1]) == 0xBB) && (static_cast<uint8_t>(iString[2]) == 0xBF)) {
         return 3;
      }
      return 0;
   }
   TSize CalculateCharLength() const {
      iCharLength = GetCharDistance(iString, iByteEnd, iByteStart, iByteEnd);
      return iCharLength;
   }
   static TSize GetCharDistance(const char* aString, TSize aEnd, TSize aPos1, TSize aPos2);

   const char* iString;
   TSize iByteEnd;
   TSize iByteStart = 0;
   TSize iBytePos = 0;
   mutable TSize iCharPos = 0; // can use lazy initialization for end iterators
   mutable TSize iCharLength = std::string::npos; // lazily initialized

   friend class CStringContainer;
};


class CStringContainer
{
   using TSize = std::size_t;
public:
   CStringContainer(const std::string& aString)
      : iString(aString.c_str()), iStart(0), iEnd(aString.length()) {}
   CStringContainer(const std::string& aString, TSize aLength)
      : iString(aString.c_str()), iStart(0), iEnd(aLength) {}
   CStringContainer(const std::string& aString, TSize aStart, TSize aEnd)
      : iString(aString.c_str()), iStart(aStart), iEnd(aEnd) {}
   CStringContainer(const char* aString)
      : iString(aString), iStart(0), iEnd(std::strlen(aString)) {}
   CStringContainer(const char* aString, TSize aLength)
      : iString(aString), iStart(0), iEnd(aLength) {}
   CStringContainer(const char* aString, TSize aStart, TSize aEnd)
      : iString(aString), iStart(aStart), iEnd(aEnd) {}
   CStringContainer(const CStringContainer& aString, TSize aStart, TSize aEnd)
      : iString(aString.iString), iStart(aString.iStart + aStart), iEnd(aEnd) {}
   CStringContainer(const CStringContainer& aSrc) // copy constructor
      : iString(aSrc.iString), iStart(aSrc.iStart), iEnd(aSrc.iEnd) {}
   CStringContainer(CStringContainer&& aSrc) // move constructor
      : iString(aSrc.iString), iStart(aSrc.iStart), iEnd(aSrc.iEnd) {}

   CStringContainer& operator=(const CStringContainer& aSrc) { // copy assignment
      iString = aSrc.iString;
      iStart = aSrc.iStart;
      iEnd = aSrc.iEnd;
      return *this;
   }

   CIterator begin() const {
      return CIterator { iString, iStart, iEnd };
   }
   CIterator end() const {
      CIterator tmp { iString, iStart, iEnd };
      tmp.iBytePos = iEnd;
      tmp.iCharPos = std::string::npos; // for lazy initialization
      return tmp;
   }
   const char* c_str() const { return iString; }
   TSize size() const { return iEnd - iStart; }

private:
   const char* iString;
   TSize iStart;
   TSize iEnd;
};

} // namespace utf8

#endif // AK_UTF8STRING_H_INCLUDED
