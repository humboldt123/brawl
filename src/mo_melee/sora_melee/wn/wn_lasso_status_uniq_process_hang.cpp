#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <gf/gf_task_scheduler.h>
#include <mt/mt_vector.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

class wnLassoStatusUniqProcessHang : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessHang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessHang g_wnLassoStatusUniqProcessHang;

class wnLassoHangLinkEvent : public soLinkEventArgs {
public:
    Vec3f m_pos;
    wnLassoHangLinkEvent(int eventKind, const Vec3f& pos) : soLinkEventArgs(eventKind), m_pos(pos) { }
};

class wnLassoHangValueEvent : public soLinkEventArgs {
public:
    int m_value;
    wnLassoHangValueEvent(int eventKind, int value) : soLinkEventArgs(eventKind), m_value(value) { }
};

void wnLassoStatusUniqProcessHang::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    wnLassoLinkView* link = wnLassoGetLink(moduleAccesser);
    wnLassoAltModelView* model = wnLassoGetAltModel(moduleAccesser);
    soPostureModule& posture = moduleAccesser->getPostureModule();
    wnLassoAltStatusView* alt = wnLassoGetAltStatus(moduleAccesser);
    workManage.onFlag(0x12000000);
    link->unkA8Float(3);
    alt->unkE0Int();
    // HYPOTHESIS: the two float results feed the model view and the posture owner scale.
    float modelValue = link->unk188Float();
    float scaleValue = link->unk88Float();
    link->unk164Int(1);
    model->unkE4Float(modelValue);
    posture.setOwnerScale(scaleValue);
    link->unk194Int(3, 2, false);
    int count = alt->unk88Int();
    if (count > alt->unk70Int()) {
        count = alt->unk70Int();
    }
    alt->unkA4Int(2);
    alt->unk30Int(1);
    wnLassoHangValueEvent endEvent(0x83b, alt->unkC8Int(count));
    link->getLinkModule()->sendEventParents(1, endEvent);
    // HYPOTHESIS: the stage object's +0x28 field is the task id of the fighter that owns the lasso.
    gfTaskScheduler::getInstance()->changeTaskPriorityRequest(moduleAccesser->getStageObject().m_taskId, 5);
    workManage.onFlag(0x12000002);
}

void wnLassoStatusUniqProcessHang::execStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: event 0x836 carries a zero position and reports back whether the link should be drawn.
    wnLassoHangLinkEvent hangEvent(0x836, Vec3f(0.0f, 0.0f, 0.0f));
    moduleAccesser->getLinkModule().sendEventParents(3, hangEvent);
    if (hangEvent._0x4) {
        wnLassoGetAltStatus(moduleAccesser)->unkCCVec(0, &hangEvent.m_pos);
    }
}
