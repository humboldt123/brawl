#define MT_VEC2F_ASSIGN_NOINLINE
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <ft/samus/ft_samus_status_uniq_process.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);
extern "C" void __ct__5Vec2fFff(Vec2f* self, float x, float y);

// Screw Attack.
void ftSamusStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
    int situation = moduleAccesser->getSituationModule().getKind();
    Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    ftSamusExtendParamSpecialHi* param = static_cast<ftSamusExtendParamSpecialHi*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[1]);
    moduleAccesser->getKineticModule().getEnergy(0)->disable();
    moduleAccesser->getKineticModule().getEnergy(1)->disable();
    moduleAccesser->getKineticModule().getEnergy(2)->disable();
    moduleAccesser->getKineticModule().getEnergy(3)->disable();
    if (situation != 2) {
        moduleAccesser->getWorkManageModule().onFlag(ftSamusWork::SpecialHi_Grounded);
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        stop.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
    } else {
        sumSpeed.m_y = param->riseSpeedY;
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        controller.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        controller.m_accelMul = param->controlAccelMul;
        controller.m_unk40 = 0.0f;
        Vec2f target;
        __ct__5Vec2fFff(&target, param->controlSpeedXMax, controller.m_speedTarget.m_y);
        controller.m_speedTarget = target;
        Vec2f limit;
        __ct__5Vec2fFff(&limit, param->controlSpeedXMax, controller.m_speedLimit.m_y);
        controller.m_speedLimit = limit;
        controller.enable();
    }
}

// MATCH-ONLY: the unit carries the out-of-line Vec2f constructor (the two controller targets are built through it).
extern "C" void __ct__5Vec2fFff(Vec2f* self, float x, float y) {
    self->m_x = x;
    self->m_y = y;
}

void ftSamusStatusUniqProcessSpecialHi::execStatus(soModuleAccesser* moduleAccesser) {
    ftSamusExtendParamSpecialHi* param = static_cast<ftSamusExtendParamSpecialHi*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[1]);
    if (moduleAccesser->getWorkManageModule().isFlag(ftSamusWork::SpecialHi_Launched)) {
        moduleAccesser->getWorkManageModule().offFlag(ftSamusWork::SpecialHi_Launched);
        soKineticEnergyNormal& motion = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        motion.resetEnergy(4, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        motion.enable();
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        controller.resetEnergy(0, &Vec2f(param->riseSpeedX * moduleAccesser->getPostureModule().getLr(), 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        controller.m_unk40 = 0.0f;
        controller.enable();
        ftKineticEnergyDisableAndClear(3, moduleAccesser);
        moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
    }
    if (!moduleAccesser->getWorkManageModule().isFlag(ftSamusWork::SpecialHi_Released)) {
        float stickMagnitude = (float)fabs(moduleAccesser->getControllerModule().getStickX());
        if (stickMagnitude > soValueAccesser::getConstantFloat(moduleAccesser, ftSamusParam::SpecialHi_StickThreshold, 0)) {
            moduleAccesser->getPostureModule().setStickLR(0.0f);
            moduleAccesser->getPostureModule().updateRotYLr();
        }
    }
}

void ftSamusStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int) {
    float landingLag = soValueAccesser::getConstantFloat(moduleAccesser, ftSamusParam::SpecialHi_LandingLag, 0);
    moduleAccesser->getWorkManageModule().setFloat(landingLag, ftSamusWork::SpecialHi_ExitLandingLag);
    float controlRatio = soValueAccesser::getConstantFloat(moduleAccesser, ftSamusParam::SpecialHi_ControlXRatio, 0);
    moduleAccesser->getWorkManageModule().setFloat(controlRatio, ftSamusWork::SpecialHi_ExitControlXRatio);
}

// Shared virtual defaults are emitted in their original function order.
bool soStatusUniqProcess::checkTransitionPrecede(soModuleAccesser*, void*, int) { return true; }
void soStatusUniqProcess::leaveStop(soModuleAccesser*, int, bool) {}
bool soStatusUniqProcess::onChangeLr(soModuleAccesser*, float, float) { return false; }
void soStatusUniqProcess::checkAttack(soModuleAccesser*, void*, float) {}
bool soStatusUniqProcess::checkDamage(soModuleAccesser*, void*) { return false; }
void soStatusUniqProcess::execFixCamera(soModuleAccesser*) {}
void soStatusUniqProcess::execFixPos(soModuleAccesser*) {}
void soStatusUniqProcess::execFixPosCounter(soModuleAccesser*) {}
void soStatusUniqProcess::execMapCorrection(soModuleAccesser*) {}
void soStatusUniqProcess::execStop(soModuleAccesser*) {}
void soStatusUniqProcess::execStatus(soModuleAccesser*) {}
void soStatusUniqProcess::exitStatus(soModuleAccesser*, int) {}
void soStatusUniqProcess::initStatus(soModuleAccesser*) {}

ftSamusStatusUniqProcessSpecialHi g_ftSamusStatusUniqProcessSpecialHi;
