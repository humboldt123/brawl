#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <cm/cm_camera_controller.h>
#include <mt/mt_prng.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_energy_3f.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the entry of the Ikarus (the weapon of Pit's side taunt) picks a random point on the edge of the stage camera
// box and flies there in the number of frames given by a constant.
class wnPitIkarusStatusUniqProcessEntry : public soStatusUniqProcess {
public:
    virtual ~wnPitIkarusStatusUniqProcessEntry() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnPitIkarusStatusUniqProcessEntry g_wnPitIkarusStatusUniqProcessEntry;

void wnPitIkarusStatusUniqProcessEntry::initStatus(soModuleAccesser* moduleAccesser) {
    Rect2D bounds = CameraController::getInstance()->m_stageCameraBounds;
    Vec2f center;
    Vec2f half;
    half.m_x = 0.5f * (bounds.m_right - bounds.m_left);
    half.m_y = 0.5f * (bounds.m_up - bounds.m_down);
    center.m_x = bounds.m_left + half.m_x;
    center.m_y = bounds.m_down + half.m_y;
    half.m_x *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfa0, 0);
    half.m_y *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfa1, 0);
    center.m_y += 2.0f * half.m_y * soValueAccesser::getConstantFloat(moduleAccesser, 0xfa2, 0);
    bounds.m_left = center.m_x - half.m_x;
    bounds.m_down = center.m_y - half.m_y;
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    float side = (pos.m_x < center.m_x) ? 1.0f : -1.0f;
    moduleAccesser->getWorkManageModule().setFloat(side, 0x21000000);
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000001);
    Vec2f base;
    if (angle <= 1.5707964f) {
        base.m_x = center.m_x;
        base.m_y = center.m_y;
    } else if (angle <= 3.1415927f) {
        base.m_x = bounds.m_left;
        base.m_y = center.m_y;
    } else if (angle <= 4.712389f) {
        base.m_x = bounds.m_left;
        base.m_y = bounds.m_down;
    } else {
        base.m_x = center.m_x;
        base.m_y = bounds.m_down;
    }
    Vec2f target;
    target.m_x = base.m_x + half.m_x * randf();
    target.m_y = base.m_y + half.m_y * randf();
    int frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc0, 0);
    float invFrames = 1.0f / frames;
    Vec3f speed;
    speed.m_x = invFrames * (target.m_x - pos.m_x);
    speed.m_y = invFrames * (target.m_y - pos.m_y);
    speed.m_z = invFrames * (moduleAccesser->getGroundModule().getZ() - pos.m_z);
    soKineticEnergy3f& energy = dynamic_cast<soKineticEnergy3f&>(*moduleAccesser->getKineticModule().getEnergy(0));
    energy.m_speed3f.m_x = speed.m_x;
    energy.m_speed3f.m_y = speed.m_y;
    energy.m_speed3f.m_z = speed.m_z;
}
