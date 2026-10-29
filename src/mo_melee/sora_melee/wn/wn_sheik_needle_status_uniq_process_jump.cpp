#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <so/stageobject.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <mt/mt_prng.h>
#include <types.h>

class wnSheikNeedleStatusUniqProcessJump : public soStatusUniqProcess {
public:
    virtual ~wnSheikNeedleStatusUniqProcessJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSheikNeedleStatusUniqProcessJump g_wnSheikNeedleStatusUniqProcessJump;

void wnSheikNeedleStatusUniqProcessJump::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    float speedXTable[8] = { 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f, 1.2f, 1.4f };
    float speedTable[8] = { -2.0f, -2.1f, -2.2f, -2.3f, -2.4f, -2.5f, -2.6f, -2.7f };
    float accelTable[8] = { -0.1f, -0.12f, -0.14f, -0.18f, -0.2f, -0.22f, -0.24f, -0.26f };
    float rotTable[8] = { 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f, 65.0f };
    wnKineticEnergyNormal& movement = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    soKineticEnergyRotNormal& rotation = dynamic_cast<soKineticEnergyRotNormal&>(*kinetic.getEnergy(1));
    float rot = rotTable[randi(8)] * (randi(2) == 0 ? 1.0f : -1.0f);
    float speedX = speedXTable[randi(8)] * (randi(2) == 0 ? 1.0f : -1.0f);
    float limitY = speedTable[randi(8)];
    float accelY = accelTable[randi(8)];
    Vec3f rotSpeed(rot, 0.0f, 0.0f);
    Vec2f speed;
    Vec2f::copy(speed, movement.getSpeed());
    speed.m_x = speedX;
    speed.m_y = __fabsf(speedTable[randi(8)]);
    Vec2f accel;
    Vec2f::copy(accel, movement.m_accel);
    accel.m_y = accelY;
    Vec2f limit(speedX, limitY);
    rotation.m_rotSpeed = rotSpeed;
    movement.m_speed = speed;
    movement.m_accel = accel;
    movement.m_speedTarget = limit;
    movement.m_speedLimit = limit;
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    pos.m_x += speed.m_x;
    pos.m_y += speed.m_y;
    moduleAccesser->getPostureModule().setPos(&pos);
    moduleAccesser->getStageObject().updateNodeSRT();
}
void wnSheikNeedleStatusUniqProcessJump::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 2) {
        soPostureModule& posture = moduleAccesser->getPostureModule();
        soEffectModule& effect = moduleAccesser->getEffectModule();
        Vec3f zero(0.0f, 0.0f, 0.0f);
        posture.setRot(&zero, 0);
        Vec3f* rot = &Vec3f(0.0f, 0.0f, 0.0f);
        Vec3f pos = posture.getPos();
        // HYPOTHESIS: effect scale from lbl_27_rodata_7870; effect id 0x22 from the requesting status.
        effect.req((EfID)0x22, &pos, rot, 1.0f, 0, -1);
    }
}
void wnSheikNeedleStatusUniqProcessJump::execFixPos(soModuleAccesser* moduleAccesser) {
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(0xff), 0)) {
        status.changeStatusRequest(2, moduleAccesser);
    }
}
