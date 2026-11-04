#include <snd/snd_system.h>
#include <ft/ft_sound_id_exchanger_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: per-kind voice sound groups, shared with ftDataProvider (fighter kind and Kirby copy kind).
extern "C" const int lbl_27_rodata_1C60[];
extern "C" const int lbl_27_rodata_1D3C[];
// HYPOTHESIS: returns whether the sound id belongs to the sound group (main.dol sound system).

struct ftStepLandingSoundId {
    int step;
    int landing;
};

static const ftStepLandingSoundId s_stepLandingSoundIds[] = {
    { 0xD0, 0xD3 },
    { 0xD1, 0xD4 },
    { 0xD2, 0xD5 },
};

ftSoundIdExchangerImpl g_ftSoundIdExchangerImpl;
ftKirbySoundIdExchangerImpl g_ftKirbySoundIdExchangerImpl;

bool ftSoundIdExchangerImpl::isVoiceId(int sndID, soModuleAccesser* moduleAccesser) {
    return sndSystem::isVoice(lbl_27_rodata_1C60[moduleAccesser->getStageObject().soGetSubKind()], sndID);
}

bool ftKirbySoundIdExchangerImpl::isVoiceId(int sndID, soModuleAccesser* moduleAccesser) {
    int copyKind = moduleAccesser->getWorkManageModule().getInt(0x10000048);
    if (copyKind != -1 && sndSystem::isVoice(lbl_27_rodata_1D3C[copyKind], sndID) == true) {
        return true;
    }
    return ftSoundIdExchangerImpl::isVoiceId(sndID, moduleAccesser);
}

bool ftSoundIdExchangerImpl::isVoiceMute(soModuleAccesser* moduleAccesser) {
    bool mute = false;
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000012) == true ||
        moduleAccesser->getWorkManageModule().isFlag(0x12000018) == true ||
        moduleAccesser->getWorkManageModule().isFlag(0x12000033) == true) {
        mute = true;
    }
    return mute;
}

SndID ftSoundIdExchangerImpl::getId(SndID sndID, soModuleAccesser* moduleAccesser) {
    if (isVoiceMute(moduleAccesser) == true && isVoiceId(sndID, moduleAccesser) == true) {
        return (SndID)-1;
    }
    return sndID;
}

int ftSoundIdExchangerImpl::getStepId(int stepId, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000012) == true) {
        int index = soValueAccesser::getConstantInt(moduleAccesser, 0x59E8, 0);
        if (index != -1) {
            return s_stepLandingSoundIds[index].step;
        }
    }
    return stepId;
}

int ftSoundIdExchangerImpl::getLandingId(int landingId, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000012) == true) {
        int index = soValueAccesser::getConstantInt(moduleAccesser, 0x59E8, 0);
        if (index != -1) {
            return s_stepLandingSoundIds[index].landing;
        }
    }
    return landingId;
}
