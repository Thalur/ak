// JavaRandom.cpp

#include "JavaRandom.h"
#include <cmath>
#include <sys/timeb.h>


CJavaRandom::CJavaRandom()
{
   timeb tb;
   ftime(&tb);
   setSeed(static_cast<uint64_t>(tb.time) * uint64_t(1000) + static_cast<uint64_t>(tb.millitm));
}

CJavaRandom::CJavaRandom(uint64_t aSeed)
{
   setSeed(aSeed);
}

void CJavaRandom::setSeed(uint64_t aSeed)
{
   iSeed = (aSeed ^ 0x5DEECE66DL) & ((uint64_t(1) << 48) - 1);
   iHaveNextNextGaussian = false;
}

uint32_t CJavaRandom::next(uint32_t aBits)
{
   iSeed = (iSeed * 0x5DEECE66DL + 0xBL) & ((uint64_t(1) << 48) - 1);
   return static_cast<uint32_t>(iSeed >> (48 - aBits)); // original Java uses >>>
}

uint32_t CJavaRandom::nextInt(uint32_t aMaxNumber)
{
   int32_t n = static_cast<int32_t>(aMaxNumber);
   if ((n & -n) == n) { // i.e., n is a power of 2
      return static_cast<uint32_t>((n * static_cast<uint64_t>(next(31))) >> 31);
   }
   int32_t bits, val;
   do {
      bits = next(31);
      val = bits % n;
   } while (bits - val + (n - 1) < 0);
   return static_cast<uint32_t>(val);
}

double CJavaRandom::nextGaussian()
{
   if (iHaveNextNextGaussian) {
      iHaveNextNextGaussian = false;
      return iNextNextGaussian;
   }
   double v1, v2, s;
   do {
      v1 = 2 * nextDouble() - 1; // Between -1.0 and 1.0.
      v2 = 2 * nextDouble() - 1; // Between -1.0 and 1.0.
      s = v1 * v1 + v2 * v2;
   } while (s >= 1);
   double norm = sqrt(-2 * log(s) / s);
   iNextNextGaussian = v2 * norm;
   iHaveNextNextGaussian = true;
   return v1 * norm;
}

