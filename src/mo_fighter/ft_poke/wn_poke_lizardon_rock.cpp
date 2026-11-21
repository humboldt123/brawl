#define SO_BUILDER_HELPERS_WEAK
#define IP_INPUT_BUTTON_RAW
#define WN_ARTICLE_ANIMCMD_INT
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/poke/wn_poke_lizardon_rock.h>
#include <ac/ac_anim_cmd_impl.h>
#include <ft/poke/ft_poke_lizardon_article_pools.h>
#include <so/so_module_accesser.h>
#include <mt/mt_prng.h>
#include <types.h>
#include <wn/wn_activate_desc.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

wnPokeLizardonRock::wnPokeLizardonRock(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonRockData* data) :
    wnWeaponBuilder<wnPokeLizardonRockModuleAccesserBuildConfig>(
        "PokeLizardonRock", articleId, info,
        wnModuleAccesserBuildData(0xE, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    soStatusUniqProcess* uniqProcs[5] = { 0 };
    uniqProcs[1] = &g_wnPokeLizardonRockStatusUniqProcessCrushed;
    uniqProcs[2] = &g_wnPokeLizardonRockStatusUniqProcessCrushed;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 5);
    m_moduleAccesser->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData::Simple>*>(const_cast<u8*>(data->unk38)), 0);
    m_moduleAccesser->getGroundModule().setOffsetY(m_param->unk0, 0);
    Weapon::deactivate(false);
}

void wnPokeLizardonRock::shoot(bool unk) {
    if (unk == true) {
        getModuleAccesser()->getStatusModule().changeStatusForce(3, getModuleAccesser());
        getModuleAccesser()->getWorkManageModule().offFlag(0x12000000);
        return;
    }
    getModuleAccesser()->getStatusModule().changeStatusForce(2, getModuleAccesser());
}

void wnPokeLizardonRock::activate(u32 founderTaskId, u32 resourceId, int team,
                                 const Vec3f& pos, float lr, float unk) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1,
                        0, 0, pos, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.flag4DBit7 = 0;
    desc.flag4DBit6 = 0;
    Weapon::activate(&desc);
    getModuleAccesser()->getWorkManageModule().offFlag(0x12000003);
    getModuleAccesser()->getWorkManageModule().offFlag(0x12000004);
    getModuleAccesser()->getWorkManageModule().setInt(team, 0x10000005);
    getModuleAccesser()->getWorkManageModule().setFloat(unk, 0x11000002);
    getModuleAccesser()->getWorkManageModule().onFlag(0x12000000);
    getModuleAccesser()->getStatusModule().changeStatusForce(0, getModuleAccesser());
    updateNodeSRT();
}

void wnPokeLizardonRock::updateNodeSRT() {
    if (getModuleAccesser()->getStatusModule().getStatusKind() == 3) {
        Vec3f scale;
        Vec3f rotation;
        Vec3f nodeRotation;
        Vec3f position;
        scale.m_x = scale.m_y = scale.m_z = getModuleAccesser()->getPostureModule().getScale();
        position = getModuleAccesser()->getPostureModule().getPos();
        rotation = getModuleAccesser()->getPostureModule().getRot(0);
        getModuleAccesser()->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
        nodeRotation = getModuleAccesser()->getPostureModule().getRot(1);
        nodeRotation.m_x += getModuleAccesser()->getWorkManageModule().getFloat(0x11000001);
        getModuleAccesser()->getModelModule().setNodeRotate(2, &nodeRotation);
    } else {
        StageObject::updateNodeSRT();
    }
}

bool wnPokeLizardonRock::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, acc, index)) return true;
    s8 group = cmd->getGroup();
    if (!isObserv(group)) return false;
    if (cmd->getType() <= -1 || cmd->getType() >= 11) return false;
    switch (cmd->getType()) {
    case 6:
        getModuleAccesser()->getStatusModule().changeStatusForce(4, getModuleAccesser());
        getModuleAccesser()->getWorkManageModule().onFlag(0x12000000);
        return true;
    }
    return false;
}

void wnPokeLizardonRock::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc,
                                       StageObject* sender, int index) {
    switch (event->m_eventKind) {
    case 0x838:
        getModuleAccesser()->getStatusModule().changeStatusForce(1, getModuleAccesser());
        return;
    }
    Weapon::notifyEventLink(event, acc, sender, index);
}

void wnPokeLizardonRock::notifyEventCollisionHit(float power, soCollisionAttackData* data,
                                               u32 index, int kind, soModuleAccesser* acc,
                                               soCollisionLog* log) {
    Weapon::notifyEventCollisionHit(power, data, index, kind, acc, log);
    if (!getModuleAccesser()->getWorkManageModule().isFlag(0x12000004)) {
        soLinkEventArgs event(0x456);
        getModuleAccesser()->getLinkModule().sendEventParents(3, event);
    }
}

void wnPokeLizardonRock::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* acc) {
    switch (statusKind) {
    case 1:
        getModuleAccesser()->getWorkManageModule().onFlag(0x12000004);
        break;
    case 2: {
        getModuleAccesser()->getLinkModule().adjustModelConstraintPosture(&Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
        getModuleAccesser()->getLinkModule().removeModelConstraint(true);
        getModuleAccesser()->getWorkManageModule().onFlag(0x12000004);
        updateNodeSRT();
        break;
    }
    case 3: {
        wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*getModuleAccesser()->getKineticModule().getEnergy(0));
        normal.m_speed = Vec2f(0.0f, 0.0f);
        normal.m_speedTarget = Vec2f(3.4028235e38f, m_param->unk8);
        normal.m_accel = Vec2f(0.0f, -m_param->unk4);
        getModuleAccesser()->getLinkModule().adjustModelConstraintPosture(&Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
        getModuleAccesser()->getLinkModule().removeModelConstraint(true);
        Vec3f pos = getModuleAccesser()->getPostureModule().getPos();
        Vec3f nodePos1 = getModuleAccesser()->getModelModule().getNodeGlobalPosition(1, false);
        Vec3f nodePos0 = getModuleAccesser()->getModelModule().getNodeGlobalPosition(0, false);
        getModuleAccesser()->getPostureModule().setPos(&(pos + (nodePos1 - nodePos0)));
        Vec3f zero(0.0f, 0.0f, 0.0f);
        Vec3f rot = getModuleAccesser()->getPostureModule().getRot(0);
        getModuleAccesser()->getPostureModule().setRot(&zero, 0);
        getModuleAccesser()->getPostureModule().setRot(&rot, 1);
        updateNodeSRT();
        break;
    }
    }
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, acc);
}

bool wnPokeLizardonRock::activeArticle(wnPokeLizardonRockStone* stone, soModuleAccesser* acc) {
    wnPokeLizardonRock& rock = dynamic_cast<wnPokeLizardonRock&>(acc->getStageObject());
    const wnPokeLizardonRockParam* param = rock.m_param;
    // Declared up front in this order (and the modules read from the enumeration): the saved registers follow it.
    soPostureModule* posture;
    soResourceModule* resource;
    int taskId;
    int team;
    acc->getWorkManageModule().getInt(0x20000000);
    float baseAngle = acc->getWorkManageModule().getFloat(0x11000002);
    float angle = baseAngle + (180.0f * randf() - 90.0f);
    acc->getWorkManageModule().setFloat(angle, 0x21000000);
    team = acc->getWorkManageModule().getInt(0x10000005);
    Vec3f pos;
    if (acc->getStatusModule().getStatusKind() == 1) {
        pos = acc->getModelModule().getNodeGlobalPosition(1, false);
    } else {
        pos = acc->getModelModule().getNodeGlobalPosition(0, false);
    }
    posture = acc->m_enumerationStart->m_postureModule;
    resource = acc->m_enumerationStart->m_resourceModule;
    taskId = acc->getStageObject().m_taskId;
    stone->activate(taskId, resource->getResourceIdAccesser()->getAnmResId(), team, &pos, posture->getLr(), angle, param->unk14,
                    param->unk18, randi(2));
    return false;
}

// TODO: clean up, give variables real names
namespace jj_104B0 {
extern "C" {
typedef struct { void* a; void* b; } Struct1F8;
extern signed char lbl_116_bss_1F4;
extern Struct1F8 lbl_116_bss_1F8;
extern unsigned char lbl_116_bss_17C[];
extern unsigned char lbl_116_data_26C4[];
extern unsigned char lbl_116_data_2698[];
extern unsigned char lbl_116_data_2630[];
extern unsigned char lbl_116_data_74F8[];
void* __dt__21soArticleMediatorNullFv(void*);
void* __register_global_object(void*, void*, void*);

void* fn_116_104B0(void) {
    if (lbl_116_bss_1F4 == 0) {
        lbl_116_bss_1F8.a = lbl_116_data_26C4;
        lbl_116_bss_1F8.b = lbl_116_data_2698;
        lbl_116_bss_1F8.a = lbl_116_data_2630;
        lbl_116_bss_1F8.b = lbl_116_data_2630 + 0x10;
        lbl_116_bss_1F8.a = lbl_116_data_74F8;
        lbl_116_bss_1F8.b = lbl_116_data_74F8 + 0x10;
        __register_global_object(&lbl_116_bss_1F8, __dt__21soArticleMediatorNullFv, &lbl_116_bss_17C);
        lbl_116_bss_1F4 = 1;
    }
    return &lbl_116_bss_1F8;
}
}
}

