/**
 * Custom UTF-8 string implementation
 */
#ifndef AK_UTF8STRING_H_INCLUDED
#define AK_UTF8STRING_H_INCLUDED

#include <common/types.h>
#include <string>
#include <vector>

using TUTF8Char = uint32_t;

class CUTF8String
{
public:
   // Create an UTF-8 string from 7-bit characters (for performance reason)
   static CUTF8String fromASCII7(const std::string& aAsciiString);
   static CUTF8String fromASCII7(const char* aAsciiString);
   template <typename T>
   static CUTF8String fromASCII7(const std::vector<T>& aAsciiVec);

   // Create an UTF-8 string from UTF-16 input data
   static CUTF8String fromUTF16(const std::string& aUTF16String);
   static CUTF8String fromUTF16(const char* aUTF16String);
   template <typename T>
   static CUTF8String fromUTF16(const std::vector<T>& aUTF16Vec);

   // Create an UTF-8 string from UTF-32 input data
   static CUTF8String fromUTF32(const std::string& aUTF32String);
   static CUTF8String fromUTF32(const char* aUTF32String);
   template <typename T>
   static CUTF8String fromUTF32(const std::vector<T>& aUTF32Vec);


   // Constructors
   explicit CUTF8String();
   explicit CUTF8String(const CUTF8String& aSrc);
   explicit CUTF8String(const CUTF8String& aSrc, std::size_t aPos, std::size_t aLength = std::string::npos);
   explicit CUTF8String(const std::string& aString);
   explicit CUTF8String(const std::string& aSrc, std::size_t aPos, std::size_t aLength = std::string::npos);
   explicit CUTF8String(const char* aString);
   explicit CUTF8String(const char* aString, std::size_t aLength);
   explicit CUTF8String(std::size_t aLength, TUTF8Char aFillChar);
   template <class InputIterator>
   explicit CUTF8String(InputIterator first, InputIterator last);
   explicit CUTF8String(std::initializer_list<TUTF8Char> il);
   explicit CUTF8String(CUTF8String&& aSrc);

   // Assignment operators
   CUTF8String& operator= (const CUTF8String& aSrc);
   CUTF8String& operator= (const std::string& aSrc);
   CUTF8String& operator= (const char* aSrc);
   CUTF8String& operator= (std::initializer_list<TUTF8Char> il);
   CUTF8String& operator= (CUTF8String&& aSrc);

   // Conversions to character arrays (wide-char symbols are converted to invalid symbol)
   std::string str() const;
   const char* c_str() const;
   template <typename T> std::vector<T> toASCII7() const;

   // Conversion to character arrays (wide-char symbols stay in)
   std::string toUTF8String() const;
   const char* toUTF8CString() const;
   const std::vector<TUTF8Char> data() const;
   template <typename T> std::vector<T> toUTF8Vector() const;

   // Conversion to UTF-16
   std::string toUTF16String() const;
   const char* toUTF16CString() const;
   template <typename T> std::vector<T> toUTF16Vector() const;

   // Conversion to UTF-32
   std::string toUTF32String() const;
   const char* toUTF32CString() const;
   template <typename T> std::vector<T> toUTF32Vector() const;

   // Iterators
   // ...

   // Capacity
   std::size_t size() const;
   std::size_t length() const;
   std::size_t max_size() const;
   void resize(std::size_t aNewSize);
   void resize(std::size_t aNewSize, TUTF8Char aFillChar);
   std::size_t capacity() const;
   void reserve(std::size_t aReserveLength = 0);
   void clear();
   bool empty() const;
   void shrink_to_fit();

   // Element access
   TUTF8Char operator[] (std::size_t aPos) const;
   TUTF8Char at(std::size_t aPos) const;
   char getChar(std::size_t aPos) const;

   // Modifiers
   CUTF8String& operator+= (const CUTF8String& aAppend);
   CUTF8String& operator+= (const std::string& aAppend);
   CUTF8String& operator+= (const char* aAppend);
   CUTF8String& operator+= (TUTF8Char aAppend);
   CUTF8String& operator+= (char aAppend);
   CUTF8String& operator+= (std::initializer_list<TUTF8Char> il);
   CUTF8String& operator+= (std::initializer_list<char> il);
   CUTF8String& append(const CUTF8String& aAppend);
   CUTF8String& append(const CUTF8String& aAppend, std::size_t aPos, std::size_t aLength = std::string::npos);
   CUTF8String& append(const std::string& aAppend);
   CUTF8String& append(const char* aAppend);
   CUTF8String& append(const char* aAppend, std::size_t aLength);
   CUTF8String& append(std::size_t aLength, TUTF8Char aFillChar);
   CUTF8String& append(std::size_t aLength, char aFillChar);
   template <class InputIterator>
   CUTF8String& append(InputIterator first, InputIterator last);

   void push_back(TUTF8Char aChar);
   void push_back(char aChar);

   CUTF8String& assign(const CUTF8String& aString);
   CUTF8String& assign(const CUTF8String &aString, std::size_t aPos, std::size_t aLength = std::string::npos);
   CUTF8String& assign(const std::string& aString);
   CUTF8String& assign(const char* aString);
   CUTF8String& assign(const char* aString, std::size_t aLength);
   CUTF8String& assign(std::size_t aLength, TUTF8Char aFillChar);
   CUTF8String& assign(std::size_t aLength, char aFillChar);
   template <class InputIterator>
   CUTF8String& assign(InputIterator first, InputIterator last);
   CUTF8String& assign(CUTF8String&& aString);

   CUTF8String& insert(std::size_t aPos, const CUTF8String& aString);
   CUTF8String& insert(std::size_t aPos, const CUTF8String& aString, std::size_t aStart, std::size_t aLength = std::string::npos);
   CUTF8String& insert(std::size_t aPos, const std::string& aString);
   CUTF8String& insert(std::size_t aPos, const char* aString);
   CUTF8String& insert(std::size_t aPos, const char* aString, std::size_t aLength);
   CUTF8String& insert(std::size_t aPos, std::size_t aLength, TUTF8Char aChar);
   CUTF8String& insert(std::size_t aPos, std::size_t aLength, char aChar);
   template <class InputIterator>
   CUTF8String& insert(std::size_t aPos, InputIterator first, InputIterator last);

   CUTF8String& erase(std::size_t aPos = 0, std::size_t aLength = std::string::npos);

   void swap(CUTF8String& aString);
   void pop_back();

   // String operations
   std::size_t find(const CUTF8String& aPattern, std::size_t aPos = 0) const;
   std::size_t find(const std::string& aPattern, std::size_t aPos = 0) const;
   std::size_t find(const char* aPattern, std::size_t aPos = 0) const;
   std::size_t find(TUTF8Char aChar, std::size_t aPos = 0) const;
   std::size_t find(char aChar, std::size_t aPos = 0) const;
   std::size_t rfind(const CUTF8String& aPattern, std::size_t aPos = std::string::npos) const;
   std::size_t rfind(const std::string& aPattern, std::size_t aPos = std::string::npos) const;
   std::size_t rfind(const char* aPattern, std::size_t aPos = std::string::npos) const;
   std::size_t rfind(TUTF8Char aChar, std::size_t aPos = std::string::npos) const;
   std::size_t rfind(char aChar, std::size_t aPos = std::string::npos) const;
   // ...
};

#endif // AK_UTF8STRING_H_INCLUDED
