#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/ness/wn_ness_yoyo_head.h>
#include <wn/ness/wn_ness_yoyo_head_utility.h>
#include <mt/mt_vector.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <wn/wn_lasso_status_view.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <math.h>
#include <so/link/so_link_event_presenter.h>

// The yo-yo head flying out: it follows the owner's node, turned by the angle the owner's arm event reports. HYPOTHESIS: all work
// variable meanings.
class wnNessYoyoHeadStatusUniqProcessShot : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoHeadStatusUniqProcessShot() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoHeadStatusUniqProcessShot g_wnNessYoyoHeadStatusUniqProcessShot;

// HYPOTHESIS: the event (0x840) asks the owner for the direction of the throwing arm; the owner answers by setting the flag.
struct wnNessYoyoHeadAimEvent : soLinkEventArgs {
    Vec3f unk8;
    wnNessYoyoHeadAimEvent(const Vec3f& dir) : soLinkEventArgs(0x840), unk8(dir) { }
};

void wnNessYoyoHeadStatusUniqProcessShot::execStatus(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    wnLassoAltModelView* model = (wnLassoAltModelView*)modules->m_modelModule;
    soPostureModule& posture = *modules->m_postureModule;
    wnLassoLinkView* link = (wnLassoLinkView*)modules->m_linkModule;
    Vec3f nodePos = link->unk98NodePos(3, 0x42, false);
    Vec3f zero(0.0f, 0.0f, 0.0f);
    wnNessYoyoHeadAimEvent event(zero);
    link->unk44SendEvent(3, event);
    if (event._0x4) {
        float angle = atan2(event.unk8.m_x, event.unk8.m_y);
        if (angle != 0.0f) {
            Vec3f hand = link->unk98NodePos(3, 5, false);
            nodePos -= hand;
            Vec3f axis(0.0f, 0.0f, 1.0f);
            nodePos.rot(&axis, -angle, &nodePos);
            nodePos += hand;
        }
    }
    posture.setPos(&nodePos);
    float first = work.getFloat(0x21000001);
    float second = work.getFloat(0x21000002);
    float value = first + second;
    model->unk70FloatInt(value, 2);
    work.setFloat(value, 0x21000001);
    if (work.isFlag(0x22000001)) {
        work.setFloat(dynamic_cast<wnNessYoyoHead&>(moduleAccesser->getStageObject()).m_param->unk0, 0x21000002);
    }
}

void wnNessYoyoHeadStatusUniqProcessShot::execFixPos(soModuleAccesser* moduleAccesser) {
    wnNessYoyoHeadFixGroundPos(moduleAccesser);
}
