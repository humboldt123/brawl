#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/catch/so_catch_module.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: the events 0x456/0x457 that the Pikmin article sends to its parents when a catch ends (kinds are verified in the
// fighters' event handlers, their meaning is not).
struct wnPikminLinkEvent : soLinkEventArgs {
    wnPikminLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

// HYPOTHESIS: the exit of the catch statuses of a Pikmin (status kinds 0x22-0x29 except 0x24 continue the catch).
class wnPikminPikminStatusUniqProcessCatchCommon : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessCatchCommon() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int next);
};

wnPikminPikminStatusUniqProcessCatchCommon g_wnPikminPikminStatusUniqProcessCatchCommon;

void wnPikminPikminStatusUniqProcessCatchCommon::exitStatus(soModuleAccesser* moduleAccesser, int next) {
    bool continuesCatch;
    switch (next) {
    case 0x22:
    case 0x23:
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
        continuesCatch = true;
        break;
    default:
        continuesCatch = false;
        break;
    }
    if (!continuesCatch) {
        static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
        if (moduleAccesser->getStatusModule().getStatusKind() == 0x21 && (next == 6 || next == 0)) {
            wnPikminLinkEvent event(0x456);
            moduleAccesser->getLinkModule().sendEventParents(3, event);
        } else {
            switch (moduleAccesser->getStatusModule().getStatusKind()) {
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
                if (next != 6 && next != 0) {
                    wnPikminLinkEvent event(0x457);
                    moduleAccesser->getLinkModule().sendEventParents(3, event);
                }
                break;
            default: {
                wnPikminLinkEvent event(0x457);
                moduleAccesser->getLinkModule().sendEventParents(3, event);
                break;
            }
            }
        }
    }
}
