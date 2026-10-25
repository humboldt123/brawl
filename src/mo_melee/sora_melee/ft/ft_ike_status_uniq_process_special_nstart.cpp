#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Aether (Ike Special N) start: the charge begins with a short slide that is stopped by a brake.
class ftIkeStatusUniqProcessSpecialNStart : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialNStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftIkeStatusUniqProcessSpecialNStart g_ftIkeStatusUniqProcessSpecialNStart;

// HYPOTHESIS: work 0x20000002 holds the situation (ground or air) the move started in.
void ftIkeStatusUniqProcessSpecialNStart::initStatus(soModuleAccesser* acc) {
    Vec2f speed;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    // HYPOTHESIS: constants 0xfa0 (speed ratio) and 0xfa1 (brake) from the Ike constant table.
    float speedX = speed.m_x * acc->getConstantFloatKirby(0xfa0);
    float speedY = speed.m_y;
    float brake = acc->getConstantFloatKirby(0xfa1);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    if (acc->getSituationModule().getKind() == 2) {
        stop.resetEnergy(6, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
        stop.m_accel = Vec2f(0.0f, 0.0f);
        stop.m_brake = Vec2f(brake, 0.0f);
        stop.m_speedTarget = Vec2f(0.0f, 0.0f);
        stop.enable();
        // Starting the aerial charge cancels downward speed, preserving upward motion.
        if (speedY < 0.0f) speedY = 0.0f;
        gravity.resetEnergy(0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f), acc);
        gravity.enable();
        acc->getWorkManageModule().setInt(2, 0x20000002);
    } else {
        stop.resetEnergy(0, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
        stop.m_accel = Vec2f(0.0f, 0.0f);
        stop.m_brake = Vec2f(brake, 0.0f);
        stop.m_speedTarget = Vec2f(0.0f, 0.0f);
        stop.enable();
        acc->getWorkManageModule().setInt(0, 0x20000002);
    }
    acc->getKineticModule().getEnergy(2)->disable();
    acc->getKineticModule().getEnergy(0)->disable();
}

void ftIkeStatusUniqProcessSpecialNStart::execStatus(soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    soSituationModule& situation = acc->getSituationModule();
    int savedSituation = work.getInt(0x20000002);
    int kind = situation.getKind();
    if (kind != savedSituation) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        Vec2f speed;
        Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        float brake = acc->getConstantFloatKirby(0xfa1);
        if (acc->getSituationModule().getKind() == 2) {
            stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
            stop.m_accel = Vec2f(0.0f, 0.0f);
            stop.m_brake = Vec2f(brake, 0.0f);
            stop.m_speedTarget = Vec2f(0.0f, 0.0f);
            stop.enable();
            gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), acc);
            gravity.enable();
            acc->getWorkManageModule().setInt(2, 0x20000002);
        } else {
            stop.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
            stop.m_speedTarget = Vec2f(0.0f, 0.0f);
            stop.enable();
            gravity.disable();
            acc->getWorkManageModule().setInt(0, 0x20000002);
        }
    }
}

void ftIkeStatusUniqProcessSpecialNStart::exitStatus(soModuleAccesser*, int) { }
