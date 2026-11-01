#define MT_VEC2F_MEMBER_COPY // MATCH-ONLY: see mt_vector.h.
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC2F_LENGTH_SQ_OUT_OF_LINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <math.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <mt/mt_common.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftSheikStatusUniqProcessSpecialHiMove : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialHiMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftSheikStatusUniqProcessSpecialHiMove::initStatus(soModuleAccesser* moduleAccesser) {
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soGroundModule& ground = moduleAccesser->getGroundModule();

    ftData* sheikData = g_ftCommonDataAccesser.getData(Fighter_Sheik);
    const float* params = static_cast<const float*>(sheikData->extendParam[2]);
    ftKineticEnergyStop& stop =
        dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));

    Vec2f speed;
    int useFreshDirection = 0;
    Vec2f stick;
    stick.m_x = controller.getStickX();
    stick.m_y = controller.getStickY();
    float magnitude = mtSqrtf(stick.m_x * stick.m_x + stick.m_y * stick.m_y);
    if (magnitude > 1.0f) {
        magnitude = 1.0f;
    }

    if (situation.getKind() == 0) {
        if (magnitude < params[5]) {
            useFreshDirection = 1;
        } else {
            Vec2f normal;
            Vec2f::copy(normal, ground.getTouchNormal(8, 0));
            if (stick.mtVecAngle(normal) < 1.57079632679f) {
                useFreshDirection = 1;
            } else if (ground.isPassableGround(0)) {
                useFreshDirection = 1;
            }
        }
    } else {
        useFreshDirection = 1;
    }

    float facing;
    float angle;
    if (useFreshDirection) {
        if (magnitude > params[5]) {
            if (__fabsf(stick.m_x) > 0.001f) {
                posture.setStickLR(0.0f);
                posture.updateRotYLr();
            }
            facing = posture.getLr();
            angle = atan2f(stick.m_y, stick.m_x * facing);
        } else {
            posture.setStickLR(0.0f);
            posture.updateRotYLr();
            facing = posture.getLr();
            angle = 1.57079632679f;
            magnitude = 1.0f;
        }
    } else {
        posture.setStickLR(0.0f);
        posture.updateRotYLr();
        facing = posture.getLr();
        angle = atan2f(stick.m_y, stick.m_x * facing);
    }

    situation.setKind(static_cast<SituationKind>(2), false);
    ground.setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
    kinetic.clearSpeedAll();

    speed.m_x = (params[7] + magnitude * params[6]) * (float)cos(angle) * facing;
    speed.m_y = (params[7] + magnitude * params[6]) * (float)sin(angle);
    stop.m_speed = speed;
    stop.enable();
}

// Return the unsigned angle between two movement directions. A zero-length
// direction has no angle; the native helper returns zero in that case.
float Vec2f::mtVecAngle(Vec2f& other) {
    float otherLengthSq = other.lengthSq();
    float selfLengthSq = lengthSq();
    float product = selfLengthSq * otherLengthSq;
    if (product == 0.0f) {
        return 0.0f;
    }

    float dot = m_x * other.m_x + m_y * other.m_y;
    register float cosine = dot * rsqrtf(product);
    register float upper = 1.0f;
    register float lower = -1.0f;
    register float fromLower = cosine - lower;
    register float lowerBounded;
    // MATCH-ONLY: preserve the native branchless clamp and float register order.
    asm { fsel lowerBounded, fromLower, cosine, lower }
    register float fromUpper = lowerBounded - upper;
    register float clamped;
    asm { fsel clamped, fromUpper, upper, lowerBounded }
    return (float)acos(clamped);
}
float Vec2f::lengthSq() {
    float xSq = m_x * m_x;
    float ySq = m_y * m_y;
    return xSq + ySq;
}


void ftSheikStatusUniqProcessSpecialHiMove::execFixPosCounter(
    soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soGroundModule& ground = moduleAccesser->getGroundModule();
    const ftData* sheikData = g_ftCommonDataAccesser.getData(Fighter_Sheik);
    const u8* params = static_cast<const u8*>(sheikData->extendParam[2]);

    (void)dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    Vec2f speed;
    Vec2f::copy(speed, kinetic.getSumSpeed(
        soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_MAIN)));
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        return;
    }

    s32 extraAngle = *reinterpret_cast<const s32*>(params + 0x24);
    float maxAngle = (static_cast<float>(extraAngle) + 90.0f) * 0.017453292f;
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(1), 0)) {
        Vec2f normal = ground.getTouchNormal(1, 0);
        if (speed.mtVecAngle(normal) > maxAngle) {
            work.onFlag(0x22000012);
        }
    }
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(4), 0)) {
        Vec2f normal = ground.getTouchNormal(4, 0);
        if (speed.mtVecAngle(normal) > maxAngle) {
            work.onFlag(0x22000012);
        }
    }
    // HYPOTHESIS: the two mask-4 probes represent distinct contact checks;
    // their apparent duplicate arguments are present in the native calls.
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(4), 0)) {
        Vec2f normal = ground.getTouchNormal(4, 0);
        if (speed.mtVecAngle(normal) > maxAngle) {
            work.onFlag(0x22000012);
        }
    }
}

void ftSheikStatusUniqProcessSpecialHiMove::execFixPos(
    soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    if (work.isFlag(0x22000012)) {
        work.offFlag(0x22000012);
        status.changeStatusRequest(0x11D, moduleAccesser);
    }
}

void ftSheikStatusUniqProcessSpecialHiMove::exitStatus(
    soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getVisibilityModule().setWhole(1);
}

ftSheikStatusUniqProcessSpecialHiMove g_ftSheikStatusUniqProcessSpecialHiMove;
