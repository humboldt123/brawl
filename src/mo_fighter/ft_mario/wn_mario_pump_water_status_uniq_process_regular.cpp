#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <math.h>
#include <so/so_module_accesser.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>

// Keep the water-shot model pointed along its current velocity.
class wnMarioPumpWaterStatusUniqProcessRegular : public soStatusUniqProcess {
public:
    wnMarioPumpWaterStatusUniqProcessRegular() { }
    virtual ~wnMarioPumpWaterStatusUniqProcessRegular() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnMarioPumpWaterStatusUniqProcessRegular g_wnMarioPumpWaterStatusUniqProcessRegular;

void wnMarioPumpWaterStatusUniqProcessRegular::execFixPos(soModuleAccesser* moduleAccesser) {
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    Vec2f speed = energy.getSpeed();
    float x = speed.m_x;
    float y = speed.m_y;
    float angle = float(atan2(y, x)) * 57.29578f;
    Vec3f rotation;
    rotation.m_x = 0.0f;
    rotation.m_y = 0.0f;
    rotation.m_z = angle;
    moduleAccesser->getPostureModule().setRot(&rotation, 0);
}
