#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/diddy/ft_diddy.h>
#include <ft/diddy/ft_diddy_extend_param_accesser.h>

#define FT_BC ftDiddyBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftDiddyExtendParamAccesser g_ftDiddyExtendParamAccesser;

ftClassInfoImpl<Fighter_Diddy, ftDiddy> g_ftClassInfoDiddy;

ftDiddy::ftDiddy(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftDiddyBuildConfig>(entryId,
                                         Fighter_Diddy,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// Shared-class destructors and accessors that this module emits itself (same pattern as ft_marth.cpp).
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
ftKineticEnergyController::~ftKineticEnergyController() { }
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
soStatusModuleImpl::~soStatusModuleImpl() { }
#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
#pragma dont_inline off

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

ftDiddy::~ftDiddy() { }

void ftDiddy::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    // Entering the two Peanut/Barrel-held states tells the linked articles about the change.
    switch (statusKind) {
    case 0x119: {
        ftDiddyLinkEvent event(0x838);
        moduleAccesser->getLinkModule().sendEventNodes(1, event, 0);
        break;
    }
    case 0x11a: {
        ftDiddyLinkEvent event(0x839);
        moduleAccesser->getLinkModule().sendEventNodes(1, event, 0);
        break;
    }
    }
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

void ftDiddy::notifyEventOnDamage(soDamage* damage, bool isDamage, soModuleAccesser* moduleAccesser) {
    if (isDamage) {
        // Taking a hit outside the two Diddy-specific statuses clears the work flags of the gun and barrel states.
        if (moduleAccesser->getStatusModule().getStatusKind() != 0x125 &&
            moduleAccesser->getStatusModule().getStatusKind() != 0x127) {
            m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
            m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
        }
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003e);
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003f);
    }
    Fighter::notifyEventOnDamage(damage, isDamage, moduleAccesser);
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftDiddy is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftDiddyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftDiddyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_115_672C(u8* p) { return *(u8*)(p + 0x4); }
void fn_115_8734() {}
void fn_115_9A18() {}
void fn_115_9A1C() {}
int fn_115_9B48() { return 0x0; }
void fn_115_9D5C() {}
void fn_115_9ED4() {}
u8 fn_115_9ED8(u8* p) { return *(u8*)(p + 0x44); }
void fn_115_9EE0() {}
void fn_115_9F20() {}
void fn_115_9F24() {}
void fn_115_9FDC() {}
void fn_115_9FE0() {}
void fn_115_9FE4() {}
void fn_115_9FE8() {}
void fn_115_9FEC() {}
void fn_115_9FF0() {}
void fn_115_9FF4() {}
void fn_115_9FF8() {}
void fn_115_9FFC() {}
void fn_115_A000() {}
void fn_115_A004() {}
void fn_115_A008() {}
void fn_115_A00C() {}
void fn_115_A010() {}
void fn_115_A014() {}
int fn_115_A018() { return 0x0; }
void fn_115_A020() {}
void fn_115_A024() {}
void fn_115_A028() {}
void fn_115_A02C() {}
void fn_115_A058() {}
void fn_115_A05C() {}
void fn_115_A060() {}
void fn_115_A064() {}
void fn_115_A068() {}
int fn_115_A06C(u8* p) { return *(int*)(p + 0x110); }
int fn_115_A084() { return 0x0; }
void fn_115_A08C() {}
void fn_115_A090() {}
void fn_115_A094() {}
void fn_115_A098() {}
int fn_115_A09C() { return 0x0; }
int fn_115_A0A4() { return 0x0; }
void fn_115_A0AC() {}
void fn_115_A0B0() {}
void fn_115_A0B4() {}
void fn_115_A0B8() {}
void fn_115_A0BC() {}
int fn_115_A270(u8* p) { return *(int*)(p + 0x20); }
int fn_115_A34C(u8* p) { return *(int*)(p + 0x18); }
int fn_115_A3E4(u8* p) { return *(int*)(p + 0x10); }
int fn_115_A4A4() { return 0x0; }
int fn_115_B97C() { return 0x4; }
void fn_115_C880() {}
void fn_115_CF8C() {}
int fn_115_CFC8() { return 0x8; }
void fn_115_D010() {}
int fn_115_D0BC() { return 0x0; }
void fn_115_D0F8() {}
int fn_115_D1A4() { return 0x0; }
void fn_115_D1E0() {}
int fn_115_D28C() { return 0x0; }
void fn_115_D2C8() {}
int fn_115_D374() { return 0x0; }
void fn_115_D3B0() {}
int fn_115_D45C() { return 0x0; }
void fn_115_D498() {}
int fn_115_D544() { return 0x0; }
void fn_115_D580() {}
int fn_115_D62C() { return 0x0; }
void fn_115_D668() {}
int fn_115_D714() { return 0x0; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_115_D938(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_115_D940(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_115_D948(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_115_D978(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_115_D980(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_115_D988(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_115_D990(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_115_D998(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_115_D9B0(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_115_D9C8(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_115_D9D8(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_115_D9E0(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_115_D9E8(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_115_D9F0(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_115_D9F8(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_115_DA00(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_115_DA08(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_115_DA10(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_115_DA18(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_115_DA20(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_115_DA28(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_115_DA30(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_115_DA38(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_115_DA48(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_115_DA58(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_115_DA60(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_115_DA68(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_115_DA70(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_115_DA78(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_115_D958(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_115_D9A0(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_115_D9A8(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_115_D9B8(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_115_D9C0(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
void fn_115_D9D0(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_115_3C4C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_115_3D54(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_115_46CC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_115_470C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_115_474C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
