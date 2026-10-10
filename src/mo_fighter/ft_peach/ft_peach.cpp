#define FT_MARTH_PHOTO_CALLBACK_NOINLINE // MATCH-ONLY: out-of-line photo base teardown and +0x186F0 photo thunks (shared shadow macro)
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/peach/ft_peach.h>
#include <ft/peach/ft_peach_extend_param_accesser.h>
#include <ft/peach/ft_peach_status_uniq_process_final.h>
#include <gf/gf_task_scheduler.h>

#define FT_BC ftPeachBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftPeachExtendParamAccesser g_ftPeachExtendParamAccesser;

ftClassInfoImpl<Fighter_Peach, ftPeach> g_ftClassInfoPeach;

ftPeach::ftPeach(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPeachBuildConfig>(entryId,
                                         Fighter_Peach,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

void ftPeach::endFinalRequest() {
    Fighter::endFinal(true, true, false);
    soPhotoCallBack::removeCallBack();
}

ftPeach::~ftPeach() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

void ftPeach::onEndFinal() {
    g_ftPeachStatusUniqProcessFinal.destroyInfo(m_moduleAccesser);
}

void ftPeach::onDeactivate() {
    Fighter::endFinal(true, true, false);
    soPhotoCallBack::removeCallBack();
}

// Final Smash blossom windows: the fighter keeps their task ids in work slots.
void ftPeach::photoMoved() {
    int taskId = m_moduleAccesser->getWorkManageModule().getInt(0x10000041);
    if (taskId != 0) {
        IfPeachFinalTask* window = dynamic_cast<IfPeachFinalTask*>(gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Info, taskId));
        if (window != NULL) {
            window->setVisibilityWhole(false);
        }
    }
    int effectId = m_moduleAccesser->getWorkManageModule().getInt(0x10000042);
    if (effectId != 0) {
        m_moduleAccesser->getEffectModule().setVisible(effectId, false);
    }
}

void ftPeach::photoExit() {
    int taskId = m_moduleAccesser->getWorkManageModule().getInt(0x10000041);
    if (taskId != 0) {
        IfPeachFinalTask* window = dynamic_cast<IfPeachFinalTask*>(gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Info, taskId));
        if (window != NULL) {
            window->setVisibilityWhole(true);
        }
    }
    int effectId = m_moduleAccesser->getWorkManageModule().getInt(0x10000042);
    if (effectId != 0) {
        m_moduleAccesser->getEffectModule().setVisible(effectId, true);
    }
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftPeach is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPeachInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPeachInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_103_72A4(u8* p) { return *(u8*)(p + 0x4); }
void fn_103_9058() {}
void fn_103_9A40() {}
int fn_103_9A44() { return 0x0; }
void fn_103_9BC4() {}
void fn_103_9D4C() {}
u8 fn_103_9D50(u8* p) { return *(u8*)(p + 0x44); }
void fn_103_9D58() {}
void fn_103_9D98() {}
void fn_103_9D9C() {}
void fn_103_9E2C() {}
void fn_103_9E58() {}
void fn_103_9E5C() {}
void fn_103_9E60() {}
void fn_103_9E64() {}
void fn_103_9E68() {}
void fn_103_9E6C() {}
void fn_103_9E70() {}
void fn_103_9E74() {}
void fn_103_9E78() {}
void fn_103_9E7C() {}
void fn_103_9E80() {}
void fn_103_9E84() {}
void fn_103_9E88() {}
void fn_103_9E8C() {}
void fn_103_9E90() {}
int fn_103_9E94() { return 0x0; }
void fn_103_9E9C() {}
void fn_103_9EA0() {}
void fn_103_9EA4() {}
void fn_103_9EA8() {}
void fn_103_9ED4() {}
void fn_103_9ED8() {}
void fn_103_9EDC() {}
void fn_103_9EE0() {}
void fn_103_9EE4() {}
int fn_103_9EE8(u8* p) { return *(int*)(p + 0x110); }
int fn_103_9F00() { return 0x0; }
void fn_103_9F08() {}
void fn_103_9F0C() {}
void fn_103_9F10() {}
void fn_103_9F14() {}
int fn_103_9F18() { return 0x0; }
int fn_103_9F20() { return 0x0; }
void fn_103_9F28() {}
void fn_103_9F2C() {}
void fn_103_9F30() {}
void fn_103_9F34() {}
int fn_103_A0A8(u8* p) { return *(int*)(p + 0x20); }
int fn_103_A184(u8* p) { return *(int*)(p + 0x18); }
int fn_103_A21C(u8* p) { return *(int*)(p + 0x10); }
int fn_103_B328() { return 0x3; }
void fn_103_C138() {}
int fn_103_C93C() { return 0x0; }
void fn_103_C978() {}
int fn_103_CA24() { return 0x0; }
int fn_103_CAD4() { return 0x0; }
int fn_103_CB84() { return 0x0; }
void fn_103_CBC0() {}
int fn_103_CC6C() { return 0x0; }
void fn_103_CCA8() {}
int fn_103_CD54() { return 0x0; }
void fn_103_CD90() {}
int fn_103_CE3C() { return 0x0; }
void fn_103_CE78() {}
int fn_103_CF24() { return 0x0; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_103_D148(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_103_D150(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_103_D158(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_103_D168(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_103_D170(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
void fn_103_D190(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
bool fn_103_D1A8(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_103_D1B0(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_103_D1B8(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_103_D1D8(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_103_D1F0(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_103_D210(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_103_D218(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_103_D220(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_103_D228(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_103_D230(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_103_D238(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_103_D240(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_103_D248(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_103_D250(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_103_D258(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_103_D260(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_103_D268(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_103_D270(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_103_D278(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_103_D280(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_103_D288(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_103_D290(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_103_D298(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
void fn_103_D160(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
bool fn_103_D180(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_103_D1C8(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_103_D1E0(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_103_D1E8(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
void fn_103_D1F8(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_103_1E28(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_3770(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_3C7C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_400C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_4408(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_4684(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_46C4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_52FC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_533C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_537C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
