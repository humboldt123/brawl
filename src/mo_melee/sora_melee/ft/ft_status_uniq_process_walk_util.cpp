#include <so/controller/so_controller_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <math.h>

// ftStatusUniqProcessWalkUtil (map names initWalkMotion, setWalkMotion, initWalkSpeed, calcWalkSpeed). The callers in
// sora_melee and the fighter modules still declare these by their symbols.txt names, so they keep C linkage here.
// The motion table holds the fast, middle and slow walk motion kinds.
// HYPOTHESIS: 0x21000005 is the walk speed multiplier work the second animation rate also scales by.

// HYPOTHESIS: the unit's .rodata starts with two floats no function here loads (likely the pool of a function
// that was stripped at link time).
namespace ft_status_uniq_process_walk_util_link_ref {
    extern const float unused_floats[] = { 0.5f, 0.75f };
}

static inline int getWalkMotionKind(const int* motionKinds, float speed, float speedMul) {
    if (speed >= 0.75f * speedMul) {
        return motionKinds[0];
    }
    if (speed >= 0.5f * speedMul) {
        return motionKinds[1];
    }
    return motionKinds[2];
}

static inline float calcWalkRate(soModuleAccesser* moduleAccesser, const int* motionKinds, int kind, float speed,
                                 float speedMul, int rateWork) {
    float rate;
    if (kind == motionKinds[2]) {
        rate = speed / (0.25f * speedMul);
    } else if (kind == motionKinds[1]) {
        rate = speed / (0.5f * (0.5f * speedMul + 0.75f * speedMul));
    } else {
        rate = speed / speedMul;
    }
    rate *= moduleAccesser->getWorkManageModule().getFloat(rateWork);
    float rateMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xc30, 0);
    if (rate > rateMax) {
        rate = rateMax;
    }
    return rate;
}

extern "C" {

void fn_27_164BF0(soModuleAccesser* moduleAccesser, const int* motionKinds, int speedWork, int rateWork, float speedMul) {
    float speed = fabs(moduleAccesser->getWorkManageModule().getFloat(speedWork));
    soMotionModule* motion = &moduleAccesser->getMotionModule();
    int kind = getWalkMotionKind(motionKinds, speed, speedMul);
    float rate = calcWalkRate(moduleAccesser, motionKinds, kind, speed, speedMul, rateWork);
    float frame = 0.0f;
    motion->getEndFrame(kind);
    soMotionChangeParam param;
    param.m_kind = kind;
    param.m_frame = frame;
    param.m_rate = rate;
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    motion->changeMotion(&param);
    motion->set2ndRate(calcWalkRate(moduleAccesser, motionKinds, motion->get2ndKind(), speed, 1.0f, 0x21000005));
}

void fn_27_164E48(soModuleAccesser* moduleAccesser, const int* motionKinds, int speedWork, int rateWork, float speedMul) {
    float speed = fabs(moduleAccesser->getWorkManageModule().getFloat(speedWork));
    soMotionModule* motion = &moduleAccesser->getMotionModule();
    if (motion->isBlend() == true) {
        motion->setRate(calcWalkRate(moduleAccesser, motionKinds, motion->getKind(), speed, speedMul, rateWork));
        motion->set2ndRate(calcWalkRate(moduleAccesser, motionKinds, motion->get2ndKind(), speed, speedMul, rateWork));
    } else {
        int kind = getWalkMotionKind(motionKinds, speed, speedMul);
        float rate = calcWalkRate(moduleAccesser, motionKinds, kind, speed, speedMul, rateWork);
        if (kind != motion->getKind()) {
            float endFrame = motion->getEndFrame(kind);
            float curEndFrame = motion->getEndFrame();
            float curFrame = motion->getFrame();
            soMotionChangeParam param;
            param.m_kind = kind;
            param.m_frame = endFrame * (curFrame / curEndFrame);
            param.m_rate = rate;
            param._12 = 0;
            param._13 = 0;
            param._14 = 0;
            param._15 = 0;
            motion->changeMotion(&param);
            motion->set2ndRate(calcWalkRate(moduleAccesser, motionKinds, motion->get2ndKind(), speed, 1.0f, 0x21000005));
        } else {
            motion->setRate(rate);
        }
    }
}

// HYPOTHESIS: sets the initial walk speed from the kinetic speed (kept when it points the way the fighter faces, or
// backwards when isBack is set; stored as a fraction of speedMax) and the walk animation rate from the motion's translation over its length.
void fn_27_1652A8(soModuleAccesser* moduleAccesser, int kind, int speedWork, int rateWork, bool isBack,
                  float speedMax, float speedMul) {
    Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    Vec2f speed = sumSpeed;
    Vec3f trans = moduleAccesser->getMotionModule().getTransNTranslateEndFrame(kind);
    float walkSpeed;
    float scaledMax = speedMul * speedMax;
    walkSpeed = 0.0f;
    if (isBack == true) {
        if (speed.m_x * moduleAccesser->getPostureModule().getLr() < 0.0f) {
            walkSpeed = speed.m_x / speedMax;
        }
    } else {
        if (speed.m_x * moduleAccesser->getPostureModule().getLr() > 0.0f) {
            walkSpeed = speed.m_x / speedMax;
        }
    }
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.setFloat(walkSpeed, speedWork);
    float endFrame = moduleAccesser->getMotionModule().getEndFrame(kind);
    moduleAccesser->getWorkManageModule().setFloat(scaledMax / ((float)__fabs(trans.m_z) / endFrame), rateWork);
}

void fn_27_165458(soModuleAccesser* moduleAccesser, int speedWork, float speed, float speedMax, float accel,
                  float accelAdd, float accelDecay, float speedMul) {
    float stickX = moduleAccesser->getControllerModule().getStickX();
    float accelRate = accel / speedMax;
    float addRate = accelAdd / speedMax;
    float delta = speedMul * (stickX * accelRate);
    float add;
    if (stickX > 0.0f) {
        add = addRate * speedMul;
    } else {
        add = -addRate * speedMul;
    }
    float target = stickX * speedMul;
    delta += add;
    bool isNeutral;
    if (target < 0.00001f && target > -0.00001f) {
        isNeutral = true;
    } else {
        isNeutral = false;
    }
    if (!isNeutral) {
        float ratio = speed / target;
        if (ratio > 0.0f && ratio < 1.0f) {
            delta *= (1.0f - ratio) * accelDecay;
        }
    } else {
        delta = 0.0f;
    }
    float brake = soValueAccesser::getConstantFloat(moduleAccesser, 0xbbc, 0) / speedMax;
    if (0.0f == target) {
        float absSpeed = __fabs(speed);
        float absBrake = __fabs(brake);
        if (absBrake > absSpeed) {
            delta = -speed;
        } else {
            if (speed > 0.0f) {
                brake = -brake;
            }
            delta = brake;
        }
    } else if (delta > 0.0f) {
        if (speed + delta > target) {
            delta = -brake;
            if (speed + delta < target) {
                delta = target - speed;
            }
        }
    } else if (speed + delta < target) {
        delta = brake;
        if (speed + delta > target) {
            delta = target - speed;
        }
    }
    moduleAccesser->getWorkManageModule().setFloat(speed + delta, speedWork);
}

}
