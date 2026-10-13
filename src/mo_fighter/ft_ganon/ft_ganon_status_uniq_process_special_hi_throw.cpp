#include <so/so_module_accesser.h>
#include <ft/ganon/ft_ganon_status_uniq_process_special_hi.h>
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Link event payload for the up-B throw. The receiver may set the four work values after the send (HYPOTHESIS names).
struct ftGanonSpecialHiThrowLinkEvent : soLinkEventArgs {
    s32 m_unk8;
    s32 m_unkC;
    s32 m_unk10;
    s32 m_unk14;
    float m_unk18;
    bool m_unk1C;
    ftGanonSpecialHiThrowLinkEvent(int kind) : soLinkEventArgs(kind), m_unk8(-1), m_unkC(-1), m_unk10(-1), m_unk14(-1), m_unk18(1.0f), m_unk1C(true) { }
};

class ftGanonStatusUniqProcessSpecialHiThrow : public ftGanonStatusUniqProcessSpecialHiCling {
public:
    virtual ~ftGanonStatusUniqProcessSpecialHiThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftGanonStatusUniqProcessSpecialHiThrow::initStatus(soModuleAccesser* moduleAccesser) {
    ftGanonSpecialHiThrowLinkEvent event(0xb);
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(event.m_unkC, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(event.m_unk10, 0x20000004);
}

ftGanonStatusUniqProcessSpecialHiThrow g_ftGanonStatusUniqProcessSpecialHiThrow;
