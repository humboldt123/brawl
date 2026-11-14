#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_shoot.h>
#include <ft/ft_util.h>
#include <math.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Walk helpers (ftStatusUniqProcessWalkUtil, ft_status_uniq_process_walk_util.cpp), declared by their symbols.txt names.
extern "C" {
void fn_27_1652A8(soModuleAccesser* moduleAccesser, int kind, int speedWork, int rateWork, bool isBack, float speedMax,
                  float speedMul);
void fn_27_164BF0(soModuleAccesser* moduleAccesser, const int* motionKinds, int speedWork, int rateWork, float speedMul);
void fn_27_165458(soModuleAccesser* moduleAccesser, int speedWork, float speed, float speedMax, float accel,
                  float accelAdd, float accelDecay, float speedMul);
void fn_27_164E48(soModuleAccesser* moduleAccesser, const int* motionKinds, int speedWork, int rateWork, float speedMul);
}

// The shoot statuses keep the status kinds of their family in work ints 0x20000001-0x2000000B (set by
// ftStatusUniqProcessItemShoot::initStatus): 1 shoot, 2 walk shoot, 3 back walk shoot, 4/5 dash and turn shoot(?),
// 8/9 air shoot, ... HYPOTHESIS: only 1, 2, 3, 8 and 9 are pinned down by the code below.

// Walk motion kinds (fast, middle, slow) of the forward and the backward walk shoot.
static const int s_shootWalkMotionKinds[3] = { 0x153, 0x152, 0x151 };
static const int s_shootWalkBackMotionKinds[3] = { 0x158, 0x157, 0x156 };

static inline bool isShootAirStatus(soModuleAccesser* moduleAccesser, int status) {
    bool result = false;
    if (status == moduleAccesser->getWorkManageModule().getInt(0x20000008) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000009)) {
        result = true;
    }
    return result;
}

static inline bool isShootGroundStatus(soModuleAccesser* moduleAccesser, int status) {
    bool result = false;
    if (status == moduleAccesser->getWorkManageModule().getInt(0x20000001) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000002) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000004) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000003) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000005) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000006) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000007) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x2000000A) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x2000000B)) {
        result = true;
    }
    return result;
}

static inline bool isShootStatus(soModuleAccesser* moduleAccesser, int status) {
    bool result = false;
    if (status == moduleAccesser->getWorkManageModule().getInt(0x20000001) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000002) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000004) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000003) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000005) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000006) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000007) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x2000000A) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x2000000B) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000008) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x20000009)) {
        result = true;
    }
    return result;
}

void ftStatusUniqProcessShoot::initShoot_Common(soModuleAccesser* moduleAccesser, int groundMotionKind,
                                                int airMotionKind) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    int prevStatus = moduleAccesser->getStatusModule().getPrevStatusKind(0);
    if (status == moduleAccesser->getWorkManageModule().getInt(0x20000001) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x2000000B)) {
        float frame = 0.0f;
        float rate = 1.0f;
        if (isShootAirStatus(moduleAccesser, prevStatus)) {
            frame = moduleAccesser->getMotionModule().getFrame();
            rate = moduleAccesser->getMotionModule().getRate();
        }
        soMotionChangeParam param;
        param.m_kind = 0x150;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        if (moduleAccesser->getMotionModule().getPartialKind(1) == -1) {
            moduleAccesser->getMotionModule().addPartialAnimChr(1, groundMotionKind, 0x135, false, frame, rate, false);
        }
    } else if (status == moduleAccesser->getWorkManageModule().getInt(0x20000002)) {
        float speedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
        fn_27_1652A8(moduleAccesser, 0xc, 0x21000004, 0x21000005, false,
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbfd, 0), speedMul);
        fn_27_164BF0(moduleAccesser, s_shootWalkMotionKinds, 0x21000004, 0x21000005, speedMul);
    } else if (status == moduleAccesser->getWorkManageModule().getInt(0x20000003)) {
        float speedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
        fn_27_1652A8(moduleAccesser, 0xc, 0x21000004, 0x21000005, true,
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbfd, 0) *
                         soValueAccesser::getConstantFloat(moduleAccesser, 0xd7d, 0),
                     speedMul);
        fn_27_164BF0(moduleAccesser, s_shootWalkBackMotionKinds, 0x21000004, 0x21000005, speedMul);
    } else if (isShootAirStatus(moduleAccesser, status)) {
        float frame = 0.0f;
        float rate = 1.0f;
        bool keepFrame = false;
        if (isShootGroundStatus(moduleAccesser, prevStatus)) {
            frame = moduleAccesser->getWorkManageModule().getFloat(0x21000006);
            if (!(-1.0f == frame)) {
                rate = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
                keepFrame = true;
            } else {
                frame = 0.0f;
            }
        }
        moduleAccesser->getMotionModule().compRemovePartialAnimChr(1);
        soMotionChangeParam param;
        param.m_kind = airMotionKind;
        param.m_frame = frame;
        param.m_rate = rate;
        param._12 = 0;
        param._13 = 0;
        param._14 = keepFrame;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
}

void ftStatusUniqProcessShoot::initShoot_CommonAirUpper(soModuleAccesser* moduleAccesser, int groundMotionKind,
                                                        int airMotionKind) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    int prevStatus = moduleAccesser->getStatusModule().getPrevStatusKind(0);
    if (status == moduleAccesser->getWorkManageModule().getInt(0x20000001) ||
        status == moduleAccesser->getWorkManageModule().getInt(0x2000000B)) {
        float frame = 0.0f;
        float rate = 1.0f;
        bool keepFrame = false;
        if (isShootAirStatus(moduleAccesser, prevStatus)) {
            frame = moduleAccesser->getMotionModule().getFrame();
            rate = moduleAccesser->getMotionModule().getRate();
            keepFrame = true;
        }
        soMotionChangeParam param;
        param.m_kind = groundMotionKind;
        param.m_frame = frame;
        param.m_rate = rate;
        param._12 = 0;
        param._13 = 0;
        param._14 = keepFrame;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    } else if (status == moduleAccesser->getWorkManageModule().getInt(0x20000002)) {
        float speedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
        fn_27_1652A8(moduleAccesser, 0xc, 0x21000004, 0x21000005, false,
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbfd, 0), speedMul);
        fn_27_164BF0(moduleAccesser, s_shootWalkMotionKinds, 0x21000004, 0x21000005, speedMul);
    } else if (status == moduleAccesser->getWorkManageModule().getInt(0x20000003)) {
        float speedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
        fn_27_1652A8(moduleAccesser, 0xc, 0x21000004, 0x21000005, true,
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbfd, 0) *
                         soValueAccesser::getConstantFloat(moduleAccesser, 0xd7d, 0),
                     speedMul);
        fn_27_164BF0(moduleAccesser, s_shootWalkBackMotionKinds, 0x21000004, 0x21000005, speedMul);
    } else if (isShootAirStatus(moduleAccesser, status)) {
        float frame = 0.0f;
        float rate = 1.0f;
        bool keepFrame = false;
        if (isShootGroundStatus(moduleAccesser, prevStatus)) {
            frame = moduleAccesser->getMotionModule().getFrame();
            rate = moduleAccesser->getMotionModule().getRate();
            keepFrame = true;
        }
        soMotionChangeParam param;
        param.m_kind = airMotionKind;
        param.m_frame = frame;
        param.m_rate = rate;
        param._12 = 0;
        param._13 = 0;
        param._14 = keepFrame;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
}

void ftStatusUniqProcessShoot::execShoot_Common(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    if (status == moduleAccesser->getWorkManageModule().getInt(0x20000002)) {
        float speedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        fn_27_165458(moduleAccesser, 0x21000004, work.getFloat(0x21000004),
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbfd, 0),
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbfb, 0),
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbfc, 0),
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0), speedMul);
        fn_27_164E48(moduleAccesser, s_shootWalkMotionKinds, 0x21000004, 0x21000005, speedMul);
    } else if (status == moduleAccesser->getWorkManageModule().getInt(0x20000003)) {
        float speedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        fn_27_165458(moduleAccesser, 0x21000004, work.getFloat(0x21000004),
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbfd, 0) *
                         soValueAccesser::getConstantFloat(moduleAccesser, 0xd7d, 0),
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbfb, 0),
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbfc, 0),
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0), speedMul);
        fn_27_164E48(moduleAccesser, s_shootWalkBackMotionKinds, 0x21000004, 0x21000005, speedMul);
    } else if (status == moduleAccesser->getWorkManageModule().getInt(0x20000004) ||
               status == moduleAccesser->getWorkManageModule().getInt(0x20000005)) {
        float epsilon = 0.001f;
        float speedX = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x;
        if ((float)__fabs(speedX) <= epsilon) {
            moduleAccesser->getMotionModule().setRate(1.0f);
        }
    }
}

void ftStatusUniqProcessShoot::exitShoot_Common(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (isShootStatus(moduleAccesser, nextStatusKind)) {
        float frame = -1.0f;
        if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
            frame = moduleAccesser->getMotionModule().getPartialFrame(1);
            float rate = moduleAccesser->getMotionModule().getPartialRate(1);
            moduleAccesser->getWorkManageModule().setFloat(rate, 0x21000007);
        }
        moduleAccesser->getWorkManageModule().setFloat(frame, 0x21000006);
        if (isShootAirStatus(moduleAccesser, nextStatusKind)) {
            moduleAccesser->getMotionModule().compRemovePartialAnimChr(1);
        }
        moduleAccesser->getWorkManageModule().onFlag(0x2200000A);
    } else {
        moduleAccesser->getMotionModule().compRemovePartialAnimChr(1);
        moduleAccesser->getItemManageModule().resetHaveItemConstraintNode(0);
    }
}

void ftStatusUniqProcessShoot::exitShoot_CommonAirUpper(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (isShootStatus(moduleAccesser, nextStatusKind)) {
        float frame = -1.0f;
        moduleAccesser->getWorkManageModule().setFloat(frame, 0x21000006);
        moduleAccesser->getWorkManageModule().onFlag(0x2200000A);
    } else {
        moduleAccesser->getMotionModule().compRemovePartialAnimChr(1);
        moduleAccesser->getItemManageModule().resetHaveItemConstraintNode(0);
    }
}

void ftStatusUniqProcessShoot::execStatus(soModuleAccesser* moduleAccesser) {
    execShoot_Common(moduleAccesser);
}

void ftStatusUniqProcessShoot::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    exitShoot_Common(moduleAccesser, nextStatusKind);
}
