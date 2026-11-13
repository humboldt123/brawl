#pragma once

#include <havok/hkVector4.h>

// Scalar helpers from the hkMathTypes unit (map names hkMathTypes__hkMath_atan2fApproximation, hkMath__hkFloatToInt).
struct hkMathTypes {
    static float hkMath_atan2fApproximation(hkReal y, hkReal x);
};

struct hkMath {
    static int hkFloatToInt(hkReal f);
    // Clamped arc sine: +-pi/2 at and beyond +-1, the C library's asin otherwise (out of line in hkSimulation.o).
    static hkReal asin(hkReal r);
    // Clamped arc cosine (out of line in hkInertiaTensorComputer.o).
    static hkReal acos(hkReal r);
    // The larger of a and b (out of line in hkPoweredChainSolverUtil.o).
    static hkReal max2f(hkReal a, hkReal b);
    // Out of line in hkVehicleFrictionSolver.o (map names hkMath__fabs, hkMath__min2_f_); the functions compiled
    // without inlining call them.
    // The name is parenthesized because the MSL math.h defines fabs(x) as a macro.
    static float(fabs)(float x);
    // Out of line in hkVehicleFrictionSolver.o (map name hkMath__sqrt); parenthesized like fabs.
    static float(sqrt)(float x);
    static float min2(float a, float b);
};
