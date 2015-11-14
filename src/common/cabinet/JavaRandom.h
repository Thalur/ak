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
   CJavaRandom(TUint64 aSeed);
   void setSeed(TUint64 aSeed);

   TUint32 nextInt() { return next(32); }
   TUint32 nextInt(TUint32 aMaxNumber);
   TUint64 nextLong() { return (static_cast<TUint64>(next(32)) << 32) + next(32); }
   bool nextBool() { return next(1) != 0; }
   float nextFloat() { return next(24) / static_cast<float>(1 << 24); }
   double nextDouble() { return ((static_cast<TUint64>(next(26)) << 27) + next(27)) / static_cast<double>(TUint64(1) << 53); }
   double nextGaussian();
	
protected:
   TUint32 next(TUint32 aBits);

private:
   bool iHaveNextNextGaussian;
   double iNextNextGaussian;
   TUint64 iSeed;
};

#endif // AK_JAVARANDOM_H_INCLUDED
