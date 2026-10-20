#define FT_FIGHTER_ANIMCMD_LONG
#define FT_FIGHTER_GET_PARTNER_VIRTUAL // MATCH-ONLY: Fighter::getPartner is reached through the vtable
#include <ft/fighter.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Helpers of the iceclimber REL that are not reconstructed yet.
extern "C" u32 fn_105_C29C(int status);
extern "C" int fn_105_133F8(float);
extern "C" void fn_105_133E4(Vec2f* dst, const Vec3f* src);

// Link event carrying only its kind (HYPOTHESIS: kind 0x45b is the partner's final-smash release).
struct ftPopoFinalLinkEvent : soLinkEventArgs {
    ftPopoFinalLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

// Link event carrying a status kind (HYPOTHESIS names; kinds 0x455 and 0x456 are the popo final-smash events).
struct ftPopoFinalStatusEvent : soLinkEventArgs {
    s32 m_status;
    ftPopoFinalStatusEvent(int kind, int status) : soLinkEventArgs(kind), m_status(status) { }
};

// Link event carrying a state value and a flag (HYPOTHESIS names).
struct ftPopoFinalStateEvent : soLinkEventArgs {
    s32 m_state;
    bool m_flag;
    ftPopoFinalStateEvent(int kind, int state) : soLinkEventArgs(kind), m_state(state), m_flag(false) { }
};


class ftPopoStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    Fighter* self = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
    if (self == 0) {
        return;
    }
    Fighter* partner = self->getPartner();
    if (partner != 0 && fn_105_C29C(soExternalValueAccesser::getStatusKind(partner)) == 1) {
        Vec3f partnerPos = soExternalValueAccesser::getPos(partner);
        Vec2f partnerXY;
        fn_105_133E4(&partnerXY, &partnerPos);
        Vec3f ownPos = moduleAccesser->getPostureModule().getPos();
        Vec2f ownXY;
        fn_105_133E4(&ownXY, &ownPos);
        Vec2f diff = partnerXY - ownXY;
        if (fn_105_133F8(diff.m_x) && fn_105_133F8(diff.m_y)) {
            float sumSq = diff.m_x * diff.m_x + diff.m_y * diff.m_y;
            float dist;
            // HYPOTHESIS: the small-value floor below is a near-zero epsilon and its fallback is 0.0f
            if ((float)__fabs(sumSq) <= 0.0001f) {
                dist = 0.0f;
            } else {
                dist = sumSq * rsqrtf(sumSq);
            }
            if (dist < soValueAccesser::getConstantFloat(moduleAccesser, 0xfdb, 0)) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000010);
                if (moduleAccesser->getLinkModule().isLink(6) == 1) {
                    ftPopoFinalStatusEvent statusEvent(0x455, 0x120);
                    moduleAccesser->getLinkModule().sendEventNodes(6, statusEvent, 0);
                }
                if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
                    moduleAccesser->getSituationModule().setKind(SituationKind(2), false);
                }
                int state = 0x1e5;
                if (moduleAccesser->getSituationModule().getKind() != 0) {
                    state = 0x1e9;
                }
                ftPopoFinalStateEvent stateEvent(0x456, state);
                moduleAccesser->getLinkModule().sendEventNodes(6, stateEvent, 0);
            }
        }
    } else {
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
    }
}

void ftPopoStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) != true) {
        return;
    }
    Fighter* self = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
    if (self == 0) {
        return;
    }
    Fighter* partner = self->getPartner();
    if (partner == 0) {
        return;
    }
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        ftPopoFinalLinkEvent event(0x45b);
        moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
    }
}

ftPopoStatusUniqProcessFinal g_ftPopoStatusUniqProcessFinal;
