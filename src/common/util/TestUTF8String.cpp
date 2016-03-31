/**
 *
 */
#include "UTF8String.h"

using TString = utf8::CStringContainer;
using TStringIt = utf8::CIterator;

int main()
{
   //printf("Target: Hallo André (%d), sizeof(char): %d\n", (int)'é', sizeof(char));
   std::string cppString { "Hallo André" };
   TStringIt it(cppString);
   for (; it; ++it) {
      printf("%u ", *it);
   }
   printf(" (Length: %d)\n", (int)it.length());
   it.reset();
   for (auto c : TString("Hallo André (not è or ê)")) {
      printf("%u ", c);
   }
   printf("\n");
   return 0;
}
