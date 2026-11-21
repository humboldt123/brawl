#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_value_accesser.h>
#include <mt/mt_common.h>
#include <mt/mt_prng.h>
#include <math.h>
#include <types.h>

class wnPokeFushigisouLeafCutterStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnPokeFushigisouLeafCutterStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnPokeFushigisouLeafCutterStatusUniqProcessMove g_wnPokeFushigisouLeafCutterStatusUniqProcessMove;

void wnPokeFushigisouLeafCutterStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) { }

// The leaf flies along a wavering angle: every few frames (constant 0x5dc1) a new target angle (work float 0x11000004)
// is picked at random within +-constant 0xfa3 of the current one (work float 0x11000003); the speed keeps its length
// and is turned to the angle, mirrored by the facing.
// HYPOTHESIS: the meaning of work floats 0x11000001/0x11000002 (an accumulator and its step).
void wnPokeFushigisouLeafCutterStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) {
    int count = moduleAccesser->getWorkManageModule().getInt(0x10000005);
    count++;
    if (count > soValueAccesser::getConstantInt(moduleAccesser, 0x5DC1, 0)) {
        float angle = moduleAccesser->getWorkManageModule().getFloat(0x11000003);
        float range = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA3, 0);
        float target = angle + (2.0f * range * randf() - range);
        moduleAccesser->getWorkManageModule().setFloat(target, 0x11000004);
        count = 0;
    }
    moduleAccesser->getWorkManageModule().setInt(count, 0x10000005);
    float accum = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    float step = moduleAccesser->getWorkManageModule().getFloat(0x11000002);
    moduleAccesser->getWorkManageModule().setFloat(accum + step, 0x11000001);
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x11000004);
    moduleAccesser->getWorkManageModule().getFloat(0x11000003); // the previous angle is read and dropped
    moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000003);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    float length = energy.getSpeed().length();
    float lr = moduleAccesser->getPostureModule().getLr();
    Vec2f speed(lr * (length * (float)cos(0.017453292f * angle)), length * (float)sin(0.017453292f * angle));
    energy.m_speed = speed;
    energy.enable();
}
