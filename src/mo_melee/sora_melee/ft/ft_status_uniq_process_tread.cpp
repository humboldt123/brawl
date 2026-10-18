#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_tread.h>
#include <so/link/so_link_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessTread g_ftStatusUniqProcessTread;

// HYPOTHESIS: link event 6 asks the linked parent to take the treading fighter (m_handled set by the parent when it did);
// m_node is the parent's node index and m_pos the position of the fighter / the offset the parent answers with.
struct ftTreadLinkEventArgs : public soLinkEventArgs {
    bool m_handled;
    bool m_unk6;
    int m_node;
    Vec3f m_pos;
    ftTreadLinkEventArgs(const Vec3f& pos) : soLinkEventArgs(6) {
        m_pos.m_z = 1.0f;
        m_handled = false;
        m_node = 0;
        m_pos.m_x = pos.m_x;
        m_pos.m_y = pos.m_y;
    }
};

struct ftTreadLinkEventArgs7 : public soLinkEventArgs {
    ftTreadLinkEventArgs7() : soLinkEventArgs(7) { }
};

void ftStatusUniqProcessTread::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x6D: {
        int parentTaskId = moduleAccesser->getWorkManageModule().getInt(0x10000007);
        if (moduleAccesser->getLinkModule().link(3, parentTaskId)) {
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            ftTreadLinkEventArgs args(pos);
            moduleAccesser->getLinkModule().sendEventParents(3, args);
            if (args.m_handled == true) {
                soMotionChangeParam param;
                param.m_kind = 0x34;
                param.m_frame = 0.0f;
                param.m_rate = 1.0f;
                param._12 = 0;
                param._13 = 0;
                param._14 = 0;
                param._15 = 0;
                moduleAccesser->getMotionModule().changeMotionRequest(&param);
                moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, 0, args.m_node, 5, true);
                moduleAccesser->getLinkModule().setParentAttackerInfo(3, moduleAccesser->m_stageObject);
            } else {
                moduleAccesser->getWorkManageModule().onFlag(0x22000010);
                pos.m_x = args.m_pos.m_x;
                pos.m_y = args.m_pos.m_y;
                moduleAccesser->getPostureModule().setPos(&pos);
            }
            if (args.m_unk6 == 0) {
                moduleAccesser->getWorkManageModule().setInt(0, 0x10000005);
            }
            moduleAccesser->getWorkManageModule().setFloat(args.m_pos.m_z, 0x21000004);
        }
        break;
    }
    case 0x71:
    {
        int frames = (int)soValueAccesser::getConstantFloat(moduleAccesser, 0xBCD, 0);
        moduleAccesser->getWorkManageModule().setInt(frames, 0x20000001);
    }
        break;
    }
}

void ftStatusUniqProcessTread::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x71:
        moduleAccesser->getWorkManageModule().countDownInt(0x20000001, 0);
        break;
    }
}

void ftStatusUniqProcessTread::exitStatus(soModuleAccesser* moduleAccesser, int) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x6D:
        removeConstraint(moduleAccesser);
        break;
    case 0x6E:
    case 0x70: {
        ftTreadLinkEventArgs7 args;
        moduleAccesser->getLinkModule().sendEventNodes(3, args, 0);
        break;
    }
    case 0x6F:
    case 0x71:
        break;
    }
}

void ftStatusUniqProcessTread::removeConstraint(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLink(3) == 1) {
        if (moduleAccesser->getLinkModule().isModelConstraint() == 1) {
            moduleAccesser->getLinkModule().adjustModelConstraintPosture(reinterpret_cast<Matrix*>(&Vec3f(0.0f, 0.0f, 0.0f)), &Vec3f(0.0f, 0.0f, 0.0f));
            moduleAccesser->getLinkModule().removeModelConstraint(true);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            pos.m_z = 0.0f;
            moduleAccesser->getPostureModule().setPos(&pos);
        }
        moduleAccesser->getLinkModule().unlink(3);
    }
}
