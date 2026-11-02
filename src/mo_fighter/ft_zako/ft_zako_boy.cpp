// BYTES: native pool teardown follows the shared runtime-helper boundaries.
#define FT_MARTH_RUNTIME_HELPERS
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#include <ft/builder/ft_dol_array_list.h>

// BYTES: this capacity uses the vector destructor owned by sora_melee.
template <> soArrayVector<ftAnimCmdConvPtr, 280>::~soArrayVector();
#include <ft/ft_class_info_impl.h>
#include <ft/ft_util.h>
#include <ft/zako/ft_zako.h>
#include <ft/zako/ft_zako_extend_param_accesser.h>
#include <ft/zako/ft_zako_impl.h>

// The four Zako fighters were compiled as separate units that share their module configurations; this one holds the shared
// definitions (the linker kept the first copy of every weak function).
#define FT_BC ftZakoBoyBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// The kinetic controller destructor is local to this REL, as its pool teardown requires.
ftKineticEnergyController::~ftKineticEnergyController() { }

// The matrix pool owns the shared base teardown retained by the REL.
#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
#pragma dont_inline off

// The owned status-change queue uses sora_melee's destructor.
template <> soArrayVector<s32, 8>::~soArrayVector();
soStatusModuleImpl::~soStatusModuleImpl() { }

ftZakoBoyExtendParamAccesser g_ftZakoBoyExtendParamAccesser;

ftClassInfoImpl<Fighter_Zako_Boy, ftZakoBoy> g_ftClassInfoZakoBoy;

FT_ZAKO_IMPL(Boy)

FT_ZAKO_ON_ACTIVATE(Boy)

// MATCH-ONLY: the REL emits Fighter's empty onActivate (the vtable base entry); keep it alive.
#pragma dont_inline on
void ftZakoKeepFighterOnActivate(Fighter* fighter) { fighter->Fighter::onActivate(); }
#pragma dont_inline off

// FIXME: Test code present only to emit the shared builder functions
void testBuilder() {
    soInsideEventManageModuleBuilder<ftZakoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
bool fn_126_98C0(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_126_98C8(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
bool fn_126_98D8(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_126_98F0(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_126_9908(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_126_9918(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_126_9920(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
void fn_126_9930(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_126_9938(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_126_9940(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_126_9948(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_126_9950(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_126_9958(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_126_9960(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_126_9968(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_126_9970(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_126_9978(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_126_9980(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_126_9988(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_126_9990(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_126_9998(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_126_99A0(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_126_99A8(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_126_98E0(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_126_98E8(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_126_98F8(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_126_9900(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_126_9910(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_126_4EB4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void fn_126_8EFC() {}
} // extern "C"

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// Zero-fill of a three-float member (Vec3f-sized), as in ft_mario.cpp.
extern "C" void fn_126_88EC(float* p) {
    p[0] = 0.0f;
    p[1] = 0.0f;
    p[2] = 0.0f;
}
