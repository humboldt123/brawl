#include <wn/robot/wn_robot_beam.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/so_module_accesser.h>
#include <math.h>
#include <memory.h>

// Angle between two vectors in radians (main dol).
float vec2fAngle(Vec2f* a, Vec2f* b);

// Work variables (from the base weapon): int 0x10000004 is the remaining life, flag 0x12000003 marks the strong beam.

// Starts the beam: it flies along the head tilt (angle, degrees) at the speed of the weak or the strong shot.
void wnRobotBeam::activate(float lr, float angle, s32 founderTaskId, u32 resourceId, s32 team, const Vec3f* position,
                           bool fullCharge, s32 variant) {
    s32 life = fullCharge ? m_param->strongLife : m_param->weakLife;
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: the original copies the position with integer moves.
    __memcpy(&desc.pos, position, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = life;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unk40 = variant;
    Weapon::activate(&desc);
    float speed;
    if (fullCharge) {
        speed = m_param->strongSpeed;
        m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
    } else {
        speed = m_param->weakSpeed;
    }
    float shotAngle = -angle;
    if (shotAngle < 0.0f) {
        shotAngle += 360.0f;
    }
    float radians = shotAngle * 0.017453292f;
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    normal.m_speed = Vec2f(lr * (speed * (float)cos(radians)), speed * (float)sin(radians));
    // Point the model along the new velocity.
    Vec3f rot = m_moduleAccesser->getPostureModule().getRot(0);
    Vec2f velocity;
    Vec2f::copy(velocity, normal.getSpeed());
    velocity.normalize();
    float forward = velocity.m_x * m_moduleAccesser->getPostureModule().getLr();
    float vertical = velocity.m_y;
    rot.m_x = -(float)atan2(vertical, forward) * 57.29578f;
    m_moduleAccesser->getPostureModule().setRot(&rot, 0);
    // Begin flight immediately, bypassing ordinary transition restrictions.
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

// Anim cmd type 6 is the wall check: the beam bounces off a surface it meets at a shallow enough angle for as long as
// it has life left, and breaks up otherwise.
bool wnRobotBeam::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, moduleAccesser, index)) {
        return true;
    }
    s8 group = cmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (cmd->getType() > -1 && cmd->getType() < 11) {
        if (cmd->getType() == 6) {
            wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
            Vec2f velocity;
            Vec2f speed;
            Vec2f::copy(speed, normal.getSpeed());
            Vec2f::copy(velocity, speed);
            Vec2f surface;
            Vec2f::copy(surface, moduleAccesser->getGroundModule().getTouchNormal(static_cast<grCollStatus::TouchMask>(0xff), 0));
            float lr = moduleAccesser->getPostureModule().getLr();
            float incidence = vec2fAngle(&velocity, &surface);
            if (m_param->ricochetAngle * 0.017453292f < incidence - 1.5707964f) {
                deactivate(false);
                return true;
            }
            if (moduleAccesser->getWorkManageModule().getInt(0x10000004) - m_param->ricochetLife <= 0) {
                moduleAccesser->getStatusModule().changeStatusRequest(1, moduleAccesser);
                return true;
            }
            // Reflect the velocity about the surface normal.
            velocity = velocity - surface * 2.0f * (surface.m_x * velocity.m_x + surface.m_y * velocity.m_y);
            float newSign = velocity.m_x < 0.0f ? -1 : 1;
            float oldSign = speed.m_x < 0.0f ? -1 : 1;
            if (newSign * oldSign < 0.0f) {
                moduleAccesser->getPostureModule().setLr(lr * -1.0f);
                moduleAccesser->getPostureModule().updateRotYLr();
                moduleAccesser->getStageObject().updateNodeSRT();
            }
            // MATCH-ONLY: the native speed assignment materializes an XY pair.
            normal.m_speed = Vec2f(velocity.m_x, velocity.m_y);
            return true;
        }
    }
    return false;
}

bool wnRobotBeam::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 0x13) {
        return false;
    }
    if (flags & 4) {
        if ((((u32)unkA4 >> 7) & 1) == 1) {
            return hop();
        }
        return false;
    }
    if ((flags & 8) && m_moduleAccesser->getReflectModule().isReflect() == true) {
        m_moduleAccesser->getPostureModule().reverseLr();
        m_moduleAccesser->getPostureModule().updateRotYLr();
        int team = m_moduleAccesser->getReflectModule().getTeam();
        m_moduleAccesser->getTeamModule().setTeam(team, true);
        m_moduleAccesser->getTeamModule().setHitTeam(team);
        soTeamModule& teamModule = m_moduleAccesser->getTeamModule();
        teamModule.setTeamOwnerId(m_moduleAccesser->getReflectModule().getTaskId());
        return reflect();
    }
    return false;
}
