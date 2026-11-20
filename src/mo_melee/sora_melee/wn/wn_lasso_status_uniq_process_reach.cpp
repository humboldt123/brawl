#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_SCALE_FRESH_REGS
#define MT_VEC3F_ADD_OPERAND_ORDER_REGS
#define MT_VEC3F_SUB_FRESH_REGS_RESULT_FIRST
#define MT_VEC3F_OPERATOR_SUB_FRESH_REGS
#include <gf/gf_task.h>
#include <mt/mt_common.h>
#include <mt/mt_vector.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/weapon.h>
#include <wn/wn_lasso_status_uniq_process_reach.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

wnLassoStatusUniqProcessReach g_wnLassoStatusUniqProcessReach;

// Asks the parents where the lasso should reach to; the answer comes back in m_pos.
class wnLassoReachPosEvent : public soLinkEventArgs {
public:
    Vec3f m_pos;
    wnLassoReachPosEvent(int eventKind, const Vec3f& pos) : soLinkEventArgs(eventKind), m_pos(pos) { }
};

// Asks the parents whether the lasso starts out attached; the answer names the node to follow.
class wnLassoReachAttachEvent : public soLinkEventArgs {
public:
    int m_node;
    wnLassoReachAttachEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

class wnLassoReachNoticeEvent : public soLinkEventArgs {
public:
    wnLassoReachNoticeEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

inline soMotionChangeParam::soMotionChangeParam(int kind, float frame, float rate, u8 a, u8 b, bool c, u8 d) :
    m_kind(kind), m_frame(frame), m_rate(rate), _12(a), _13(b), _14(c), _15(d) { }

// The reach missed: stop the chain and move on to the status stored for a miss, if there is one.
static inline void wnLassoReachMiss(soModuleAccesser* moduleAccesser, soWorkManageModule& workManage) {
    wnLassoGetAltStatus(moduleAccesser)->unkA4Int(1);
    workManage.onFlag(0x22000001);
    int nextStatus = workManage.getInt(0x20000001);
    if (nextStatus != -1) {
        wnLassoReachNoticeEvent missEvent(0x83e);
        moduleAccesser->getLinkModule().sendEventParents(3, missEvent);
        moduleAccesser->getStatusModule().changeStatusRequest(nextStatus, moduleAccesser);
    }
}

// The lasso is fully extended: raise the end flag and tell the parents.
static inline void wnLassoReachEnd(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().onFlag(0x22000000);
    wnLassoReachNoticeEvent endEvent(0x83d);
    moduleAccesser->getLinkModule().sendEventParents(3, endEvent);
}

static inline float wnLassoReachDistance(Vec3f& from, Vec3f& to) {
    Vec3f v = from - to;
    float lengthSq = v.m_x * v.m_x + v.m_y * v.m_y + v.m_z * v.m_z;
    double absolute = __fabs(lengthSq);
    float magnitude = absolute;
    if (magnitude <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSq * rsqrtf(lengthSq);
}

void wnLassoStatusUniqProcessReach::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soLinkModule& link = moduleAccesser->getLinkModule();
    moduleAccesser->getMotionModule().changeMotionRequest(&soMotionChangeParam(-1, 0.0f, 1.0f, 0, 0, 0, 0));
    wnLassoReachAttachEvent attachEvent(0x83a);
    link.sendEventParents(3, attachEvent);
    if (attachEvent._0x4) {
        link.removeModelConstraint(true);
        link.setModelConstraintPosOrt(3, workManage.getInt(0x10000006), attachEvent.m_node, 3, true);
        workManage.onFlag(0x22000005);
    } else {
        workManage.offFlag(0x22000005);
    }
    workManage.offFlag(0x12000003);
    wnLassoReachPosEvent posEvent(0x836, Vec3f(0.0f, 0.0f, 0.0f));
    link.sendEventParents(3, posEvent);
    Vec3f start = moduleAccesser->getLinkModule().getModelConstraintTargetNodePosition();
    Vec3f target = posEvent.m_pos;
    Vec3f dir;
    Vec3fSub(&dir, &target, &start);
    dir.normalize();
    target = start + dir;
    wnLassoGetAltStatus(moduleAccesser)->unkA4Int(7);
    wnLassoGetAltStatus(moduleAccesser)->unk68VecPair(&start, &target);
    workManage.onFlag(0x22000004);
}

void wnLassoStatusUniqProcessReach::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000005)) {
        if (moduleAccesser->getWorkManageModule().getInt(0x20000001) == kind) {
            moduleAccesser->getLinkModule().removeModelConstraint(true);
            dynamic_cast<Weapon&>(moduleAccesser->getStageObject()).constraint(-1, -1, -1);
        }
    }
}

void wnLassoStatusUniqProcessReach::execStatus(soModuleAccesser* moduleAccesser) {
    if (wnLassoGetAltStatus(moduleAccesser)->unkD4Int() != 0) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000000);
    }
}

void wnLassoStatusUniqProcessReach::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soLinkModule* const link = &moduleAccesser->getLinkModule();
    wnLassoAltStatusView* const alt = wnLassoGetAltStatus(moduleAccesser);
    wnLassoReachPosEvent posEvent(0x836, Vec3f(0.0f, 0.0f, 0.0f));
    link->sendEventParents(3, posEvent);
    if (workManage.isFlag(0x22000002)) {
        if (posEvent._0x4) {
            int nextStatus = workManage.getInt(0x20000000);
            moduleAccesser->getStatusModule().changeStatusRequest(nextStatus, moduleAccesser);
        } else {
            wnLassoReachMiss(moduleAccesser, workManage);
        }
        workManage.offFlag(0x22000002);
        return;
    }
    if (workManage.isFlag(0x22000001)) {
        return;
    }
    if (workManage.isFlag(0x22000000)) {
        wnLassoReachMiss(moduleAccesser, workManage);
        return;
    }
    Vec3f start = link->getModelConstraintTargetNodePosition();
    Vec3f target = posEvent.m_pos;
    float distance = wnLassoReachDistance(start, target);
    float length = workManage.getFloat(0x21000000);
    float maxLength = moduleAccesser->getPostureModule().getScale();
    float reach = workManage.getFloat(0x21000001);
    maxLength = reach * maxLength;
    int holdCount = workManage.getInt(0x20000002);
    int last = alt->unk70Int() - 1;
    if (alt->unk88Int() < last || workManage.isFlag(0x22000003) == true) {
        length += workManage.getFloat(0x21000002);
    }
    if (length > maxLength) {
        length = maxLength;
    }
    if (length >= distance) {
        length = distance;
        StageObject* owner = &dynamic_cast<StageObject&>(*gfTask::getTask(link->getParentTaskId(3)));
        if (soExternalValueAccesser::getWorkInt(owner, 0x1000003e) >= 3 ||
            soExternalValueAccesser::getWorkFlag(owner, 0x22000014) ||
            soExternalValueAccesser::getWorkFlag(owner, 0x22000015) || !posEvent._0x4) {
            wnLassoReachEnd(moduleAccesser);
        } else {
            workManage.onFlag(0x22000002);
        }
    } else if (workManage.isFlag(0x22000004)) {
        workManage.offFlag(0x22000004);
        if (workManage.isFlag(0x22000003)) {
            alt->unkD0Int(1);
        }
    } else if (workManage.isFlag(0x22000003)) {
        if (length >= maxLength) {
            wnLassoReachEnd(moduleAccesser);
        }
    } else if (length >= maxLength || (holdCount > 0 && alt->unk88Int() >= holdCount) || alt->unk88Int() >= last) {
        wnLassoReachEnd(moduleAccesser);
    }
    float slack = 0.0f;
    if (!workManage.isFlag(0x22000006)) {
        slack = alt->unk74Float(last);
    }
    Vec3f dir;
    Vec3fSub(&dir, &target, &start);
    dir.normalize();
    target = dir * (length + slack) + start;
    wnLassoGetAltStatus(moduleAccesser)->unk68VecPair(&start, &target);
    workManage.setFloat(length, 0x21000000);
}
