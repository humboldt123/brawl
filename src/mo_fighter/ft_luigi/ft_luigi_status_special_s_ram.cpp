#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/luigi/ft_luigi_status_special_s.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <mt/mt_prng.h>
#include <math.h>

void ftLuigiStatusUniqProcessSpecialSRam::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getGroundModule().setTestCollStopStatus(true, 0);
}

void ftLuigiStatusUniqProcessSpecialSRam::execFixPos(soModuleAccesser* moduleAccesser) {
    int touchFlags = moduleAccesser->getGroundModule().getTouchFlag(0);
    if ((touchFlags & 0x6) != 0) {
        Vec3f speed = moduleAccesser->getKineticModule().getSumSpeed3f(
            soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL));
        int status = 0x119;
        if (moduleAccesser->getGroundModule().isAttachable(touchFlags, 0) == true) {
            double absoluteSpeed = __fabs(speed.m_x);
            float horizontalSpeed = absoluteSpeed;
            float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xFBB, 0);
            if (horizontalSpeed >= threshold) {
                float chance = soValueAccesser::getConstantFloat(moduleAccesser, 0xFBC, 0);
                if (randf() < chance) {
                    if ((touchFlags & 0x2) != 0) {
                        if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
                            status = 0x11A;
                        }
                    } else if (moduleAccesser->getPostureModule().getLr() == 1.0f) {
                        status = 0x11A;
                    }
                }
            }
        }
        moduleAccesser->getStatusModule().changeStatusRequest(status, moduleAccesser);
    } else {
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000012) &&
            !moduleAccesser->getWorkManageModule().isFlag(0x22000015) &&
            moduleAccesser->getCollisionAttackModule().isAttack(0, false) == true) {
            soWorkManageModule& work = moduleAccesser->getWorkManageModule();
            float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAC, 0);
            int count = work.getInt(0x20000000);
            float scaledCount = count * scale;
            float offset = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAB, 0);
            float power = offset + scaledCount;
            moduleAccesser->getCollisionAttackModule().setPower(0, (int)power, false);
            moduleAccesser->getWorkManageModule().onFlag(0x22000015);
        }
    }
}

ftLuigiStatusUniqProcessSpecialSRam g_ftLuigiStatusUniqProcessSpecialSRam;
