#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/luigi/ft_luigi_status_special_s.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_util.h>
#include <so/so_module_accesser.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>

void ftLuigiStatusUniqProcessSpecialSWall::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getGroundModule().update(1);
    soMotionChangeParam motion;
    motion.m_kind = 0x1D5;
    motion.m_frame = 0.0f;
    motion.m_rate = 1.0f;
    motion._12 = 0;
    motion._13 = 0;
    motion._14 = 0;
    motion._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&motion);

    float lr = moduleAccesser->getPostureModule().getLr();
    if (lr == -1.0f) {
        moduleAccesser->getGroundModule().attach(2, 0);
    } else {
        moduleAccesser->getGroundModule().attach(4, 0);
    }

    ftUtil::adjustWall(moduleAccesser, lr, -lr, -1.0f, 1);
    ftUtil::adjustWall(moduleAccesser, lr, (float)(lr == -1.0f), 1.0f, 0);

    soKineticEnergyGroundMovement& groundMovement =
        dynamic_cast<soKineticEnergyGroundMovement&>(
            *moduleAccesser->getKineticModule().getEnergy(6));
    Vec2f attachNormal;
    Vec2f::copy(attachNormal, moduleAccesser->getGroundModule().getAttachNormal(0));
    groundMovement.enableRot(&attachNormal);

    moduleAccesser->getKineticModule().getEnergy(5)->disable();
}

ftLuigiStatusUniqProcessSpecialSWall g_ftLuigiStatusUniqProcessSpecialSWall;
