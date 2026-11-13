// Havok translation unit hkConstraintSolverSetup.o (main.dol 0x802DF6E4-0x802E0E40).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802DF6E4    28  integrate   [map: hkConstraintSolverSetup__integrate]
//   0x802DF700   192  oneStepIntegrate   [map: hkConstraintSolverSetup__oneStepIntegrate]
//   0x802DF7C0  2528  solve   [map: hkConstraintSolverSetup__solve]
//   0x802E01A0   356  initializeSolverState   [map: hkConstraintSolverSetup__initializeSolverState]
//   0x802E0304     4  shutdownSolver   [map: hkConstraintSolverSetup__shutdownSolver]
//   0x802E0308   916  internalAddAccumulators   [map: hkConstraintSolverSetup__internalAddAccumulators]
//   0x802E069C   828  internalAddJacobianElements   [map: hkConstraintSolverSetup__internalAddJacobianElements]
//   0x802E09D8    60  internalIsMemoryOkForNewAccumulators   [map: hkConstraintSolverSetup__internalIsMemoryOkForNewAccumulators]
//   0x802E0A14   880  internalIsMemoryOkForNewJacobianElements   [map: hkConstraintSolverSetup__internalIsMemoryOkForNewJacobianElements]
//   0x802E0D84   188  subSolve   [map: hkConstraintSolverSetup__subSolve]

#include <havok/hkConstraintSolverSetup.h>
#include <havok/hkRigidMotionUtil.h>
#include <havok/hkEntity.h>

void hkConstraintSolverSetup::integrate(void* a, void* b, void* c) {
    hkRigidMotionUtil::hkRigidMotionUtilApplyAccumulators(*(const hkStepInfo*)this,
                                                          (hkVelocityAccumulator*)((u8*)c + 0x80), (hkEntity**)a,
                                                          (int)b, 0xa0, a);
}

void hkConstraintSolverSetup::oneStepIntegrate(hkEntity** entities, int count, u8* base) {
    for (int i = 0; i < count; i++) {
        hkEntity* entity = entities[i];
        u8* state = base + entity->m_solverData;
        float* src = (float*)(state + 0x10);
        float* dst = (float*)(state + 0x40);
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        void* record[2];
        record[0] = (u8*)entity + 0xa0;
        hkRigidMotionUtil::hkRigidMotionUtilApplyAccumulators(*(const hkStepInfo*)this, (hkVelocityAccumulator*)state,
                                                              (hkEntity**)record, 1, 0, (u8*)entity + 0xa0);
    }
}

void hkConstraintSolverSetup::shutdownSolver() {}

// Function body below is taken from https://github.com/jj-link/brawl (src/havok/fn_802E09D8.cpp).
// hkConstraintSolverSetup::internalIsMemoryOkForNewAccumulators
// TODO: cleanup (shape from jj-link/brawl fn_802E09D8)
extern "C" int fn_802E09D8(const char* o, int, int n) {
    unsigned int a = *(const unsigned int*)(o + 0x4C);
    unsigned int b = *(const unsigned int*)(o + 0xC);
    int d = *(const int*)(o + 0x10) - (int)a;
    if (a == b) n++;
    if ((n << 7) > d - 0x10) return 0;
    return 0x1000000;
}

// fn_802E0D84: body from https://github.com/jj-link/brawl src/havok/fn_802E0D84.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802E0D84 {
extern "C" void fn_802987AC(void*, u32, u32, u32, u32);
extern "C" f32 lbl_805A4310;

struct Fn802E0D84Obj {
    u8 pad0[0x4];
    void* unk4;
    u8 pad8[0x4];
    u32 unkC;
    u8 pad10[0xC];
    u32 unk1C;
    u8 pad20[0x4];
    u32 unk24;
    u8 pad28[0x4];
    u32 unk2C;
    u8 pad30[0x4];
    u32 unk34;
    u32 unk38;
    u32 unk3C;
    u8 pad40[0x4];
    u32 unk44;
    u32 unk48;
    u32 unk4C;
    u32 unk50;
    f32* unk54;
    f32* unk58;
};

// TODO: cleanup (shape from jj-link/brawl fn_802E0D84)
extern "C" void fn_802E0D84(Fn802E0D84Obj* self, s32 arg1) {
    f32* p;
    f32* end;
    f32 val;

    if (arg1 == 0) {
        self->unk38 = self->unk2C;
        self->unk48 = self->unk3C;
    }
    p = self->unk58;
    val = lbl_805A4310;
    while (p < (end = self->unk54)) {
        *p = val;
        p++;
    }
    self->unk58 = end;
    if (self->unk38 != self->unk34) {
        fn_802987AC(self->unk4, self->unk38, self->unkC, self->unk1C, self->unk24);
    }
    if (self->unk48 != self->unk44) {
        fn_802987AC(self->unk4, self->unk48, self->unkC, self->unk1C, self->unk24);
    }
    self->unk38 = self->unk34;
    self->unk48 = self->unk44;
}
} // namespace jj_fn_802E0D84
