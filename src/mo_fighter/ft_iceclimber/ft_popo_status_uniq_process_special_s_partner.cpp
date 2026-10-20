#define FT_FIGHTER_ANIMCMD_LONG
#define FT_FIGHTER_GET_PARTNER_VIRTUAL // MATCH-ONLY: Fighter::getPartner is reached through the vtable
#include <ft/fighter.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Helpers of the iceclimber REL that are not reconstructed yet.
extern "C" void fn_105_BD50(soMotionChangeParam* param, int kind, float frame, float rate, u8 a, u8 b, u8 c, u8 d);

// Link event carrying only its kind (HYPOTHESIS: kind 0x458 is the popo's partner release event).
struct ftPopoSpecialSPartnerLinkEvent : soLinkEventArgs {
    ftPopoSpecialSPartnerLinkEvent(int kind) : soLinkEventArgs(kind) { }
};


class ftPopoStatusUniqProcessSpecialSPartner : public soStatusUniqProcess {
    void detachConstraint(soModuleAccesser* moduleAccesser);

public:
    virtual ~ftPopoStatusUniqProcessSpecialSPartner() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// UNVERIFIED: placeholder for the float constant lbl_105_rodata_40
static const float s_partnerLrScale = 1.0f;

// Partner-side motion change: the partner switches to the new kind when its motion is the expected kind.
static void changeMotionOnPartner(soModuleAccesser* moduleAccesser, int expectKind, int newKind) {
    // HYPOTHESIS: the default frame and rate are the constants lbl_105_rodata_44 and lbl_105_rodata_48 (unverified)
    float frame = 0.0f;
    u8 haveFrame = 0;
    soMotionModule& motion = moduleAccesser->getMotionModule();
    if (motion.getKind() == expectKind) {
        frame = motion.getFrame();
        haveFrame = 1;
    }
    soMotionChangeParam param;
    fn_105_BD50(&param, newKind, frame, 1.0f, 0, 0, haveFrame, 0);
    motion.changeMotionRequest(&param);
}

void ftPopoStatusUniqProcessSpecialSPartner::initStatus(soModuleAccesser* moduleAccesser) {
    Fighter* self = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
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
    // UNVERIFIED: the partner's facing is scaled by lbl_105_rodata_40; the value is not checked yet
    float partnerLr = soExternalValueAccesser::getLr(partner);
    moduleAccesser->getPostureModule().setLr(s_partnerLrScale * partnerLr);
}

void ftPopoStatusUniqProcessSpecialSPartner::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        changeMotionOnPartner(moduleAccesser, 0x1d7, 0x1d6);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
        changeMotionOnPartner(moduleAccesser, 0x1d6, 0x1d7);
    }
    if (moduleAccesser->getMotionModule().isEnd() == true) {
        if (moduleAccesser->getLinkModule().isLink(6) == 1) {
            moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0), false);
        }
        detachConstraint(moduleAccesser);
        Fighter* self = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
        if (self == 0) {
            return;
        }
        Fighter* partner = self->getPartner();
        if (partner == 0) {
            return;
        }
        if (soExternalValueAccesser::getSituationKind(partner) == 0 &&
            moduleAccesser->getGroundModule().isStCliff(0) == true) {
            moduleAccesser->getSituationModule().setKind(SituationKind(0), false);
            moduleAccesser->getStatusModule().changeStatus(0, moduleAccesser);
        } else {
            moduleAccesser->getSituationModule().setKind(SituationKind(2), false);
            moduleAccesser->getStatusModule().changeStatus(0x10, moduleAccesser);
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

// HYPOTHESIS: detaches the partner's link constraint and re-attaches the posture; the link queries at slots
// 0x168, 0x164 and the model query at 0x98 are not reconstructed yet.
void ftPopoStatusUniqProcessSpecialSPartner::detachConstraint(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xd), true);
    }
    moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xc), false);
}

ftPopoStatusUniqProcessSpecialSPartner g_ftPopoStatusUniqProcessSpecialSPartner;
