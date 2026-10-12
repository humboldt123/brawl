#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <snd/snd_system.h>
#include <so/article/so_generate_article_manage_module.h>
#include <types.h>


class ftDonkeyStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessFinalEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDonkeyStatusUniqProcessFinalEnd::initStatus(soModuleAccesser* moduleAccesser) {
    g_sndSystem->finalOffState(0x1b4b);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
}

void ftDonkeyStatusUniqProcessFinalEnd::exitStatus(soModuleAccesser* moduleAccesser, int) {
    static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->removeExist(0, 0);
    int handle = moduleAccesser->getWorkManageModule().getInt(0x10000042);
    if (handle >= 0) {
        static_cast<soSoundModule*>(moduleAccesser->m_enumerationStart->m_soundModule)->stopSEHandle(handle, 0);
        moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
    }
}

ftDonkeyStatusUniqProcessFinalEnd g_ftDonkeyStatusUniqProcessFinalEnd;
