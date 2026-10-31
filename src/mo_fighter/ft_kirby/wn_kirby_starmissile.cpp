#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
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
#include <so/model/so_model_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <math.h>

wnKirbyStarmissileStatusUniqProcess g_wnKirbyStarmissileStatusUniqProcess;

wnKirbyStarmissile::wnKirbyStarmissile(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyStarmissileData* data) :
    wnWeaponBuilder<wnKirbyStarmissileModuleAccesserBuildConfig>(
        "wnKirbyStarmissile", articleId, info,
        wnModuleAccesserBuildData(8, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_energyGravity(0.0f, -m_param->unk30, -m_param->unk34) {
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[0] = &g_wnKirbyStarmissileStatusUniqProcess;
    uniqProcs[1] = &g_wnKirbyStarmissileStatusUniqProcess;
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
    desc.pos = *position;
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

void wnKirbyStarmissileStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(2));
    wnKirbyStarmissile& missile = dynamic_cast<wnKirbyStarmissile&>(moduleAccesser->getStageObject());
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
        gravity.m_speedY = 0.0f;
        gravity.disable();
        break;
    case 1:
        moduleAccesser->getWorkManageModule().setInt(missile.getData()->m_param->unk48, 0x20000002);
        gravity.m_speedY = missile.getData()->m_param->unk2C;
        gravity.enable();
        break;
    }
}

void wnKirbyStarmissileStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) {
    wnKirbyStarmissile& missile = dynamic_cast<wnKirbyStarmissile&>(moduleAccesser->getStageObject());
    if (moduleAccesser->getWorkManageModule().getInt(0x20000001) == 2) {
        Vec3f rot = moduleAccesser->getModelModule().getNodeRotate(1);
        float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000002);
        angle += missile.getData()->m_param->unk24;
        if (angle > 360.0f) {
            angle -= 360.0f;
        }
        rot.m_x = angle;
        moduleAccesser->getModelModule().setNodeRotate(1, &rot);
        moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000002);
    } else {
        wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        Vec2f speed = energy.getSpeed();
        float brake = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
        moduleAccesser->getPostureModule().getLr();
        float length = speed.length();
        if (length > brake) {
            float remain = length - brake;
            speed.m_x = speed.m_x * remain / length;
            speed.m_y = speed.m_y * remain / length;
        } else {
            speed.m_x = 0.0f;
        }
        energy.m_speed = speed;
        Vec3f rot = moduleAccesser->getModelModule().getNodeRotate(1);
        float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000002);
        angle += missile.getData()->m_param->unk14;
        if (angle > 360.0f) {
            angle -= 360.0f;
        }
        rot.m_x = angle;
        moduleAccesser->getModelModule().setNodeRotate(1, &rot);
        moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000002);
    }
}

void wnKirbyStarmissileStatusUniqProcess::execStop(soModuleAccesser* moduleAccesser) {
    Vec3f rot = moduleAccesser->getModelModule().getNodeRotate(1);
    rot.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000002);
    moduleAccesser->getModelModule().setNodeRotate(1, &rot);
}

void wnKirbyStarmissileStatusUniqProcess::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000001) != 2) {
        int power = moduleAccesser->getWorkManageModule().getInt(0x20000000);
        for (int i = 0; i < (int)moduleAccesser->getCollisionAttackModule().getPartSize(); i++) {
            if (moduleAccesser->getCollisionAttackModule().isAttack(i, false)) {
                moduleAccesser->getCollisionAttackModule().setPower(i, power, false);
            }
        }
    }
    if (moduleAccesser->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(0xf7), 0)) {
        Vec2f prevSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        Vec2f speed = prevSpeed;
        Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(0xff, 0);
        if (normal.length()) {
            float dot = normal.m_x * speed.m_x + normal.m_y * speed.m_y;
            speed = speed - normal * 2.0f * dot;
            wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
            energy.m_speed = speed;
            if (prevSpeed.m_x * speed.m_x + prevSpeed.m_y * speed.m_y < 0.0f) {
                soLinkEventStarShot event(0x1a);
                event.m_lr = normal.m_x < 0.0f ? -1 : 1;
                moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
            }
        }
    }
}

void wnKirbyStarmissileStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
        soLinkEventStarShot event(0x1a);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().onFlag(0x22000000);
    }
}
