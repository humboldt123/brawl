#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_catch_module_view.h>
#include <ft/ft_link_event_throw.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessThrow : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessThrow g_ftStatusUniqProcessThrow;

// Starts a throw: tells the thrown fighter (the link node) about the throw through a link event, stores the node's task ID and
// the event's outputs in the work variables, and gives the thrower the invincibility frames of the throw.
void ftStatusUniqProcessThrow::initStatus(soModuleAccesser* moduleAccesser) {
    int throwIndex = moduleAccesser->getWorkManageModule().getInt(0x20000000) - 0x72;
    ftLinkEventThrow event(0xB, throwIndex + 0x76, ((1 << throwIndex) & soValueAccesser::getConstantInt(moduleAccesser, 0x59F2, 0)) != 0);
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(event.m_output0, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(event.m_output1, 0x20000004);
    moduleAccesser->getWorkManageModule().setFloat(event.m_output2, 0x21000004);
    moduleAccesser->getCollisionHitModule().setInvincibleFrameGlobal(soValueAccesser::getConstantInt(moduleAccesser, 0x5A2E, 0), false, 0);
}

void ftStatusUniqProcessThrow::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    if (kind == 0x3b) {
        return;
    }
    if (kind >= 0x3b || kind != 0) {
        reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->notifyThrowExit(0, 0);
    }
}

bool ftStatusUniqProcessThrow::checkDamage(soModuleAccesser* moduleAccesser, void* damageInfo) {
    return reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->checkThrowDamage(moduleAccesser, damageInfo);
}
