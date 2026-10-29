#define FT_FIGHTER_ANIMCMD_LONG
#define FT_FIGHTER_GET_PARTNER_VIRTUAL // MATCH-ONLY: Fighter::getPartner is reached through the vtable
#include <ft/fighter.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/model/so_model_module_simple.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: a local shell of ftPopo deriving Fighter directly; this unit only needs its RTTI and getPartner.
class ftPopo : public Fighter {
public:
    virtual Fighter* getPartner();
};

// Link event carrying only its kind (HYPOTHESIS: kind 0x458 is the popo's partner release event).
struct ftPopoSpecialSPartnerLinkEvent : soLinkEventArgs {
    ftPopoSpecialSPartnerLinkEvent(int kind) : soLinkEventArgs(kind) { }
};


class ftPopoStatusUniqProcessSpecialSPartner : public soStatusUniqProcess {
    static void detachConstraint(soModuleAccesser* moduleAccesser);

public:
    virtual ~ftPopoStatusUniqProcessSpecialSPartner() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessSpecialSPartner::initStatus(soModuleAccesser* moduleAccesser) {
    ftPopo* self = dynamic_cast<ftPopo*>(&moduleAccesser->getStageObject());
    if (self == 0) {
        return;
    }
    Fighter* partner = self->getPartner();
    if (partner == 0) {
        return;
    }
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        moduleAccesser->getLinkModule().setModelConstraintPosOrt(6, 0, 0, 3, true);
        moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0), true);
        moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xc), true);
    }
    moduleAccesser->getPostureModule().setLr(-1.0f * soExternalValueAccesser::getLr(partner));
}

// HYPOTHESIS: the native soMotionChangeParam constructor takes this flag as a bool (the target passes it
// unmasked); with the u8 declaration the call masks it, so this function stays fn_.
void ftPopoStatusUniqProcessSpecialSPartner::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        float frame = 0.0f;
        bool keepFrame = false;
        if (moduleAccesser->getMotionModule().getKind() == 0x1d7) {
            frame = moduleAccesser->getMotionModule().getFrame();
            keepFrame = true;
        }
        soMotionChangeParam param(0x1d6, frame, 1.0f, 0, 0, keepFrame, 0);
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
        float frame = 0.0f;
        bool keepFrame = false;
        if (moduleAccesser->getMotionModule().getKind() == 0x1d6) {
            frame = moduleAccesser->getMotionModule().getFrame();
            keepFrame = true;
        }
        soMotionChangeParam param(0x1d7, frame, 1.0f, 0, 0, keepFrame, 0);
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
    if (moduleAccesser->getMotionModule().isEnd() == true) {
        if (moduleAccesser->getLinkModule().isLink(6) == 1) {
            moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0), false);
        }
        detachConstraint(moduleAccesser);
        ftPopo* self = dynamic_cast<ftPopo*>(&moduleAccesser->getStageObject());
        if (self == 0) {
            return;
        }
        Fighter* partner = self->getPartner();
        if (partner == 0) {
            return;
        }
        if (soExternalValueAccesser::getSituationKind(partner) == 0 &&
            moduleAccesser->getGroundModule().attachGround(0) == true) {
            moduleAccesser->getSituationModule().setKind(SituationKind(0), false);
            moduleAccesser->getStatusModule().changeStatusRequest(0, moduleAccesser);
        } else {
            moduleAccesser->getSituationModule().setKind(SituationKind(2), false);
            moduleAccesser->getStatusModule().changeStatusRequest(0x10, moduleAccesser);
        }
    }
}

void ftPopoStatusUniqProcessSpecialSPartner::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0), false);
    }
    detachConstraint(moduleAccesser);
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        if (nextStatus != 0 && nextStatus != 0x10 && nextStatus != 0x117 && nextStatus != 0x120 && nextStatus != 0x11a) {
            ftPopoSpecialSPartnerLinkEvent event(0x458);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
        }
    }
}

void ftPopoStatusUniqProcessSpecialSPartner::detachConstraint(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isModelConstraint()) {
        Vec3f pos = moduleAccesser->getModelModule().getNodeGlobalPosition(2, false);
        pos.m_z = moduleAccesser->getGroundModule().getZ();
        moduleAccesser->getLinkModule().removeModelConstraint(true);
        moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xd), true);
        moduleAccesser->getPostureModule().setPos(&pos);
        moduleAccesser->getStageObject().updateNodeSRT();
        moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xc), false);
    }
}

ftPopoStatusUniqProcessSpecialSPartner g_ftPopoStatusUniqProcessSpecialSPartner;
