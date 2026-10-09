// Havok translation unit hkMathTypes.o (main.dol 0x802820A4-0x802821DC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802820A4   164  hkMath_atan2fApproximation   [map: hkMathTypes__hkMath_atan2fApproximation]
//   0x80282148   148  hkFloatToInt   [map: hkMath__hkFloatToInt]
#pragma fp_contract on
#include <havok/hkMath.h>
#include <math.h>

// Polynomial atan2 approximation; the ratio is the smaller over the larger absolute value.
float hkMathTypes::hkMath_atan2fApproximation(hkReal y, hkReal x) {
    hkReal ay = (hkReal)fabs(y);
    hkReal ax = (hkReal)fabs(x);
    hkReal r;

    if (ay <= ax) {
        r = ay / (ax + 1.1920929e-07f);
        hkReal p = r + r * (-0.121079f * r);
        r = p + r * (r * (-0.09352282f * r));
    } else {
        r = ax / (ay + 1.1920929e-07f);
        hkReal p = r + r * (-0.121079f * r);
        r = 1.5707964f - (p + r * (r * (-0.09352282f * r)));
    }
    if (x < 0.0f) {
        r = 3.1415927f - r;
    }
    if (y < 0.0f) {
        r = -r;
    }
    return r;
}

// Float to int by bit manipulation (truncation toward zero), written as the original's branchless sequence.
// HYPOTHESIS: the sign/zero/exponent masks follow the original data flow; the source-level expression is a reconstruction.
int hkMath::hkFloatToInt(hkReal f) {
    union {
        hkReal f;
        s32 i;
    } u;
    u.f = f;
    s32 bits = u.i;
    s32 exp = (s32)(((u32)bits >> 23) & 0xff);
    s32 absBits = bits & 0x7fffffff;
    s32 e = exp - 127;
    s32 hiExp = exp - 150;
    s32 lt24 = (hiExp - 1) >> 31;
    s32 zeroMask = (absBits - 1) >> 31;
    s32 sh = (23 - e) & lt24;
    s32 hi = hiExp & ~lt24;
    s32 t = (23 & lt24) - sh;
    s32 mask = ((s32)0xff800000 >> t) | ~lt24;
    s32 nz = bits & ~zeroMask;
    s32 mant = (nz & 0x7fffff) | 0x800000;
    s32 signMask = (nz & (s32)0x80000000) >> 31;
    s32 m = mant & mask;
    s32 q = ((m << 1) - 1) | signMask;
    s32 v = (q - m) + 1;
    s32 negE = e >> 31;
    s32 r = (v & ~negE) >> sh;
    return r << hi;
}
