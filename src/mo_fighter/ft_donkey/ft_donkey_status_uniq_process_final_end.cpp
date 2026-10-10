#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <snd/snd_system.h>
#include <types.h>

// HYPOTHESIS: the generate-article manage module (soModuleEnumeration::m_generateArticleManageModule) is
// still unrecovered; only its slot at 0x48 is called here, so the earlier slots are placeholders.
class ftGenerateArticleManageStub {
public:
    virtual ~ftGenerateArticleManageStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48(int, int);
};

extern "C" void fn_80076274(void* sndSystem, int soundId); // HYPOTHESIS: sound helper, name not recovered

class ftDonkeyStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessFinalEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDonkeyStatusUniqProcessFinalEnd::initStatus(soModuleAccesser* moduleAccesser) {
    fn_80076274(g_sndSystem, 0x1b4b);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
}

void ftDonkeyStatusUniqProcessFinalEnd::exitStatus(soModuleAccesser* moduleAccesser, int) {
    static_cast<ftGenerateArticleManageStub*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->unk48(0, 0);
    int handle = moduleAccesser->getWorkManageModule().getInt(0x10000042);
    if (handle >= 0) {
        static_cast<soSoundModule*>(moduleAccesser->m_enumerationStart->m_soundModule)->stopSEHandle(handle, 0);
        moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
    }
}

ftDonkeyStatusUniqProcessFinalEnd g_ftDonkeyStatusUniqProcessFinalEnd;
