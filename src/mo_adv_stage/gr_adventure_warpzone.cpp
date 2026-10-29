#include <ef/ef_screen.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <gm/gm_global.h>
#include <sora_adv_stage/gr_adventure_common.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_warpzone.h>
#include <sora_adv_stage/st_adventure2.h>
#include <st/stage.h>

// HYPOTHESIS: the running stAdventure2 instance (sora_adv_stage bss) and two of its members (map names, parameter
// types unknown): stAdventure2::setFighterStartDamage, setFighterWarp.
extern "C" stAdventure2* lbl_40_bss_15C;
extern "C" void fn_40_5CD8(stAdventure2* stage, int entryId);
extern "C" void fn_40_5814(stAdventure2* stage, void* warpDest, u8 isSecondWarped);
// MATCH-ONLY: Stage::setStageOutEffectInit takes a flag that the shared header does not declare; vtable slots 113/114.
static inline void stageSetStageOutEffectInit(Stage* stage, int flag) {
    typedef void (*Func)(Stage*, int);
    Func** vtable = reinterpret_cast<Func**>(reinterpret_cast<u8*>(stage) + 0x3C);
    (*vtable)[115](stage, flag);
}
static inline void stageSetStageInEffectInit(Stage* stage) {
    typedef void (*Func)(Stage*);
    Func** vtable = reinterpret_cast<Func**>(reinterpret_cast<u8*>(stage) + 0x3C);
    (*vtable)[116](stage);
}

grAdventureWarpZone* grAdventureWarpZone::create(int mdlIndex, const char* taskName) {
    grAdventureWarpZone* ground = new (Heaps::StageInstance) grAdventureWarpZone(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureWarpZone::~grAdventureWarpZone() { }

void grAdventureWarpZone::processFixPosition() {
    float deltaFrame = lbl_27_data_547E0;
    switch (m_state) {
    case 1: {
        int entryId1 = g_ftManager->getEntryId(0);
        int entryId2 = g_ftManager->getEntryId(1);
        if (entryId1 == -1 || !g_ftManager->isFighterEnableWarp(entryId1)) {
            m_state = 0;
            break;
        }
        fn_40_5CD8(lbl_40_bss_15C, 0);
        fn_40_5CD8(lbl_40_bss_15C, 1);
        // HYPOTHESIS: the trigger data at +0x6C of the warp data (the shared header leaves it unnamed)
        g_stTriggerMng->setTriggerFlag(reinterpret_cast<stTriggerData*>(reinterpret_cast<u8*>(m_warpData) + 0x6C));
        startGimmickSE(0);
        startGimmickEffect(0);
        if (g_ftManager->isSubFighterDead(entryId1)) {
            g_ftManager->getOwner(entryId1)->setSubFighterStartStandby(true);
        }
        g_ftManager->standbyFighter(entryId1, 1);
        m_isSecondWarped = 0;
        if (entryId2 != -1 && g_ftManager->isFighterEnableWarp(entryId2)) {
            if (g_ftManager->isSubFighterDead(entryId2)) {
                g_ftManager->getOwner(entryId2)->setSubFighterStartStandby(true);
            }
            g_ftManager->standbyFighter(entryId2, 1);
            m_isSecondWarped = 1;
        }
        m_state = 2;
        GXColor black = { 0, 0, 0, 255 };
        g_efScreen->requestFill(60.0f, 7, 0, &black);
        if (g_Stage != NULL) {
            stageSetStageOutEffectInit(g_Stage, 0);
        }
        break;
    }
    case 2:
        if (m_frameCount >= 60.0f) {
            // HYPOTHESIS: the warp destination at +0x30 of the warp data
            fn_40_5814(lbl_40_bss_15C, reinterpret_cast<u8*>(m_warpData) + 0x30, m_isSecondWarped);
            m_state = 0;
            m_frameCount = 0.0f;
            if (g_Stage != NULL) {
                stageSetStageInEffectInit(g_Stage);
            }
        }
        break;
    }
    m_frameCount += deltaFrame;
}

void grAdventureWarpZone::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    int kind = eventInfo->m_kind;
    getPlayerNumber(taskId);
    if (getPlayerNumber(taskId) == 0 && kind == 0x32) {
        m_state = 1;
        m_frameCount = 0.0f;
    }
}
