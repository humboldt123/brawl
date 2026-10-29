#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <mt/mt_prng.h>
#include <math.h>
#include <types.h>

class wnSheikNeedleStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnSheikNeedleStatusUniqProcessMove() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSheikNeedleStatusUniqProcessMove g_wnSheikNeedleStatusUniqProcessMove;

void wnSheikNeedleStatusUniqProcessMove::execFixPos(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    soGroundModule& ground = moduleAccesser->getGroundModule();
    Vec2f speed;
    Vec2f::copy(speed, kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float negativeLr = -posture.getLr();
    float angle = (float)atan2(speed.m_y, speed.m_x * negativeLr);
    Vec3f rot(57.29578f * angle, 0.0f, 0.0f);
    posture.setRot(&rot, 0);
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(0xff), 0)) {
        status.changeStatusRequest(randi(5) < 3 ? 2 : 3, moduleAccesser);
    }
}

void wnSheikNeedleStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 2 && status != 3) {
        soPostureModule& posture = moduleAccesser->getPostureModule();
        soEffectModule& effect = moduleAccesser->getEffectModule();
        Vec3f zero(0.0f, 0.0f, 0.0f);
        posture.setRot(&zero, 0);
        Vec3f* rot = &Vec3f(0.0f, 0.0f, 0.0f);
        Vec3f pos = posture.getPos();
        // HYPOTHESIS: effect scale from lbl_27_rodata_7770; effect id 0x22 from the requesting status.
        effect.req((EfID)0x22, &pos, rot, 1.0f, 0, -1);
    }
}
