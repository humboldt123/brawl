#define MT_VEC3F_SUB_FRESH_REGS
#define MT_VEC3F_SCALE_FRESH_REGS
#define MT_VEC3F_ADD_FRESH_REGS
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_MUL_INLINE
#include <so/status/so_status_module_impl.h>
#include <wn/fox/wn_fox_land_master.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/stageobject.h>
#include <mt/mt_vector.h>
#include <st/st_utility.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_linework.h>
#include <math.h>
#include <types.h>

// Class shapes from the module map.
class wnFoxLandMasterStatusUniqProcessCommon : public soStatusUniqProcess {
public:
    virtual ~wnFoxLandMasterStatusUniqProcessCommon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    void controllCaterpillar(soModuleAccesser* moduleAccesser);
    void slideZFromSpeed(soModuleAccesser* moduleAccesser);
    void vibrationYFromSpeed(soModuleAccesser* moduleAccesser);
    void vibrationYByLanding(soModuleAccesser* moduleAccesser);
};

wnFoxLandMasterStatusUniqProcessCommon g_wnFoxLandMasterStatusUniqProcessCommon;

void wnFoxLandMasterStatusUniqProcessCommon::initStatus(soModuleAccesser* moduleAccesser) {
    const wnFoxLandMasterParam* param = dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject()).getParam();
    if (moduleAccesser->getStatusModule().getStatusKind() == 5) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        float speed = __fabsf(work.getFloat(0x11000003));
        work.setFloat(speed * param->unkC0, 0x21000008);
        work.setFloat(0.0f, 0x21000001);
        int motionKind;
        if (speed >= param->unk48) {
            motionKind = 10;
        } else if (speed >= param->unk4C) {
            motionKind = 9;
        } else {
            motionKind = 8;
        }
        float frame = work.getFloat(0x21000006);
        soMotionChangeParam motionParam;
        motionParam.m_kind = motionKind;
        motionParam.m_frame = frame;
        motionParam.m_rate = 0.0f;
        motionParam._12 = 0;
        motionParam._13 = 0;
        motionParam._14 = 0;
        motionParam._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&motionParam);
    }
    moduleAccesser->getWorkManageModule().setFloat(param->unk44, 0x11000008);
}

// The tread pose: the slide, vibration and tilt the status helpers left in the work floats go to model node 4.
static inline void setTreadPose(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soModelModule& model = moduleAccesser->getModelModule();
    float slideZ = work.getFloat(0x21000000);
    float rotX = work.getFloat(0x21000002);
    float offsetY = work.getFloat(0x21000001);
    float rotZ = work.getFloat(0x21000003);
    Vec3f translate(0.0f, offsetY, slideZ);
    model.setNodeTranslate(4, &translate);
    Vec3f rotate(rotX, 0.0f, rotZ);
    model.setNodeRotate(4, &rotate);
}

void wnFoxLandMasterStatusUniqProcessCommon::execStatus(soModuleAccesser* moduleAccesser) {
    const wnFoxLandMasterParam* param = dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject()).getParam();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    if (statusKind == 5) {
        controllCaterpillar(moduleAccesser);
        vibrationYByLanding(moduleAccesser);
        slideZFromSpeed(moduleAccesser);
        setTreadPose(moduleAccesser);
    } else if (statusKind == 0 || statusKind == 1) {
        float lr = moduleAccesser->getPostureModule().getLr();
        float stickX = moduleAccesser->getControllerModule().getStickX();
        int backFrames = work.getInt(0x20000000);
        if (stickX * lr < 0.0f) {
            backFrames++;
        } else {
            backFrames = 0;
        }
        work.setInt(backFrames, 0x20000000);
        controllCaterpillar(moduleAccesser);
        vibrationYFromSpeed(moduleAccesser);
        slideZFromSpeed(moduleAccesser);
        setTreadPose(moduleAccesser);
    } else if (statusKind == 3 || statusKind == 4) {
        slideZFromSpeed(moduleAccesser);
        setTreadPose(moduleAccesser);
    } else if (statusKind == 6) {
        if (moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL)).m_y > 0.0f) {
            float rotX = work.getFloat(0x11000004);
            if (rotX > 0.0f) {
                rotX -= param->unk44;
                if (rotX < 0.0f) {
                    rotX = 0.0f;
                }
            } else if (rotX < 0.0f) {
                rotX += param->unk44;
                if (rotX > 0.0f) {
                    rotX = 0.0f;
                }
            }
            work.setFloat(rotX, 0x11000004);
            Vec3f rot(-rotX, 0.0f, 0.0f);
            moduleAccesser->getPostureModule().setRot(&rot, 0);
        }
    }
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL));
    work.setFloat(speed.m_x, 0x11000002);
    work.setFloat(speed.m_y, 0x11000003);
}

// The length of a vector, with the reciprocal square root (the header declares Vec3f::length out of line; this unit
// expands it).
inline float Vec3f::length() {
    float lengthSquared = m_z * m_z + (m_x * m_x + m_y * m_y);
    double absolute = __fabs(lengthSquared);
    float magnitude = absolute;
    if (magnitude <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSquared * rsqrtf(lengthSquared);
}

// MATCH-ONLY: local copies of the paired-single Vec3f subtract/add. The shared Vec3fSub/Vec3fAdd give one register
// form per unit, while execMapCorrection needs six; each copy declares its registers in the order that reproduces the
// original allocation at its call sites (the allocator follows the declaration order of `register f32` variables).
static inline void subLeftRegs(register Vec3f* out, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 lhsXY, rhsXY, lhsZ, rhsZ;
    // clang-format off
    asm {
        psq_l  lhsXY, Vec3f.m_x(lhs), 0, 0
        psq_l  rhsXY, Vec3f.m_x(rhs), 0, 0
        psq_l  lhsZ, Vec3f.m_z(lhs), 1, 0
        psq_l  rhsZ, Vec3f.m_z(rhs), 1, 0
        ps_sub lhsXY, lhsXY, rhsXY
        ps_sub lhsZ, lhsZ, rhsZ
        psq_st lhsXY, Vec3f.m_x(out), 0, 0
        psq_st lhsZ, Vec3f.m_z(out), 1, 0
    }
    // clang-format on
}

static inline void subCompact(register Vec3f* out, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 fr1, fr0, fr2;
    // clang-format off
    asm {
        psq_l  fr0, Vec3f.m_x(lhs), 0, 0
        psq_l  fr1, Vec3f.m_x(rhs), 0, 0
        psq_l  fr2, Vec3f.m_z(lhs), 1, 0
        ps_sub fr1, fr0, fr1
        psq_l  fr0, Vec3f.m_z(rhs), 1, 0
        ps_sub fr2, fr2, fr0
        psq_st fr1, Vec3f.m_x(out), 0, 0
        psq_st fr2, Vec3f.m_z(out), 1, 0
    }
    // clang-format on
}

static inline void subZIntoRight(register Vec3f* out, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 rightXY, resultZ, leftZ, resultXY, leftXY, rightZ;
    // clang-format off
    asm {
        psq_l  leftXY, Vec3f.m_x(lhs), 0, 0
        psq_l  rightXY, Vec3f.m_x(rhs), 0, 0
        psq_l  leftZ, Vec3f.m_z(lhs), 1, 0
        psq_l  rightZ, Vec3f.m_z(rhs), 1, 0
        ps_sub resultXY, leftXY, rightXY
        ps_sub resultZ, leftZ, rightZ
        psq_st resultXY, Vec3f.m_x(out), 0, 0
        psq_st resultZ, Vec3f.m_z(out), 1, 0
    }
    // clang-format on
}

static inline void subSpan(register Vec3f* out, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 rightXY, resultZ, leftXY, resultXY, rightZ, leftZ;
    // clang-format off
    asm {
        psq_l  leftXY, Vec3f.m_x(lhs), 0, 0
        psq_l  rightXY, Vec3f.m_x(rhs), 0, 0
        psq_l  leftZ, Vec3f.m_z(lhs), 1, 0
        psq_l  rightZ, Vec3f.m_z(rhs), 1, 0
        ps_sub resultXY, leftXY, rightXY
        ps_sub resultZ, leftZ, rightZ
        psq_st resultXY, Vec3f.m_x(out), 0, 0
        psq_st resultZ, Vec3f.m_z(out), 1, 0
    }
    // clang-format on
}

static inline void subPlain(register Vec3f* out, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 rightZ, resultZ, leftZ, rightXY, leftXY, resultXY;
    // clang-format off
    asm {
        psq_l  leftXY, Vec3f.m_x(lhs), 0, 0
        psq_l  rightXY, Vec3f.m_x(rhs), 0, 0
        psq_l  leftZ, Vec3f.m_z(lhs), 1, 0
        psq_l  rightZ, Vec3f.m_z(rhs), 1, 0
        ps_sub resultXY, leftXY, rightXY
        ps_sub resultZ, leftZ, rightZ
        psq_st resultXY, Vec3f.m_x(out), 0, 0
        psq_st resultZ, Vec3f.m_z(out), 1, 0
    }
    // clang-format on
}

static inline void addProbe(register Vec3f* out, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 rightZ, rightXY, resultZ, resultXY, leftZ, leftXY;
    // clang-format off
    asm {
        psq_l  leftXY, Vec3f.m_x(lhs), 0, 0
        psq_l  rightXY, Vec3f.m_x(rhs), 0, 0
        psq_l  leftZ, Vec3f.m_z(lhs), 1, 0
        psq_l  rightZ, Vec3f.m_z(rhs), 1, 0
        ps_add resultXY, leftXY, rightXY
        ps_add resultZ, leftZ, rightZ
        psq_st resultXY, Vec3f.m_x(out), 0, 0
        psq_st resultZ, Vec3f.m_z(out), 1, 0
    }
    // clang-format on
}

static inline float hitDistance(const Vec3f* a, const Vec3f* b) {
    Vec3f disp;
    subLeftRegs(&disp, a, b);
    return disp.length();
}

static inline float reachLength(const Vec3f* a, const Vec3f* b) {
    Vec3f disp;
    subCompact(&disp, a, b);
    return disp.length();
}


// Moves the pitch toward the target by at most one step.
static inline void approachPitch(float& pitch, float target, float step) {
    if (pitch > target) {
        pitch -= step;
        if (pitch < target) {
            pitch = target;
        }
    } else if (pitch < target) {
        pitch += step;
        if (pitch > target) {
            pitch = target;
        }
    }
}

// Keeps the Landmaster on the ground: three rays go down (from the centre and from the two tread nodes 9 and 12 along
// the tilted down vector), the pitch (work 0x11000004) moves toward the ground angle they measure (work 0x11000006),
// and the work flags 0x12000003..0x1200000B record which rays hit and whether their lines can be dropped through.
// HYPOTHESIS: the roles of the two tread nodes (front/back) are not established; they are named by node index.
// TODO(S2): 99.73%: register allocation and scheduling only. The stfs of one ray start's lowered y (y - 0.1) is
// scheduled six instructions later in the original, and in both slope branches the original reloads slope.x into f2
// (Z result in f0) where this build uses f0 (Z result in f3). Stack layout and control flow match.
void wnFoxLandMasterStatusUniqProcessCommon::execMapCorrection(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    bool rising = status == 6 && moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL)).m_y >= 0.0f;
    if (rising) {
        return;
    }
    bool groundOnly = status != 4;
    dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject());
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soModelModule& model = moduleAccesser->getModelModule();
    Vec3f pos = posture.getPos();
    Vec3f hitPos9;
    Vec3f hitPos12;
    Vec3f hitPosCenter;
    Vec3f hitNormal9;
    Vec3f hitNormal12;
    Vec3f hitNormalCenter;
    float lr = posture.getLr();
    float pitch = work.getFloat(0x11000004);
    float savedAngle = work.getFloat(0x11000006);
    int lineId9 = 0xFFFF;
    int lineId12 = 0xFFFF;
    int lineIdCenter = 0xFFFF;
    float angle9 = savedAngle;
    float angle12 = savedAngle;
    float angleCenter = savedAngle;
    bool hit9 = false;
    bool hit12 = false;
    bool hitCenter = false;
    bool through9 = false;
    bool through12 = false;
    bool throughCenter = false;
    Vec3f node9 = model.getNodeGlobalOffsetFromTop(9);
    Vec3f node12 = model.getNodeGlobalOffsetFromTop(12);
    Vec3f probe9;
    addProbe(&probe9, &pos, &node9);
    Vec3f probe12;
    addProbe(&probe12, &pos, &node12);
    Vec3f centerStart = pos;
    probe9.m_z = 0.0f;
    probe12.m_z = 0.0f;
    Vec3f dir(0.0f, -1000.0f, 0.0f);
    centerStart.m_y += 8.284f;
    centerStart.m_y -= 0.1f;
    if (stRayCheck(&centerStart, &dir, &lineIdCenter, &hitPosCenter, &hitNormalCenter, groundOnly, false, 1)) {
        if (hitDistance(&centerStart, &hitPosCenter) <= 8.384001f) {
            angleCenter = 57.29578f * atan2f(-(hitNormalCenter.m_x * lr), hitNormalCenter.m_y);
            hitCenter = true;
        }
    }
    if (hitCenter) {
        grCollisionLineWork lineWork;
        u32 id = lineIdCenter;
        bool found = grCollision::getLineWork(&id, &lineWork);
        if (found) {
            throughCenter = lineWork.m_collisionLine->m_isThroughable;
        }
        if (!found) {
            hitCenter = false;
        }
    }
    Vec2f down(0.0f, -1.0f);
    down.rot(0.017453292f * (pitch * lr), &down);
    Vec3f down3(down, 0.0f);
    dir = down3 * 8.284f;
    Vec3f start12 = probe12;
    start12.m_y -= 0.1f;
    Vec3f start9 = probe9;
    start9.m_y -= 0.1f;
    Vec3f end12 = probe12 + dir;
    Vec3f end9 = probe9 + dir;
    Vec3f low12 = probe12;
    low12.m_y -= 8.284f;
    Vec3f low9 = probe9;
    low9.m_y -= 8.284f;
    float reach9 = 8.384001f + reachLength(&end9, &low9);
    float reach12 = 8.384001f + reachLength(&end12, &low12);
    bool ray9 = stRayCheck(&start9, &dir, &lineId9, &hitPos9, &hitNormal9, groundOnly, false, 1);
    if (ray9) {
        if (hitDistance(&start9, &hitPos9) <= reach9) {
            angle9 = 57.29578f * atan2f(-(hitNormal9.m_x * lr), hitNormal9.m_y);
            hit9 = true;
        }
    }
    if (hit9) {
        grCollisionLineWork lineWork;
        u32 id = lineId9;
        bool found = grCollision::getLineWork(&id, &lineWork);
        if (found) {
            through9 = lineWork.m_collisionLine->m_isThroughable;
        }
        if (!found) {
            hit9 = false;
        }
    }
    bool ray12 = stRayCheck(&start12, &dir, &lineId12, &hitPos12, &hitNormal12, groundOnly, false, 1);
    if (ray12) {
        if (hitDistance(&start12, &hitPos12) <= reach12) {
            angle12 = 57.29578f * atan2f(-(hitNormal12.m_x * lr), hitNormal12.m_y);
            hit12 = true;
        }
    }
    if (hit12) {
        grCollisionLineWork lineWork;
        u32 id = lineId12;
        bool found = grCollision::getLineWork(&id, &lineWork);
        if (found) {
            through12 = lineWork.m_collisionLine->m_isThroughable;
        }
        if (!found) {
            hit12 = false;
        }
    }
    float step = work.getFloat(0x11000008);
    float prevPitch = pitch;
    float groundAngle = savedAngle;
    if (hitCenter) {
        groundAngle = angleCenter;
        approachPitch(pitch, angleCenter, step);
        pos = hitPosCenter;
    } else if (hit9 && hit12) {
        Vec3f slope;
        subZIntoRight(&slope, &hitPos9, &hitPos12);
        groundAngle = 57.29578f * atan2f(slope.m_y, __fabsf(slope.m_x));
        pitch = groundAngle;
        // HYPOTHESIS: the scaled span between the hits is computed and left unused (its length too).
        Vec3f span;
        subSpan(&span, &hitPos9, &hitPos12);
        // BYTES: the original computes the length (the inline length sequence is present) and never reads it.
        float spanLength = span.length();
        span.normalize();
        Vec3fScale(&span, &span, 14.442f);
    } else if (hit9 || hit12) {
        if (ray9 && ray12) {
            Vec3f slope;
            subZIntoRight(&slope, &hitPos9, &hitPos12);
            groundAngle = 57.29578f * atan2f(slope.m_y, __fabsf(slope.m_x));
        } else if (ray9) {
            groundAngle = 90.0f;
        } else {
            groundAngle = -90.0f;
        }
        approachPitch(pitch, groundAngle, step);
        float delta = 0.017453292f * (pitch - prevPitch);
        Vec3f axis(0.0f, 0.0f, lr);
        Vec3f rotated9;
        Vec3f rotated12;
        node9.rot(&axis, delta, &rotated9);
        node12.rot(&axis, delta, &rotated12);
        if (hit9) {
            Vec3f span;
            subPlain(&span, &rotated12, &rotated9);
            span.normalize();
            Vec3fScale(&span, &span, 29.058f);
            groundAngle = angle9;
        } else if (hit12) {
            Vec3f span;
            subPlain(&span, &rotated9, &rotated12);
            span.normalize();
            Vec3fScale(&span, &span, 14.442f);
            groundAngle = angle12;
        }
    }
    work.setFloat(pitch, 0x11000004);
    work.setFloat(groundAngle, 0x11000006);
    // BYTES: a temporary passed by address; a named local lands in a different stack slot.
    posture.setRot(&Vec3f(-pitch, 0.0f, 0.0f), 0);
    moduleAccesser->m_stageObject->updateNodeSRT();
    if (hit9 || hit12 || hitCenter) {
        work.onFlag(0x12000003);
    } else {
        work.offFlag(0x12000003);
    }
    work.setFlag(hit9, 0x12000004);
    work.setFlag(hit12, 0x12000005);
    work.setFlag(hitCenter, 0x12000006);
    bool throughable = false;
    if (through9 && through12 && throughCenter) {
        throughable = true;
    } else if (throughCenter) {
        if (!hit9 && !hit12) {
            throughable = true;
        } else if (through9) {
            if (!hit12) {
                throughable = true;
            }
        } else if (through12) {
            if (!hit9) {
                throughable = true;
            }
        }
    } else if (through9) {
        if (!hitCenter && !hit12) {
            throughable = true;
        } else if (through12) {
            throughable = true;
        }
    } else if (through12) {
        if (!hit9 && !hitCenter) {
            throughable = true;
        }
    }
    work.setFlag(throughable, 0x12000008);
    work.setFlag(through9, 0x12000009);
    work.setFlag(through12, 0x1200000A);
    work.setFlag(throughCenter, 0x1200000B);
}

// TODO(S2): 99.89%: the loop counter of the status 1 loop (`for (i = 2; i < count; ...)`) sits in r25 where the original
// reuses the register of the parameter block (r29). The other two loops and everything else match.
// The four tread attacks follow the work flag 0x22000000 and the Landmaster's speed.
void wnFoxLandMasterStatusUniqProcessCommon::execFixPos(soModuleAccesser* moduleAccesser) {
    const wnFoxLandMasterParam* param = dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject()).getParam();
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL));
    int status = moduleAccesser->getStatusModule().getStatusKind();
    if (status == 0 || status == 1 || (u32)(status - 3) <= 3) {
        if (!work.isFlag(0x22000000)) {
            int i;
            bool any = false;
            int count = 0;
            for (i = 0; i < 4; i++) {
                if (attack.isAttack(i, false)) {
                    attack.clear(i);
                    any = true;
                    count++;
                }
            }
            work.setFlag(any, 0x22000000);
            work.setInt(count, 0x20000003);
        }
    }
    if (work.isFlag(0x22000000)) {
        int i;
        int count = work.getInt(0x20000003);
        if (status == 1) {
            float absSpeed = __fabsf(speed.m_x);
            if (absSpeed < param->unk24) {
                if (attack.isAttack(0, false)) {
                    attack.clear(0);
                }
                if (attack.isAttack(1, false)) {
                    attack.clear(1);
                }
            } else {
                attack.setPowerMulStatus(param->unk28 * absSpeed);
                if (speed.m_x * moduleAccesser->getPostureModule().getLr() > 0.0f) {
                    if (!attack.isAttack(0, false)) {
                        attack.set(0, 0);
                    }
                } else {
                    if (!attack.isAttack(1, false)) {
                        attack.set(1, 0);
                    }
                }
            }
            for (i = 2; i < count; i++) {
                if (!attack.isAttack(i, false)) {
                    attack.set(i, 0);
                }
            }
        } else if (status == 3 || status == 4 || status == 6) {
            if (speed.m_y > -param->unk2C) {
                for (i = 0; i < count; i++) {
                    if (attack.isAttack(i, false)) {
                        attack.clear(i);
                    }
                }
            } else {
                for (i = 0; i < count; i++) {
                    if (!attack.isAttack(i, false)) {
                        attack.set(i, 0);
                    }
                }
            }
        }
    }
}

void wnFoxLandMasterStatusUniqProcessCommon::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 2) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        float rotX = work.getFloat(0x11000004);
        work.setFloat(-rotX, 0x11000004);
        work.setFloat(-work.getFloat(0x11000005), 0x11000005);
        work.setFloat(-work.getFloat(0x11000006), 0x11000006);
        work.setFloat(-work.getFloat(0x11000007), 0x11000007);
        soPostureModule& posture = moduleAccesser->getPostureModule();
        posture.reverseLr();
        Vec3f rot(rotX, 0.0f, 0.0f);
        posture.setRot(&rot, 0);
        posture.updateRotYLr();
        moduleAccesser->m_stageObject->updateNodeSRT();
    }
}

void wnFoxLandMasterStatusUniqProcessCommon::controllCaterpillar(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soMotionModule& motion = moduleAccesser->getMotionModule();
    float speed = moduleAccesser->getPostureModule().getLr();
    speed *= kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL)).m_x;
    float frame = motion.getFrame();
    frame += 1.9212328f * speed;
    while (frame >= 100.0f) {
        frame -= 100.0f;
    }
    while (frame < 0.0f) {
        frame += 100.0f;
    }
    motion.setFrame(frame);
    moduleAccesser->getWorkManageModule().setFloat(frame, 0x21000006);
}

void wnFoxLandMasterStatusUniqProcessCommon::slideZFromSpeed(soModuleAccesser* moduleAccesser) {
    const wnFoxLandMasterParam* param = dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject()).getParam();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    float lr = moduleAccesser->getPostureModule().getLr();
    float slide = work.getFloat(0x21000000);
    float unk5 = work.getFloat(0x21000005);
    float prevSpeed = work.getFloat(0x11000002);
    float accel = lr * (kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL)).m_x - prevSpeed);
    if (0.0f == accel) {
        if (slide > 0.0f) {
            slide -= param->unkB0;
            if (slide < 0.0f) {
                slide = 0.0f;
            }
        } else if (slide < 0.0f) {
            slide += param->unkB0;
            if (slide > 0.0f) {
                slide = 0.0f;
            }
        }
    } else {
        slide -= accel * param->unkB4;
        if (slide > param->unkAC) {
            slide = param->unkAC;
        } else if (slide < -param->unkAC) {
            slide = -param->unkAC;
        }
    }
    float rate = param->unkB8 * (slide / param->unkAC);
    work.setFloat(slide, 0x21000000);
    work.setFloat(rate, 0x21000002);
    work.setFloat(unk5, 0x21000005);
}

void wnFoxLandMasterStatusUniqProcessCommon::vibrationYFromSpeed(soModuleAccesser* moduleAccesser) {
    const wnFoxLandMasterParam* param = dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject()).getParam();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    float offset = work.getFloat(0x21000001);
    float direction = work.getFloat(0x21000004);
    float speed = __fabsf(kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL)).m_x);
    if (speed >= param->unk9C && speed < param->unkA0) {
        float ratio = 1.0f - (speed - param->unk9C) / (param->unkA0 - param->unk9C);
        float step = ratio * param->unkA8;
        if (0.0f == direction) {
            direction = 1.0f;
        }
        offset += step * direction;
        if (offset > param->unkA4) {
            offset = param->unkA4 - (offset - param->unkA4);
            direction = -1.0f;
        } else if (offset < -param->unkA4) {
            offset = -param->unkA4 - (offset + param->unkA4);
            direction = 1.0f;
        }
    }
    work.setFloat(offset, 0x21000001);
    work.setFloat(direction, 0x21000004);
}

void wnFoxLandMasterStatusUniqProcessCommon::vibrationYByLanding(soModuleAccesser* moduleAccesser) {
    const wnFoxLandMasterParam* param = dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject()).getParam();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float offset = work.getFloat(0x21000001);
    float direction = work.getFloat(0x21000004);
    if (0.0f == direction) {
        direction = -1.0f;
    }
    float rate = 0.0f;
    float amplitude = work.getFloat(0x21000008);
    if (amplitude > 0.0f) {
        offset += direction * (4.0f * amplitude / param->unkBC);
        if (offset > amplitude) {
            offset = amplitude - (offset - amplitude);
            direction = -1.0f;
        } else if (offset < -amplitude) {
            offset = -amplitude - (offset + amplitude);
            direction = 1.0f;
        }
        rate = param->unkC4 * (offset / amplitude);
    }
    work.setFloat(offset, 0x21000001);
    work.setFloat(rate, 0x21000003);
    work.setFloat(direction, 0x21000004);
}
