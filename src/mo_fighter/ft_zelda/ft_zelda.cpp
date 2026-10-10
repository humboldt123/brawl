#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/zelda/ft_zelda.h>
#include <ft/zelda/ft_zelda_extend_param_accesser.h>

#define FT_BC ftZeldaBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftZeldaExtendParamAccesser g_ftZeldaExtendParamAccesser;

ftClassInfoImpl<Fighter_Zelda, ftZelda> g_ftClassInfoZelda;

ftZelda::ftZelda(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftZeldaBuildConfig>(entryId,
                                         Fighter_Zelda,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftZelda is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftZeldaInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftZeldaInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtuals and no-op helpers.
void fn_104_A024() {}
void fn_104_A470() {}
void fn_104_A5CC() {}
void fn_104_A5D8() {}
void fn_104_A618() {}
void fn_104_A61C() {}
void fn_104_A6AC() {}
void fn_104_A6D8() {}
void fn_104_A6DC() {}
void fn_104_A6E0() {}
void fn_104_A6E4() {}
void fn_104_A6E8() {}
void fn_104_A6EC() {}
void fn_104_A6F0() {}
void fn_104_A6F4() {}
void fn_104_A6F8() {}
void fn_104_A6FC() {}
void fn_104_A700() {}
void fn_104_A704() {}
void fn_104_A708() {}
void fn_104_A70C() {}
void fn_104_A710() {}
void fn_104_A71C() {}
void fn_104_A720() {}
void fn_104_A724() {}
void fn_104_A728() {}
void fn_104_A754() {}
void fn_104_A758() {}
void fn_104_A75C() {}
void fn_104_A760() {}
void fn_104_A764() {}
void fn_104_A788() {}
void fn_104_A78C() {}
void fn_104_A790() {}
void fn_104_A79C() {}
void fn_104_A7A0() {}
void fn_104_A7A4() {}
void fn_104_A7A8() {}
void fn_104_A7AC() {}
void fn_104_A7B8() {}
void fn_104_98C0() {}
void fn_104_CDA8() {}
void fn_104_D4B8() {}
void fn_104_D53C() {}
void fn_104_D624() {}
void fn_104_D70C() {}
void fn_104_D7F4() {}
void fn_104_D8DC() {}
void fn_104_D9C4() {}
void fn_104_DAAC() {}
void fn_104_DB94() {}

// Constant returns.
int fn_104_A304() { return 0; }
int fn_104_A714() { return 0; }
int fn_104_A780() { return 0; }
int fn_104_A794() { return 0; }
int fn_104_A7BC() { return 0; }
int fn_104_AB68() { return 0; }
int fn_104_D5E8() { return 0; }
int fn_104_D6D0() { return 0; }
int fn_104_D7B8() { return 0; }
int fn_104_D8A0() { return 0; }
int fn_104_D988() { return 0; }
int fn_104_DA70() { return 0; }
int fn_104_DB58() { return 0; }
int fn_104_DC40() { return 0; }
int fn_104_A7B0() { return 0x7ffe; }
int fn_104_BF28() { return 4; }
int fn_104_D4F4() { return 8; }

// Field getters.
u8 fn_104_7930(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_104_A5D0(u8* p) { return *(u8*)(p + 0x44); }
int fn_104_A768(u8* p) { return *(int*)(p + 0x110); }
int fn_104_A934(u8* p) { return *(int*)(p + 0x20); }
int fn_104_AA10(u8* p) { return *(int*)(p + 0x18); }
int fn_104_AAA8(u8* p) { return *(int*)(p + 0x10); }

// Sub-object pointers.
u8* fn_104_A884(u8* p) { return p + 0x458; }
u8* fn_104_A88C(u8* p) { return p + 0x3C8; }
u8* fn_104_A894(u8* p) { return p + 0x8; }
u8* fn_104_A89C(u8* p) { return p + 0x84; }
u8* fn_104_A8A4(u8* p) { return p + 0x70; }
u8* fn_104_A8AC(u8* p) { return p + 0x5C; }
u8* fn_104_A8B4(u8* p) { return p + 0x48; }
u8* fn_104_A8BC(u8* p) { return p + 0x34; }
u8* fn_104_A8C4(u8* p) { return p + 0x20; }
u8* fn_104_A8CC(u8* p) { return p + 0x8; }
u8* fn_104_A7C4(u8* p, u32 idx) { return p + 4 + (idx << 3); }

// Byte flags.
void fn_104_CE90(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_104_CE9C(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_104_D4FC(s16* dst, s16* src) { *dst = *src; }

// Bit flags in the u32 array at +0x1C.
bool fn_104_A9B0(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_104_A9CC(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_104_A9E4(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_104_A9F8(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_104_AA18(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_104_AA40(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_104_AA58(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_104_AA70(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_104_AA88(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_104_AA98(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_104_AAB0(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_104_AAC8(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_104_AAE0(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_104_AB00(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_104_AB18(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_104_AB30(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_104_AB48(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }

// Conditional sub-object pointers selected by an index.
u8* fn_104_BFB8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_104_BFD0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_104_BFE8(u8* p, int x) {
    if (x == 2) {
        return p + 0x3E4C;
    }
    if (x == 1) {
        return p + 0x1F30;
    }
    if (x == 0) {
        return p + 0x14;
    }
    return 0;
}
u8* fn_104_C020(u8* p, int x) {
    if (x == 2) {
        return p + 0x41CC;
    }
    if (x == 1) {
        return p + 0x20F0;
    }
    if (x == 0) {
        return p + 0x14;
    }
    return 0;
}

// Single-store and pointer-chain accessors.
void fn_104_BF30(u8* p, u8 v) { *(u8*)(p + 0x10000 - 0x17C) = v; }
u8* fn_104_9974(u8* p) { return *(u8**)(p + 0x18670) + 0x7C; }

// Virtual destructors: delete the object when the flag is positive, then return it.
void* fn_104_1E70(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3804(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3C90(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3CD0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3D10(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3E0C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3EE8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_421C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_4618(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_4978(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_49B8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_6AD0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

// Polymorphic view used only to emit virtual calls; the slots are never defined in this unit.
struct ftZeldaFlagByte {
    u8 flag : 1;
    u8 rest : 7;
};
struct ftZeldaVirtualSlots {
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

// Calls the slot-1 virtual of the object when byte 5 bit 7 is set and byte 6 is clear.

// Tail-call virtual slot (offset 0x58) of the object.
void fn_104_BFA8(void* p) { return ((ftZeldaVirtualSlots*)p)->slot20(); }

}
