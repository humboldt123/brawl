#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/wolf/ft_wolf_builder.h>

#define FT_BC ftWolfBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftClassInfoImpl<Fighter_Wolf, ftWolf> g_ftClassInfoWolf;

ftWolf::ftWolf(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftWolfBuildConfig>(entryId,
                                         Fighter_Wolf,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftWolf is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftWolfInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftWolfInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;


void ftWolf::onStart(int startKind) {
    // Discard old position samples before the shared fighter startup.
    m_postureHistory->clear();
    Fighter::onStart(startKind);
}

void ftWolf::processUpdate() {
    Fighter::processUpdate();
    // Reflector collision results are consumed within one update, then reset.
    unk1cc89 = 0;
    unk1cc88 = 0;
}

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers.
void fn_125_9F74() {}
void fn_125_DE30() {}
void fn_125_DF18() {}
void fn_125_E000() {}
void fn_125_E0E8() {}
void fn_125_E1D0() {}
void fn_125_E2B8() {}
void fn_125_E3A0() {}
void fn_125_E488() {}

// Constant-zero returns. The parameters are unused; they exist so callers can pass arguments.
int fn_125_DEDC(void*, void*, void*) { return 0; }
int fn_125_DFC4(void*, void*, void*) { return 0; }
int fn_125_E0AC(void*, void*, void*) { return 0; }
int fn_125_E194(void*, void*, void*) { return 0; }
int fn_125_E27C(void*, void*, void*) { return 0; }
int fn_125_E364(void*, void*, void*) { return 0; }
int fn_125_E44C(void*, void*, void*) { return 0; }
int fn_125_E534(void*, void*, void*) { return 0; }

} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_125_1EAC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_3720(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_3F78(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_42F0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_45C8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_4608(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_4CD8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_4E6C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_6130(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_6170(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_61B0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
int fn_125_AAEC() { return 0; }
int fn_125_B160() { return 0x1; }
void fn_125_B168() {}
int fn_125_B16C() { return 0x0; }
void fn_125_B174() {}
int fn_125_B178() { return 0x0; }
void fn_125_B180() {}
void fn_125_B184() {}
void fn_125_B188() {}
void fn_125_B18C() {}
void fn_125_B190() {}
void fn_125_DDAC() {}
u8* fn_125_DE34(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_DF1C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E004(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E0EC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E1D4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E2BC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E3A4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E48C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
