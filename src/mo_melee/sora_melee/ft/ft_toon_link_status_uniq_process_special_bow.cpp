#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: the two article ids of the Toon Link bow shots are stored in the fighter data at +0xac and +0xb4.
struct ftToonLinkBowArticleIdsView {
    u8 unk00[0xac];
    s32 m_arrowId;
    u8 unkB0[0x4];
    s32 m_arrowId2;
};

// HYPOTHESIS: parameter block of ftCommonDataAccesser::getToonLinkBowParam(subKind); the motion rate is at +4.
extern "C" float* fn_27_14998C(ftCommonDataAccesser* accesser, int subKind);

class ftToonLinkStatusUniqProcessSpecialBow : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessSpecialBow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftToonLinkStatusUniqProcessSpecialBow g_ftToonLinkStatusUniqProcessSpecialBow;

void ftToonLinkStatusUniqProcessSpecialBow::initStatus(soModuleAccesser* acc) {
    float rate = fn_27_14998C(&g_ftCommonDataAccesser, acc->getStageObject().soGetSubKind())[1];
    acc->getMotionModule().setRate(rate);
}

void ftToonLinkStatusUniqProcessSpecialBow::execStatus(soModuleAccesser* acc) {
    float* param = fn_27_14998C(&g_ftCommonDataAccesser, acc->getStageObject().soGetSubKind());
    int state = acc->getWorkManageModule().getInt(0x20000000);
    if (state == 0) {
        if (acc->getWorkManageModule().isFlag(0x22000013)) {
            acc->getWorkManageModule().addFloat(1.0f, 0x21000004);
        }
    } else if (state == 1) {
        float value = acc->getWorkManageModule().getFloat(0x21000004);
        if (value < param[0]) {
            acc->getWorkManageModule().setFloat(param[0], 0x21000004);
        }
    }
    acc->getKineticModule().enableEnergy(0);
}

void ftToonLinkStatusUniqProcessSpecialBow::execFixPos(soModuleAccesser* acc) {
    ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*acc->getKineticModule().getEnergy(0));
    // HYPOTHESIS: the energy kind is 2 while the ground is touched (touch flag 8), 4 otherwise.
    int kind = acc->getGroundModule().isTouch(8, 0) ? 2 : 4;
    // HYPOTHESIS: constant from lbl_27_rodata_7EE4 (zero).
    float zero = 0.0f;
    Vec2f speed;
    Vec3f rot;
    rot.m_x = zero;
    rot.m_y = zero;
    rot.m_z = zero;
    speed.m_x = zero;
    speed.m_y = zero;
    motion.resetEnergy(kind, &speed, &rot, acc);
}

void ftToonLinkStatusUniqProcessSpecialBow::exitStatus(soModuleAccesser* acc, int) {
    if (acc->getStageObject().soGetSubKind() == 0x29) {
        ftToonLinkBowArticleIdsView* ids = reinterpret_cast<ftToonLinkBowArticleIdsView*>(g_ftCommonDataAccesser.getData(static_cast<ftKind>(0x29)));
        static_cast<soGenerateArticleManageModule*>(acc->m_enumerationStart->m_generateArticleManageModule)->removeExist(ids->m_arrowId, 0);
        static_cast<soGenerateArticleManageModule*>(acc->m_enumerationStart->m_generateArticleManageModule)->removeExist(ids->m_arrowId2, 0);
    } else {
        static_cast<soGenerateArticleManageModule*>(acc->m_enumerationStart->m_generateArticleManageModule)->removeExist(0xd, 0);
        static_cast<soGenerateArticleManageModule*>(acc->m_enumerationStart->m_generateArticleManageModule)->removeExist(0xe, 0);
    }
}
