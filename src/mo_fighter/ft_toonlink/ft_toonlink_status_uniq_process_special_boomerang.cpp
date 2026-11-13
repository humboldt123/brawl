#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/toonlink/ft_toonlink_data.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/fighter.h>
#include <ft/ft_owner.h>
#include <ft/ft_entry.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class ftToonLinkStatusUniqProcessSpecialBoomerang : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessSpecialBoomerang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// Common parameter offsets are observed in the native reads; the descriptive field names are HYPOTHESIS.
struct ftToonLinkBoomerangCommonView {
    u8 unk00[0x18];
    float m_stickThreshold; // 0x18: the stick must reach this magnitude
    int m_flickFrame;       // 0x1c
    float m_flickFrameAdd;  // 0x20
};

// The throw path requires the stick magnitude to reach the common threshold while the flick count remains below its frame window.
void ftToonLinkStatusUniqProcessSpecialBoomerang::initStatus(soModuleAccesser* moduleAccesser) {
    double absolute = __fabs(moduleAccesser->getControllerModule().getStickX());
    float stickX = absolute;
    ftToonLinkBoomerangCommonView* common = reinterpret_cast<ftToonLinkBoomerangCommonView*>(g_ftCommonDataAccesser.getParamCommon());
    if (!(stickX >= common->m_stickThreshold)) {
        return;
    }

    soControllerModule& controller = moduleAccesser->getControllerModule();
    float limit = (float)reinterpret_cast<ftToonLinkBoomerangCommonView*>(g_ftCommonDataAccesser.getParamCommon())->m_flickFrame
        + reinterpret_cast<ftToonLinkBoomerangCommonView*>(g_ftCommonDataAccesser.getParamCommon())->m_flickFrameAdd;
    if (!((float)controller.getFlickNoResetX() < limit)) {
        return;
    }

    Fighter& fighter = dynamic_cast<Fighter&>(moduleAccesser->getStageObject());
    moduleAccesser->getWorkManageModule().onFlag(0x22000011);
    fighter.getOwner()->addLogInt(1, 0x19, 0);
}
void ftToonLinkStatusUniqProcessSpecialBoomerang::execStatus(soModuleAccesser*) { }

// The boomerang article is released on leaving the status unless the throw flag is still set.
void ftToonLinkStatusUniqProcessSpecialBoomerang::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x1200003D)) {
        ftToonLinkData* data = static_cast<ftToonLinkData*>(g_ftCommonDataAccesser.getData(Fighter_ToonLink));
        static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)
            ->removeExist(data->m_boomerangArticleId, 0);
    }
}

ftToonLinkStatusUniqProcessSpecialBoomerang g_ftToonLinkStatusUniqProcessSpecialBoomerang;
