#define IP_INPUT_BUTTON_RAW
#define IP_NULL_STICK_ZERO_FIELDS // BYTES: native IpNull::getStick* store the two zero fields inline (Vec2f ctor is out of line here)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/toonlink/wn_toon_link_boomerang.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <ac/ac_anim_cmd_impl.h>


// The status unique processes of the boomerang (wn_link_boomerang_status_uniq_process_*.cpp).
extern soStatusUniqProcess g_wnToonLinkBoomerangStatusUniqProcessHaved;
extern soStatusUniqProcess g_wnToonLinkBoomerangStatusUniqProcessTurn;

wnToonLinkBoomerang::wnToonLinkBoomerang(s32 articleId, const wnArticleBuildInfo& info, const wnToonLinkBoomerangData* data) :
    wnWeaponBuilder<wnToonLinkBoomerangModuleAccesserBuildConfig>(
        "wnToonLinkBoomerang", articleId, info,
        wnModuleAccesserBuildData(0x8D, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[4];
    uniqProcs[0] = 0;
    uniqProcs[2] = 0;
    uniqProcs[1] = &g_wnToonLinkBoomerangStatusUniqProcessHaved;
    uniqProcs[3] = &g_wnToonLinkBoomerangStatusUniqProcessTurn;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 4);
    Weapon::deactivate(false);
}

void wnToonLinkBoomerang::activate(int founderTaskId, int resourceId, int team) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, position, 1.0f, team, 0, 2,
                        0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    m_moduleAccesser->getGroundModule().setOffsetY(m_param->unk0, 0);
    m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
}

// HYPOTHESIS: two small helpers of the original source set the flight energy from a length and an angle; their named
// vector locals are what give shoot its native stack slots (the vectors sit below the safe-position return temporary).
static inline void setSpeedAtAngle(wnKineticEnergyNormal& normal, float speed, float angle) {
    Vec2f velocity(speed * cosf(angle), speed * sinf(angle));
    normal.m_speed = velocity;
}

// The deceleration points against the flight direction.
static inline void setAccelAtAngle(wnKineticEnergyNormal& normal, float deceleration, float angle) {
    Vec2f decel(-deceleration * cosf(angle), -deceleration * sinf(angle));
    normal.m_accel = decel;
}

// The boomerang is released from the thrower's hand node 3 and flies towards the angle, slowing down by the deceleration
// parameter. HYPOTHESIS: the work int 0x10000005 is the time until it turns around and the work float 0x11000001 the angle.
void wnToonLinkBoomerang::shoot(float lr, float angle, u32 kind) {
    Vec3f position = m_moduleAccesser->getModelModule().getNodeGlobalPosition(3, false);
    position.m_z = m_moduleAccesser->getGroundModule().getZ();
    Vec3f rotation = m_moduleAccesser->getModelModule().getNodeGlobalRotation(3, false);
    rotation.m_y = m_param->unk14;
    m_moduleAccesser->getPostureModule().init(&position, lr);
    Vec2f safePos = m_moduleAccesser->getLinkModule().getParentShapeCenterPos(3);
    m_moduleAccesser->getGroundModule().setShapeSafePos(&safePos, 0);
    removeConstraint();
    int time;
    float speed;
    if (kind == 1) {
        time = m_param->unk8;
        speed = m_param->unk10;
    } else {
        time = m_param->unk4;
        speed = m_param->unkC;
    }
    m_moduleAccesser->getWorkManageModule().setInt(time, 0x10000005);
    m_moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000001);
    Vec3f angles;
    angles.m_x = 0.0f;
    angles.m_y = 0.0f;
    angles.m_z = 57.29578f * angle;
    m_moduleAccesser->getPostureModule().setRot(&angles, 0);
    m_moduleAccesser->getPostureModule().setRot(&rotation, 1);
    updateNodeSRT();
    m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    float deceleration = m_param->unk1C;
    setSpeedAtAngle(normal, speed, angle);
    setAccelAtAngle(normal, deceleration, angle);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000004);
}

void wnToonLinkBoomerang::updateNodeSRT() {
    Vec3f scaling;
    Vec3f rotation;
    Vec3f rotation2;
    Vec3f position;
    float scale = m_moduleAccesser->getPostureModule().getScale();
    scaling.m_z = scale;
    scaling.m_y = scale;
    scaling.m_x = scale;
    position = m_moduleAccesser->getPostureModule().getPos();
    rotation = m_moduleAccesser->getPostureModule().getRot(0);
    rotation2 = m_moduleAccesser->getPostureModule().getRot(1);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scaling, &rotation, &position);
    m_moduleAccesser->getModelModule().setNodeRotate(1, &rotation2);
}

bool wnToonLinkBoomerang::reflect() {
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000004);
    return Weapon::reflect();
}

bool wnToonLinkBoomerang::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

// HYPOTHESIS: small vector helpers of the original source. The stack slots of notifyEventAnimCmd show their nesting:
// the bounce temporaries come after the speed and deceleration temporaries, in the order a helper inlined through a
// second helper gives (one level of inlining lists a helper's temporaries in reverse).
static inline void setSpeed(wnKineticEnergyNormal& normal, const Vec2f& speed) {
    normal.m_speed = speed;
}

// Mirrors v on a surface with unit normal n, given the dot product of the two.
static inline void reflect(Vec2f& v, const Vec2f& n, float dot) {
    v = v - (n * 2.0f) * dot;
}

// Mirrors v on a surface with unit normal n.
static inline void reflect(Vec2f& v, const Vec2f& n) {
    float dot = n.m_x * v.m_x + n.m_y * v.m_y;
    ::reflect(v, n, dot);
}

// The animation command 6 of the flight bounces the boomerang off the ground it touches: the speed is mirrored on the contact
// normal (or the boomerang turns around when the surface is steeper than the turn angle) and the deceleration follows the new
// direction. HYPOTHESIS: the touch masks 0x52 / 0xA4 are the walls on the facing side.
bool wnToonLinkBoomerang::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, int unk3) {
    if (Weapon::notifyEventAnimCmd(cmd, acc, unk3)) {
        return true;
    }
    s8 group = cmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (cmd->getType() <= -1 || cmd->getType() >= 11) {
        return false;
    }
    switch (cmd->getType()) {
    case 6: {
        Vec3f angles;
        const wnToonLinkBoomerangParam* param;
        wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*acc->getKineticModule().getEnergy(0));
        Vec2f speed = normal.getSpeed();
        param = m_param;
        if (!acc->getGroundModule().isTouch((grCollStatus::TouchMask)9, 0)) {
            if (acc->getPostureModule().getLr() == 1.0f) {
                if (acc->getGroundModule().isTouch((grCollStatus::TouchMask)0xa4, 0) == 1) {
                    Vec2f wall = acc->getGroundModule().getTouchNormal(0xa4, 0);
                    if (wall.angle(&speed) >= 1.57079637f + param->unk28) {
                        m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
                        return true;
                    }
                }
            } else {
                if (acc->getGroundModule().isTouch((grCollStatus::TouchMask)0x52, 0) == 1) {
                    Vec2f wall = acc->getGroundModule().getTouchNormal(0x52, 0);
                    if (wall.angle(&speed) >= 1.57079637f + param->unk28) {
                        m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
                        return true;
                    }
                }
            }
        }
        Vec2f contact = acc->getGroundModule().getTouchNormal(0xff, 0);
        ::reflect(speed, contact);
        ::setSpeed(normal, Vec2f(speed.m_x, speed.m_y));
        float lengthSq = speed.m_x * speed.m_x + speed.m_y * speed.m_y;
        float length;
        if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
            length = 0.0f;
        } else {
            length = lengthSq * rsqrtf(lengthSq);
        }
        if (length > 9.99999975e-06f) {
            Vec2f accel = speed * -(m_param->unk1C / length);
            normal.m_accel = accel;
        }
        float angle = atan2f(speed.m_y, speed.m_x);
        m_moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000001);
        angles.m_x = 0.0f;
        angles.m_y = 0.0f;
        angles.m_z = 57.29578f * angle;
        m_moduleAccesser->getPostureModule().setRot(&angles, 0);
        return true;
    }
    }
    return false;
}

// BYTES: the native unit carries a weak out-of-line copy of the base accesser destructor.
#pragma dont_inline on
soParamAccesser::~soParamAccesser() { }
#pragma dont_inline off
