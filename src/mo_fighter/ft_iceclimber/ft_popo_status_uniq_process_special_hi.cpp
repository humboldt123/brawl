#define FT_FIGHTER_ANIMCMD_LONG
#define FT_FIGHTER_GET_PARTNER_VIRTUAL // MATCH-ONLY: Fighter::getPartner is reached through the vtable
#include <ft/fighter.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Helpers of the iceclimber REL that are not reconstructed yet.
extern "C" u32 fn_105_C29C(int status);
extern "C" int fn_105_133F8(float);
extern "C" void fn_105_133E4(Vec2f* dst, const Vec3f* src);

// Link event carrying a status kind (HYPOTHESIS: 0x455 is the popo special-hi release event).
struct ftPopoSpecialHiStatusEvent : soLinkEventArgs {
    s32 m_status;
    ftPopoSpecialHiStatusEvent(int kind, int status) : soLinkEventArgs(kind), m_status(status) { }
};

// Link event carrying the ledge position the partner is pulled to (HYPOTHESIS names).
struct ftPopoSpecialHiPosEvent : soLinkEventArgs {
    Vec3f m_pos;
    ftPopoSpecialHiPosEvent(int kind, Vec3f pos) : soLinkEventArgs(kind), m_pos(pos) { }
};

// Link event carrying a state value and a flag (HYPOTHESIS names).
struct ftPopoSpecialHiStateEvent : soLinkEventArgs {
    s32 m_state;
    bool m_flag;
    ftPopoSpecialHiStateEvent(int kind, int state) : soLinkEventArgs(kind), m_state(state), m_flag(false) { }
};



class ftPopoStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftPopoStatusUniqProcessSpecialHi::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000012);
        Fighter* self = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
        Fighter* partner = 0;
        if (self != 0) {
            partner = self->getPartner();
        }
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
                if (dist < soValueAccesser::getConstantFloat(moduleAccesser, 0xfbb, 0)) {
                    moduleAccesser->getWorkManageModule().onFlag(0x22000010);
                    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
                        ftPopoSpecialHiStatusEvent statusEvent(0x455, 0x11a);
                        moduleAccesser->getLinkModule().sendEventNodes(6, statusEvent, 0);
                    }
                    int state = 0x1de;
                    if (moduleAccesser->getSituationModule().getKind() != 0) {
                        state = 0x1df;
                    }
                    ftPopoSpecialHiStateEvent stateEvent(0x456, state);
                    moduleAccesser->getLinkModule().sendEventNodes(6, stateEvent, 0);
                }
            }
        }
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000013) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        Fighter* self = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
        if (self == 0) {
            return;
        }
        Fighter* partner = self->getPartner();
        if (partner == 0) {
            return;
        }
        if (soExternalValueAccesser::getStatusKind(partner) != 0x11a) {
            return;
        }
        int nextStatus = 0x11b;
        // HYPOTHESIS: the ground query at slot 0x90 selects the 0x11d branch when it returns true
        soGroundModule& ground = moduleAccesser->getGroundModule();
        if (((bool (*)(soGroundModule*, int))((void**)*(void**)&ground)[0x90 / 4])(&ground, 0)) {
            nextStatus = 0x11d;
        }
        if (moduleAccesser->getLinkModule().isLink(6) == 1) {
            ftPopoSpecialHiStatusEvent statusEvent(0x455, nextStatus);
            moduleAccesser->getLinkModule().sendEventNodes(6, statusEvent, 0);
        }
        if (nextStatus == 0x11d) {
            Vec3f pos = moduleAccesser->getGroundModule().getCliffPos(0);
            ftPopoSpecialHiPosEvent posEvent(0x459, pos);
            moduleAccesser->getLinkModule().sendEventNodes(6, posEvent, 0);
        }
    }
}

void ftPopoStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x119) {
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
    if (soExternalValueAccesser::getStatusKind(partner) != 0x11a) {
        return;
    }
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        ftPopoSpecialHiStatusEvent statusEvent(0x455, 0xe);
        moduleAccesser->getLinkModule().sendEventNodes(6, statusEvent, 0);
    }
}

ftPopoStatusUniqProcessSpecialHi g_ftPopoStatusUniqProcessSpecialHi;
