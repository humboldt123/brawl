#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC2F_PLAIN_BASE
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/wolf/ft_wolf_status_uniq_process_reflector.h>
#include <so/turn/so_turn_module.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// The reflector (Fox's shine) shared by Fox, Falco and Wolf; each fighter supplies its own setTurn (the turn applied when the
// stick opposes the facing). Status 0x115 is the ground start, 0x11a/0x11b the reflecting states and 0x11a is also where
// the turn is checked. The work int 0x20000002 counts the frames the aerial shine holds the fighter.
// HYPOTHESIS: names of the statuses and of the work value; the class shape is taken from
// include/ft/wolf/ft_wolf_status_uniq_process_reflector.h.
void ftFoxStatusUniqProcessReflector::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x115: {
        // The carried speed is slowed down and held by the stop energy.
        Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
        float speedX = sumSpeed.m_x / soValueAccesser::getConstantFloat(moduleAccesser, 0xfba, 0);
        stop.m_speed = Vec2f(speedX, 0.0f);
        stop.enable();
        ftKineticEnergyDisableAndClear(2, moduleAccesser);
        ftKineticEnergyDisableAndClear(0, moduleAccesser);
        break;
    }
    case 0x11a:
    case 0x11b:
        moduleAccesser->getCollisionReflectorModule().setStatus(0, 1, 2);
        break;
    }
}

void ftFoxStatusUniqProcessReflector::exitStatus(soModuleAccesser* moduleAccesser, int) { }

void ftFoxStatusUniqProcessReflector::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11a:
        // Pushing the stick against the facing turns the reflector around (unless a turn is already running).
        if (!static_cast<soTurnModule*>(moduleAccesser->m_enumerationStart->m_turnModule)->isTurn()) {
            float turnLimit = soValueAccesser::getConstantFloat(moduleAccesser, 0xc31, 0);
            float stickX = moduleAccesser->getControllerModule().getStickX();
            if (stickX * moduleAccesser->getPostureModule().getLr() <= turnLimit) {
                setTurn(moduleAccesser);
            }
        }
        break;
    }
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        // In the air the shine holds the fall for a number of frames, then gravity pulls again.
        int frames = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.enable();
        if (frames - 1 > 0) {
            gravity.m_speedY = 0.0f;
            gravity.m_gravity = 0.0f;
        } else {
            gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfbb, 0);
        }
        moduleAccesser->getWorkManageModule().setInt(frames - 1, 0x20000002);
    }
}
