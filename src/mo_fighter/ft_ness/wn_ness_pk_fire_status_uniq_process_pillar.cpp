#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_STATUS_PLAIN_NULL_ARRAY
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/ness/wn_ness_pk_fire.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// The PK Fire column: the bolt turns into a pillar that grows from the ground (the posture scale goes from a start scale to full
// size over the pillar time). HYPOTHESIS: parameter meanings.
class wnNessPKFireStatusUniqProcessPillar : public soStatusUniqProcess {
public:
    virtual ~wnNessPKFireStatusUniqProcessPillar() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnNessPKFireStatusUniqProcessPillar g_wnNessPKFireStatusUniqProcessPillar;

void wnNessPKFireStatusUniqProcessPillar::initStatus(soModuleAccesser* moduleAccesser) {
    const wnNessPKFireParam* param = dynamic_cast<wnNessPKFire&>(moduleAccesser->getStageObject()).m_param;
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    pos.m_y += param->unk8;
    moduleAccesser->getPostureModule().setPos(&pos);
    moduleAccesser->getCollisionHitModule().setStatus(1, 0, 0);
    moduleAccesser->getCollisionHitModule().setStatus(0, 3, 0);
}

void wnNessPKFireStatusUniqProcessPillar::execStatus(soModuleAccesser* moduleAccesser) {
    const wnNessPKFireParam* param = dynamic_cast<wnNessPKFire&>(moduleAccesser->getStageObject()).m_param;
    int time = moduleAccesser->getWorkManageModule().getInt(0x10000004);
    float scale = param->unk18 + (float)time * (1.0f - param->unk18) / (float)param->unk4;
    moduleAccesser->getPostureModule().setScale(scale);
}
