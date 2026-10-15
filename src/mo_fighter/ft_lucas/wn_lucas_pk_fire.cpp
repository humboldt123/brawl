#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_STATUS_PLAIN_NULL_ARRAY
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/lucas/wn_lucas_pk_fire.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

// The status unique processes of the bolt (wn_lucas_pk_fire_status_uniq_process_*.cpp).
extern soStatusUniqProcess g_wnLucasPKFireStatusUniqProcessShoot;
extern soStatusUniqProcess g_wnLucasPKFireStatusUniqProcessPillar;

wnLucasPKFire::wnLucasPKFire(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKFireData* data) :
    wnWeaponBuilder<wnLucasPKFireModuleAccesserBuildConfig>(
        "LucasPKFire", articleId, info,
        wnModuleAccesserBuildData(78, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_energyGravity() {
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[0] = &g_wnLucasPKFireStatusUniqProcessShoot;
    uniqProcs[1] = &g_wnLucasPKFireStatusUniqProcessPillar;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyGravity, 2, soKineticEnergy::AttributeFlag(1), -1);
    m_moduleAccesser->getCollisionHitModule().add((soSet<soCollisionHitData::Simple>*)data->m_hitData, 0);
    Weapon::deactivate(false);
}

void wnLucasPKFire::activate(int founderTaskId, int team, Vec3f* pos, Vec2f* speed, float lr, float rot) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.unkC = desc.unk8 = desc.resourceId = 0xFFFF;
    desc.unk14 = desc.unk10 = -1;
    desc.unk1C = desc.unk18 = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, pos, sizeof(Vec3f));
#else
    desc.pos = *pos;
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
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
    soStatusModule& status = m_moduleAccesser->getStatusModule();
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*kinetic.getEnergy(2));
    normal.m_speed = *speed;
    normal.m_accel = Vec2f(0.0f, 0.0f);
    normal.m_speedTarget = *speed;
    normal.m_speedLimit = *speed;
    normal.enable();
    gravity.m_speedY = 0.0f;
    gravity.m_gravity = -m_param->m_gravity;
    gravity.m_speedLimit = m_param->m_speedLimit;
    gravity.disable();
    posture.setRot(&Vec3f(rot, 0.0f, 0.0f), 0);
    status.changeStatusForce(0, m_moduleAccesser);
}

bool wnLucasPKFire::notifyEventCollisionAttackCheck(u32 flags) {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0) {
        if (flags & 3) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
            return false;
        }
        if (flags & 0x14) {
            deactivate(false);
            return true;
        }
    } else if (flags & 0x17) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnLucasPKFire::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int unk,
                                           soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
    Weapon::notifyEventCollisionHit(power, attackData, index, unk, moduleAccesser, collisionLog);
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 1) {
        const wnLucasPKFireParam* param = dynamic_cast<wnLucasPKFire&>(*moduleAccesser->m_stageObject).m_param;
        float damage = moduleAccesser->getDamageModule().getDamage(0);
        s32 lifeNow = moduleAccesser->getWorkManageModule().getInt(Weapon::Instance::Work::Int_Life);
        float life = lifeNow - damage * param->m_damageLifeMul;
        if (life < 0.0f) {
            life = 1.0f;
        }
        moduleAccesser->getWorkManageModule().setInt(life, Weapon::Instance::Work::Int_Life);
    }
}
