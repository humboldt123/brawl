#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_value_accesser.h>
#include <mt/mt_common.h>
#include <mt/mt_prng.h>
#include <math.h>
#include <types.h>

// HYPOTHESIS: float table at lbl_116_rodata_C0 (random scale, default length, length epsilon, angle rate).
extern "C" const float lbl_116_rodata_C0[4];

// Local view of the leaf cutter's kinetic energy: m_speed is at +0x08; byte +0x05 carries a flag set on update.
class wnPokeFushigisouLeafCutterEnergyView : public soKineticEnergyNormal {
};

class wnPokeFushigisouLeafCutterStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnPokeFushigisouLeafCutterStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnPokeFushigisouLeafCutterStatusUniqProcessMove g_wnPokeFushigisouLeafCutterStatusUniqProcessMove;

void wnPokeFushigisouLeafCutterStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) { }

void wnPokeFushigisouLeafCutterStatusUniqProcessMove::execStatus(soModuleAccesser* a) {
    const float* table = lbl_116_rodata_C0;
    int count = a->getWorkManageModule().getInt(0x10000005);
    int next = count + 1;
    if (next > soValueAccesser::getConstantInt(a, 0x5DC1, 0)) {
        float base = a->getWorkManageModule().getFloat(0x11000003);
        float scale = soValueAccesser::getConstantFloat(a, 0xFA3, 0);
        float value = base + (table[0] * scale * randf() - scale);
        a->getWorkManageModule().setFloat(value, 0x11000004);
        next = 0;
    }
    a->getWorkManageModule().setInt(next, 0x10000005);
    float t = a->getWorkManageModule().getFloat(0x11000001);
    float delta = a->getWorkManageModule().getFloat(0x11000002);
    a->getWorkManageModule().setFloat(t + delta, 0x11000001);
    float angle = a->getWorkManageModule().getFloat(0x11000004);
    a->getWorkManageModule().getFloat(0x11000003);
    a->getWorkManageModule().setFloat(angle, 0x11000003);
    wnPokeFushigisouLeafCutterEnergyView& energy =
        dynamic_cast<wnPokeFushigisouLeafCutterEnergyView&>(*a->getKineticModule().getEnergy(0));
    soKineticEnergy& base = energy;
    Vec2f speed = base.getSpeed();
    float lenSq = speed.m_x * speed.m_x + speed.m_y * speed.m_y;
    float length;
    if (static_cast<float>(fabs(lenSq)) <= table[2]) {
        length = table[1];
    } else {
        length = lenSq * rsqrtf(lenSq);
    }
    float lr = a->getPostureModule().getLr();
    float sinValue = static_cast<float>(sin(table[3] * angle));
    float cosValue = static_cast<float>(cos(table[3] * angle));
    float x = lr * (length * cosValue);
    float y = length * sinValue;
    energy.m_speed.m_x = x;
    energy.m_speed.m_y = y;
    energy.enable();
}
