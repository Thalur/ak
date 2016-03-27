/**
 *
 */
#include "UTF8String.h"

using TStringIt = utf8::CIterator;

void main()
{
   printf("Target: Hallo André (%d), sizeof(char): %d\n", (int)'é', sizeof(char));
   std::string cppString { "Hallo André" };
   TStringIt it(cppString);
   for (; it; ++it) {
      printf("%u ", *it);
   }
   printf(" (Length: %d)\n",it.length());
   it.reset();
   for (auto c : TStringIt("Hallo André (not è or ê)")) {
      printf("%u ", c);
   }
   printf("\n");
}
