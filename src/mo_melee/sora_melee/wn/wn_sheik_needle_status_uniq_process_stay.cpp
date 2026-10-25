#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class wnSheikNeedleStatusUniqProcessStay : public soStatusUniqProcess {
public:
    virtual ~wnSheikNeedleStatusUniqProcessStay() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnSheikNeedleStatusUniqProcessStay g_wnSheikNeedleStatusUniqProcessStay;

// Leaving the stay state clears the needle's rotation and plays its release effect at the current position.
void wnSheikNeedleStatusUniqProcessStay::exitStatus(soModuleAccesser* moduleAccesser, int) {
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soEffectModule& effect = moduleAccesser->getEffectModule();
    Vec3f zero(0.0f, 0.0f, 0.0f);
    posture.setRot(&zero, 0);
    Vec3f* rot = &Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f pos = posture.getPos();
    // HYPOTHESIS: effect scale from lbl_27_rodata_77EC; effect id 0x22 from the requesting status.
    effect.req((EfID)0x22, &pos, rot, 1.0f, 0, -1);
}
