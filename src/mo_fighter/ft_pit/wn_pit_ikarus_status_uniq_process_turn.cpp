#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: the status processes of the Ikarus (the drifting weapon of Pit's side taunt); the turn exit puts the weapon
// at the ground plane depth with its rotation reset.
class wnPitIkarusStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    virtual ~wnPitIkarusStatusUniqProcessTurn() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnPitIkarusStatusUniqProcessTurn g_wnPitIkarusStatusUniqProcessTurn;

void wnPitIkarusStatusUniqProcessTurn::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getPostureModule().initRot();
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    pos.m_z = moduleAccesser->getGroundModule().getZ();
    moduleAccesser->getPostureModule().setPos(&pos);
}
