#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/link/wn_link_boomerang.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <ac/ac_anim_cmd_impl.h>

// Native helper: vec2fAngle__FP5Vec2fP5Vec2f at 0x8003DDD0.
float vec2fAngle(Vec2f*, Vec2f*);

// The status unique processes of the boomerang (wn_link_boomerang_status_uniq_process_*.cpp).
extern soStatusUniqProcess g_wnLinkBoomerangStatusUniqProcessHaved;
extern soStatusUniqProcess g_wnLinkBoomerangStatusUniqProcessTurn;

wnLinkBoomerang::wnLinkBoomerang(s32 articleId, const wnArticleBuildInfo& info, const wnLinkBoomerangData* data) :
    wnWeaponBuilder<wnLinkBoomerangModuleAccesserBuildConfig>(
        "wnLinkBoomerang", articleId, info,
        wnModuleAccesserBuildData(3, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[4];
    uniqProcs[0] = 0;
    uniqProcs[1] = &g_wnLinkBoomerangStatusUniqProcessHaved;
    uniqProcs[2] = 0;
    uniqProcs[3] = &g_wnLinkBoomerangStatusUniqProcessTurn;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 4);
    Weapon::deactivate(false);
}

void wnLinkBoomerang::activate(int founderTaskId, int resourceId, int team) {
    Vec3f position(0.0f, 0.0f, 0.0f);
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
    __memcpy(&desc.pos, &position, sizeof(Vec3f));
#else
    desc.pos = position;
#endif
    desc.lr = 1.0f;
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
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    m_moduleAccesser->getGroundModule().setOffsetY(m_param->unk0, 0);
    m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
}

// The boomerang is released from the thrower's hand node 3 and flies towards the angle, slowing down by the deceleration
// parameter. HYPOTHESIS: the work int 0x10000005 is the time until it turns around and the work float 0x11000001 the angle.
void wnLinkBoomerang::shoot(float lr, float angle, u32 kind) {
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
    normal.m_speed = Vec2f(speed * cosf(angle), speed * sinf(angle));
    normal.m_accel = Vec2f(-deceleration * cosf(angle), -deceleration * sinf(angle));
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000004);
}

void wnLinkBoomerang::updateNodeSRT() {
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

bool wnLinkBoomerang::reflect() {
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000004);
    return Weapon::reflect();
}

bool wnLinkBoomerang::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

// The animation command 6 of the flight bounces the boomerang off the ground it touches: the speed is mirrored on the contact
// normal (or the boomerang turns around when the surface is steeper than the turn angle) and the deceleration follows the new
// direction. HYPOTHESIS: the touch masks 0x52 / 0xA4 are the walls on the facing side.
bool wnLinkBoomerang::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, int unk3) {
    if (Weapon::notifyEventAnimCmd(cmd, acc, unk3)) {
        return true;
    }
    if (!isObserv(cmd->getGroup())) {
        return false;
    }
    if (cmd->getType() <= -1 || cmd->getType() >= 11) {
        return false;
    }
    switch (cmd->getType()) {
    case 6: {
        Vec3f angles;
        const wnLinkBoomerangParam* param;
        wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*acc->getKineticModule().getEnergy(0));
        Vec2f speed = normal.getSpeed();
        param = m_param;
        if (!acc->getGroundModule().isTouch((grCollStatus::TouchMask)9, 0)) {
            if (acc->getPostureModule().getLr() == 1.0f) {
                if (acc->getGroundModule().isTouch((grCollStatus::TouchMask)0xa4, 0) == 1) {
                    Vec2f wall = acc->getGroundModule().getTouchNormal(0xa4, 0);
                    if (vec2fAngle(&wall, &speed) >= 1.57079637f + param->unk28) {
                        acc->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
                        return true;
                    }
                }
            } else {
                if (acc->getGroundModule().isTouch((grCollStatus::TouchMask)0x52, 0) == 1) {
                    Vec2f wall = acc->getGroundModule().getTouchNormal(0x52, 0);
                    if (vec2fAngle(&wall, &speed) >= 1.57079637f + param->unk28) {
                        acc->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
                        return true;
                    }
                }
            }
        }
        Vec2f contact = acc->getGroundModule().getTouchNormal(0xff, 0);
        float dot = contact.m_x * speed.m_x + contact.m_y * speed.m_y;
        speed -= (contact * 2.0f) * dot;
        Vec2f newSpeed(speed.m_x, speed.m_y);
        normal.m_speed = newSpeed;
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
        float angle = atan2(speed.m_y, speed.m_x);
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
