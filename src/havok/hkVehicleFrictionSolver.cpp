// Havok translation unit hkVehicleFrictionSolver.o (main.dol 0x802A0BCC-0x802A0C68).
// Functions in address order (method names from the Havok TU map):
//   0x802A0BCC    12  fabs   [map: hkMath__fabs]
//   0x802A0BD8    24  __opb   [map: hkBool____opb]
//   0x802A0BF0     8  __as   [map: hkBool____as]
//   0x802A0BF8    84  sqrt   [map: hkMath__sqrt]
//   0x802A0C4C    16  min2<f>   [map: hkMath__min2_f_]
//   0x802A0C5C    12  __sinit_\hkVehicleFrictionSolver_cpp   [map: hkVehicleFrictionSolvercpp____sinit_]
#pragma fp_contract on
#include <havok/hkBase.h>

// hkMath members (declared locally; hkMath.h belongs to another worker).
struct hkMath {
    static float fabs(float x);
    static float sqrt(float x);
    static float min2(float a, float b);
};

float hkMath::fabs(float x) {
    return (float)__fabs((double)x);
}

// HYPOTHESIS: hkBool::operator bool() const. Free function because hkBool members live in hkBase.h.
bool hkBoolOpBool(const hkBool* b) {
    return b->m_bool != 0;
}

// HYPOTHESIS: hkBool assignment from bool. Free function because hkBool has no operator= in hkBase.h.
void hkBoolAssign(hkBool* b, bool v) {
    b->m_bool = v;
}

float hkMath::sqrt(float x) {
    float y;
    if (x <= 0.0f) {
        union {
            u32 bits;
            float f;
        } inf;
        inf.bits = 0x7f800000; // +inf
        y = inf.f;
    } else {
        float y0 = (float)__frsqrte((double)x);
        y = 0.5f * y0 * (1.5f - y0 * (x * y0));
    }
    return 1.0f / y;
}

float hkMath::min2(float a, float b) {
    return a < b ? a : b;
}

// Unnamed static flag (lbl_805A0F00); initialised through hkBool(bool) so the TU gets a __sinit.
static hkBool s_hkVehicleFrictionSolver_unk(false);
