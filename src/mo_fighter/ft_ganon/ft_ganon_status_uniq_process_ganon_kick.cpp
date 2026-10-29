// MATCH-ONLY: the Vec3f constructor stays out of line in this unit.
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ganon/ft_ganon_status_uniq_process_ganon_kick.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: this translation unit holds Ganondorf's Wizard's Foot status processes. Only Kick::onHit is
// reconstructed; the KickEnd bodies are not (their work and parameter ids are listed in the notes).

// Group 4 of the fighter's extend parameters (ftData + 0x88); defined in ft_ganon.cpp.
extern "C" void* fn_109_862C();

static void setEndBrake(soModuleAccesser* moduleAccesser, int groundParamId, int commonParamId);

// MATCH-ONLY: the original inlines this member here without emitting an out-of-line copy.
inline void soKineticEnergyNormal::setBrake(Vec2f* brake) {
    m_brake.m_x = brake->m_x;
    m_brake.m_y = brake->m_y;
}

void ftGanonStatusUniqProcessGanonKick::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessGanonKick::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessGanonKickEnd::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: work int 0x20000000 is 0 for the ground kick and 2 for the air kick; the end status rebuilds the
    // stop (and, in the air, gravity) energies from the speed the kick carried.
    int situation = moduleAccesser->getSituationModule().getKind();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    int variant = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    if (variant == 0 && situation == 0) {
        Vec2f sumSpeed;
        Vec2f::copy(sumSpeed, kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        soKineticEnergy* stop = kinetic.getEnergy(3);
        stop->resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    } else if (variant == 2 && situation == 2) {
        Vec2f sumSpeed;
        Vec2f::copy(sumSpeed, kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        kinetic.unableEnergyAll();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        soKineticEnergy* stop = kinetic.getEnergy(3);
        gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop->resetEnergy(9, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        stop->enable();
    }
}

void ftGanonStatusUniqProcessGanonKickEnd::execStatus(soModuleAccesser* moduleAccesser) {
    int situation = moduleAccesser->getSituationModule().getKind();
    int variant = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    if (variant == 0 && situation == 0) {
        setEndBrake(moduleAccesser, 0xfb8, 0xbbc);
    } else if (variant == 2 && situation == 0) {
        setEndBrake(moduleAccesser, 0xfb9, 0xbbc);
    }
}

void ftGanonStatusUniqProcessGanonKickEnd::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        // HYPOTHESIS: kinetic module slot 0x2C clears the speed of every energy.
        moduleAccesser->getKineticModule().clearSpeedAll();
    }
}

// HYPOTHESIS: horizontal brake of the stop energy; the ground variant 0x22000010 uses the move's parameter, otherwise
// the common one.
static void setEndBrake(soModuleAccesser* moduleAccesser, int groundParamId, int commonParamId) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    float brake;
    if (work.isFlag(0x22000010)) {
        brake = soValueAccesser::getConstantFloat(moduleAccesser, groundParamId, 0);
    } else {
        brake = soValueAccesser::getConstantFloat(moduleAccesser, commonParamId, 0);
    }
    stop.soKineticEnergyNormal::setBrake(&Vec2f(brake, 0.0f));
}

void ftGanonStatusUniqProcessGanonKick::onHit(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    // HYPOTHESIS: work int 0x20000000 is 0 for the ground kick, 0x20000001 counts the hits that still slow the kick.
    int variant = work.getInt(0x20000000);
    moduleAccesser->getSituationModule().getKind();
    switch (status.getStatusKind()) {
    case 0x115:
        if (variant == 0) {
            int hitsLeft = work.getInt(0x20000001);
            if (hitsLeft >= 1) {
                // HYPOTHESIS: the first float of group 4 is the speed multiplier applied per hit.
                float* param = static_cast<float*>(fn_109_862C());
                // Scale the animation-driven velocity for the kick.
                float current = work.getFloat(0x21000004);
                float speedMul = current * param[0];
                dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0)).m_speedMul = speedMul;
                work.setFloat(speedMul, 0x21000004);
                work.setInt(hitsLeft - 1, 0x20000001);
            }
        }
        break;
    case 0x11e:
        break;
    }
}

ftGanonStatusUniqProcessGanonKick g_ftGanonStatusUniqProcessGanonKick;
ftGanonStatusUniqProcessGanonKickEnd g_ftGanonStatusUniqProcessGanonKickEnd;
