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
