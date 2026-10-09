// Havok translation unit hkSolverTimeDate.o (main.dol 0x802A0B88-0x802A0BCC).
// Functions in address order (method names from the Havok TU map):
//   0x802A0B88    68  hkSolverGetSystemTime   [map: hkSolverTimeDate__hkSolverGetSystemTime]
#include <havok/hkBase.h>

typedef long long OSTime;

extern "C" OSTime OSGetTime(void);
// HYPOTHESIS: 64-bit by 64-bit division helper (__div2i in the image).
extern "C" OSTime __div2i(OSTime num, OSTime den);
// Unidentified 64-bit-to-float conversion and float post-processing helpers (fn_803F17E0, fn_803F1960).
extern "C" float fn_803F17E0(OSTime value);
extern "C" float fn_803F1960(float value);

// Elapsed system time in seconds-like units: OS time divided by the bus clock quarter.
float hkSolverGetSystemTime() {
    OSTime now = OSGetTime();
    OSTime tick = *(volatile u32*)0x800000F8 >> 2;
    float t = fn_803F17E0(__div2i(now, tick));
    return fn_803F1960(t + 0.0f);
}
