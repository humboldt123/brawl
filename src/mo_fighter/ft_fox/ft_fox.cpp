#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/fox/ft_fox.h>
#include <ft/fox/ft_fox_extend_param_accesser.h>
#include <so/so_value_accesser.h>

#define FT_BC ftFoxBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

ftFoxExtendParamAccesser g_ftFoxExtendParamAccesser;

ftClassInfoImpl<Fighter_Fox, ftFox> g_ftClassInfoFox;

ftFox::ftFox(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftFoxBuildConfig>(entryId,
                                         Fighter_Fox,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftFox is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftFoxInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftFoxInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers.
void fn_97_B000() {}
void fn_97_B004() {}
void fn_97_B008() {}
void fn_97_B00C() {}
void fn_97_B010() {}
void fn_97_B014() {}
void fn_97_B018() {}
void fn_97_B01C() {}
void fn_97_B020() {}
void fn_97_B024() {}
void fn_97_B028() {}
void fn_97_B02C() {}
void fn_97_B038() {}
void fn_97_B03C() {}
void fn_97_B040() {}
void fn_97_B044() {}
void fn_97_B070() {}
void fn_97_B074() {}
void fn_97_B078() {}
void fn_97_B07C() {}
void fn_97_B080() {}
void fn_97_B0A4() {}
void fn_97_B0A8() {}
void fn_97_B0AC() {}
void fn_97_B0B0() {}
void fn_97_B0C4() {}
void fn_97_B0C8() {}
void fn_97_B0CC() {}
void fn_97_B0D0() {}
void fn_97_B0D4() {}
void fn_97_B250() {}
void fn_97_B25C() {}
void fn_97_B268() {}
void fn_97_B26C() {}
void fn_97_B270() {}
void fn_97_B274() {}
void fn_97_B278() {}
void fn_97_D784() {}
void fn_97_DE94() {}
void fn_97_DF18() {}
void fn_97_E000() {}
void fn_97_E0E8() {}
void fn_97_E1D0() {}
void fn_97_E2B8() {}
void fn_97_E3A0() {}
void fn_97_E488() {}
void fn_97_E570() {}

// Constant and field accessors.
int fn_97_B778() { return g_soValueVariation; }
int fn_97_AE20(void*, s8 x) { return x == 12; }
u8 fn_97_8138(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_97_AEEC(u8* p) { return *(u8*)(p + 0x44); }
int fn_97_B084(u8* p) { return *(int*)(p + 0x110); }
u8* fn_97_B27C(u8* p) { return *(u8**)(p + 0x20); }
u8* fn_97_B358(u8* p) { return *(u8**)(p + 0x18); }
u8* fn_97_B198(u8* p) { return p + 0x458; }
u8* fn_97_B1A0(u8* p) { return p + 0x3C8; }
u8* fn_97_B1A8(u8* p) { return p + 0x8; }
u8* fn_97_C904(u8* p) { return p + 0x1C794; }
u8* fn_97_C924(u8* p) { return p + 0x1C7D0; }
u8* fn_97_C930(u8* p) { return p + 0x1CC88; }
void fn_97_C8F8(u8* p, u8 v) { *(u8*)(p + 0x14C14) = v; }
void fn_97_A308(float* dst, float* src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}
void fn_97_D86C(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_97_D878(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_97_D884(float* p) {
    p[0] = 0.0f;
    p[1] = 0.0f;
    p[2] = 0.0f;
}

// Sub-object pointer selectors by index (null for unknown indices).
u8* fn_97_C998(u8* p, int x) {
    if (x == 1) {
        return p + 0x1DF8;
    }
    if (x == 0) {
        return p + 0x10;
    }
    return 0;
}
u8* fn_97_C9C0(u8* p, int x) {
    if (x == 4) {
        return p + 0x860C;
    }
    if (x == 3) {
        return p + 0x6490;
    }
    if (x == 2) {
        return p + 0x4314;
    }
    if (x == 1) {
        return p + 0x2198;
    }
    if (x == 0) {
        return p + 0x1C;
    }
    return 0;
}
u8* fn_97_E1D4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_97_E2BC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }

// Bit flags in the u32 array at +0x1C.
bool fn_97_B2F8(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_97_B314(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_97_B32C(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_97_B340(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_97_B360(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_97_B388(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_97_B3A0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_97_B3B8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_97_B3D0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_97_B3E0(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_97_B3F8(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_97_B410(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_97_B428(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_97_B448(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_97_B460(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_97_B478(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_97_B490(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }

// Polymorphic view used only to emit virtual calls; slot k lives at vtable offset 8 + 4 * k.
struct ftFoxVirtualSlots {
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

// Fighter state flags at +0x1CD44 / +0x1CD45, cleared after the base Fighter call.
void fn_97_A100(u8* p, int x) {
    ((ftFoxVirtualSlots*)(*(void**)(p + 0x1CD48)))->slot8();
    ((Fighter*)p)->Fighter::onStart(x);
    *(u8*)(p + 0x1CD45) = 0;
    *(u8*)(p + 0x1CD44) = 0;
}
void fn_97_A168(u8* p) {
    ((Fighter*)p)->Fighter::processUpdate();
    *(u8*)(p + 0x1CD45) = 0;
    *(u8*)(p + 0x1CD44) = 0;
}
void fn_97_C970(void* p) { return ((ftFoxVirtualSlots*)p)->slot20(); }
void fn_97_C910(u8* p) { return ((ftFoxVirtualSlots*)(p + 0x1C794))->slot2(); }

}
