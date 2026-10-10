#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/dedede/ft_dedede.h>
#include <ft/dedede/ft_dedede_extend_param_accesser.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/so_value_accesser.h>

#define FT_BC ftDededeBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftDededeExtendParamAccesser g_ftDededeExtendParamAccesser;

ftClassInfoImpl<Fighter_Dedede, ftDedede> g_ftClassInfoDedede;

ftDedede::ftDedede(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftDededeBuildConfig>(entryId,
                                         Fighter_Dedede,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftDedede is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftDededeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftDededeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Destructors whose bodies are empty; the compiler emits the deleting variants.
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
ftKineticEnergyController::~ftKineticEnergyController() { }
#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
#pragma dont_inline off

// ftDedede::notifyArticleEventRemove forwards to the StageObject version (tail call).
extern "C" void fn_117_99E0(StageObject* self, int unk1, int* unk2) { self->StageObject::notifyArticleEventRemove(unk1, unk2); }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_117_67D4(u8* p) { return *(u8*)(p + 0x4); }
void fn_117_8A60() {}
int fn_117_99D8(u8* p) { return *(int*)(p + 0xb8); }
int fn_117_9F40() { return 0x0; }
void fn_117_A13C() {}
void fn_117_A2B4() {}
u8 fn_117_A2B8(u8* p) { return *(u8*)(p + 0x44); }
void fn_117_A2C0() {}
void fn_117_A300() {}
void fn_117_A304() {}
void fn_117_A394() {}
void fn_117_A3C0() {}
void fn_117_A3C4() {}
void fn_117_A3C8() {}
void fn_117_A3CC() {}
void fn_117_A3D0() {}
void fn_117_A3D4() {}
void fn_117_A3D8() {}
void fn_117_A3DC() {}
void fn_117_A3E0() {}
void fn_117_A3E4() {}
void fn_117_A3E8() {}
void fn_117_A3EC() {}
void fn_117_A3F0() {}
void fn_117_A3F4() {}
void fn_117_A3F8() {}
int fn_117_A3FC() { return 0x0; }
void fn_117_A404() {}
void fn_117_A408() {}
void fn_117_A40C() {}
void fn_117_A410() {}
void fn_117_A43C() {}
void fn_117_A440() {}
void fn_117_A444() {}
void fn_117_A448() {}
void fn_117_A44C() {}
int fn_117_A450(u8* p) { return *(int*)(p + 0x110); }
int fn_117_A468() { return 0x0; }
void fn_117_A470() {}
void fn_117_A474() {}
void fn_117_A478() {}
void fn_117_A47C() {}
int fn_117_A480() { return 0x0; }
int fn_117_A488() { return 0x0; }
void fn_117_A490() {}
void fn_117_A494() {}
void fn_117_A498() {}
void fn_117_A49C() {}
void fn_117_A4A0() {}
int fn_117_A654(u8* p) { return *(int*)(p + 0x20); }
int fn_117_A730(u8* p) { return *(int*)(p + 0x18); }
int fn_117_A7C8(u8* p) { return *(int*)(p + 0x10); }
int fn_117_A888() { return 0x0; }
int fn_117_C104() { return 0x5; }
void fn_117_D0F8() {}
void fn_117_D804() {}
int fn_117_D840() { return 0x8; }
void fn_117_D888() {}
int fn_117_D934() { return 0x0; }
void fn_117_D970() {}
int fn_117_DA1C() { return 0x0; }
void fn_117_DA58() {}
int fn_117_DB04() { return 0x0; }
void fn_117_DB40() {}
int fn_117_DBEC() { return 0x0; }
void fn_117_DC28() {}
int fn_117_DCD4() { return 0x0; }
void fn_117_DD10() {}
int fn_117_DDBC() { return 0x0; }
void fn_117_DDF8() {}
int fn_117_DEA4() { return 0x0; }
void fn_117_DEE0() {}
int fn_117_DF8C() { return 0x0; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_117_E1B0(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_117_E1B8(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_117_E1C0(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_117_E1C8(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_117_E1D0(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_117_E208(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_117_E210(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_117_E218(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_117_E220(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_117_E238(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_117_E250(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_117_E270(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_117_E278(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_117_E280(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_117_E288(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_117_E290(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_117_E298(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_117_E2A0(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_117_E2A8(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_117_E2B0(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_117_E2B8(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_117_E2C0(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_117_E2C8(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_117_E2D0(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_117_E2D8(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_117_E2E0(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_117_E2E8(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_117_E2F0(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_117_E2F8(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_117_E1E0(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_117_E228(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_117_E230(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_117_E240(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_117_E248(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
void fn_117_E258(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"
