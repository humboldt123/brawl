#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/fighter.h>
#include <so/stageobject.h>
#include <ft/pikmin/ft_pikmin.h>
#include <ft/pikmin/ft_pikmin_extend_param_accesser.h>
#include <so/sound/so_sound_module_impl.h>

// HYPOTHESIS: soValueAccesser's variation value is a plain global here (no header declaration yet)
extern s32 g_soValueVariation;

#define FT_BC ftPikminBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// MATCH-ONLY: unlike the other fighters, ftPikmin calls the ftFighterBuilder constructor out of line (it has a large
// constructor body of its own that MWCC's inliner no longer fits it into).
#pragma dont_inline on
template ftFighterBuilder<ftPikminBuildConfig>::ftFighterBuilder(s32, ftKind, Heaps::HeapType, Heaps::HeapType, Heaps::HeapType);
#pragma dont_inline off

ftPikminExtendParamAccesser g_ftPikminExtendParamAccesser;

ftClassInfoImpl<Fighter_Pikmin, ftPikmin> g_ftClassInfoPikmin;

ftPikmin::ftPikmin(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPikminBuildConfig>(entryId,
                                         Fighter_Pikmin,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftPikmin is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPikminInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPikminInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
// Vtable view used only to emit the virtual call of soArticle::checkActivate (slot at +0x58, first slot at +0x8).
// Declared but never defined; the slots before it are placeholders.
struct ftPikminArticleSlots {
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
    virtual int checkActivate();
};

extern "C" {

int fn_113_686C(u8* p) { return *(int*)(p + 0x60); }
int fn_113_7FA8() { return 0x0; }
int fn_113_7FBC() { return 0x0; }
int fn_113_8094() { return 0x0; }
int fn_113_809C() { return 0x0; }
int fn_113_80A4() { return 0x0; }
int fn_113_80AC() { return 0x0; }
int fn_113_80D8() { return 0x0; }
int fn_113_80E0() { return 0x0; }
u8 fn_113_80F4(u8* p) { return *(u8*)(p + 0x4); }
void fn_113_9F54() {}
int fn_113_C534() { return 0x0; }
void fn_113_C5F4() {}
void fn_113_C810() {}
u8 fn_113_C814(u8* p) { return *(u8*)(p + 0x44); }
void fn_113_C81C() {}
void fn_113_C85C() {}
void fn_113_C860() {}
void fn_113_C8F0() {}
void fn_113_C91C() {}
void fn_113_C920() {}
void fn_113_C924() {}
void fn_113_C928() {}
void fn_113_C92C() {}
void fn_113_C930() {}
void fn_113_C934() {}
void fn_113_C938() {}
void fn_113_C93C() {}
void fn_113_C940() {}
void fn_113_C944() {}
void fn_113_C948() {}
void fn_113_C94C() {}
void fn_113_C950() {}
void fn_113_C954() {}
int fn_113_C958() { return 0x0; }
void fn_113_C960() {}
void fn_113_C964() {}
void fn_113_C968() {}
void fn_113_C96C() {}
void fn_113_C998() {}
void fn_113_C99C() {}
void fn_113_C9A0() {}
void fn_113_C9A4() {}
void fn_113_C9A8() {}
int fn_113_C9AC(u8* p) { return *(int*)(p + 0x110); }
int fn_113_C9C4() { return 0x0; }
void fn_113_C9CC() {}
void fn_113_C9D0() {}
void fn_113_C9D4() {}
void fn_113_C9D8() {}
int fn_113_C9DC() { return 0x0; }
int fn_113_C9E4() { return 0x0; }
void fn_113_C9EC() {}
void fn_113_C9F0() {}
void fn_113_C9F4() {}
void fn_113_C9F8() {}
void fn_113_C9FC() {}
int fn_113_CB70() { return 0x1; }
void fn_113_CB78() {}
int fn_113_CB7C() { return 0x0; }
void fn_113_CB84() {}
int fn_113_CB88() { return 0x0; }
void fn_113_CB90() {}
void fn_113_CB94() {}
void fn_113_CB98() {}
void fn_113_CB9C() {}
void fn_113_CBA0() {}
void fn_113_CBA4() {}
void fn_113_CBA8() {}
void fn_113_CBAC() {}
int fn_113_CBB0(u8* p) { return *(int*)(p + 0x20); }
int fn_113_CC8C(u8* p) { return *(int*)(p + 0x18); }
int fn_113_CD24(u8* p) { return *(int*)(p + 0x10); }
int fn_113_E000() { return 0x3; }
void fn_113_EEFC() {}
void fn_113_F678() {}
int fn_113_F724() { return 0x0; }
void fn_113_F760() {}
int fn_113_F80C() { return 0x0; }
void fn_113_F848() {}
int fn_113_F8F4() { return 0x0; }
void fn_113_F930() {}
int fn_113_F9DC() { return 0x0; }
void fn_113_FA18() {}
int fn_113_FAC4() { return 0x0; }
void fn_113_FB00() {}
int fn_113_FBAC() { return 0x0; }
void fn_113_FBE8() {}
int fn_113_FC94() { return 0x0; }
void fn_113_FCD0() {}
int fn_113_FD7C() { return 0x0; }

// Field, sub-object and vector helpers recovered from the map names (Class__method).
// soArticleMediatorImpl<...>::setAutoRecycle
void fn_113_E008(u8* p, u8 v) { *(u8*)(p + 0x17758) = v; }
// soArticle::checkActivate (virtual slot 0x58)
int fn_113_E080(void* p) { return static_cast<ftPikminArticleSlots*>(p)->checkActivate(); }
// soInstancePoolSub<...>::getInstanceAt (Chain / Dolfin: one slot at +0xC)
void* fn_113_E090(u8* p, s32 idx) {
    if (idx == 0) {
        return p + 0xc;
    }
    return 0;
}
void* fn_113_E0A8(u8* p, s32 idx) {
    if (idx == 0) {
        return p + 0xc;
    }
    return 0;
}
// soInstancePoolSub<...>::getInstanceAt (Pikmin pool, six slots)
void* fn_113_E0C0(u8* p, s32 idx) {
    if (idx == 5) {
        return p + 0x10088;
    }
    if (idx == 4) {
        return p + 0xCD40;
    }
    if (idx == 3) {
        return p + 0x99F8;
    }
    if (idx == 2) {
        return p + 0x66B0;
    }
    if (idx == 1) {
        return p + 0x3368;
    }
    if (idx == 0) {
        return p + 0x20;
    }
    return 0;
}
// soSoundIdExchangerNull singleton address
void* fn_113_8088() { return &g_soSoundIdExchangerNull; }
// ftCommonDataAccesser::getInstance
void* fn_113_62F4() { return &g_ftCommonDataAccesser; }
// soModuleAccesser::getStatusModule
void* fn_113_6860(u8* p) { return *(void**)(*(u8**)(p + 0xd8) + 0x70); }
// soValueAccesser::getValueVariation
s32 fn_113_D0AC() { return g_soValueVariation; }

// wnPikminPikminTroopsManager (sub-object at this+0x1FD08) forwarders; callees are not reconstructed yet
void fn_113_23FA4(void* troops);  // wnPikminPikminTroopsManager::addPikmin
void fn_113_25530(void* troops);  // wnPikminPikminTroopsManager::updateJostlePriority
void fn_113_24558(void* troops, void* other);  // wnPikminPikminTroopsManager::reducePikminAll
void fn_113_C394(u8* p) {
    fn_113_23FA4(p + 0x1FD08);
    fn_113_25530(p + 0x1FD08);
}
void fn_113_C3D4(u8* p) { fn_113_24558(p + 0x1FD08, p + 0x1FE8C); }

void fn_113_25698(void* troops, int team, int teamOwner, int notify);  // wnPikminPikminTroopsManager::setTeamAll
// ftPikmin::setTeam: base Fighter::setTeam first, then the troops manager
void fn_113_C4E0(u8* p, int team, int teamOwner) {
    reinterpret_cast<Fighter*>(p)->Fighter::setTeam(team, teamOwner);
    fn_113_25698(p + 0x1FD08, team, teamOwner, 0);
}

// ftPikmin overrides that forward to StageObject (direct tail calls, not virtual)
void fn_113_AD94(u8* p) { reinterpret_cast<StageObject*>(p)->StageObject::renderDebug(); }
void fn_113_B918(u8* p) { reinterpret_cast<StageObject*>(p)->StageObject::updateNodeSRT(); }

// Fighter virtual overrides reached through the Fighter base sub-object (this - delta): direct tail calls to Fighter::*
void fn_113_FFA8(u8* p, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) {
    reinterpret_cast<Fighter*>(p - 0x54)->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4);
}
void fn_113_FFB0(u8* p, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x64)->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}
bool fn_113_FFB8(u8* p, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) {
    return reinterpret_cast<Fighter*>(p - 0xC4)->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3);
}
void fn_113_FFC0(u8* p, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0xE8)->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser);
}
void fn_113_FFF0(u8* p, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x70)->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}
bool fn_113_FFF8(u8* p, u32 flags) {
    return reinterpret_cast<Fighter*>(p - 0x7C)->Fighter::notifyEventCollisionAttackCheck(flags);
}
void fn_113_10000(u8* p, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x7C)->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser);
}
void fn_113_10008(u8* p, int index, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x88)->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser);
}
bool fn_113_10010(u8* p) {
    return reinterpret_cast<Fighter*>(p - 0x94)->Fighter::notifyEventCollisionShieldCheck();
}
void fn_113_10018(u8* p, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x94)->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser);
}
void fn_113_10020(u8* p, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x94)->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}
bool fn_113_10028(u8* p) {
    return reinterpret_cast<Fighter*>(p - 0xA0)->Fighter::notifyEventCollisionReflectorCheck();
}
void fn_113_10030(u8* p, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0xA0)->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser);
}
void fn_113_10038(u8* p, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float unk7) {
    reinterpret_cast<Fighter*>(p - 0xA0)->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, unk7);
}
bool fn_113_10040(u8* p) {
    return reinterpret_cast<Fighter*>(p - 0xAC)->Fighter::notifyEventCollisionAbsorberCheck();
}
void fn_113_10048(u8* p, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float unk7) {
    reinterpret_cast<Fighter*>(p - 0xAC)->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, unk7);
}
bool fn_113_10050(u8* p) {
    return reinterpret_cast<Fighter*>(p - 0xB8)->Fighter::notifyEventCollisionSearchCheck();
}
void fn_113_10058(u8* p, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0xB8)->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser);
}
void fn_113_10068(u8* p, BaseItem* item, int unk2, bool unk3, u8 unk4) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4);
}
void fn_113_10070(u8* p, BaseItem* item, u32 index, bool unk3) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyEjectAttachItem(item, index, unk3);
}
void fn_113_10078(u8* p, BaseItem* item, u32 index, bool unk3) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyEjectItem(item, index, unk3);
}
void fn_113_10080(u8* p, BaseItem* item) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyShootBulletItem(item);
}
void fn_113_10088(u8* p, BaseItem* item) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyDropItem(item);
}
void fn_113_10090(u8* p, BaseItem* item, u32 nodeIndex, int* unk3) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyThrowItem(item, nodeIndex, unk3);
}
void fn_113_10098(u8* p, BaseItem* item, u32 nodeIndex, int* unk3) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyUseItem(item, nodeIndex, unk3);
}
void fn_113_100A0(u8* p, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5);
}
bool fn_113_100B0(u8* p, BaseItem* item, bool* unk2) {
    return reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyHaveItemPreCheck(item, unk2);
}
void fn_113_100B8(u8* p, soDamage* damage, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0xE8)->Fighter::notifyEventAddDamage(damage, moduleAccesser);
}
void fn_113_100C8(u8* p, float unk1, int unk2) {
    reinterpret_cast<Fighter*>(p - 0xF4)->Fighter::notifyEventPikminFinalAttack(unk1, unk2);
}
void fn_113_100D0(u8* p) {
    reinterpret_cast<Fighter*>(p - 0xF4)->Fighter::notifyEventChangeAdvUnit();
}
void fn_113_100D8(u8* p) {
    reinterpret_cast<Fighter*>(p - 0xF4)->Fighter::notifyEventBeat();
}
void fn_113_100E0(u8* p, float unk1) {
    reinterpret_cast<Fighter*>(p - 0xF4)->Fighter::notifyEventSetDamage(unk1);
}
void fn_113_100E8(u8* p, float unk1, float unk2, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x100)->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser);
}

// Thunks to functions not reconstructed yet (this - delta, then a direct tail call with the same arguments)
void fn_113_4770(u8* p, int a1, int a2, int a3);
void fn_113_DD20(u8* p, int a1, int a2, int a3);
void fn_113_1EB4(u8* p, int a1, int a2, int a3);
void fn_113_C6B4(u8* p, int a1, int a2, int a3);
void fn_113_C3E8(u8* p, int a1, int a2, int a3);
void fn_113_AE80(u8* p, int a1, int a2, int a3);
void fn_113_A42C(u8* p, int a1, int a2, int a3);
void fn_113_BC14(u8* p, int a1, int a2, int a3);
void fn_113_C318(u8* p, int a1, int a2, int a3);
void fn_113_FF80(u8* p, int a1, int a2, int a3) { fn_113_4770(p - 0x4, a1, a2, a3); }
void fn_113_FF88(u8* p, int a1, int a2, int a3) { fn_113_DD20(p - 0x4, a1, a2, a3); }
void fn_113_FF90(u8* p, int a1, int a2, int a3) { fn_113_1EB4(p - 0x4, a1, a2, a3); }
void fn_113_FFC8(u8* p, int a1, int a2, int a3) { fn_113_C6B4(p - 0x40, a1, a2, a3); }
void fn_113_FFD0(u8* p, int a1, int a2, int a3) { fn_113_C3E8(p - 0x48, a1, a2, a3); }
void fn_113_FFE0(u8* p, int a1, int a2, int a3) { fn_113_AE80(p - 0x54, a1, a2, a3); }
void fn_113_FFE8(u8* p, int a1, int a2, int a3) { fn_113_A42C(p - 0x64, a1, a2, a3); }
void fn_113_10060(u8* p, int a1, int a2, int a3) { fn_113_BC14(p - 0xC4, a1, a2, a3); }
void fn_113_100C0(u8* p, int a1, int a2, int a3) { fn_113_C318(p - 0xE8, a1, a2, a3); }

// Vec2f helpers (stored as two floats)
void fn_113_B7F0(float* dst, float* src) {
    dst[0] = src[0];
    dst[1] = src[1];
}
void fn_113_C30C(float* dst, float x, float y) {
    dst[0] = x;
    dst[1] = y;
}
float fn_113_BF74(float* v) { return v[0] * v[0] + v[1] * v[1]; }
void fn_113_BF8C(float* a, float* b, float* out) {
    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1];
}

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_113_100A8(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_113_FFA0(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
} // extern "C"
