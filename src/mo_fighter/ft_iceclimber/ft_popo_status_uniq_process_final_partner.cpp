#define FT_FIGHTER_ANIMCMD_LONG
#define FT_FIGHTER_GET_PARTNER_VIRTUAL // MATCH-ONLY: Fighter::getPartner is reached through the vtable
#include <ft/fighter.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>


class ftPopoStatusUniqProcessFinalPartner : public soStatusUniqProcess {
    void detachConstraint(soModuleAccesser* moduleAccesser);
    void setSafePosPartner(soModuleAccesser* moduleAccesser);

public:
    virtual ~ftPopoStatusUniqProcessFinalPartner() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessFinalPartner::initStatus(soModuleAccesser* moduleAccesser) {
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
    }
    float partnerLr = soExternalValueAccesser::getLr(partner);
    moduleAccesser->getPostureModule().setLr(partnerLr);
}

// HYPOTHESIS: the per-frame partner tracking is not reconstructed yet; this stub keeps the class shape.
void ftPopoStatusUniqProcessFinalPartner::execFixPos(soModuleAccesser* moduleAccesser) {
}

// HYPOTHESIS: when the link query at slot 0x168 reports the constraint, the partner detaches and re-syncs its
// position; the model call at the end is not reconstructed yet.
void ftPopoStatusUniqProcessFinalPartner::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        detachConstraint(moduleAccesser);
        setSafePosPartner(moduleAccesser);
    }
}

// HYPOTHESIS: detaches the partner link constraint; not reconstructed yet.
void ftPopoStatusUniqProcessFinalPartner::detachConstraint(soModuleAccesser* moduleAccesser) {
}

// HYPOTHESIS: moves the partner to a safe position; not reconstructed yet.
void ftPopoStatusUniqProcessFinalPartner::setSafePosPartner(soModuleAccesser* moduleAccesser) {
}

ftPopoStatusUniqProcessFinalPartner g_ftPopoStatusUniqProcessFinalPartner;
