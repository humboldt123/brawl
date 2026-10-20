#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/ness/wn_ness_yoyo.h>
#include <mt/mt_vector.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_lasso_status_view.h>

// HYPOTHESIS: a data word of the REL (the node or color the string model is attached with).
extern u32 lbl_101_data_B5D0;

extern soStatusUniqProcess g_wnNessYoyoStatusUniqProcessShot;
extern soStatusUniqProcess g_wnNessYoyoStatusUniqProcessShotFall;

// HYPOTHESIS: the event the owner sends to hand the string the task id of the yo-yo head to link and a work value (0x455).
struct wnNessYoyoOwnerLinkEvent : soLinkEventArgs {
    int unk8;
    int unkC;
    wnNessYoyoOwnerLinkEvent() : soLinkEventArgs(0x455) { }
};

wnNessYoyo::wnNessYoyo(s32 articleId, const wnArticleBuildInfo& info, const wnNessYoyoData* data) :
    wnWeaponBuilder<wnNessYoyoModuleAccesserBuildConfig>(
        "NessYoyo", articleId, info,
        wnModuleAccesserBuildData(107, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[3];
    uniqProcs[0] = &g_wnNessYoyoStatusUniqProcessShot;
    uniqProcs[1] = &g_wnNessYoyoStatusUniqProcessShotFall;
    uniqProcs[2] = &g_wnNessYoyoStatusUniqProcessShot;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 3);
    Weapon::deactivate(false);
}

void wnNessYoyo::activate(int founderTaskId, int kind, int team, bool flag) {
    Vec3f zero(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = kind;
    desc.unk8 = kind;
    desc.unkC = kind;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    __memcpy(&desc.pos, &zero, sizeof(Vec3f));
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    wnLassoAltStatusView* alt = wnLassoGetAltStatus(m_moduleAccesser);
    u32 nodeId = lbl_101_data_B5D0;
    int noValue = 0;
    alt->unk18Attach(m_moduleAccesser->getResourceModule().getResourceIdAccesser()->getBinResId(), noValue, nodeId, 0, nodeId, 0, noValue, -1);
    alt->unkDCFloat(m_moduleAccesser->getPostureModule().getScale());
    alt->unk60Int(2);
    m_moduleAccesser->getPostureModule().setLr(m_moduleAccesser->getLinkModule().getParentLr(3));
    m_moduleAccesser->getWorkManageModule().setFlag(flag, 0x22000000);
}

void wnNessYoyo::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc, StageObject* sender, int unk4) {
    if (sender->soGetKind() == 0 && sender->soGetSubKind() == 10) {
        if (event->m_eventKind == 0x455) {
            wnNessYoyoOwnerLinkEvent* owner = (wnNessYoyoOwnerLinkEvent*)event;
            soWorkManageModule& work = acc->getWorkManageModule();
            soLinkModule& link = acc->getLinkModule();
            if (link.isLink(5) == false) {
                link.link(5, owner->unk8);
                work.setInt(owner->unkC, 0x20000000);
            }
            return;
        }
    } else if (sender->soGetKind() == 2 && sender->soGetSubKind() == 0x6c) {
        if (event->m_eventKind == 0x83f) {
            wnNessYoyoHeadLinkEvent* head = (wnNessYoyoHeadLinkEvent*)event;
            acc->getStopModule().setHitStopFrame(head->unk8, head->unkC);
            return;
        }
    }
    Weapon::notifyEventLink(event, acc, sender, unk4);
}
