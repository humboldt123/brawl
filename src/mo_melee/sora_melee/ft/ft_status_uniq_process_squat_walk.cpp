#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_status_posture_view.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessSquatWalk : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSquatWalk() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessSquatWalk g_ftStatusUniqProcessSquatWalk;

// Squat-walk stick-rate helper (first function of this unit): maps the stick position (times the facing) to a 0.5..1.0 rate using the
// dead zone constants of the current squat motion (status 0x13 / 0x14).
extern "C" float soModuleAccesser__SquatWalk_GetStickXRate(soModuleAccesser* moduleAccesser) {
    float value = moduleAccesser->getControllerModule().getStickX() * reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk2c();
    float lo = 0.0f;
    float hi = lo;
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x13:
        lo = soValueAccesser::getConstantFloat(moduleAccesser, 0xc3e, 0);
        hi = soValueAccesser::getConstantFloat(moduleAccesser, 0xc3f, 0);
        if (value < lo) {
            return 0.5f;
        }
        if (value > hi) {
            return 1.0f;
        }
        break;
    case 0x14:
        lo = soValueAccesser::getConstantFloat(moduleAccesser, 0xc40, 0);
        hi = soValueAccesser::getConstantFloat(moduleAccesser, 0xc41, 0);
        if (value > lo) {
            return 0.5f;
        }
        if (value < hi) {
            return 1.0f;
        }
        break;
    }
    float span = hi - lo;
    if ((float)__fabs(span) > 1.1920929e-07f) {
        return 0.5f + 0.5f * ((value - lo) / span);
    }
    return 1.0f;
}


// HYPOTHESIS: seeds the squat-walk work floats: 0x21000004 is the walk rate (current speed over the per-fighter limit when
// walking along the facing) and 0x21000005 the motion rate that makes the squat motion's travel match the limit.
void ftStatusUniqProcessSquatWalk::initStatus(soModuleAccesser* moduleAccesser) {
    g_ftCommonDataAccesser.getParamEtc((ftKind)moduleAccesser->m_stageObject->soGetSubKind());
    ftParamEtc* etc = g_ftCommonDataAccesser.getParamEtc((ftKind)moduleAccesser->m_stageObject->soGetSubKind());
    float limit;
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x13:
        limit = etc->unk3C[0];
        break;
    case 0x14:
        limit = etc->unk3C[1];
        break;
    default:
        limit = 0.0f;
        break;
    }
    float speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x;
    float rate = (speed * moduleAccesser->getPostureModule().getLr() < 0.0f) ? 0.0f : speed / limit;
    moduleAccesser->getWorkManageModule().setFloat(rate, 0x21000004);
    int motionKind;
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x13:
        motionKind = 0x2e;
        break;
    case 0x14:
        motionKind = 0x2f;
        break;
    default:
        motionKind = -1;
        break;
    }
    Vec3f travel = moduleAccesser->m_enumerationStart->m_motionModule->getTransNTranslateEndFrame(motionKind);
    float endFrame = moduleAccesser->m_enumerationStart->m_motionModule->getEndFrame(motionKind);
    if ((float)__fabs(travel.m_z) < 0.5f) {
        travel.m_z = 20.0f;
    }
    double rateWide = fabs(limit / (travel.m_z / endFrame));
    moduleAccesser->getWorkManageModule().setFloat(rateWide, 0x21000005);
}
// MSL's float fabs overload: the double fabs inlined into a float result (the frsp after each fabs).
static inline double fabsDouble(double x) { return __fabs(x); }
static inline float fabsFloat(float x) { return fabsDouble(x); }
// HYPOTHESIS: steps the walk rate (work float 0x21000004, in units of the per-fighter limit) towards the stick rate with the
// accelerations in constants 0xbb9/0xbba/0xbbc, then sets the motion rate from the result (work float 0x21000005).
void ftStatusUniqProcessSquatWalk::execStatus(soModuleAccesser* moduleAccesser) {
    ftParamEtc* etc = g_ftCommonDataAccesser.getParamEtc((ftKind)moduleAccesser->m_stageObject->soGetSubKind());
    float remain;
    float limit = etc->unk3C[0];
    float current = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    float target = soModuleAccesser__SquatWalk_GetStickXRate(moduleAccesser);
    float step;
    float accel = soValueAccesser::getConstantFloat(moduleAccesser, 0xbb9, 0) / limit;
    float bias = soValueAccesser::getConstantFloat(moduleAccesser, 0xbba, 0) / limit;
    step = target * accel;
    bias = (target > 0.0f) ? bias : -bias;
    float ratio = current / target;
    step += bias;
    if (ratio > 0.0f && ratio < 1.0f) {
        remain = 1.0f - ratio;
        step *= remain * soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0);
    }
    float brake = soValueAccesser::getConstantFloat(moduleAccesser, 0xbbc, 0) / limit;
    if (target == 0.0f) {
        if (fabsFloat(brake) > fabsFloat(current)) {
            step = -current;
        } else {
            if (current > 0.0f) {
                brake = -brake;
            }
            step = brake;
        }
    } else if (step > 0.0f) {
        if (current + step > target) {
            step = -brake;
            if (current + step < target) {
                step = target - current;
            }
        }
    } else if (current + step < target) {
        step = brake;
        if (current + brake > target) {
            step = target - current;
        }
    }
    moduleAccesser->getWorkManageModule().setFloat(current + step, 0x21000004);
    float walk = fabsFloat(moduleAccesser->getWorkManageModule().getFloat(0x21000004));
    moduleAccesser->m_enumerationStart->m_motionModule->setRate(walk * moduleAccesser->getWorkManageModule().getFloat(0x21000005));
}
