#include <ft/ft_entry.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <types.h>

// HYPOTHESIS: the shared smash attack initStatus (sora_melee REL, map name ftStatusUniqProcessSmashAttack__initStatus,
// fn_27_169554) is called non-virtually by the derived class; called by address symbol (not declared in the headers).
extern "C" void fn_27_169554(void* self, soModuleAccesser* moduleAccesser);

class ftNessStatusUniqProcessAttackHi4 : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessAttackHi4() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

struct ftNessAttackHi4LinkEvent : public soLinkEventArgs {
    float m_value;
    ftNessAttackHi4LinkEvent(int eventKind, float value) : soLinkEventArgs(eventKind), m_value(value) { }
};

void ftNessStatusUniqProcessAttackHi4::initStatus(soModuleAccesser* moduleAccesser) {
    fn_27_169554(this, moduleAccesser);
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000003)) {
        float value = moduleAccesser->getCollisionAttackModule().getPowerMulStatus();
        ftNessAttackHi4LinkEvent event(0x456, value);
        moduleAccesser->getLinkModule().sendEventParents(7, event);
    }
}

ftNessStatusUniqProcessAttackHi4 g_ftNessStatusUniqProcessAttackHi4;
