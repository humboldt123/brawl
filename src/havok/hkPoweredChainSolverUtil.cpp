#include <types.h>
// Havok translation unit hkPoweredChainSolverUtil.o (main.dol 0x80290594-0x80293CBC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80290594  1016  hkPoweredChainBuildJacobian   [map: hkPoweredChainSolverUtil__hkPoweredChainBuildJacobian]
//   0x8029098C   308  hkPoweredChain_BuildConstraintMatrixAndLuDecomposition   [map: hkPoweredChainSolverUtil__hkPoweredChain_BuildConstraintMatrixAndLuDecomposition]
//   0x80290AC0     4  __ct   [map: hkMatrix6____ct]
//   0x80290AC4   288  setZero   [map: hkMatrix6__setZero]
//   0x80290BE4  1676  hkPoweredChain_ComputeConstraintMatrix_DiagonalAtRow   [map: hkPoweredChainSolverUtil__hkPoweredChain_ComputeConstraintMatrix_DiagonalAtRow]
//   0x80291270    20  __cl   [map: hkMatrix3____cl]
//   0x80291284   824  hkPoweredChain_ComputeConstraintMatrix_NextOffDiagonalAtR   [map: hkPoweredChainSolverUtil__hkPoweredChain_ComputeConstraintMatrix_NextOffDiagonalAtRow]
//   0x802915BC     8  __ct   [map: hkBool____ct]
//   0x802915C4   888  hkPoweredChain_UpdateLuDecomposition   [map: hkPoweredChainSolverUtil__hkPoweredChain_UpdateLuDecomposition]
//   0x8029193C   820  __as   [map: hkMatrix6____as]
//   0x80291C70   360  hkPoweredChain_ComputeConstraintMatrixLuDecomposition_For   [map: hkPoweredChainSolverUtil__hkPoweredChain_ComputeConstraintMatrixLuDecomposition_ForOneRow]
//   0x80291DD8    16  max2<f>   [map: hkMath__max2_f_]
//   0x80291DE8   820  __ct   [map: hkMatrix6____ct1]
//   0x8029211C   148  hkPoweredChain_DisableMotorInMatrixRow_ThisConstraint   [map: hkPoweredChainSolverUtil__hkPoweredChain_DisableMotorInMatrixRow_ThisConstraint]
//   0x802921B0   172  hkPoweredChain_DisableMotorInMatrixRow_NextConstraint   [map: hkPoweredChainSolverUtil__hkPoweredChain_DisableMotorInMatrixRow_NextConstraint]
//   0x8029225C  2248  hkPoweredChain_CalculateVelocities   [map: hkPoweredChainSolverUtil__hkPoweredChain_CalculateVelocities]
//   0x80292B24    84  hkPoweredChain_OverwriteVelocityWithExplicitImpulse   [map: hkPoweredChainSolverUtil__hkPoweredChain_OverwriteVelocityWithExplicitImpulse]
//   0x80292B78   964  hkPoweredChain_RestoreVelocityValue   [map: hkPoweredChainSolverUtil__hkPoweredChain_RestoreVelocityValue]
//   0x80292F3C   332  hkPoweredChain_SolveConstraintMatrix_ComputeVectorG   [map: hkPoweredChainSolverUtil__hkPoweredChain_SolveConstraintMatrix_ComputeVectorG]
//   0x80293088   636  hkPoweredChain_ScanAndDisableMotors   [map: hkPoweredChainSolverUtil__hkPoweredChain_ScanAndDisableMotors]
//   0x80293304  2488  hkPoweredChain_ScanAndEnableMotors   [map: hkPoweredChainSolverUtil__hkPoweredChain_ScanAndEnableMotors]

// Function bodies below are taken from https://github.com/jj-link/brawl.

namespace hkMath {
float max2f(float a, float b) {
    if (a > b) {
        return a;
    }
    return b;
}
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_80292B24.cpp.
// hkPoweredChain_OverwriteVelocityWithExplicitImpulse
struct hkPoweredChainMotorEntry_80292B24 {
    float x, a, b, c, d, e;
};
struct hkPoweredChainMotorInput_80292B24 {
    unsigned char flags;
    unsigned char pad[3];
    hkPoweredChainMotorEntry_80292B24 entries[3];
};
struct hkPoweredChainMotorOutput_80292B24 {
    unsigned char pad[16];
    float values[4];
};
// TODO: cleanup (shape from jj-link/brawl fn_80292B24)
extern "C" void fn_80292B24(int index, int channel, hkPoweredChainMotorInput_80292B24* input,
                            hkPoweredChainMotorOutput_80292B24* output) {
    float value;
    hkPoweredChainMotorInput_80292B24* in = &input[index];
    hkPoweredChainMotorOutput_80292B24* out = &output[index];
    if ((((int)in->flags >> (channel * 2)) & 3) == 1)
        value = in->entries[channel].a;
    else
        value = in->entries[channel].b;
    out->values[channel] = value;
}

// fn_802921B0: body from https://github.com/jj-link/brawl src/havok/fn_802921B0.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802921B0 {
extern "C" {
extern "C" float lbl_805A3E00;

// TODO: cleanup (shape from jj-link/brawl fn_802921B0)
void fn_802921B0(unsigned char* arg0, float* arg1) {
    int flags = *arg0;
    if (flags != 0) {
        float v = lbl_805A3E00;
        for (int i = 0; i < 3; ++i) {
            if ((flags >> (i << 1)) & 3) {
                arg1[12 + i * 4 + 3] = v;
                arg1[12 + i * 4 + 2] = v;
                arg1[12 + i * 4 + 1] = v;
                arg1[12 + i * 4] = v;
                arg1[36 + i * 4 + 3] = v;
                arg1[36 + i * 4 + 2] = v;
                arg1[36 + i * 4 + 1] = v;
                arg1[36 + i * 4] = v;
            }
            flags = *arg0;
        }
    }
}
}
} // namespace jj_fn_802921B0

// fn_80292F3C: body from https://github.com/jj-link/brawl src/havok/fn_80292F3C.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkMatrix6.h>
namespace jj_fn_80292F3C {
struct Block {
    float v[8];
};

struct Context {
    int unk0;
    int count;
    char pad8[0x18];
    char* matrices;
    char pad24[8];
    Block* values;
};

extern "C" const float lbl_805A3E00;

// TODO: cleanup (shape from jj-link/brawl fn_80292F3C)
extern "C" void fn_80292F3C(Context* ctx, Block* out)
{
    Block tmp __attribute__((aligned(16)));
    int m, v;
    Block* base = out + 1;
    int i;

    base[-1].v[0] = base[-1].v[1] = base[-1].v[2] = base[-1].v[3] = lbl_805A3E00;
    base[-1].v[4] = base[-1].v[5] = base[-1].v[6] = base[-1].v[7] = lbl_805A3E00;

    for (i = 0, m = 0, v = 0; i < ctx->count;
         m += 0x3c0, v += 0x20, ++i) {
        const Block* p = (const Block*)((const char*)ctx->values + v);
        hkMatrix6SetMulV(*(hkVector8*)(&tmp), *(const hkMatrix6*)(ctx->matrices + m), *(const hkVector8*)(base + (i - 1)));

        const void* matrix = ctx->matrices + m + 0xc0;
        tmp.v[0] = p->v[0] - tmp.v[0];
        tmp.v[1] = p->v[1] - tmp.v[1];
        tmp.v[2] = p->v[2] - tmp.v[2];
        tmp.v[3] = p->v[3] - tmp.v[3];
        tmp.v[4] = p->v[4] - tmp.v[4];
        tmp.v[5] = p->v[5] - tmp.v[5];
        tmp.v[6] = p->v[6] - tmp.v[6];
        tmp.v[7] = p->v[7] - tmp.v[7];

        hkMatrix6SetMulV(*(hkVector8*)((Block*)((char*)base + v)), *(const hkMatrix6*)(matrix), *(const hkVector8*)(&tmp));
    }
}
} // namespace jj_fn_80292F3C

// TODO: cleanup (shape from jj-link/brawl fn_80291DE8)
// fn_80291DE8: body from https://github.com/jj-link/brawl src/havok/fn_80291DE8.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
namespace jj_fn_80291DE8 {
extern "C" void fn_80291DE8(float *dst, const float *src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    dst[6] = src[6];
    dst[7] = src[7];
    dst[8] = src[8];
    dst[9] = src[9];
    dst[10] = src[10];
    dst[11] = src[11];
    dst[12] = src[12];
    dst[13] = src[13];
    dst[14] = src[14];
    dst[15] = src[15];
    dst[16] = src[16];
    dst[17] = src[17];
    dst[18] = src[18];
    dst[19] = src[19];
    dst[20] = src[20];
    dst[21] = src[21];
    dst[22] = src[22];
    dst[23] = src[23];
    dst[24] = src[24];
    dst[25] = src[25];
    dst[26] = src[26];
    dst[27] = src[27];
    dst[28] = src[28];
    dst[29] = src[29];
    dst[30] = src[30];
    dst[31] = src[31];
    dst[32] = src[32];
    dst[33] = src[33];
    dst[34] = src[34];
    dst[35] = src[35];
    dst[36] = src[36];
    dst[37] = src[37];
    dst[38] = src[38];
    dst[39] = src[39];
    dst[40] = src[40];
    dst[41] = src[41];
    dst[42] = src[42];
    dst[43] = src[43];
    dst[44] = src[44];
    dst[45] = src[45];
    dst[46] = src[46];
    dst[47] = src[47];
}
} // namespace jj_fn_80291DE8
