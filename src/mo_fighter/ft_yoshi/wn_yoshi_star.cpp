#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/yoshi/wn_yoshi_star.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <ac/ac_anim_cmd_impl.h>

void wnYoshiStar::activate(int founderTaskId, int team, Vec3f* pos, float lr) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy from the caller position.
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
    // The consumer reads only the upper two bits of the second flag byte.
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(
        *m_moduleAccesser->getKineticModule().getEnergy(0));
    // Preserve the default horizontal limit while applying the spawn tunings.
    Vec2f speed;
    Vec2f accel;
    Vec2f limit = normal.m_speedLimit;
    speed.m_x = m_param->horizontalSpeed * lr;
    speed.m_y = m_param->verticalSpeed;
    accel.m_x = m_param->horizontalAcceleration * lr;
    accel.m_y = -m_param->gravity;
    limit.m_y = m_param->verticalSpeedLimit;
    normal.m_speed = speed;
    normal.m_accel = accel;
    normal.m_speedTarget = limit;
    normal.m_speedLimit = limit;
}

bool wnYoshiStar::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 0x13) return true;
    if (flags & 4) {
        // MATCH-ONLY: express the high-bit test as the native extracted bit value.
        if (((static_cast<u32>(unkA4) >> 7) & 1) == 1) return hop();
        return true;
    }
    // Reflection changes both facing and attack ownership before the inherited
    // projectile response. Keep the reflector queries in their native order.
    if ((flags & 8) && m_moduleAccesser->getReflectModule().isReflect() == true) {
        m_moduleAccesser->getPostureModule().reverseLr();
        m_moduleAccesser->getPostureModule().updateRotYLr();
        int team = m_moduleAccesser->getReflectModule().getTeam();
        m_moduleAccesser->getTeamModule().setTeam(team, true);
        m_moduleAccesser->getTeamModule().setHitTeam(team);
        soReflectModule& reflector = m_moduleAccesser->getReflectModule();
        soTeamModule& teamModule = m_moduleAccesser->getTeamModule();
        teamModule.setTeamOwnerId(reflector.getTaskId());
        return reflect();
    }
    return false;
}

bool wnYoshiStar::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* a, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, a, index)) return true;
    s8 group = cmd->getGroup();
    if (!isObserv(group)) return false;
    if (cmd->getType() > -1) cmd->getType();
    return false;
}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
int fn_95_172AC(u8* p) { return *(int*)(p + 0xC0); }
int fn_95_172B4(u8* p) { return *(int*)(p + 0x28); }
int fn_95_17414(u8* p) { return *(int*)(p + 0xb8); }
} // extern "C"
