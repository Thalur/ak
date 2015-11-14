// JavaRandom.cpp

#include "JavaRandom.h"
#include <cmath>
#include <sys/timeb.h>


CJavaRandom::CJavaRandom()
{
	timeb tb;
	ftime(&tb);
	setSeed(static_cast<TUint64>(tb.time)*TUint64(1000) + static_cast<TUint64>(tb.millitm));
}

CJavaRandom::CJavaRandom(TUint64 aSeed)
{
	setSeed(aSeed);
}

void CJavaRandom::setSeed(TUint64 aSeed)
{
	iSeed = (aSeed ^ 0x5DEECE66DL) & ((TUint64(1) << 48) - 1);
	iHaveNextNextGaussian = false;
}

TUint32 CJavaRandom::next(TUint32 aBits)
{
	iSeed = (iSeed * 0x5DEECE66DL + 0xBL) & ((TUint64(1) << 48) - 1);
	return static_cast<TUint32>(iSeed >> (48 - aBits)); // original Java uses >>>
}

TUint32 CJavaRandom::nextInt(TUint32 aMaxNumber)
 {
   TInt32 n = static_cast<TInt32>(aMaxNumber);
	if ((n & -n) == n) { // i.e., n is a power of 2
		return static_cast<TUint32>((n * static_cast<TUint64>(next(31))) >> 31);
   }
	TInt32 bits, val;
	do {
		bits = next(31);
		val = bits % n;
	} while (bits - val + (n - 1) < 0);
	return static_cast<TUint32>(val);
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
