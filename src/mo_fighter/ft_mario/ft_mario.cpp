#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/mario/ft_mario.h>
#include <ft/mario/ft_mario_extend_param_accesser.h>

#define FT_BC ftMarioBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
// Parameter-table variation getter (soValueAccesser::getValueVariation) under its map address.
extern "C" int fn_91_ABE8() { return g_soValueVariation; }

ftMarioExtendParamAccesser g_ftMarioExtendParamAccesser;
ftMarioDExtendParamAccesser g_ftMarioDExtendParamAccesser;

ftClassInfoImpl<Fighter_Mario, ftMario> g_ftClassInfoMario;
ftClassInfoImpl<Fighter_MarioD, ftMarioD> g_ftClassInfoMarioD;

ftMario::ftMario(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftMarioBuildConfig>(entryId,
                                         Fighter_Mario,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftMario is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftMarioInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftMarioInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Empty virtual overrides of this module (ftEntryEventObserver / Fighter no-ops), under their map addresses.
extern "C" {
void fn_91_A4A4() {}
void fn_91_A4A8() {}
void fn_91_A4AC() {}
void fn_91_A4B0() {}
void fn_91_A4B4() {}
void fn_91_A4B8() {}
void fn_91_A4BC() {}
void fn_91_A4C0() {}
void fn_91_A4C4() {}
void fn_91_A4C8() {}
void fn_91_A4CC() {}
void fn_91_A4D0() {}
void fn_91_A4DC() {}
void fn_91_A4E0() {}
void fn_91_A4E4() {}
void fn_91_A4E8() {}
void fn_91_A514() {}
void fn_91_A518() {}
void fn_91_A51C() {}
void fn_91_A520() {}
void fn_91_A524() {}
void fn_91_A548() {}
void fn_91_A54C() {}
void fn_91_A550() {}
void fn_91_A554() {}
void fn_91_A568() {}
void fn_91_A56C() {}
void fn_91_A570() {}
void fn_91_A574() {}
void fn_91_A578() {}

}

// Field accessors of this module under their map addresses (offsets read from the target code).
extern "C" {

u8 fn_91_22B4(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_91_A0AC(u8* p) { return *(u8*)(p + 0x44); }
int fn_91_A528(u8* p) { return *(int*)(p + 0x110); }
bool fn_91_9D0C(void* self, s8 kind) { return kind == 0xc; }

}

// Work-area accessors of soGeneralWorkSimple, the virtual-node matrix getters and the fighter-builder
// getters emitted in this unit, under their map addresses (offsets read from the target code).
extern "C" {

u8* fn_91_A63C(u8* p) { return p + 0x458; }
u8* fn_91_A644(u8* p) { return p + 0x3c8; }
u8* fn_91_A64C(u8* p) { return p + 0x8; }
int fn_91_A6EC(u8* p) { return *(int*)(p + 0x20); }
int fn_91_A7C8(u8* p) { return *(int*)(p + 0x18); }
void fn_91_A840(u8* p, int idx, float v) { ((float*)*(u8**)(p + 0x14))[idx] = v; }
float fn_91_A850(u8* p, int idx) { return ((float*)*(u8**)(p + 0x14))[idx]; }
void fn_91_A7F8(u8* p, int idx, float v) { ((float*)*(u8**)(p + 0x14))[idx] *= v; }
void fn_91_A810(u8* p, int idx, float v) { ((float*)*(u8**)(p + 0x14))[idx] -= v; }
void fn_91_A828(u8* p, int idx, float v) { ((float*)*(u8**)(p + 0x14))[idx] += v; }
void fn_91_A900(u8* p, int v, int idx) { ((int*)*(u8**)(p + 0xc))[idx] = v; }
int fn_91_A868(u8* p, int idx) {
    int* a = *(int**)(p + 0xc);
    int t = a[idx];
    a[idx] = t - 1;
    return t;
}
int fn_91_A880(u8* p, int idx) {
    int* a = *(int**)(p + 0xc);
    int t = a[idx];
    a[idx] = t + 1;
    return t;
}
void fn_91_A898(u8* p, int v, int idx) {
    if (v == 0) {
        return;
    }
    int* a = *(int**)(p + 0xc);
    a[idx] = a[idx] / v;
}
void fn_91_A8B8(u8* p, int v, int idx) { int* a = *(int**)(p + 0xc); a[idx] = a[idx] * v; }
void fn_91_A8D0(u8* p, int v, int idx) { int* a = *(int**)(p + 0xc); a[idx] = a[idx] - v; }
void fn_91_A8E8(u8* p, int v, int idx) { int* a = *(int**)(p + 0xc); a[idx] = a[idx] + v; }
bool fn_91_A768(u8* p, u32 mask, int idx) { u32* a = *(u32**)(p + 0x1c); return (a[idx] & mask) != 0; }
void fn_91_A784(u8* p, u32 mask, int idx) { u32* a = *(u32**)(p + 0x1c); a[idx] &= ~mask; }
void fn_91_A79C(u8* p, int idx) { u32* a = *(u32**)(p + 0x1c); a[idx] = 0; }
void fn_91_A7B0(u8* p, u32 mask, int idx) { u32* a = *(u32**)(p + 0x1c); a[idx] |= mask; }
void fn_91_CD00(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_91_CD0C(u8* p) { *(u8*)(p + 0x31) = 1; }
u8* fn_91_BD50(u8* p) { return p + 0x2D634; }
u8* fn_91_BD70(u8* p) { return p + 0x2D670; }
u8* fn_91_BD7C(u8* p) { return p + 0x2DB28; }
void fn_91_CC18() {}
void fn_91_D328() {}
void fn_91_D3AC() {}
void fn_91_D494() {}
void fn_91_D57C() {}
void fn_91_D664() {}
void fn_91_D74C() {}
void fn_91_D834() {}
void fn_91_D91C() {}
void fn_91_DA04() {}

}

// Float helpers: divide is guarded by a zero test.
extern "C" {

void fn_91_A7D0(u8* p, int idx, float v) {
    if (v == 0.0f) {
        return;
    }
    ((float*)*(u8**)(p + 0x14))[idx] /= v;
}
void fn_91_CD18(float* p) {
    p[0] = 0.0f;
    p[1] = 0.0f;
    p[2] = 0.0f;
}

}

// Fighter-builder cancel query, forwarding to the cancel module through its virtual interface.
extern "C" bool fn_91_BD5C(u8* p) { return ((ftCancelModule*)(p + 0x2D634))->isEnableCancel(); }

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
u8* fn_91_D580(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_91_D668(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_91_D750(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
