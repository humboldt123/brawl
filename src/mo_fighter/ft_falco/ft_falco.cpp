#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/falco/ft_falco.h>
#include <ft/falco/ft_falco_extend_param_accesser.h>

#define FT_BC ftFalcoBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftFalcoExtendParamAccesser g_ftFalcoExtendParamAccesser;

ftClassInfoImpl<Fighter_Falco, ftFalco> g_ftClassInfoFalco;

ftFalco::ftFalco(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftFalcoBuildConfig>(entryId,
                                         Fighter_Falco,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

ftFalco::~ftFalco() { }

void ftFalco::processUpdate() {
    Fighter::processUpdate();
    unk1D091 = 0;
    unk1D090 = 0;
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftFalco is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftFalcoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftFalcoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Small functions of this translation unit (empty virtuals, constant returns, field accessors and work-area
// operations) under their symbol names from the REL map. The bodies are the leaf code the REL contains.
extern int g_soValueVariation;

extern "C" {

// Empty bodies.
void fn_108_AEA4() {}
void fn_108_AEA8() {}
void fn_108_AEAC() {}
void fn_108_AEB0() {}
void fn_108_AEB4() {}
void fn_108_AEB8() {}
void fn_108_AEBC() {}
void fn_108_AEC0() {}
void fn_108_AEC4() {}
void fn_108_AEC8() {}
void fn_108_AECC() {}
void fn_108_AED0() {}
void fn_108_AEDC() {}
void fn_108_AEE0() {}
void fn_108_AEE4() {}
void fn_108_AEE8() {}
void fn_108_AF14() {}
void fn_108_AF18() {}
void fn_108_AF1C() {}
void fn_108_AF20() {}
void fn_108_AF24() {}
void fn_108_AF48() {}
void fn_108_AF4C() {}
void fn_108_AF50() {}
void fn_108_AF54() {}
void fn_108_AF68() {}
void fn_108_AF6C() {}
void fn_108_AF70() {}
void fn_108_AF74() {}
void fn_108_AF78() {}
void fn_108_D5F4() {}
void fn_108_DD00() {}
void fn_108_DD84() {}
void fn_108_DE6C() {}
void fn_108_DF54() {}
void fn_108_E03C() {}
void fn_108_E124() {}
void fn_108_E20C() {}
void fn_108_E2F4() {}
void fn_108_E3DC() {}

// Field accessors.
u8 fn_108_8008(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_108_AD90(u8* p) { return *(u8*)(p + 0x44); }
int fn_108_AF28(u8* p) { return *(int*)(p + 0x110); }
int fn_108_B0EC(u8* p) { return *(int*)(p + 0x20); }
int fn_108_B1C8(u8* p) { return *(int*)(p + 0x18); }
u8* fn_108_B03C(u8* p) { return p + 0x458; }
u8* fn_108_B044(u8* p) { return p + 0x3C8; }
u8* fn_108_B04C(u8* p) { return p + 0x8; }
int fn_108_B5E8() { return g_soValueVariation; }
void fn_108_D6DC(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_108_D6E8(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_108_C768(u8* p, u8 v) { *(u8*)(p + 0x14F84) = v; }
u8* fn_108_C774(u8* p) { return p + 0x1CAE0; }
u8* fn_108_C794(u8* p) { return p + 0x1CB1C; }
u8* fn_108_C7A0(u8* p) { return p + 0x1CFD4; }
u8* fn_108_C808(u8* p, s32 index) {
    if (index == 1) { return p + 0x1FB4; }
    if (index == 0) { return p + 0x10; }
    return NULL;
}
u8* fn_108_E040(u8* p, s32 index) {
    if (index == 0) { return p + 0xC; }
    return NULL;
}
u8* fn_108_E128(u8* p, s32 index) {
    if (index == 0) { return p + 0xC; }
    return NULL;
}

// Flag work area (array at +0x1C).
bool fn_108_B168(u8* p, u32 mask, u32 idx) { return (*(u32**)(p + 0x1C))[idx] & mask; }
void fn_108_B184(u8* p, u32 mask, u32 idx) { u32* flags = *(u32**)(p + 0x1C); flags[idx] &= ~mask; }
void fn_108_B19C(u8* p, u32 idx) { u32* flags = *(u32**)(p + 0x1C); flags[idx] = 0; }
void fn_108_B1B0(u8* p, u32 mask, u32 idx) { u32* flags = *(u32**)(p + 0x1C); flags[idx] |= mask; }

// Int work area (array at +0xC).
s32 fn_108_B268(u8* p, u32 idx) { s32* ints = *(s32**)(p + 0xC); s32 v = ints[idx]; ints[idx] = v - 1; return v; }
s32 fn_108_B280(u8* p, u32 idx) { s32* ints = *(s32**)(p + 0xC); s32 v = ints[idx]; ints[idx] = v + 1; return v; }
void fn_108_B298(u8* p, s32 divisor, u32 idx) { if (divisor == 0) { return; } s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] / divisor; }
void fn_108_B2B8(u8* p, s32 factor, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] * factor; }
void fn_108_B2D0(u8* p, s32 value, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] - value; }
void fn_108_B2E8(u8* p, s32 value, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] + value; }
void fn_108_B300(u8* p, s32 value, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = value; }

// Float work area (array at +0x14).
void fn_108_B1F8(u8* p, u32 idx, float factor) { float* floats = *(float**)(p + 0x14); floats[idx] *= factor; }
void fn_108_B210(u8* p, u32 idx, float value) { float* floats = *(float**)(p + 0x14); floats[idx] -= value; }
void fn_108_B228(u8* p, u32 idx, float value) { float* floats = *(float**)(p + 0x14); floats[idx] += value; }
void fn_108_B240(u8* p, u32 idx, float value) { float* floats = *(float**)(p + 0x14); floats[idx] = value; }
float fn_108_B250(u8* p, u32 idx) { float* floats = *(float**)(p + 0x14); return floats[idx]; }

}
