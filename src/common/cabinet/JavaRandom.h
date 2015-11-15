/**
 *
 */
#ifndef AK_JAVARANDOM_H_INCLUDED
#define AK_JAVARANDOM_H_INCLUDED

#include "common/types.h"

class CJavaRandom
{
public:
   CJavaRandom();
   CJavaRandom(uint64_t aSeed);
   void setSeed(uint64_t aSeed);

   uint32_t nextInt() { return next(32); }
   uint32_t nextInt(uint32_t aMaxNumber);
   uint64_t nextLong() { return (static_cast<uint64_t>(next(32)) << 32) + next(32); }
   bool nextBool() { return next(1) != 0; }
   float nextFloat() { return next(24) / static_cast<float>(1 << 24); }
   double nextDouble() { return ((static_cast<uint64_t>(next(26)) << 27) + next(27)) / static_cast<double>(uint64_t(1) << 53); }
   double nextGaussian();
	
protected:
   uint32_t next(uint32_t aBits);

private:
   bool iHaveNextNextGaussian;
   double iNextNextGaussian;
   uint64_t iSeed;
};

#endif // AK_JAVARANDOM_H_INCLUDED
