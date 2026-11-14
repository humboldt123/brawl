#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/collision/so_collision_shield_module_impl.h>
#include <types.h>

// Native effect tuning: zero, vertical offset, scale, zero.
// PSI Magnet starts a following effect and looping sound when their handles are absent.
// HYPOTHESIS: work int 0x20000003 holds an effect handle and 0x20000002 a sound handle; both are cleared on exit.
class ftNessStatusUniqProcessSpecialLwHold : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessSpecialLwHold() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

ftNessStatusUniqProcessSpecialLwHold g_ftNessStatusUniqProcessSpecialLwHold;

void ftNessStatusUniqProcessSpecialLwHold::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: these flags configure the absorber group for PSI Magnet.
    moduleAccesser->getCollisionAbsorberModule().setStatus(0, 1, 0);
    if (moduleAccesser->getWorkManageModule().getInt(0x20000003) < 0) {
        int effectId = moduleAccesser->getEffectModule().reqFollow(static_cast<EfID>(0xB000C), 4,
            &Vec3f(0.0f, 6.5f, 0.0f),
            &Vec3f(0.0f, 0.0f, 0.0f),
            0.9f, false, 0, 0, -1);
        moduleAccesser->getWorkManageModule().setInt(effectId, 0x20000003);
    }
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) < 0) {
        int soundHandle = moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0xE7F), true, 0, 0);
        moduleAccesser->getWorkManageModule().setInt(soundHandle, 0x20000002);
    }
}

void ftNessStatusUniqProcessSpecialLwHold::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11e && nextStatus != 0x11f) {
        int effectId = moduleAccesser->getWorkManageModule().getInt(0x20000003);
        if (effectId >= 0) {
            moduleAccesser->getEffectModule().kill(effectId, true, true);
        }
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000003);
        int soundHandle = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        if (soundHandle >= 0) {
            moduleAccesser->getSoundModule().stopSEHandle(soundHandle, 0);
        }
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000002);
    }
}
