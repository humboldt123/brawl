// MATCH-ONLY: retain the native vector helper calls and aggregate copies.
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_SCALE_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <wn/poke/wn_poke_fushigisou_leaf_cutter.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <ac/ac_anim_cmd_impl.h>
#include <mt/mt_prng.h>
#include <math.h>

float vec2fAngle(Vec2f*, Vec2f*);

// HYPOTHESIS: descriptive names, corroborated by the movement-status reader.
enum {
    Work_Float_SpinAngle = 0x11000001,
    Work_Float_SpinStep = 0x11000002,
    Work_Float_Heading = 0x11000003,
    Work_Float_TargetHeading = 0x11000004,
};

void wnPokeFushigisouLeafCutter::activate(int founderTaskId, int resourceId, int team,
                                        Vec3f* position, float lr, float speed, float angle) {
    const wnPokeFushigisouLeafCutterParam* param;
    int life = m_param->lifeFrames;
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    __memcpy(&desc.pos, position, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = life;
    desc.unk38 = 5;
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
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);

    float sine = (float)sin(0.017453292f * angle);
    float cosine = (float)cos(0.017453292f * angle);
    Vec2f velocity(lr * (speed * cosine), speed * sine);
    float brake = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
    wnKineticEnergyNormal& normal =
        dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    normal.resetEnergy(1, &velocity, &Vec3f(0.0f, 0.0f, 0.0f), m_moduleAccesser);
    Vec2f braking;
    braking.m_x = brake;
    braking.m_y = 0.0f;
    normal.m_brake = braking;
    normal.enable();
    m_moduleAccesser->getCollisionAttackModule().setSpeed(&velocity);

    param = m_param;
    float spin = param->spinMin + (param->spinMax - param->spinMin) * randf();
    m_moduleAccesser->getWorkManageModule().setFloat(spin, Work_Float_SpinStep);
    m_moduleAccesser->getWorkManageModule().setFloat(angle, Work_Float_Heading);
    m_moduleAccesser->getWorkManageModule().setFloat(angle, Work_Float_TargetHeading);
    updateNodeSRT();
}

// The root follows the article's posture. Node 1 spins in the flight plane;
// its 90-degree Z rotation places the leaf model flat in that plane.
void wnPokeFushigisouLeafCutter::updateNodeSRT() {
    Vec3f scale;
    Vec3f rotation;
    Vec3f leafRotation;
    Vec3f position;
    scale.m_x = scale.m_y = scale.m_z = m_moduleAccesser->getPostureModule().getScale();
    position = m_moduleAccesser->getPostureModule().getPos();
    rotation = m_moduleAccesser->getPostureModule().getRot(0);
    rotation.m_y += m_moduleAccesser->getPostureModule().getRotYLr();
    leafRotation.m_x = 0.0f;
    soWorkManageModule* work = &m_moduleAccesser->getWorkManageModule();
    float spin = work->getFloat(Work_Float_SpinAngle);
    float heading = work->getFloat(Work_Float_Heading);
    leafRotation.m_y = heading + spin;
    leafRotation.m_z = 90.0f;
    m_moduleAccesser->getModelModule().setNodeRotate(1, &leafRotation);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

bool wnPokeFushigisouLeafCutter::notifyEventAnimCmd(acAnimCmd* cmd,
                                                  soModuleAccesser* accesser, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, accesser, index)) return true;
    s8 group = cmd->getGroup();
    if (!isObserv(group)) return false;
    if (cmd->getType() <= -1 || cmd->getType() >= 11) return false;
    switch (cmd->getType()) {
    case 6: {
        wnKineticEnergyNormal& normal =
            dynamic_cast<wnKineticEnergyNormal&>(*accesser->getKineticModule().getEnergy(0));
        Vec2f velocity = normal.getSpeed();
        Vec2f surface;
        Vec2f::copy(surface, accesser->getGroundModule().getTouchNormal(
            static_cast<grCollStatus::TouchMask>(0xFF), 0));
        float incidence = 57.29578f * vec2fAngle(&surface, &velocity);
        if (incidence - 90.0f < soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA4, 0)) {
            float retention = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA5, 0);
            float lr = m_moduleAccesser->getPostureModule().getLr();
            float dot = surface.m_x * velocity.m_x + surface.m_y * velocity.m_y;
            // MATCH-ONLY: retain the native vector expression value copies.
            Vec2f doubledResult;
            surface.scale(2.0f, &doubledResult);
            Vec2f doubledNormal = doubledResult;
            Vec2f projectionResult;
            doubledNormal.scale(dot, &projectionResult);
            Vec2f projection = projectionResult;
            Vec2f reflected = velocity - projection;
            velocity.m_x = reflected.m_x;
            velocity.m_y = reflected.m_y;
            velocity.scale(retention, &velocity);
            normal.m_speed = velocity;
            float angle = 57.29578f * (float)atan2(velocity.m_y, velocity.m_x * lr);
            m_moduleAccesser->getWorkManageModule().setFloat(angle, Work_Float_TargetHeading);
        } else {
            m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
        }
        return true;
    }
    }
    return false;
}

bool wnPokeFushigisouLeafCutter::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) return false;
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
