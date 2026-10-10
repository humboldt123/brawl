#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/pikachu/ft_pikachu.h>
#include <ft/pikachu/ft_pikachu_extend_param_accesser.h>
#include <so/so_value_accesser.h>
#include <ft/ft_common_data_accesser.h>

// ftManager::setParamPattern selects the shared parameter-table variation (same accessor as ft_marth.cpp).
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

#define FT_BC ftPikachuBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftPikachuExtendParamAccesser g_ftPikachuExtendParamAccesser;

ftClassInfoImpl<Fighter_Pikachu, ftPikachu> g_ftClassInfoPikachu;

ftPikachu::ftPikachu(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPikachuBuildConfig>(entryId,
                                         Fighter_Pikachu,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftPikachu is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPikachuInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPikachuInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_98_744C(u8* p) { return *(u8*)(p + 0x4); }
void fn_98_939C() {}
int fn_98_98C0() { return 0x0; }
void fn_98_9AAC() {}
void fn_98_9C24() {}
u8 fn_98_9C28(u8* p) { return *(u8*)(p + 0x44); }
void fn_98_9C30() {}
void fn_98_9C70() {}
void fn_98_9C74() {}
void fn_98_9D04() {}
void fn_98_9D30() {}
void fn_98_9D34() {}
void fn_98_9D38() {}
void fn_98_9D3C() {}
void fn_98_9D40() {}
void fn_98_9D44() {}
void fn_98_9D48() {}
void fn_98_9D4C() {}
void fn_98_9D50() {}
void fn_98_9D54() {}
void fn_98_9D58() {}
void fn_98_9D5C() {}
void fn_98_9D60() {}
void fn_98_9D64() {}
void fn_98_9D68() {}
int fn_98_9D6C() { return 0x0; }
void fn_98_9D74() {}
void fn_98_9D78() {}
void fn_98_9D7C() {}
void fn_98_9D80() {}
void fn_98_9DAC() {}
void fn_98_9DB0() {}
void fn_98_9DB4() {}
void fn_98_9DB8() {}
void fn_98_9DBC() {}
int fn_98_9DC0(u8* p) { return *(int*)(p + 0x110); }
int fn_98_9DD8() { return 0x0; }
void fn_98_9DE0() {}
void fn_98_9DE4() {}
void fn_98_9DE8() {}
int fn_98_9DEC() { return 0x0; }
int fn_98_9DF4() { return 0x0; }
void fn_98_9DFC() {}
void fn_98_9E00() {}
void fn_98_9E04() {}
void fn_98_9E08() {}
void fn_98_9E0C() {}
void fn_98_9E10() {}
int fn_98_9F84(u8* p) { return *(int*)(p + 0x20); }
int fn_98_A060(u8* p) { return *(int*)(p + 0x18); }
int fn_98_A0F8(u8* p) { return *(int*)(p + 0x10); }
int fn_98_B59C() { return 0x4; }
void fn_98_C430() {}
void fn_98_CCDC() {}
int fn_98_CD18() { return 0x8; }
int fn_98_CFC0() { return 0x0; }
int fn_98_D25C() { return 0x0; }
int fn_98_D4F4() { return 0x0; }
int fn_98_D790() { return 0x0; }
int fn_98_DA2C() { return 0x0; }
int fn_98_DCC8() { return 0x0; }
int fn_98_DF64() { return 0x0; }
int fn_98_E200() { return 0x0; }

// soArticleMediatorImpl<...wnPikachuThunderJolt...>::setAutoRecycle: stores the flag at this+0x22A2C.
void fn_98_B5A4(u8* p, u8 flag) { *(u8*)(p + 0x22A2C) = flag; }

// ftPikachu::notify_thunder_reflect / adjust_camera_subject: forward this+0x60 to the thunder-hit process handlers.
extern "C" int fn_98_11344(void* p);
extern "C" int fn_98_11B40(void* p);
int fn_98_98B0(u8* p) { return fn_98_11344(*(void**)(p + 0x60)); }
int fn_98_98B8(u8* p) { return fn_98_11B40(*(void**)(p + 0x60)); }

// ftPikachu::getExtendParam: the extend-param block pointer stored at the end of the fighter, offset 0x7C.
void* fn_98_9530(u8* p) { return *(u8**)(p + 0x2AE44) + 0x7C; }

// soInstancePoolSub::getInstanceAt for the wnPikachu* instance holders: instance slots by index, null past the last one.
void* fn_98_B62C(u8* p, int index) {
    if (index == 2) return p + 0x3FA4;
    if (index == 1) return p + 0x1FDC;
    if (index == 0) return p + 0x14;
    return 0;
}
void* fn_98_B664(u8* p, int index) {
    if (index == 0) return p + 0xC;
    return 0;
}
void* fn_98_B67C(u8* p, int index) {
    if (index == 1) return p + 0xA610;
    if (index == 0) return p + 0x10;
    return 0;
}
void* fn_98_B6A8(u8* p, int index) {
    if (index == 2) return p + 0x3F44;
    if (index == 1) return p + 0x1FAC;
    if (index == 0) return p + 0x14;
    return 0;
}

// soKineticMediatorImpl::addSpeed:forwards to the transactor with the pools at this+4.
void fn_98_CCE0(u8* p, void* speed, soModuleAccesser* acc) { ftKineticTransactor::addSpeed(speed, p + 4, acc); }

// ftPikachu::getThunderParam:the common fighter data entry for Pikachu, field at 0x88.
void* fn_98_9540() { return *(void**)((u8*)g_ftCommonDataAccesser.getData(Fighter_Pikachu) + 0x88); }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_98_E424(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_98_E444(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_98_E454(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_98_E45C(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_98_E464(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_98_E46C(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_98_E474(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_98_E48C(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_98_E4A4(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_98_E4B4(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_98_E4BC(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_98_E4C4(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_98_E4CC(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_98_E4D4(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_98_E4DC(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_98_E4E4(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_98_E4EC(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_98_E4F4(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_98_E4FC(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_98_E504(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_98_E50C(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_98_E514(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_98_E51C(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_98_E524(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_98_E52C(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_98_E534(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_98_E53C(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_98_E544(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_98_E54C(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_98_E434(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_98_E47C(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_98_E484(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_98_E494(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_98_E49C(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
void fn_98_E4AC(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_98_1DD8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4048(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4480(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_47F8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4B58(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4B98(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4D60(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4DA0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4DE0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
