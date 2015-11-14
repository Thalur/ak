/**
 * Wide-char string conversion implementation for Windows.
 * This should be used instead of LPWSTR for better memory management.
 */
#ifndef AK_STRINGWINDOWS_H_INCLUDED
#define AK_STRINGWINDOWS_H_INCLUDED

#define UNICODE
#include <windows.h>

#include "common/types.h"
#include <string>
#include <vector>


class CStringWindows
{
   std::vector<WCHAR> iString;

public:
   CStringWindows()
   {
      iString.push_back('\0');
   }

   CStringWindows(TInt32 aSize) : iString(aSize)
   {
      iString.push_back('\0');
   }

   CStringWindows(const std::string& aString)
   {
      const char* multiByteString = aString.c_str();
	   const TInt32 l = MultiByteToWideChar(CP_UTF8, 0, multiByteString, -1, NULL, 0);
	   iString.resize(l);
	   MultiByteToWideChar(CP_UTF8, 0, multiByteString, -1, &iString[0], l);
   }

   CStringWindows(const char* const aString)
   {
      const TInt32 l = MultiByteToWideChar(CP_UTF8, 0, aString, -1, NULL, 0);
	   iString.resize(l);
	   MultiByteToWideChar(CP_UTF8, 0, aString, -1, &iString[0], l);
   }

   CStringWindows(const WCHAR* const aString)
   {
      const size_t l = wcslen(aString);
	   iString.resize(l);
	   wcsncpy_s(&iString[0], l, aString, l);
   }

   CStringWindows(const CStringWindows& aString) : iString(aString.iString) { }

   virtual ~CStringWindows() {}

   const WCHAR* GetLPWSTR() { return &iString[0]; }
};

#endif // AK_STRINGWINDOWS_H_INCLUDED
