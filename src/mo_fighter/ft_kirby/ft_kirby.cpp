#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/kirby/ft_kirby.h>
#include <ft/kirby/ft_kirby_extend_param_accesser.h>

#define FT_BC ftKirbyBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftKirbyExtendParamAccesser g_ftKirbyExtendParamAccesser;

ftClassInfoImpl<Fighter_Kirby, ftKirby> g_ftClassInfoKirby;

ftKirby::ftKirby(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftKirbyBuildConfig>(entryId,
                                         Fighter_Kirby,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftKirby is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftKirbyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftKirbyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_96_28D0(u8* p) { return *(u8*)(p + 0x4); }
int fn_96_BC6C(u8* p) { return *(int*)(p + 0x28); }
int fn_96_C17C(u8* p) { return *(int*)(p + 0xc0); }
int fn_96_F3AC(u8* p) { return *(int*)(p + 0x20); }
int fn_96_F488(u8* p) { return *(int*)(p + 0x18); }
int fn_96_115E8() { return 12; }
void fn_96_12530() {}
void fn_96_12C28() {}
void fn_96_12CAC() {}
void fn_96_12FA4() {}
void fn_96_1308C() {}
void fn_96_13174() {}
void fn_96_1325C() {}

// Kirby work and extend-param getters (map: ftKirby__getExtendParam, ftKirby__getWorkStone, ftKirby__isObserv).
void* fn_96_AF98(u8* p) { return *(u8**)(p + 0x1B678) + 0x7C; }
void* fn_96_B488(u8* p) { return p + 0x1B690; }
#pragma dont_inline on
int fn_96_EFD0(u8* p, s32 kind) { return (s8)kind == (s8)0xc; }

// Kirby overrides still undefined here (defined later in this unit).
void fn_96_B108(u8* p);
void fn_96_B2FC(u8* p);
void fn_96_C184(u8* p);
void fn_96_CE58(u8* p);
bool fn_96_D160(u8* p);
void fn_96_D334(u8* p);
bool fn_96_DA58(u8* p);
bool fn_96_DAEC(u8* p);
bool fn_96_DBA0(u8* p);
void fn_96_DCD8(u8* p);
void fn_96_DD50(u8* p);
bool fn_96_DEC4(u8* p);
bool fn_96_DF94(u8* p);
void fn_96_DBFC(u8* p);
// Fighter overrides defined in other translation units.
void fn_96_1DB1C(u8* p);
void fn_96_1C4E8(u8* p);

// Event thunks: a second-base adjustment (this - N) followed by a tail call to the Kirby override.
// (fn_96_1359C, the isObserv thunk, takes no second argument in the target and is left out.)
void fn_96_135A4(u8* p) { fn_96_C184(p - 0x54); }
void fn_96_135AC(u8* p) { fn_96_B108(p - 0x64); }
void fn_96_135B4(u8* p) { fn_96_B2FC(p - 0x70); }
bool fn_96_135BC(u8* p) { return fn_96_DBA0(p - 0x7c); }
bool fn_96_135D4(u8* p) { return fn_96_DAEC(p - 0x94); }
bool fn_96_135E4(u8* p) { return fn_96_DA58(p - 0x94); }
bool fn_96_135EC(u8* p) { return fn_96_DF94(p - 0xa0); }
bool fn_96_135FC(u8* p) { return fn_96_DEC4(p - 0xa0); }
bool fn_96_13614(u8* p) { return fn_96_D160(p - 0xb8); }
void fn_96_1361C(u8* p) { fn_96_CE58(p - 0xb8); }
void fn_96_13684(u8* p) { fn_96_D334(p - 0xe8); }
void fn_96_1368C(u8* p) { fn_96_DD50(p - 0xf4); }
void fn_96_13694(u8* p) { fn_96_DCD8(p - 0xf4); }
void fn_96_1369C(u8* p) { fn_96_DBFC(p - 0xf4); }

// Second-base forwarders to the out-of-unit Fighter overrides (MATCH-ONLY: same adjustment and tail call).
void fn_96_DB94(u8* p) { fn_96_1DB1C(p + 0x1B72C); }
void fn_96_DBFC(u8* p) { fn_96_1C4E8(p + 0x1B72C); }
#pragma dont_inline off

}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
u8* fn_96_12CB0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_96_12D60(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_96_12E10(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_96_12EC0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_96_12FA8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
