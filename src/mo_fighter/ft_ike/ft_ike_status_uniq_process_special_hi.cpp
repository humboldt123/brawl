#define MT_VEC2F_ASSIGN_NOINLINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ike/ft_ike.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

class ftIkeStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// HYPOTHESIS: two floats of the fighter attributes (0x74 limits the horizontal speed, 0x7c is the stable speed), as in
// Kirby's Final Cutter.
struct ftIkeSpecialHiParam {
    u8 unk0[0x74];
    float unk74;
    u8 unk78[4];
    float unk7c;
};

void ftIkeStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
    dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    soKineticEnergyNormal& motion = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x114: {
        ftIke& ike = dynamic_cast<ftIke&>(moduleAccesser->getStageObject());
        ftIkeSpecialHiParam* param = reinterpret_cast<ftIkeSpecialHiParam*>(g_ftCommonDataAccesser.getParam(ike.getFtKind()));
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            controller.resetEnergy(0xb, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            moduleAccesser->getWorkManageModule().setInt(0, 0x20000000);
        } else {
            controller.resetEnergy(0, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            moduleAccesser->getWorkManageModule().setInt(2, 0x20000000);
        }
        controller.mulXSpeedMax(param->unk74 * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0));
        controller.m_speedTarget = Vec2f(param->unk7c, 100.0f);
        controller.enable();
        ftKineticEnergyDisableAndClear(3, moduleAccesser);
        ftKineticEnergyDisableAndClear(1, moduleAccesser);
        ftKineticEnergyDisableAndClear(0, moduleAccesser);
        break;
    }
    case 0x11e:
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            motion.resetEnergy(3, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        } else {
            motion.resetEnergy(5, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        }
        motion.enable();
        ftKineticEnergyDisableAndClear(3, moduleAccesser);
        ftKineticEnergyDisableAndClear(2, moduleAccesser);
        ftKineticEnergyDisableAndClear(1, moduleAccesser);
        break;
    case 0x120:
        ftKineticEnergyDisableAndClear(2, moduleAccesser);
        ftKineticEnergyDisableAndClear(0, moduleAccesser);
        break;
    }
}

// MATCH-ONLY: these helpers and the propagation pragma keep the reset mode in a saved register and the speed
// temporaries in their original slots and argument order.
static inline void resetController(ftKineticEnergyController& controller, int mode, const Vec2f& speed, soModuleAccesser* moduleAccesser) {
    controller.resetEnergy(mode, const_cast<Vec2f*>(&speed), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
}

static inline void setTargetSpeed(ftKineticEnergyController& controller, const Vec2f& speed) {
    controller.m_speedTarget = speed;
}

#pragma push
#pragma opt_propagation off
void ftIkeStatusUniqProcessSpecialHi::execStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getControllerModule().getStickX();
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x114: {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        soSituationModule& situation = moduleAccesser->getSituationModule();
        if (situation.getKind() != work.getInt(0x20000000)) {
            ftIke& ike = dynamic_cast<ftIke&>(moduleAccesser->getStageObject());
            ftIkeSpecialHiParam* param = reinterpret_cast<ftIkeSpecialHiParam*>(g_ftCommonDataAccesser.getParam(ike.getFtKind()));
            ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
            if (moduleAccesser->getSituationModule().getKind() == 0) {
                resetController(controller, 0xb, Vec2f(sumSpeed.m_x, 0.0f), moduleAccesser);
                moduleAccesser->getWorkManageModule().setInt(0, 0x20000000);
            } else {
                resetController(controller, 0, Vec2f(sumSpeed.m_x, 0.0f), moduleAccesser);
                moduleAccesser->getWorkManageModule().setInt(2, 0x20000000);
            }
            controller.mulXSpeedMax(param->unk74 * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0));
            setTargetSpeed(controller, Vec2f(param->unk7c, 100.0f));
            controller.enable();
        }
        break;
    }
    case 0x11e:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) && moduleAccesser->getWorkManageModule().isFlag(0x22000014)) {
            ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
            float speedMul = 1.0f;
            if (motion.getSpeed().m_y > 0.0f) {
                speedMul = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb2, 0);
            }
            motion.m_speedMul = speedMul;
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
            ftIke& ike = dynamic_cast<ftIke&>(moduleAccesser->getStageObject());
            ftIkeSpecialHiParam* param = reinterpret_cast<ftIkeSpecialHiParam*>(g_ftCommonDataAccesser.getParam(ike.getFtKind()));
            ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
            int mode = 0;
            resetController(controller, mode, Vec2f(0.0f, 0.0f), moduleAccesser);
            controller.mulXSpeedMax(param->unk74 * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0));
            setTargetSpeed(controller, Vec2f(param->unk7c, 100.0f));
            controller.enable();
            moduleAccesser->getWorkManageModule().offFlag(0x22000012);
        }
        break;
    }
}

#pragma pop

void ftIkeStatusUniqProcessSpecialHi::execStop(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialHi::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x11e:
    case 0x120:
        return;
    case 0x11f:
        moduleAccesser->getGenerateArticleManageModule().removeExist(0, 0);
        return;
    case 0x73:
        moduleAccesser->getWorkManageModule().addInt(1, 0x10000040);
    }
    moduleAccesser->getGenerateArticleManageModule().removeExist(0, 0);
}

ftIkeStatusUniqProcessSpecialHi g_ftIkeStatusUniqProcessSpecialHi;
