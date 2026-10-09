#pragma once

#include <havok/hkVector4.h>

// Scalar helpers from the hkMathTypes unit (map names hkMathTypes__hkMath_atan2fApproximation, hkMath__hkFloatToInt).
struct hkMathTypes {
    static float hkMath_atan2fApproximation(hkReal y, hkReal x);
};

struct hkMath {
    static int hkFloatToInt(hkReal f);
};
