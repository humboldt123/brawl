#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/ike/ft_ike.h>
#include <ft/ike/ft_ike_extend_param_accesser.h>

#define FT_BC ftIkeBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftIkeExtendParamAccesser g_ftIkeExtendParamAccesser;

ftClassInfoImpl<Fighter_Ike, ftIke> g_ftClassInfoIke;

ftIke::ftIke(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftIkeBuildConfig>(entryId,
                                         Fighter_Ike,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftIke is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftIkeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftIkeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Resource-id accesser teardown is an empty out-of-line definition (same as ft_marth.cpp).
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_119_58E4(u8* p) { return *(u8*)(p + 0x4); }
void fn_119_7610() {}
int fn_119_91E4() { return 0; }
void fn_119_9698() {}
void fn_119_969C() {}
void fn_119_96A0() {}
void fn_119_96A4() {}
int fn_119_96A8() { return 0; }
void fn_119_96EC() {}
void fn_119_96F0() {}
void fn_119_96F4() {}
void fn_119_96F8() {}
int fn_119_9714() { return 0; }
int fn_119_9728() { return 0; }
int fn_119_9730() { return 0; }
void fn_119_974C() {}
u8* fn_119_9810(u8* p) { return p + 0x458; }
u8* fn_119_9818(u8* p) { return p + 0x3C8; }
u8* fn_119_9820(u8* p) { return p + 0x8; }
int fn_119_9860(u8* p) { return *(int*)(p + 0x20); }
int fn_119_993C(u8* p) { return *(int*)(p + 0x18); }
int fn_119_99D4(u8* p) { return *(int*)(p + 0x10); }
int fn_119_A89C() { return 1; }
void fn_119_A8A4(u8* p, u8 v) { *(u8*)(p + 0x2318) = v; }
u8* fn_119_A8AC(u8* p) { return p + 0xA5CC; }
u8* fn_119_A8CC(u8* p) { return p + 0xA608; }
u8* fn_119_A8D8(u8* p) { return p + 0xAAC0; }
void fn_119_B690() {}
void fn_119_B778(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_119_B784(u8* p) { *(u8*)(p + 0x31) = 1; }
int fn_119_BECC() { return 0; }
int fn_119_BFB4() { return 0; }
int fn_119_C09C() { return 0; }
int fn_119_C184() { return 0; }
int fn_119_C26C() { return 0; }
int fn_119_C354() { return 0; }
int fn_119_C43C() { return 0; }
int fn_119_C524() { return 0; }

}

// Status-change override: entering status 0x126 first sends the final-finish event (fn_119_8FFC, defined elsewhere), then forwards to Fighter.
extern "C" void fn_119_8FFC(Fighter* p, soModuleAccesser* moduleAccesser);
extern "C" {

void fn_119_8F90(Fighter* p, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    switch (statusKind) {
    case 0x126:
        fn_119_8FFC(p, moduleAccesser);
        break;
    }
    p->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

}

// Placeholders for weak inline methods of library classes (their names exist in sora_melee, so the REL copies cannot be renamed): the shim calls the inline method.
extern "C" {

bool fn_119_98DC(const soGeneralWorkSimple* p, u32 a0, u32 a1) { return p->soGeneralWorkSimple::isFlag(a0, a1); }
void fn_119_98F8(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::offFlag(a0, a1); }
void fn_119_9910(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::clearFlag(a0); }
void fn_119_9924(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::onFlag(a0, a1); }
void fn_119_996C(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::mulFloatWork(a0, a1); }
void fn_119_9984(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::subFloatWork(a0, a1); }
void fn_119_999C(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::addFloatWork(a0, a1); }
void fn_119_99B4(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::setFloatWork(a0, a1); }
float fn_119_99C4(const soGeneralWorkSimple* p, u32 a0) { return p->soGeneralWorkSimple::getFloatWork(a0); }
void fn_119_99DC(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::decIntWork(a0); }
void fn_119_99F4(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::incIntWork(a0); }
void fn_119_9A0C(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::divIntWork(a0, a1); }
void fn_119_9A2C(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::mulIntWork(a0, a1); }
void fn_119_9A44(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::subIntWork(a0, a1); }
void fn_119_9A5C(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::addIntWork(a0, a1); }
void fn_119_9A74(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::setIntWork(a0, a1); }
Vec2f fn_119_B7A4(soKineticEnergyNormal* p) { return p->soKineticEnergyNormal::getSpeed(); }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_119_C774(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_119_C77C(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
bool fn_119_C784(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
bool fn_119_C78C(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_119_C79C(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_119_C7A4(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
void fn_119_C7AC(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_119_C794(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"
