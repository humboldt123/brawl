#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_views.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftSwallowedLinkEvent : public soLinkEventArgs {
public:
    ftSwallowedLinkEvent(int eventKind) : soLinkEventArgs(eventKind) {}
};

class ftStatusUniqProcessSwallowed : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwallowed() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessSwallowed g_ftStatusUniqProcessSwallowed;

void ftStatusUniqProcessSwallowed::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSwallowed::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessSwallowed::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        ftSwallowedLinkEvent event(0x14);
        moduleAccesser->getLinkModule().sendEventParents(0, event);
        moduleAccesser->getStatusModule().changeStatusRequest(0xd1, moduleAccesser);
    }
}

void ftStatusUniqProcessSwallowed::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    moduleAccesser->getControllerModule().stopRumbleKind(2, 4);
    switch (nextStatus) {
    case 0xd1:
        return;
    default:
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
        break;
    }
}

bool ftStatusUniqProcessSwallowed::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    if (moduleAccesser->m_enumerationStart->m_damageModule->isCaptureCut() == true) {
        return false;
    }
    if (reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotC8(0) == 1) {
        return false;
    }
    return !moduleAccesser->getWorkManageModule().isFlag(0x22000014);
}
