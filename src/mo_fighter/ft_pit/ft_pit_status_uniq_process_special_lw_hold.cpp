#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// Writable float global defined in ft_pit.cpp (module data offset 0x6A48), value 25.0; its name is not recovered.
extern float g_ftPitUnk6A48;

class ftPitStatusUniqProcessSpecialLwHold : public soStatusUniqProcess {
public:
    virtual ~ftPitStatusUniqProcessSpecialLwHold() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    void updateShield(soModuleAccesser* moduleAccesser);
};

void ftPitStatusUniqProcessSpecialLwHold::initStatus(soModuleAccesser* moduleAccesser) {
    updateShield(moduleAccesser);
}

void ftPitStatusUniqProcessSpecialLwHold::execStatus(soModuleAccesser* moduleAccesser) {
    soMotionModule& motion = moduleAccesser->getMotionModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float prevValue = work.getFloat(0x21000004);
    float stick = moduleAccesser->getControllerModule().getStickY();
    // HYPOTHESIS: the stick is clamped to [0, limit] before smoothing.
    // The divisor is a float global (25.0) in ft_pit.cpp data, not a literal.
    if (stick < 0.0f) {
        stick = 0.0f;
    } else {
        float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb8, 0) / g_ftPitUnk6A48;
        if (stick > limit) stick = limit;
    }
    stick = prevValue + (stick - prevValue) * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb9, 0);
    motion.setFrame(stick * motion.getEndFrame(0x1eb));
    work.setFloat(stick, 0x21000004);
    updateShield(moduleAccesser);
}

void ftPitStatusUniqProcessSpecialLwHold::execStop(soModuleAccesser* moduleAccesser) {
    updateShield(moduleAccesser);
}

void ftPitStatusUniqProcessSpecialLwHold::updateShield(soModuleAccesser* moduleAccesser) {
    float radius = soValueAccesser::getConstantFloat(moduleAccesser, 0xfba, 0);
    Vec3f scale;
    scale.m_x = radius;
    scale.m_z = 1.0f;
    scale.m_y = 1.0f;
    moduleAccesser->getModelModule().setNodeScale(0x2e, &scale);
}

ftPitStatusUniqProcessSpecialLwHold g_ftPitStatusUniqProcessSpecialLwHold;
