#define FT_FIGHTER_ANIMCMD_LONG // MATCH-ONLY: Fighter::notifyEventAnimCmd(long) overrides the StageObject slot (as in ft_marth.cpp)
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_manager.h>
#include <ft/ft_class_info_impl.h>
#include <ft/iceclimber/ft_iceclimber.h>
#include <ft/iceclimber/ft_iceclimber_extend_param_accesser.h>

#define FT_BC ftPopoBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftPopoExtendParamAccesser g_ftPopoExtendParamAccesser;

ftClassInfoImpl<Fighter_Popo, ftPopo> g_ftClassInfoPopo;

ftPopo::ftPopo(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPopoBuildConfig>(entryId,
                                         Fighter_Popo,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftPopo is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPopoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPopoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_105_8130(u8* p) { return *(u8*)(p + 0x4); }
void fn_105_A0DC() {}
void fn_105_AAFC() {}
void fn_105_AB14() {}
void fn_105_AB2C() {}
void fn_105_AB44() {}
void fn_105_AF48() {}
u8 fn_105_AF4C(u8* p) { return *(u8*)(p + 0x44); }
void fn_105_BF14() {}
int fn_105_D540() { return 0x0; }
void fn_105_D76C() {}
void fn_105_D8E4() {}
void fn_105_D924() {}
void fn_105_D928() {}
void fn_105_D9E0() {}
void fn_105_D9E4() {}
void fn_105_D9E8() {}
void fn_105_D9EC() {}
void fn_105_D9F0() {}
void fn_105_D9F4() {}
void fn_105_D9F8() {}
void fn_105_D9FC() {}
void fn_105_DA00() {}
void fn_105_DA04() {}
void fn_105_DA08() {}
void fn_105_DA0C() {}
void fn_105_DA10() {}
void fn_105_DA14() {}
void fn_105_DA18() {}
int fn_105_DA1C() { return 0x0; }
void fn_105_DA24() {}
void fn_105_DA28() {}
void fn_105_DA2C() {}
void fn_105_DA30() {}
void fn_105_DA5C() {}
void fn_105_DA60() {}
void fn_105_DA64() {}
void fn_105_DA68() {}
void fn_105_DA6C() {}
int fn_105_DA70(u8* p) { return *(int*)(p + 0x110); }
int fn_105_DA88() { return 0x0; }
void fn_105_DA90() {}
void fn_105_DA94() {}
void fn_105_DA98() {}
int fn_105_DA9C() { return 0x0; }
int fn_105_DAA4() { return 0x0; }
void fn_105_DAAC() {}
void fn_105_DAB0() {}
void fn_105_DAB4() {}
void fn_105_DAB8() {}
void fn_105_DABC() {}
void fn_105_DAC0() {}
void fn_105_DAC4() {}
void fn_105_DAC8() {}
void fn_105_DACC() {}
void fn_105_DAD0() {}
int fn_105_DAD4() { return -0x1; }
int fn_105_DAEC(u8* p) { return *(int*)(p + 0xc); }
int fn_105_DAF4() { return 0x0; }
void fn_105_DAFC() {}
void fn_105_DB00(u8* p, int v) { *(int*)(p + 0xc) = v; }
int fn_105_DC18(u8* p) { return *(int*)(p + 0x20); }
int fn_105_DCF4(u8* p) { return *(int*)(p + 0x18); }
int fn_105_DD8C(u8* p) { return *(int*)(p + 0x10); }
int fn_105_DE4C() { return 0x0; }
int fn_105_F458() { return 0x5; }
void fn_105_1038C() {}
void fn_105_10D70() {}
int fn_105_10DAC() { return 0x8; }
int fn_105_11194() { return 0x0; }
int fn_105_11564() { return 0x0; }
int fn_105_11930() { return 0x0; }
int fn_105_11CF4() { return 0x0; }
int fn_105_120D0() { return 0x0; }
int fn_105_124AC() { return 0x0; }
int fn_105_12888() { return 0x0; }
int fn_105_12C64() { return 0x0; }

}

// "Delete when flag > 0" destructors (identical bodies; one per map class name).
extern "C" {
#pragma dont_inline on
void* fn_105_1E2C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // ftVirtualNodeMatrixPool____dt
void* fn_105_3674(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soParamAccesser____dt
void* fn_105_3D5C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soInstanceManagerFullProperty_P15soKineticEnergy_____dt
void* fn_105_41E8(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soInstanceManagerFullProperty_20soAnimCmdControlUnit_____dt
void* fn_105_45E4(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soTeam____dt
void* fn_105_48D8(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soResourceIdAccesser____dt
void* fn_105_4918(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soParamCustomizeModuleBuilder dt
void* fn_105_628C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soArticleMediator____dt
void* fn_105_62CC(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soArticleOperator____dt
void* fn_105_630C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soArticleGenerator____dt
#pragma dont_inline reset
}

// Flag-free register-offset helpers (leaf getters returning sub-object addresses by index).
extern "C" {
void fn_105_F460(u8* p, u8 v) { *(u8*)(p + 0x17A10) = v; } // soArticleMediatorImpl setAutoRecycle
void* fn_105_F4E8(u8* p, s32 index) { if (index == 0) { return p + 0xc; } return 0; } // soInstancePoolSub getInstanceAt
void* fn_105_F500(u8* p, s32 index) { if (index == 0) { return p + 0xc; } return 0; } // soInstancePoolSub getInstanceAt
void* fn_105_F518(u8* p, s32 index) { if (index == 0) { return p + 0xc; } return 0; } // soInstancePoolSub getInstanceAt
void* fn_105_F58C(u8* p, s32 index) {
    if (index == 2) { return p + 0x3eb4; }
    if (index == 1) { return p + 0x1f64; }
    if (index == 0) { return p + 0x14; }
    return 0;
} // soInstancePoolSub_74...wnPopoBlizzard getInstanceAt
void* fn_105_F530(u8* p, s32 index) {
    if (index == 4) { return p + 0x8f6c; }
    if (index == 3) { return p + 0x6b98; }
    if (index == 2) { return p + 0x47c4; }
    if (index == 1) { return p + 0x23f0; }
    if (index == 0) { return p + 0x1c; }
    return 0;
} // soInstancePoolSub_73...wnPopoIceShot getInstanceAt
}

// Fighter-base this-adjusting thunks: subtract the sub-object offset, then tail call the Fighter method.
extern "C" {
void* fn_105_12E68(void* p, s32 flags) { return fn_105_628C((u8*)p - 0x4, flags); } // Fighter-base dtor thunk for soArticleMediator
void fn_105_12E88(u8* p, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) { ((Fighter*)(p - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4); } // Fighter::notifyEventLink
void fn_105_12E90(u8* p, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); } // Fighter::notifyEventChangeStatus
void fn_105_12E98(u8* p, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); } // Fighter::notifyEventChangeSituation
void fn_105_12EA0(u8* p, BaseItem* item, u32 nodeIndex, int* unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyUseItem(item, nodeIndex, unk); } // Fighter::notifyUseItem
#pragma dont_inline on
bool fn_105_12EB8(u8* p, int unk1) { return ((Fighter*)(p - 0x48))->Fighter::isObserv(unk1); } // Fighter::isObserv (header inline body must not be folded in)
#pragma dont_inline reset
void fn_105_12ED8(u8* p, u32 flags) { ((Fighter*)(p - 0x7c))->Fighter::notifyEventCollisionAttackCheck(flags); } // Fighter::notifyEventCollisionAttackCheck
void fn_105_12EE0(u8* p, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x7c))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); } // Fighter::notifyEventCollisionAttack
void fn_105_12EE8(u8* p, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); } // Fighter::notifyEventChangeCollisionHit
void fn_105_12EF0(u8* p) { ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldCheck(); } // Fighter::notifyEventCollisionShieldCheck
void fn_105_12EF8(u8* p, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); } // Fighter::notifyEventCollisionShieldSearch
void fn_105_12F00(u8* p, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); } // Fighter::notifyEventCollisionShield
void fn_105_12F08(u8* p) { ((Fighter*)(p - 0xa0))->Fighter::notifyEventCollisionReflectorCheck(); } // Fighter::notifyEventCollisionReflectorCheck
void fn_105_12F10(u8* p, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xa0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); } // Fighter::notifyEventCollisionReflectorSearch
void fn_105_12F18(u8* p, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(p - 0xa0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); } // Fighter::notifyEventCollisionReflector
void fn_105_12F20(u8* p) { ((Fighter*)(p - 0xac))->Fighter::notifyEventCollisionAbsorberCheck(); } // Fighter::notifyEventCollisionAbsorberCheck
void fn_105_12F28(u8* p, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(p - 0xac))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); } // Fighter::notifyEventCollisionAbsorber
void fn_105_12F30(u8* p) { ((Fighter*)(p - 0xb8))->Fighter::notifyEventCollisionSearchCheck(); } // Fighter::notifyEventCollisionSearchCheck
void fn_105_12F38(u8* p, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xb8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); } // Fighter::notifyEventCollisionSearch
void fn_105_12F40(u8* p, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { ((Fighter*)(p - 0xc4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); } // Fighter::notifyEventCaptureStatus
void fn_105_12F48(u8* p, BaseItem* item, int index, bool unk2, u8 unk3) { ((Fighter*)(p - 0xd0))->Fighter::notifyVisibilityItem(item, index, unk2, unk3); } // Fighter::notifyVisibilityItem
void fn_105_12F50(u8* p, BaseItem* item, u32 index, bool unk2) { ((Fighter*)(p - 0xd0))->Fighter::notifyEjectAttachItem(item, index, unk2); } // Fighter::notifyEjectAttachItem
void fn_105_12F58(u8* p, BaseItem* item, u32 index, bool unk2) { ((Fighter*)(p - 0xd0))->Fighter::notifyEjectItem(item, index, unk2); } // Fighter::notifyEjectItem
void fn_105_12F60(u8* p, BaseItem* item) { ((Fighter*)(p - 0xd0))->Fighter::notifyShootBulletItem(item); } // Fighter::notifyShootBulletItem
void fn_105_12F68(u8* p, BaseItem* item) { ((Fighter*)(p - 0xd0))->Fighter::notifyDropItem(item); } // Fighter::notifyDropItem
void fn_105_12F70(u8* p, BaseItem* item, u32 nodeIndex, int* unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyThrowItem(item, nodeIndex, unk); } // Fighter::notifyThrowItem
void fn_105_12F80(u8* p, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk1, bool unk2) { ((Fighter*)(p - 0xd0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk1, unk2); } // Fighter::notifyAttachItem
void fn_105_12F88(u8* p, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk); } // Fighter::notifyHaveItem
void fn_105_12F90(u8* p, BaseItem* item, bool* unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyHaveItemPreCheck(item, unk); } // Fighter::notifyHaveItemPreCheck
void fn_105_12FA0(u8* p, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xe8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); } // Fighter::notifyEventAddDamage
void fn_105_12FA8(u8* p, soDamage* damage, bool unk, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xe8))->Fighter::notifyEventOnDamage(damage, unk, moduleAccesser); } // Fighter::notifyEventOnDamage
void fn_105_12FB0(u8* p, float power, int unk) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventPikminFinalAttack(power, unk); } // Fighter::notifyEventPikminFinalAttack
void fn_105_12FB8(u8* p) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventChangeAdvUnit(); } // Fighter::notifyEventChangeAdvUnit
void fn_105_12FC0(u8* p) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventBeat(); } // Fighter::notifyEventBeat
void fn_105_12FC8(u8* p, float damage) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventSetDamage(damage); } // Fighter::notifyEventSetDamage
void fn_105_12FD0(u8* p, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x100))->Fighter::notifyEventTurn(posX, posY, moduleAccesser); } // Fighter::notifyEventTurn
}

extern "C" {
bool fn_105_12EB0(u8* p, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, long unk3) { return ((Fighter*)(p - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); } // Fighter::notifyEventAnimCmd
}

// Sub-object destructor wrappers: destroy one base sub-object (flag -1 or 0), then delete when flags > 0.
extern "C" {
void* fn_27_C31CC(void* p, s16 flags);
void* fn_27_63EBC(void* p, s16 flags);
void* fn_27_5C674(void* p, s16 flags);
void* fn_27_5AE44(void* p, s16 flags);
void* fn_27_6880C(void* p, s16 flags);
void* fn_27_D3B50(void* p, s16 flags);
void __dt__15soKineticEnergyFv(void* p, s16 flags);
void __dt__16soStopModuleImplFv(void* p, s16 flags);
void __dt__17soSoundModuleImplFv(void* p, s16 flags);
void __dt__19soGeneralWorkSimpleFv(void* p, s16 flags);
void* fn_105_327C(void* p, s16 flags);
void* fn_105_3348(void* p, s16 flags);
void* fn_105_3490(void* p, s16 flags);
void* fn_105_4CF4(void* p, s16 flags);

#pragma dont_inline on
void* fn_105_3220(void* p, s32 flags) { if (p != 0) { fn_105_327C(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soSelectInstanceHolder dt
void* fn_105_32EC(void* p, s32 flags) { if (p != 0) { fn_105_3348(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soLineHierarchy dt
void* fn_105_3430(void* p, s16 flags) { if (p != 0) { fn_105_3490((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
void* fn_105_36B4(void* p, s32 flags) { if (p != 0) { __dt__15soKineticEnergyFv(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soKineticEnergyRotNormal dt
void* fn_105_3A24(void* p, s32 flags) { if (p != 0) { fn_27_C31CC(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soGlowModuleBuilder dt
void* fn_105_3A80(void* p, s32 flags) { if (p != 0) { fn_27_63EBC(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soReflectModuleBuilder dt
void* fn_105_3ADC(void* p, s32 flags) { if (p != 0) { fn_27_5C674(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soSlowModuleBuilder dt
void* fn_105_3BC4(void* p, s32 flags) { if (p != 0) { __dt__19soGeneralWorkSimpleFv((u8*)p + 0x38, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soGeneralWorkBuilder dt
void* fn_105_4100(void* p, s32 flags) { if (p != 0) { __dt__19soGeneralWorkSimpleFv((u8*)p + 0x14, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soGeneralWorkBuilder dt
void* fn_105_42F0(void* p, s32 flags) { if (p != 0) { fn_27_5AE44(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soVisibilityModuleBuilder dt
void* fn_105_4418(void* p, s32 flags) { if (p != 0) { fn_27_6880C((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soShakeModuleBuilder dt
void* fn_105_43B8(void* p, s32 flags) { if (p != 0) { __dt__17soSoundModuleImplFv((u8*)p + 0xc, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soSoundModuleBuilder dt
void* fn_105_4478(void* p, s32 flags) { if (p != 0) { __dt__16soStopModuleImplFv(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soStopModuleBuilder dt
void* fn_105_4C94(void* p, s16 flags) { if (p != 0) { fn_105_4CF4((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
void* fn_105_51C4(void* p, s32 flags) { if (p != 0) { fn_27_D3B50(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soDamageEventObserver dt
void* fn_105_78F8(void* p, s32 flags) { if (p != 0) { fn_105_45E4(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // ftTeam dt
#pragma dont_inline reset
}

// Cursor and loupe forwarders: call the Fighter setter unless the sub-object flag byte at +0x1FE4C is set.
extern "C" {
void fn_105_AAE8(u8* p, bool b) { if (*(p + 0x1FE4C) == 0) { ((Fighter*)p)->Fighter::setCursor(b); } } // ftPopo setCursor
void fn_105_AB00(u8* p, bool b) { if (*(p + 0x1FE4C) == 0) { ((Fighter*)p)->Fighter::setNameCursor(b); } } // ftPopo setNameCursor
void fn_105_AB18(u8* p, bool b) { if (*(p + 0x1FE4C) == 0) { ((Fighter*)p)->Fighter::setLoupe(b); } } // ftPopo setLoupe
void fn_105_AB30(u8* p, bool b) { if (*(p + 0x1FE4C) == 0) { ((Fighter*)p)->Fighter::setLoupeDamage(b); } } // ftPopo setLoupeDamage

// Owner and input lookups: the manager's sub-owner/sub-input variant is used when the flag byte equals 1.
ftOwner* fn_105_AF54(u8* p) { if (*(p + 0x1FE4C) == 1) { return g_ftManager->getSubOwner(*(int*)(p + 0x10c)); } return g_ftManager->getOwner(*(int*)(p + 0x10c)); } // ftPopo getOwner
void* fn_105_AF88(u8* p) { if (*(p + 0x1FE4C) == 1) { return g_ftManager->getSubInput(*(int*)(p + 0x10c)); } return g_ftManager->getInput(*(int*)(p + 0x10c)); } // ftPopo getInput
}

// Shared value variation getter (the Marth translation unit owns the C++ member; this is the REL copy).
extern int g_soValueVariation;
extern "C" {
int fn_105_E114() { return g_soValueVariation; } // soValueAccesser getValueVariation
}

// Forwarders to ftPopo methods that are not written yet (the target's `this` is the adjusted pointer).
extern "C" {
void* fn_105_F0C8(void* p, s32 flags);
void* fn_105_12E70(void* p, s32 flags) { return fn_105_F0C8((u8*)p - 0x4, flags); } // soArticleMediatorImpl shoot thunk
void* fn_105_12E78(void* p, s32 flags) { return fn_105_327C((u8*)p - 0x4, flags); } // soArticleMediatorImpl thunk
void fn_105_B138(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
void fn_105_A240(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
void fn_105_AFBC(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
void fn_105_AA64(u8* self, BaseItem* item, u32 nodeIndex, int* unk);
void fn_105_12EC0(u8* p, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) { fn_105_B138(p - 0x54, eventInfo, moduleAccesser, stageObject, unk4); } // ftPopo notifyEventLink thunk
void fn_105_12EC8(u8* p, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { fn_105_A240(p - 0x64, statusKind, prevStatusKind, statusData, moduleAccesser); } // ftPopo notifyEventChangeStatus thunk
void fn_105_12ED0(u8* p, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { fn_105_AFBC(p - 0x70, kind, prevKind, moduleAccesser); } // ftPopo notifyEventChangeSituation thunk
void fn_105_12F78(u8* p, BaseItem* item, u32 nodeIndex, int* unk) { fn_105_AA64(p - 0xd0, item, nodeIndex, unk); } // ftPopo notifyUseItem thunk
}

// Chained sub-object destructor wrappers (same shape as above): destroy the sub-object, then delete when flags > 0.
extern "C" {
void* fn_105_5C34(void* p, s16 flags);
void* fn_105_56B8(void* p, s16 flags);
void* fn_105_6090(void* p, s16 flags);
void* fn_105_5BD4(void* p, s16 flags);
void* fn_105_5FD0(void* p, s16 flags);
void* fn_105_5658(void* p, s16 flags);
void* fn_105_6030(void* p, s16 flags);

#pragma dont_inline on
void* fn_105_4B4C(void* p, s32 flags) { if (p != 0) { fn_105_3430((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_5520(void* p, s32 flags) { if (p != 0) { fn_105_4C94((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_55F8(void* p, s32 flags) { if (p != 0) { fn_105_5658((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_5658(void* p, s16 flags) { if (p != 0) { fn_105_56B8((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
void* fn_105_5B74(void* p, s32 flags) { if (p != 0) { fn_105_5BD4((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_5BD4(void* p, s16 flags) { if (p != 0) { fn_105_5C34((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
void* fn_105_5F70(void* p, s32 flags) { if (p != 0) { fn_105_5FD0((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_5FD0(void* p, s16 flags) { if (p != 0) { fn_105_6030((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_6030(void* p, s16 flags) { if (p != 0) { fn_105_6090((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
#pragma dont_inline reset
}

// Virtual slot forwarder: the object's vtable slot at offset 0x58 (slot 20) is tail called with the same arguments.
extern "C" {
struct ftPopoVSlotsArticle {
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
    virtual int slot20();
};
int fn_105_F4D8(ftPopoVSlotsArticle* p) { return p->slot20(); } // soArticle checkActivate (virtual tail call)

// Range check on the index: 1 for 1, and for 4 and 5; otherwise 0.
int fn_105_AB48(int index) { if (index < 4) { if (index == 1) { return 1; } } else if (index < 6) { return 1; } return 0; } // ftPopo table test
}
