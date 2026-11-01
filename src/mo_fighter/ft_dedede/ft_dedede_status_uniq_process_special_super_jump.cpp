#include <ft/ft_kinetic_energy.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
// Constant table shared with ft_dedede.cpp data (lbl_117_rodata_18): 0.0, 1.0, -1.0.
static const float s_superJumpConst[3] = { 0.0f, 1.0f, -1.0f };

class ftDededeStatusUniqProcessSpecialSuperJump : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialSuperJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

// MATCH-ONLY: retain reset mode across the sum-speed query and vector construction, as in the native register lifetime.
#pragma push
#pragma opt_propagation off
void ftDededeStatusUniqProcessSpecialSuperJump::initStatus(soModuleAccesser* moduleAccesser) {
    const float* k = s_superJumpConst;
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
    int mode = 0xb;
    Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    if (sumSpeed.m_x > soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0)) {
        sumSpeed.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
    }
    controller.resetEnergy(mode, &Vec2f(sumSpeed.m_x, k[0]), &Vec3f(k[0], k[0], k[0]), moduleAccesser);
    controller.m_accelMul = k[1];
    controller.m_unk40 = k[0];
    controller.enable();

    ftKineticEnergyGravity& energy = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    float a = k[2] * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb5, 0);
    float b = k[2] * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb6, 0);
    energy.resetEnergy(0, &Vec2f(k[0], a), &Vec3f(k[0], k[0], k[0]), moduleAccesser);
    energy.m_gravity = b;
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        energy.enable();
    } else {
        energy.disable();
    }
}
#pragma pop

void ftDededeStatusUniqProcessSpecialSuperJump::execStatus(soModuleAccesser* moduleAccesser) {
    float stickX = moduleAccesser->getControllerModule().getStickX();
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
    Vec2f speed;
    speed.m_x = stickX * scale;
    speed.m_y = s_superJumpConst[0];
    controller.m_speed = speed;
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        moduleAccesser->getKineticModule().getEnergy(1)->enable();
    } else {
        moduleAccesser->getKineticModule().getEnergy(1)->disable();
    }
}

void ftDededeStatusUniqProcessSpecialSuperJump::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialSuperJump::exitStatus(soModuleAccesser* moduleAccesser, int) {
}

ftDededeStatusUniqProcessSpecialSuperJump g_ftDededeStatusUniqProcessSpecialSuperJump;
