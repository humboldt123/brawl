// The catch module is a void* in the accesser.
#include <so/catch/so_catch_module.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftKirbyStatusUniqProcessSpecialNSwallow : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessSpecialNSwallow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// Link event with only the base fields; the event kind is passed in.
struct ftKirbySwallowLinkEvent : public soLinkEventArgs {
    ftKirbySwallowLinkEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

void ftKirbyStatusUniqProcessSpecialNSwallow::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x119:
        reinterpret_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->setCatch(-1);
        break;
    case 0x11a:
        moduleAccesser->getCollisionHitModule().setXluGlobal(0);
        break;
    }
}

void ftKirbyStatusUniqProcessSpecialNSwallow::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000019)) {
        ftKirbySwallowLinkEvent event(0x36);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().offFlag(0x22000019);
    }
}

void ftKirbyStatusUniqProcessSpecialNSwallow::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    switch (status) {
    case 0x11b:
    case 0x121:
        if (moduleAccesser->getLinkModule().isLinked(0) == true) {
            ftKirbySwallowLinkEvent event(0x13);
            moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        }
        break;
    case 0x11a:
        break;
    default:
        if (moduleAccesser->getLinkModule().isLinked(0) == true) {
            reinterpret_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, true);
        }
        moduleAccesser->getCollisionHitModule().cancelXluGlobal(0);
        break;
    }
}

ftKirbyStatusUniqProcessSpecialNSwallow g_ftKirbyStatusUniqProcessSpecialNSwallow;
