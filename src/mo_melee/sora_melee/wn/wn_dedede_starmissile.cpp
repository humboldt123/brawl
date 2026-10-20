#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/dedede/wn_dedede_starmissile.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/reflect/so_reflect_module.h>
#include <so/team/so_team_module.h>
#include <so/ground/so_ground_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_class_info_impl.h>
#include <math.h>

// Registers the Dedede star missile article class (kind 53) in the class info table during static initialization.
wnClassInfoImpl<53, wnDededeStarmissile, wnDededeStarmissileData> g_wnDededeStarmissileClassInfo;

extern soStatusUniqProcess g_wnDededeStarmissileStatusUniqProcess;

wnDededeStarmissile::wnDededeStarmissile(s32 articleId, const wnArticleBuildInfo& info, const wnDededeStarmissileData* data) :
    wnWeaponBuilder<wnDededeStarmissileModuleAccesserBuildConfig>(
        "DededeStarmissile", articleId, info,
        wnModuleAccesserBuildData(0x35, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    soStatusUniqProcess* uniqProcs[1];
    uniqProcs[0] = &g_wnDededeStarmissileStatusUniqProcess;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 1);
    Weapon::deactivate(false);
}

const wnDededeStarmissileData* wnDededeStarmissile::getData() {
    return static_cast<const wnDededeStarmissileData*>(m_data);
}

void wnDededeStarmissile::activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float unk,
                                   int unk2, int mode) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, position, sizeof(Vec3f));
#else
    desc.pos = *position;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getReflectModule().setNoSpeedMul(true);
    Vec2f speed;
    if (mode == 0) {
        speed.m_x = m_param->unk0 * lr;
        speed.m_y = 0.0f;
        m_moduleAccesser->getWorkManageModule().setFloat(m_param->unk4, 0x21000000);
    }
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    energy.m_speed = speed;
    Vec2f brake(0.0f, 0.0f);
    energy.m_brake = brake;
    m_moduleAccesser->getWorkManageModule().setFloat(unk, 0x21000001);
    m_moduleAccesser->getWorkManageModule().setInt(unk2, 0x20000000);
    m_moduleAccesser->getPostureModule().setScale(unk);
    float up = m_param->unkC * unk;
    float down = m_param->unk10 * unk;
    float width = m_param->unk14 * unk;
    m_moduleAccesser->getGroundModule().modifyRhombus(up, down, width, 0);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
}

// HYPOTHESIS: link event 0x19 is the founder's request to remove the missile.
void wnDededeStarmissile::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject,
                                          int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x19:
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000000);
            setNotifyArticleEventRemove(m_moduleAccesser);
            if (isActive() == true) {
                deactivate(false);
            }
        }
        break;
    }
    Weapon::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4);
}

bool wnDededeStarmissile::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 3) {
        return false;
    }
    if (flags & 0x10) {
        return false;
    }
    if (flags & 4) {
        if (unkA4Bit7 == 1) {
            return hop();
        }
        return false;
    }
    if (flags & 8) {
        if (m_moduleAccesser->getReflectModule().isReflect() == 1) {
            m_moduleAccesser->getPostureModule().reverseLr();
            m_moduleAccesser->getPostureModule().updateRotYLr();
            int reflectTeam = m_moduleAccesser->getReflectModule().getTeam();
            m_moduleAccesser->getTeamModule().setTeam(reflectTeam, true);
            m_moduleAccesser->getTeamModule().setHitTeam(reflectTeam);
            m_moduleAccesser->getTeamModule().setTeamOwnerId(m_moduleAccesser->getReflectModule().getTaskId());
            return reflect();
        }
    }
    return false;
}
