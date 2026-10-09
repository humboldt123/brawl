// Brawl physics wrapper translation unit ph_ik_solver.o (main.dol 0x80096690-0x800966EC).
// Not yet decompiled. Functions in address order with their map names:
//   0x80096690     4  __ct   [map: nw4r4math5MTX34Fv____ct]
//   0x80096694    16  __ct   [map: nw4r4math4VEC3Ffff____ct]
//   0x800966A4    12  __cl   [map: hkVector4____cl]
//   0x800966B0    60  MTX34Mult   [map: nw4r4mathFPQ34nw4r4math5__MTX34Mult]
#include <revolution/MTX.h>

// MATCH-ONLY/LOCAL-TYPES: this unit does not include the shared nw4r math or hkVector4 headers, because those
// define the same classes with inline members and would clash with the out-of-line definitions below. The
// declarations here keep the same names, layouts and signatures, so the mangled names match the map.

namespace nw4r {
namespace math {

struct MTX34 {
    f32 m[3][4];

    MTX34();
};

struct VEC3 {
    f32 x, y, z;

    VEC3(f32 fx, f32 fy, f32 fz);
};

MTX34* MTX34Mult(MTX34* pOut, const MTX34* pA, const MTX34* pB);

} // namespace math
} // namespace nw4r

// Component access by index (x, y, z, w) as used by the Havok math unit; declared locally, see note above.
struct hkVector4 {
    f32 x, y, z, w;

    f32& operator()(int i);
};

namespace nw4r {
namespace math {

MTX34::MTX34() {}

VEC3::VEC3(f32 fx, f32 fy, f32 fz) {
    x = fx;
    y = fy;
    z = fz;
}

} // namespace math
} // namespace nw4r

f32& hkVector4::operator()(int i) {
    return (&x)[i];
}

namespace nw4r {
namespace math {

MTX34* MTX34Mult(MTX34* pOut, const MTX34* pA, const MTX34* pB) {
    PSMTXConcat(pA->m, pB->m, pOut->m);
    return pOut;
}

} // namespace math
} // namespace nw4r
