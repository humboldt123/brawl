#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/kirby/wn_kirby_starmissile.h>
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
#include <math.h>

// HYPOTHESIS: the status unique process shared by both statuses of the missile (not yet named).
extern "C" soStatusUniqProcess lbl_96_bss_122C;

wnKirbyStarmissile::wnKirbyStarmissile(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyStarmissileData* data) :
    wnWeaponBuilder<wnKirbyStarmissileModuleAccesserBuildConfig>(
        "wnKirbyStarmissile", articleId, info,
        wnModuleAccesserBuildData(8, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_energyGravity(0.0f, -m_param->unk30, -m_param->unk34) {
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[0] = &lbl_96_bss_122C;
    uniqProcs[1] = &lbl_96_bss_122C;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyGravity, 2, soKineticEnergy::AttributeFlag(1), -1);
    m_moduleAccesser->getKineticModule().unableEnergy(2);
    Weapon::deactivate(false);
}

const wnKirbyStarmissileData* wnKirbyStarmissile::getData() {
    return static_cast<const wnKirbyStarmissileData*>(m_data);
}

void wnKirbyStarmissile::activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float unk,
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
    Vec2f speed(0.0f, 0.0f);
    float scale = unk;
    if (mode == 0) {
        speed.m_x = m_param->unk0 * lr;
        speed.m_y = 0.0f;
        m_moduleAccesser->getWorkManageModule().setFloat(m_param->unk4, 0x21000000);
    } else if (mode == 1) {
        float cosine = cos(m_param->unk8);
        speed.m_x = -lr * (m_param->unkC * cosine);
        float sine = sin(m_param->unk8);
        speed.m_y = m_param->unkC * sine;
        m_moduleAccesser->getWorkManageModule().setFloat(m_param->unk10, 0x21000000);
    } else {
        speed.m_x = m_param->unk28 * lr;
        speed.m_y = 0.0f;
        scale = m_param->unk38;
    }
    m_moduleAccesser->getWorkManageModule().setInt(mode, 0x20000001);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    energy.m_speed = speed;
    Vec2f brake(0.0f, 0.0f);
    energy.m_brake = brake;
    m_moduleAccesser->getWorkManageModule().setFloat(unk, 0x21000001);
    m_moduleAccesser->getWorkManageModule().setInt(unk2, 0x20000000);
    m_moduleAccesser->getPostureModule().setScale(scale);
    if (mode == 2) {
        m_moduleAccesser->getGroundModule().modifyRhombus(m_param->unk3C, m_param->unk40, m_param->unk44, 0);
        m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
    } else {
        float up = m_param->unk18 * unk;
        float down = m_param->unk1C * unk;
        float width = m_param->unk20 * unk;
        m_moduleAccesser->getGroundModule().modifyRhombus(up, down, width, 0);
        m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    }
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
}

// HYPOTHESIS: link event 0x19 is the founder's request to remove the missile.
void wnKirbyStarmissile::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject,
                                         int unk4) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000001) != 2) {
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
    }
    Weapon::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4);
}

bool wnKirbyStarmissile::notifyEventCollisionAttackCheck(u32 flags) {
    if (m_moduleAccesser->getWorkManageModule().getInt(0x20000001) == 2) {
        return false;
    }
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
