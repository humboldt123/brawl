#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_BIKE_UTILITY_OPERATORS
#include <wn/wn_module_accesser_builder.h>
#include <wn/ness/wn_ness_yoyo_head.h>
#include <wn/ness/wn_ness_yoyo_head_utility.h>
#include <mt/mt_vector.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_lasso_status_view.h>

// The yo-yo head being taken up again: it eases from where it is to the owner's node. HYPOTHESIS: all work variable meanings.
class wnNessYoyoHeadStatusUniqProcessTakeup : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoHeadStatusUniqProcessTakeup() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoHeadStatusUniqProcessTakeup g_wnNessYoyoHeadStatusUniqProcessTakeup;

void wnNessYoyoHeadStatusUniqProcessTakeup::initStatus(soModuleAccesser* moduleAccesser) {
    const wnNessYoyoHeadParam* param = dynamic_cast<wnNessYoyoHead&>(moduleAccesser->getStageObject()).m_param;
    moduleAccesser->getWorkManageModule().setFloat(param->unk0, 0x21000002);
}

void wnNessYoyoHeadStatusUniqProcessTakeup::execStatus(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    wnLassoAltModelView* model = (wnLassoAltModelView*)modules->m_modelModule;
    soPostureModule& posture = *modules->m_postureModule;
    Vec3f target = ((wnLassoLinkView*)modules->m_linkModule)->unk98NodePos(3, 0x42, true);
    Vec3f current = posture.getPos();
    Vec3f pos;
    if (work.getInt(0x20000002) < 10) {
        // The head eases to the node over ten frames.
        float t = (float)work.getInt(0x20000002) * 0.1f;
        if (t > 1.0f) {
            t = 1.0f;
        } else if (t < 0.0f) {
            t = 0.0f;
        }
        pos = current * (1.0f - t) + target * t;
    } else {
        pos = target;
    }
    posture.setPos(&pos);
    float first = work.getFloat(0x21000001);
    float second = work.getFloat(0x21000002);
    float value = first + second;
    model->unk70FloatInt(value, 2);
    work.setFloat(value, 0x21000001);
    if (work.isFlag(0x22000002)) {
        work.offFlag(0x22000002);
        work.setFloat(0.0f, 0x21000002);
    }
    work.incInt(0x20000002);
}

void wnNessYoyoHeadStatusUniqProcessTakeup::execFixPos(soModuleAccesser* moduleAccesser) {
    wnNessYoyoHeadFixGroundPos(moduleAccesser);
}
