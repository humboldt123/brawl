#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/wario/ft_wario_builder.h>

#define FT_BC ftWarioBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftClassInfoImpl<Fighter_Wario, ftWario> g_ftClassInfoWario;

ftWario::ftWario(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftWarioBuildConfig>(entryId,
                                         Fighter_Wario,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftWario is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftWarioInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftWarioInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers.
void fn_110_9464() {}
void fn_110_9470() {}
void fn_110_94B0() {}
void fn_110_94B4() {}
void fn_110_9544() {}
void fn_110_9570() {}
void fn_110_9574() {}
void fn_110_9578() {}
void fn_110_957C() {}
void fn_110_9580() {}
void fn_110_9584() {}
void fn_110_9588() {}
void fn_110_958C() {}
void fn_110_9590() {}
void fn_110_9594() {}
void fn_110_9598() {}
void fn_110_959C() {}
void fn_110_95A0() {}
void fn_110_95A4() {}
void fn_110_95A8() {}
void fn_110_95B4() {}
void fn_110_95B8() {}
void fn_110_95BC() {}
void fn_110_95C0() {}
void fn_110_95EC() {}
void fn_110_95F0() {}
void fn_110_95F4() {}
void fn_110_95F8() {}
void fn_110_95FC() {}
void fn_110_9620() {}
void fn_110_9624() {}
void fn_110_9628() {}
void fn_110_9634() {}
void fn_110_9638() {}
void fn_110_963C() {}
void fn_110_9648() {}
void fn_110_9654() {}
void fn_110_9658() {}
void fn_110_B824() {}
void fn_110_BF34() {}
void fn_110_BFB8() {}
void fn_110_C0A0() {}
void fn_110_C188() {}
void fn_110_C270() {}
void fn_110_C358() {}
void fn_110_C440() {}
void fn_110_C528() {}
void fn_110_C610() {}

// Work-array sizes and field read.
u8 fn_110_9468(u8* p) { return *(u8*)(p + 0x44); }
int fn_110_97CC(u8* p) { return *(int*)(p + 0x20); }
int fn_110_98A8(u8* p) { return *(int*)(p + 0x18); }
int fn_110_9940(u8* p) { return *(int*)(p + 0x10); }

// Bit flags in the u32 array at +0x1C (soGeneralWorkSimple).
bool fn_110_9848(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_110_9864(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_110_987C(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_110_9890(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_110_98B0(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_110_98D8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_110_98F0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_110_9908(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_110_9920(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_110_9930(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_110_9948(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_110_9960(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_110_9978(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_110_9998(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_110_99B0(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_110_99C8(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_110_99E0(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }
int fn_110_99F0(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); return a[idx]; }

// Sub-object accessors (ftVirtualNodeMatrixPool, ftStatusGimmickUniqProcessPool, ftSound3dGeneratorAccesser).
u8* fn_110_971C(u8* p) { return p + 0x458; }
u8* fn_110_9724(u8* p) { return p + 0x3C8; }
u8* fn_110_972C(u8* p) { return p + 0x8; }
u8* fn_110_9734(u8* p) { return p + 0x84; }
u8* fn_110_973C(u8* p) { return p + 0x70; }
u8* fn_110_9744(u8* p) { return p + 0x5C; }
u8* fn_110_974C(u8* p) { return p + 0x48; }
u8* fn_110_9754(u8* p) { return p + 0x34; }
u8* fn_110_975C(u8* p) { return p + 0x20; }
u8* fn_110_9764(u8* p) { return p + 0x8; }
u8* fn_110_965C(u8* p, u32 idx) { return p + 4 + (idx << 3); }

// Field reads and constant returns.
int fn_110_9600(u8* p) { return *(int*)(p + 0x110); }
int fn_110_9618(u8* p) { return 0; }
int fn_110_91B8(u8* p) { return 0; }
bool fn_110_9A00(u8* p) { return false; }
bool fn_110_8EF4(u8* p, s8 kind) { return kind == 12; }
int fn_110_A9BC(u8* p) { return 2; }
void fn_110_A9C4(u8* p, u8 v) { *(u8*)(p + 0x4B0C) = v; }

// Cancel / pool sub-object accessors on the fighter (+0x10000 base adjusted).
u8* fn_110_A9CC(u8* p) { return p + 0xCD4C; }
u8* fn_110_A9EC(u8* p) { return p + 0xCD88; }
u8* fn_110_A9F8(u8* p) { return p + 0xD240; }

// Single-node gate on an index.
u8* fn_110_AA48(u8* p, u32 idx) { return idx == 0 ? p + 0xC : 0; }
u8* fn_110_AA60(u8* p, u32 idx) { return idx == 0 ? p + 0xC : 0; }

// Ground friction flag at +0x31.
void fn_110_B90C(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_110_B918(u8* p) { *(u8*)(p + 0x31) = 1; }

// Destructor wrappers: delete the object when the flag is positive, then return it.
// Sub-object destructor wrappers first run the sibling wrapper with flags 0.
#pragma dont_inline on
void* fn_110_1E6C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_1F08(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_22C4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_2304(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_3A60(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_3D64(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_3E14(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_40EC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_41B4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_451C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4804(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4844(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4FB4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4FF4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_5034(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_5074(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_6000(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

void* fn_110_1EAC(void* p, s16 flags) {
    if (p != 0) {
        fn_110_1F08(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_3D08(void* p, s16 flags) {
    if (p != 0) {
        fn_110_3D64(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_408C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_40EC((u8*)p + 0xC, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_50B4(void* p, s16 flags) {
    if (p != 0) {
        fn_110_3D08((u8*)p + 0x1C0, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_5C9C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_3D08((u8*)p + 0x88, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_6620(void* p, s16 flags) {
    if (p != 0) {
        fn_110_451C(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AAD8(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_ACB8(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AD14(void* p, s16 flags) {
    if (p != 0) {
        fn_110_ACB8((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AD74(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AD14((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B4A0(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B4FC(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B4A0((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B55C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B4FC((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B634(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B690(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B634((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B6F0(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B690((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AA78(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AB34(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AA78((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AB94(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AB34((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_ABF4(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AB94((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AC54(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_AB94((u8*)p + 0x4, -1); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AE44(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AEA4(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AE44((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AF04(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AEA4((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AFDC(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B03C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AFDC((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B09C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B03C((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B174(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AFDC(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B1D0(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B174((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B230(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B1D0((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B308(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B368(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B308((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B3C8(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B368((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_65C0(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_451C(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
#pragma dont_inline reset


// Field read, constant returns and a global value-variation pointer.
u8 fn_110_6E04(u8* p) { return *(u8*)(p + 0x4); }
int fn_110_962C(u8* p) { return 0; }
int fn_110_9640(u8* p) { return 0x7FAD; }
int fn_110_964C(u8* p) { return 0; }
extern void* g_soValueVariation;
void* fn_110_9CC8() { return g_soValueVariation; }

// Virtual slot tail calls (vtable offset 8+4k for slot k).
struct ftWarioVSlotsB {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
};
struct ftWarioVSlotsI {
    virtual int slot0();
    virtual int slot1();
    virtual int slot2();
    virtual int slot3();
};
void fn_110_AA38(u8* p) { ((ftWarioVSlotsB*)p)->slot20(); }
int fn_110_A9D8(u8* p) { return ((ftWarioVSlotsI*)(p + 0xCD4C))->slot2(); }


// Vector zeroing (float triple at the start of the object).
void fn_110_B924(float* p) { p[0] = 0.0f; p[1] = 0.0f; p[2] = 0.0f; }

// Virtual slot 3 result compared against zero.
bool fn_110_AA04(u8* p) { return ((ftWarioVSlotsI*)p)->slot3() == 0; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_110_C914(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_110_C91C(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_110_C924(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
void fn_110_C954(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_110_C95C(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_110_C964(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_110_C96C(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_110_C974(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_110_C98C(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_110_C9A4(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_110_C9B4(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
bool fn_110_C9C4(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_110_C9CC(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_110_C9D4(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_110_C9DC(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_110_C9E4(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_110_C9EC(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_110_C9F4(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_110_C9FC(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_110_CA04(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_110_CA0C(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_110_CA14(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_110_CA1C(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_110_CA24(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_110_CA2C(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_110_CA34(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_110_CA3C(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_110_CA44(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_110_CA4C(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_110_C90C(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_110_C97C(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_110_C984(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_110_C994(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_110_C99C(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
void fn_110_C9AC(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"
