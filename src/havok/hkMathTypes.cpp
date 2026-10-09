// Havok translation unit hkMathTypes.o (main.dol 0x802820A4-0x802821DC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802820A4   164  hkMath_atan2fApproximation   [map: hkMathTypes__hkMath_atan2fApproximation]
//   0x80282148   148  hkFloatToInt   [map: hkMath__hkFloatToInt]
#include <havok/hkMath.h>
#include <math.h>

// Polynomial atan2 approximation; the ratio is the smaller over the larger absolute value.
float hkMathTypes::hkMath_atan2fApproximation(hkReal y, hkReal x) {
    hkReal ay = (hkReal)fabs(y);
    hkReal ax = (hkReal)fabs(x);
    hkReal r;

    if (ax >= ay) {
        r = ay / (ax + 1.1920929e-07f);
        r = r + r * (-0.121079f * r) + r * (r * (-0.09352282f * r));
    } else {
        r = ax / (ay + 1.1920929e-07f);
        r = 1.5707964f - (r + r * (-0.121079f * r) + r * (r * (-0.09352282f * r)));
    }
    if (x < 0.0f) {
        r = 3.1415927f - r;
    }
    if (y < 0.0f) {
        r = -r;
    }
    return r;
}

// HYPOTHESIS placeholder: the original is a bit-manipulation float-to-int (see the TU map); plain truncation here.
int hkMath::hkFloatToInt(hkReal f) {
    return (int)f;
}
