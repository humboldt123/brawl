#define WN_BUILD_DATA_INLINE_QUERIES
#define MT_VEC2F_FROM_VEC3_CTOR_NOINLINE
#include <wn/pikmin/wn_pikmin_pikmin_module_accesser_build_data.h>
#include <gf/gf_task_scheduler.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <types.h>

void* wnPikminPikminModuleAccesserBuildData::getAnimCmdData(u32 eventThread, s32 p2) const {
    if (p2 == 1) {
        switch (eventThread) {
        case 0:
            return const_cast<void*>(m_animCmdData);
        }
    }
    return wnModuleAccesserBuildData::getAnimCmdData(eventThread, p2);
}

bool wnPikminPikminUtility::isOwnerConditionGround(int condition) {
    bool result;
    switch (condition) {
    case 0:
    case 1:
    case 3:
        result = true;
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void wnPikminPikminUtility::tryAttachGround(soModuleAccesser* acc) {
    // MATCH-ONLY: the goto reproduces the branch layout of an inlined isOwnerConditionGround.
    switch (acc->getWorkManageModule().getInt(0x1000000F)) {
    case 0:
    case 1:
    case 3:
        goto attach;
    default:
        return;
    }
attach:
    bool rising = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_y > 0.0f;
    if (!rising) {
        if (acc->getGroundModule().attachGround(0) == true) {
            acc->getSituationModule().setKind(Situation_Ground, false);
            acc->getStatusModule().changeStatusRequest(9, acc);
        }
    }
}

float wnPikminPikminUtility::getTargetDist(soModuleAccesser* acc, int mode) {
    Vec2f target(acc->getWorkManageModule().getFloat(0x11000004), acc->getWorkManageModule().getFloat(0x11000005));
    return getDist(&target, &Vec2f(acc->getPostureModule().getPos()), mode);
}

float wnPikminPikminUtility::getOwnerDist(soModuleAccesser* acc, int mode) {
    u32 ownerId = acc->getWorkManageModule().getInt(0x10000000);
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, ownerId);
    StageObject* owner = dynamic_cast<StageObject*>(task);
    float result;
    if (owner == NULL) {
        result = 0.0f;
    } else {
        Vec2f ownerPos(soExternalValueAccesser::getPos(owner));
        result = getDist(&ownerPos, &Vec2f(acc->getPostureModule().getPos()), mode);
    }
    return result;
}

float wnPikminPikminUtility::getDist(Vec2f* from, Vec2f* to, int mode) {
    float result = 0.0f;
    switch (mode) {
    case 0:
        result = __fabs(to->m_x - from->m_x);
        break;
    case 1:
        result = from->m_y - to->m_y;
        break;
    case 2:
        result = to->m_y - from->m_y;
        break;
    case 3: {
        result = (*to - *from).length();
        break;
    }
    }
    return result;
}
