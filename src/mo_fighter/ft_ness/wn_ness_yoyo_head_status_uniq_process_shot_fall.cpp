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
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_lasso_status_view.h>

// The yo-yo head falling after being shot out: gravity-like acceleration along the owner's facing. HYPOTHESIS: all work variable
// meanings.
class wnNessYoyoHeadStatusUniqProcessShotFall : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoHeadStatusUniqProcessShotFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoHeadStatusUniqProcessShotFall g_wnNessYoyoHeadStatusUniqProcessShotFall;

void wnNessYoyoHeadStatusUniqProcessShotFall::initStatus(soModuleAccesser* moduleAccesser) {
    const wnNessYoyoHeadParam* param = dynamic_cast<wnNessYoyoHead&>(moduleAccesser->getStageObject()).m_param;
    float lr = moduleAccesser->getPostureModule().getLr();
    Vec2f speed;
    speed.m_x = param->unkC * lr;
    speed.m_y = param->unk18;
    Vec2f accel;
    accel.m_x = param->unk10 * lr;
    accel.m_y = -param->unk1C;
    Vec2f limit;
    limit.m_x = param->unk14;
    limit.m_y = param->unk20;
    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    energy.m_speed = speed;
    energy.m_accel = accel;
    energy.m_speedTarget = limit;
    energy.m_speedLimit = limit;
    // HYPOTHESIS: this bit of the energy's enable word keeps the energy running.
    *((u8*)&energy + 5) |= 0x80;
    moduleAccesser->getStageObject().updateNodeSRT();
}

void wnNessYoyoHeadStatusUniqProcessShotFall::execStatus(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    wnLassoLinkView* link = (wnLassoLinkView*)modules->m_linkModule;
    wnLassoAltModelView* model = (wnLassoAltModelView*)modules->m_modelModule;
    const wnNessYoyoHeadParam* param = dynamic_cast<wnNessYoyoHead&>(moduleAccesser->getStageObject()).m_param;
    if (work.isFlag(0x22000004)) {
        Vec3f nodePos = link->unk98NodePos(5, work.getInt(0x20000001), false);
        moduleAccesser->getPostureModule().setPos(&nodePos);
    }
    float range = param->unk8 - param->unk4;
    float elapsed = moduleAccesser->getMotionModule().getFrame();
    float value = param->unk8 - range * (elapsed / 60.0f);
    work.addFloat(value, 0x21000001);
    work.setFloat(value, 0x21000002);
    model->unk70FloatInt(work.getFloat(0x21000001), 2);
}

void wnNessYoyoHeadStatusUniqProcessShotFall::execFixPos(soModuleAccesser* moduleAccesser) {
    wnNessYoyoHeadFixGroundPos(moduleAccesser);
}
