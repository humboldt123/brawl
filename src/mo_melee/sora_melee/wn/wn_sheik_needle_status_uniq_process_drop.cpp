#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <mt/mt_prng.h>
#include <types.h>

class wnSheikNeedleStatusUniqProcessDrop : public soStatusUniqProcess {
public:
    virtual ~wnSheikNeedleStatusUniqProcessDrop() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSheikNeedleStatusUniqProcessDrop g_wnSheikNeedleStatusUniqProcessDrop;

void wnSheikNeedleStatusUniqProcessDrop::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    float speedTable[8] = { -2.0f, -2.1f, -2.2f, -2.3f, -2.4f, -2.5f, -2.6f, -2.7f };
    float accelTable[8] = { -0.1f, -0.12f, -0.14f, -0.18f, -0.2f, -0.22f, -0.24f, -0.26f };
    float rotTable[8] = { 12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f, 26.0f };
    wnKineticEnergyNormal& movement = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    soKineticEnergyRotNormal& rotation = dynamic_cast<soKineticEnergyRotNormal&>(*kinetic.getEnergy(1));
    float rot = rotTable[randi(8)] * (randi(2) == 0 ? 1.0f : -1.0f);
    float zero = 0.0f;
    float limitY = speedTable[randi(8)];
    float accelY = accelTable[randi(8)];
    Vec3f rotSpeed(rot, 0.0f, 0.0f);
    Vec2f speed;
    Vec2f::copy(speed, movement.getSpeed());
    speed.m_x = zero;
    speed.m_y = __fabsf(speedTable[randi(8)]);
    Vec2f accel;
    Vec2f::copy(accel, movement.m_accel);
    accel.m_y = accelY;
    Vec2f limit(zero, limitY);
    rotation.m_rotSpeed = rotSpeed;
    movement.m_speed = speed;
    movement.m_accel = accel;
    movement.m_speedTarget = limit;
    movement.m_speedLimit = limit;
}
void wnSheikNeedleStatusUniqProcessDrop::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 3) {
        soPostureModule& posture = moduleAccesser->getPostureModule();
        soEffectModule& effect = moduleAccesser->getEffectModule();
        Vec3f zero(0.0f, 0.0f, 0.0f);
        posture.setRot(&zero, 0);
        Vec3f* rot = &Vec3f(0.0f, 0.0f, 0.0f);
        Vec3f pos = posture.getPos();
        // HYPOTHESIS: effect scale from lbl_27_rodata_77D8; effect id 0x22 from the requesting status.
        effect.req((EfID)0x22, &pos, rot, 1.0f, 0, -1);
    }
}
void wnSheikNeedleStatusUniqProcessDrop::execFixPos(soModuleAccesser* moduleAccesser) {
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(0xff), 0)) {
        status.changeStatusRequest(3, moduleAccesser);
    }
}
