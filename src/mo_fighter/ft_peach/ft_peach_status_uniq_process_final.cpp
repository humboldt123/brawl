#include <ft/peach/ft_peach.h>
#include <ft/ft_value_accesser.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_rot_utility.h>
#include <ft/peach/ft_peach_status_uniq_process_final.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_slow_manager.h>
#include <gf/gf_task_scheduler.h>
#include <gm/gm_global.h>
#include <if/if_mngr.h>
#include <it/it_manager.h>
#include <mt/mt_prng.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <st/stage.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <snd/snd_system.h>
#include <types.h>

// HYPOTHESIS: two unused file-scope sheet records; the original runs their constructor (0xFF, 0 and 0xFF, 1) from this
// unit's static initializer ahead of the status object.
static itGenSheetKind s_sheetKind0((Stages::srStageKind)0xFF, 0);
static itGenSheetKind s_sheetKind1((Stages::srStageKind)0xFF, 1);

ftPeachStatusUniqProcessFinal g_ftPeachStatusUniqProcessFinal;

// HYPOTHESIS: work variable meanings, read from their use below: 0x21000004..6 are the spin speeds added to the blossom model's rotation
// each frame (X, Y, Z), 0x20000002 the number of food items still to drop, 0x20000003 the frames until the next drop,
// 0x22000010 / 0x22000011 the "dropping" and "leaving" flags, 0x10000041 the screen task id, 0x10000042 the effect handle.
void ftPeachStatusUniqProcessFinal::initStatus(soModuleAccesser* accesser) {
    const float firstMinAngle = soValueAccesser::getConstantFloat(accesser, 0xFBE, 0);
    const float firstMaxAngle = soValueAccesser::getConstantFloat(accesser, 0xFBF, 0);
    const float firstAngle = firstMinAngle + (firstMaxAngle - firstMinAngle) * randf();
    accesser->getWorkManageModule().setFloat(firstAngle, 0x21000004);

    accesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(accesser, 0xFBD, 0), 0x21000005);

    const float secondMinAngle = soValueAccesser::getConstantFloat(accesser, 0xFBE, 0);
    const float secondMaxAngle = soValueAccesser::getConstantFloat(accesser, 0xFBF, 0);
    const float secondAngle = secondMinAngle + (secondMaxAngle - secondMinAngle) * randf();
    accesser->getWorkManageModule().setFloat(secondAngle, 0x21000006);

    accesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(accesser, 0x5DC3, 0), 0x20000002);

    createInfo(accesser);
    accesser->getEffectModule().setSyncScale(false);
    const int effectHandle = accesser->getEffectModule().req2D(static_cast<EfID>(0xED000A), &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0);
    accesser->getEffectModule().setSyncScale(true);
    accesser->getWorkManageModule().setInt(effectHandle, 0x10000042);
    accesser->getVisibilityModule().setStatusDefault(0, -1, true);
    ftPeach* peach = dynamic_cast<ftPeach*>(&accesser->getStageObject());
    if (peach != NULL) peach->addCallback();
}

void ftPeachStatusUniqProcessFinal::execStatus(soModuleAccesser* accesser) {
    Vec3f rotation = accesser->getPostureModule().getRot(1);
    // BYTES: an in-place paired-single add (the shared Vec3f::operator+= goes through a temporary and an out-of-line assignment).
    // The work module is loaded once for the three getFloat calls of this one expression.
    Vec3fAdd(&rotation, &rotation,
             &Vec3f(accesser->getWorkManageModule().getFloat(0x21000004),
                    accesser->getWorkManageModule().getFloat(0x21000005),
                    accesser->getWorkManageModule().getFloat(0x21000006)));
    rotation = soRotUtility::clampDeg(rotation);
    accesser->getPostureModule().setRot(&rotation, 1);

    if (accesser->getWorkManageModule().isFlag(0x22000010) == 1) {
        if (accesser->getWorkManageModule().getInt(0x20000003) <= 0 && accesser->getWorkManageModule().getInt(0x20000002) > 0) {
            Vec3f position(0.0f, 0.0f, 0.0f);
            Stage::getInstance()->getRandItemPos(&position);
            BaseItem* item = itManager::getInstance()->createItem(&position,
                accesser->getPostureModule().getLr(), Item_Food, 0x1C,
                accesser->getStageObject().getTaskId());
            Stage* stage = Stage::getInstance();
            if (item == NULL || stage == NULL) return;
            item->appear(&Vec3f(position), 0, 0.0f);
            accesser->getWorkManageModule().decInt(0x20000002);
            accesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(accesser, 0x5DC4, 0), 0x20000003);
        } else {
            accesser->getWorkManageModule().decInt(0x20000003);
        }
    }
}

void ftPeachStatusUniqProcessFinal::execFixPos(soModuleAccesser* accesser) {
    if (accesser->getWorkManageModule().isFlag(0x22000011) == 1) setOutAction(accesser);
}

void ftPeachStatusUniqProcessFinal::exitStatus(soModuleAccesser* accesser, int nextStatus) {
    if (nextStatus != 0 && nextStatus != 0xE) destroyInfo(accesser);
    else setOutAction(accesser);
    if (g_sndSystem != NULL) g_sndSystem->finalOffState(0x1B4A);
}

void ftPeachStatusUniqProcessFinal::setOutAction(soModuleAccesser* accesser) {
    accesser->getWorkManageModule().offFlag(0x22000011);
    int taskId = accesser->getWorkManageModule().getInt(0x10000041);
    if (taskId != 0) {
        gfTask* base = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Info, taskId);
        if (base != NULL) {
            IfPeachFinalTask* task = dynamic_cast<IfPeachFinalTask*>(base);
            if (task != NULL && !task->m_actionActive) {
                if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen == 1) {
                    task->setAction(3);
                } else {
                    task->setAction(1);
                }
                task->m_actionActive = true;
            }
        }
    }
}

bool ftPeachStatusUniqProcessFinal::createInfo(soModuleAccesser* accesser) {
    if (accesser->getWorkManageModule().getInt(0x10000041) != 0) destroyInfo(accesser);
    ftResourceIdAccesserImpl& resourceIdAccesser =
        dynamic_cast<ftResourceIdAccesserImpl&>(*accesser->getResourceModule().getResourceIdAccesser());
    const int resourceId = resourceIdAccesser.getFinalResId();
    void* data = accesser->getResourceModule().getBinFile(resourceId, 0, -1);
    if (data != NULL) {
        IfPeachFinalTask* task = IfPeachFinalTask::create(data, Heaps::HeapType(0x1F), accesser->getStageObject().getTaskId(), true);
        if (task != NULL) {
            accesser->getWorkManageModule().setInt(task->m_taskId, 0x10000041);
            if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen == 1) {
                task->setAction(2);
            } else {
                task->setAction(0);
            }
            return true;
        }
    }
    return false;
}

void ftPeachStatusUniqProcessFinal::destroyInfo(soModuleAccesser* accesser) {
    const int taskId = accesser->getWorkManageModule().getInt(0x10000041);
    if (taskId != 0) {
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Info, taskId);
        if (task != NULL) {
            task->exit();
            accesser->getEffectModule().killAll(0, true, true);
        }
        accesser->getWorkManageModule().setInt(0, 0x10000041);
    }
    accesser->getWorkManageModule().setInt(0, 0x10000042);
}
