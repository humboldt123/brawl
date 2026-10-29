#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/metaknight/ft_metaknight_status_views.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftMetaknightStatusUniqProcessFinalHitWait : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessFinalHitWait() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    static int getFreeAttackGalleryStatusWorkId(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: parameter types; true when one of the linked targets 6..11 belongs to the entry.
    static bool isFinalTarget(int entryId, soModuleAccesser* moduleAccesser);
};

void ftMetaknightStatusUniqProcessFinalHitWait::initStatus(soModuleAccesser* moduleAccesser) {
    for (int id = 0x20000003; id < 0x2000000a; id++) {
        moduleAccesser->getWorkManageModule().setInt(-1, id);
    }
    int entryIds[12];
    // HYPOTHESIS: the argument of enumEntryId is really the array that receives the entry ids.
    int entryCount = g_ftManager->enumEntryId(reinterpret_cast<int>(entryIds));
    int ownEntryId = moduleAccesser->getWorkManageModule().getInt(0x10000000);
    for (int i = 0; i < entryCount; i++) {
        if (ownEntryId != entryIds[i] && isFinalTarget(entryIds[i], moduleAccesser) != true) {
            int workId = getFreeAttackGalleryStatusWorkId(moduleAccesser);
            if (workId < 0) {
                break;
            }
            moduleAccesser->getWorkManageModule().setInt(entryIds[i], workId);
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        }
    }
    ftMetaknightStatusUniqProcessFinalUtil::correctCameraRange(moduleAccesser);
}

void ftMetaknightStatusUniqProcessFinalHitWait::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftMetaknightStatusUniqProcessFinalHitWait::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x121 && nextStatus != 0x120) {
        ftMetaknightStatusUniqProcessFinalUtil::notifyMetaknightFinalEnd(moduleAccesser);
        ftMetaknightStatusUniqProcessFinalUtil::unLinkFinalTargetAll(moduleAccesser);
    }
}

int ftMetaknightStatusUniqProcessFinalHitWait::getFreeAttackGalleryStatusWorkId(soModuleAccesser* moduleAccesser) {
    for (int id = 0x20000003; id < 0x2000000a; id++) {
        if (moduleAccesser->getWorkManageModule().getInt(id) == -1) {
            return id;
        }
    }
    return -1;
}

bool ftMetaknightStatusUniqProcessFinalHitWait::isFinalTarget(int entryId, soModuleAccesser* moduleAccesser) {
    Fighter* subFighter = g_ftManager->getSubFighter(entryId);
    u32 subTaskId = -1;
    if (subFighter != NULL) {
        subTaskId = subFighter->m_taskId;
    }
    for (int i = 6; i < 12; i++) {
        if (moduleAccesser->getLinkModule().isLink(i) == 1) {
            int parentTaskId = moduleAccesser->getLinkModule().getParentTaskId(i);
            if (subTaskId == parentTaskId) {
                return false;
            }
            if (g_ftManager->getEntryIdFromTaskId(parentTaskId, NULL) == entryId) {
                return true;
            }
        }
    }
    return false;
}

void ftMetaknightStatusUniqProcessFinalUtil::notifyMetaknightFinalEnd(soModuleAccesser* moduleAccesser) {
    for (int i = 6; i < 12; i++) {
        if (moduleAccesser->getLinkModule().isLink(i) == 1) {
            ftMetaknightFinalLinkEvent event(0x44e);
            moduleAccesser->getLinkModule().sendEventParents(i, event);
        }
    }
}

void ftMetaknightStatusUniqProcessFinalUtil::unLinkFinalTargetAll(soModuleAccesser* moduleAccesser) {
    for (int i = 6; i < 12; i++) {
        if (moduleAccesser->getLinkModule().isLink(i) == 1) {
            moduleAccesser->getLinkModule().unlink(i);
        }
    }
}

void ftMetaknightStatusUniqProcessFinalUtil::correctCameraRange(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        return;
    }
    Rect2D range;
    moduleAccesser->getCameraModule().getCameraRangeGlobalRect(&range, 0);
    for (int i = 6; i < 12; i++) {
        StageObject* parent = moduleAccesser->getLinkModule().getParent(i);
        if (parent != NULL) {
            Rect2D parentRange;
            soExternalValueAccesser::getCameraModule(parent)->getCameraRangeGlobalRect(&parentRange, 0);
            range = sumRect(&range, &parentRange);
        }
    }
    moduleAccesser->getCameraModule().setOffsetGlobalRectCenter(&range, moduleAccesser, 0);
    moduleAccesser->getCameraModule().setCameraRangeGlobalRect(&range, moduleAccesser, 0);
}

Rect2D ftMetaknightStatusUniqProcessFinalUtil::sumRect(const Rect2D* a, const Rect2D* b) {
    Rect2D rect = *a;
    if (rect.m_up < b->m_up) {
        rect.m_up = b->m_up;
    }
    if (rect.m_down > b->m_down) {
        rect.m_down = b->m_down;
    }
    if (rect.m_left > b->m_left) {
        rect.m_left = b->m_left;
    }
    if (rect.m_right < b->m_right) {
        rect.m_right = b->m_right;
    }
    return rect;
}

ftMetaknightStatusUniqProcessFinalHitWait g_ftMetaknightStatusUniqProcessFinalHitWait;
