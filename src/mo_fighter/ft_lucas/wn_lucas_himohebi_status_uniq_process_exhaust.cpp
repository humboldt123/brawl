#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// The lasso exhaust status of the sora_melee module (wn_lasso_status_uniq_process_exhaust.cpp).
class wnLassoStatusUniqProcessExhaust : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessExhaust() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

// HYPOTHESIS: the link event 0x83a asks the grabbed fighter (the parent) for the node its model is attached to.
struct wnLucasHimohebiLinkEvent : soLinkEventArgs {
    int unk8;
    wnLucasHimohebiLinkEvent() : soLinkEventArgs(0x83a) { }
};

class wnLucasHimohebiStatusUniqProcessExhaust : public wnLassoStatusUniqProcessExhaust {
public:
    virtual ~wnLucasHimohebiStatusUniqProcessExhaust() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnLucasHimohebiStatusUniqProcessExhaust g_wnLucasHimohebiStatusUniqProcessExhaust;

void wnLucasHimohebiStatusUniqProcessExhaust::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soLinkModule& link = moduleAccesser->getLinkModule();
    wnLassoStatusUniqProcessExhaust::initStatus(moduleAccesser);
    wnLucasHimohebiLinkEvent event;
    link.sendEventParents(3, event);
    if (event._0x4) {
        link.removeModelConstraint(true);
        link.setModelConstraintPosOrt(3, work.getInt(0x10000006), event.unk8, 3, true);
    }
}
