#define ST_ADVENTURE2_IMPL
#define ST_STAGE_OUT_EFFECT_INIT_FLAG

#include <OS/OSError.h>
#include <ad/ad_keep_manager.h>
#include <ai/ai_mgr.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_adventure.h>
#include <cm/cm_controller_ai.h>
#include <ec/ec_mgr.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <em/em_manager.h>
#include <em/em_weapon_manager.h>
#include <gf/gf_archive.h>
#include <gf/gf_camera.h>
#include <gf/gf_task_scheduler.h>
#include <gm/gm_global.h>
#include <gm/gm_global_mode_melee.h>
#include <gm/gm_global_record.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_catapult.h>
#include <gr/gr_gimmick_ladder.h>
#include <gr/gr_hit_circle_observer.h>
#include <gr/ground.h>
#include <it/it_figure_manager.h>
#include <it/it_manager.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_resnode.h>
#include <snd/snd_system.h>
#include <so/damage/so_damage_attacker_info.h>
#include <so/so_external_value_accesser.h>
#include <sora_adv_stage/gr_adventure_background.h>
#include <sora_adv_stage/gr_adventure_rebirth_pos.h>
#include <sora_adv_stage/st_adventure2.h>
#include <sora_adv_stage/st_adventure2_gimmick_data.h>
#include <sora_adv_stage/st_adventure_info_mediation.h>
#include <st/st_enemy_id_manager.h>
#include <st/st_positions.h>
#include <st/st_trigger.h>
#include <st/st_trigger_actor_common.h>
#include <st/st_utility.h>
#include <types.h>

// Forward declaration matching src/mo_adv_stage/st_adv_versus_item_drop.cpp
class stAdventureVersusItemDrop : public gfTask {
public:
    void start();
    void end();
    bool isReady();
    bool isStarted();
};

extern stAdventure2* lbl_40_bss_15C;
extern u32 lbl_40_bss_148;
extern int g_ftItemEnabled;
extern emManager* g_emManager;
extern emWeaponManager* g_emWeaponManager;

static u32 s_gimmickMagics[75] = {
    *(const u32*)"GMWA", *(const u32*)"GMWO", *(const u32*)"GET1", *(const u32*)"GET2",
    *(const u32*)"GFRT", *(const u32*)"GFT2", *(const u32*)"GITR", *(const u32*)"GTRR",
    *(const u32*)"GEPT", *(const u32*)"GEG1", *(const u32*)"GFG1", *(const u32*)"GFSR",
    *(const u32*)"GCAM", *(const u32*)"GITM", *(const u32*)"GRIM", *(const u32*)"GFIM",
    *(const u32*)"GNDV", *(const u32*)"GQUK", *(const u32*)"GEFF", *(const u32*)"GSTG",
    *(const u32*)"GMOV", *(const u32*)"GMV4", *(const u32*)"GSHT", *(const u32*)"GMOT",
    *(const u32*)"GMOA", *(const u32*)"GMVA", *(const u32*)"GMCA", *(const u32*)"GMEA",
    *(const u32*)"GMTA", *(const u32*)"GBRF", *(const u32*)"GFRK", *(const u32*)"GFPR",
    *(const u32*)"GMAO", *(const u32*)"GMRL", *(const u32*)"GELA", *(const u32*)"GTIK",
    *(const u32*)"GBC2", *(const u32*)"GBC1", *(const u32*)"GSPR", *(const u32*)"GCAT",
    *(const u32*)"GLAD", *(const u32*)"GTRC", *(const u32*)"GWND", *(const u32*)"GWD2",
    *(const u32*)"GBLT", *(const u32*)"GWAT", *(const u32*)"GHPE", *(const u32*)"GELE",
    *(const u32*)"GMSJ", *(const u32*)"GDOR", *(const u32*)"GDBF", *(const u32*)"GDTP",
    *(const u32*)"GDBS", *(const u32*)"GWAP", *(const u32*)"GSAV", *(const u32*)"GLOK",
    *(const u32*)"GLK2", *(const u32*)"GBLK", *(const u32*)"GSBK", *(const u32*)"GMIN",
    *(const u32*)"GENC", *(const u32*)"GDMD", *(const u32*)"GBKI", *(const u32*)"GIBS",
    *(const u32*)"GIB2", *(const u32*)"GMPS", *(const u32*)"GPS2", *(const u32*)"GSCN",
    *(const u32*)"GCMP", *(const u32*)"GKNG", *(const u32*)"GFIN", *(const u32*)"EPBM",
    *(const u32*)"EPSP", *(const u32*)"BLOC", *(const u32*)"ADSJ",
};

void srReportLessRelease(const char* format, ...) {
}

static inline u8 GmGetAdventureGameLevel() {
    int level = g_GameGlobal->m_advData->m_difficultyLevel - g_GameGlobal->m_advData->unk4916;
    if (level < g_GameGlobal->m_advData->m_difficultyLevel - 5) {
        level = g_GameGlobal->m_advData->m_difficultyLevel - 5;
    }
    if (level < 0) {
        level = 0;
    }
    if (level > 14) {
        level = 14;
    }
    return level;
}

template <class T, void (stAdventure2::*Fn)(T&)>
class stActTriggerCreteGimmick : public stTriggerCB, public stActTriggerFrameCBZero {
public:
    T m_data;

    stActTriggerCreteGimmick(const T& data) : m_data(data) { }
    virtual ~stActTriggerCreteGimmick() { }
    virtual void userProc() {
        if (lbl_40_bss_15C != NULL) {
            (lbl_40_bss_15C->*Fn)(m_data);
        }
    }
};

stClassInfoImpl<Stages::Adventure, stAdventure2> stAdventure2::bss_loc_14;

stAdventure2* stAdventure2::create() {
    return new (Heaps::StageInstance) stAdventure2();
}

bool stAdventure2::loading() {
    return true;
}

static inline bool loadLocalEnemyData(stAdventure2* self) {
    if (g_emManager == NULL) {
        return true;
    }
    if (!self->m_flag550_3) {
        void* data = self->m_fileData->getData(Data_Type_Misc, 7, 0xFFFE);
        if (data != NULL) {
            self->m_enemyCreatorContainer = stDataMultiContainer::create(static_cast<stDataContainerData*>(data), Heaps::StageInstance);
        }
        stDataMultiContainer* container = self->m_enemyCreatorContainer;
        if (container != NULL) {
            if (container->isMultiContainer()) {
                u32 magic = s_gimmickMagics[9];
                u16 enemyKind;
                u32 i;
                u32 nodeIdx = 0;
                stDataContainer* sub;
                while ((sub = container->searchContainer(magic, nodeIdx++)) != NULL) {
                    for (i = 0; i < sub->getDataNum(); i++) {
                        struct EnemyCreatorEntry {
                            u8 _0[0x1C];
                            u16 m_enemyKind;
                        };
                        EnemyCreatorEntry* entry = reinterpret_cast<EnemyCreatorEntry*>(sub->getData(i));
                        enemyKind = entry->m_enemyKind;
                        gfArchive* param;
                        gfArchive* brres;
                        gfArchive* enmCommon;
                        gfArchive* primFace;
                        self->getEnemyPac(&brres, &param, &enmCommon, &primFace, static_cast<EnemyKind>(enemyKind));
                        emManager::getInstance()->preloadArchive(param, brres, enmCommon, primFace, static_cast<EnemyKind>(enemyKind), true);
                        if (static_cast<int>(entry->m_enemyKind) == 4) {
                            self->m_preloadItemFlag0 = 1;
                        }
                        if (static_cast<int>(entry->m_enemyKind) == 0x19) {
                            self->m_preloadItemFlag1 = 1;
                        }
                    }
                }
            }
            delete self->m_enemyCreatorContainer;
            self->m_enemyCreatorContainer = NULL;
        }
        self->m_flag550_3 = true;
    }
    return emManager::getInstance()->isCompArchiveAll();
}

bool stAdventure2::isLoadLocalData() {
    switch (GmGetAdventureGameLevel()) {
    case 0:
    case 1:
    case 2:
        itManager::getInstance()->setItemGenLevel(static_cast<itGenParam::Level>(0));
        break;
    case 3:
    case 4:
    case 5:
        itManager::getInstance()->setItemGenLevel(static_cast<itGenParam::Level>(1));
        break;
    case 6:
    case 7:
    case 8:
        itManager::getInstance()->setItemGenLevel(static_cast<itGenParam::Level>(2));
        break;
    case 9:
    case 10:
    case 11:
        itManager::getInstance()->setItemGenLevel(static_cast<itGenParam::Level>(3));
        break;
    case 12:
    case 13:
    case 14:
        itManager::getInstance()->setItemGenLevel(static_cast<itGenParam::Level>(4));
        break;
    }
    bool ret = true;
    if (m_advData->m_mode != 3 && m_advData->m_mode != 4) {
        ret = loadLocalEnemyData(this);
        if (ret) {
            if (m_preloadItemFlag0 == 1) {
                if (!itManager::getInstance()->isCompItemKindArchive(static_cast<itKind>(0x50), 0, true)) {
                    itManager::getInstance()->preloadItemKindArchive(static_cast<itKind>(0x50), 0, static_cast<itArchive::Type>(0), true);
                }
                m_preloadItemFlag0 = 0;
            }
            if (m_preloadItemFlag1 == 1) {
                if (!itManager::getInstance()->isCompItemKindArchive(static_cast<itKind>(0x52), 0, true)) {
                    itManager::getInstance()->preloadItemKindArchive(static_cast<itKind>(0x52), 0, static_cast<itArchive::Type>(0), true);
                }
                if (!itManager::getInstance()->isCompItemKindArchive(static_cast<itKind>(0x51), 0, true)) {
                    itManager::getInstance()->preloadItemKindArchive(static_cast<itKind>(0x51), 0, static_cast<itArchive::Type>(0), true);
                }
                m_preloadItemFlag1 = 0;
            }
            ret = itManager::getInstance()->isCompItemKindArchivePreloadAll();
        }
    }
    return ret;
}

void stAdventure2::removeLocalData() {
    if (g_ftItemEnabled != 0) {
        itManager::getInstance()->removeItemAll();
    }
    if (m_advData->m_mode != 3 && m_advData->m_mode != 4) {
        if (g_emManager != NULL) {
            emManager::getInstance()->removeEnemyAll();
        }
        if (g_stEnemyIdManager != NULL) {
            g_stEnemyIdManager->removeAll();
            srReportLessRelease("stEnemyIdManager::removeAll()\n");
        }
        emWeaponManager* mgr = emWeaponManager::getInstance();
        mgr->reset();
        mgr->m_isEmpty = true;
    }
}

void stAdventure2::requestStepJump(u32 jumpData) {
    m_advData->unk628 = jumpData;
    m_advData->m_saveRequest = 1;
}

void stAdventure2::processFixCamera() {
    if (m_stagePositions != NULL && m_flag550_0) {
        Vec3f pos;
        pos = gfCameraManager::getManager()->getCamera(0)->m_targetPos;
        m_stagePositions->m_centerPos = pos;
    }
    Stage::processFixCamera();
}

void stAdventure2::processEnd() {
    Stage::processEnd();
    changeStep();
}

void stAdventure2::getScrollLockPoketrainerPos(Vec3f* outPos, int index) {
    nw4r::g3d::ResFile resFile(m_fileData->getData(Data_Type_Model, index, 0xFFFE));
    nw4r::g3d::ResMdl resMdl = resFile.GetResMdl(0);
    nw4r::g3d::ResNode resNode = resMdl.GetResNode(1);
    Vec3f* trans = &resNode.ptr()->m_translation;
    outPos->m_x = trans->m_x;
    outPos->m_y = trans->m_y;
    outPos->m_z = trans->m_z;
}

Ground* stAdventure2::createGimmickNodeVisibility(grGimmickNodeVisibilityData* data) {
    u16 triggerId = data->m_triggerId;
    if (triggerId == 0 || adKeepManager::getInstance()->getKeepFlag(triggerId, data->m_keepKindByte) == 1) {
        Vec3f pos;
        setGimmickNodeVisibility(*data, &pos);
    } else {
        stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_None, reinterpret_cast<stTriggerData*>(&data->m_triggerId));
        trigger->setValidEdgeCallback(
            new (Heaps::StageInstance) stActTriggerCreteGimmick<grGimmickNodeVisibilityData, &stAdventure2::setGimmickNodeVisibilityEffect>(*data));
        u8 prevVis = data->m_isVisible;
        if (prevVis != 0) {
            data->m_isVisible = 0;
        } else {
            data->m_isVisible = 1;
        }
        trigger->setUnValidEdgeCallback(
            new (Heaps::StageInstance) stActTriggerCreteGimmick<grGimmickNodeVisibilityData, &stAdventure2::setGimmickNodeVisibilityEffect>(*data));
        data->m_isVisible = prevVis;
    }
    return NULL;
}

void stAdventure2::setGimmickNodeVisibilityEffect(grGimmickNodeVisibilityData& data) {
    Vec3f pos;
    if (setGimmickNodeVisibility(data, &pos)) {
        g_sndSystem->playSE(data.m_sndId, -1, 0, 0, -1);
        g_ecMgr->setEffect(data.m_efId, &pos);
    }
}

void stAdventure2::changeGimmickCameraPath(grAdventureChangeCameraPathData& data) {
    u8 pathIndex = data.m_pathIndex;
    m_cameraPath = NULL;
    grFixedPathCollection* path =
        static_cast<grFixedPathCollection*>(m_fileData->getData(Data_Type_Misc, pathIndex, 0xFFFE));
    if (path != NULL) {
        path->relocation();
        m_cameraPath = path;
        CameraController* cc = CameraController::getInstance();
        static_cast<cmAdventureController*>(cc->m_adventureController)->unkA0 =
            reinterpret_cast<cmAdventurePolygonCollection*>(m_cameraPath);
    }
}

int stAdventure2::getPokeTrainerPointNum() {
    return m_pokeTrainerPointNum;
}

void stAdventure2::getPokeTrainerPointData(int* unk1, int unk2) {
    Vec2f* dst = reinterpret_cast<Vec2f*>(unk1);
    *dst = m_pokeTrainerPointData[unk2];
}

float stAdventure2::getPokeTrainerPositionZ() {
    if (m_advStageParam != NULL) {
        return m_advStageParam->m_pokeTrainerPosZ;
    }
    return 20.0f;
}

void stAdventure2::changeCameraForceTargetMode(bool unk) {
    m_unk52C = 2;
    updateStagePositions();
    if (CameraController::getInstance() != NULL) {
        cmAdventureController* ctrl = dynamic_cast<cmAdventureController*>(CameraController::getInstance()->m_currentController);
        if (ctrl != NULL) {
            if (unk) {
                ctrl->unk9_mode = 3;
                ctrl->unk144 = 1;
                ctrl->initialize();
            } else {
                ctrl->unk9_mode = 1;
                ctrl->unk144 = 1;
                ctrl->initialize();
            }
            ctrl->resetTarget();
        }
    }
}

void stAdventure2::setCameraAdvCameraTarget(Vec3f* pos) {
    cmAdventureController* ctrl = dynamic_cast<cmAdventureController*>(CameraController::getInstance()->m_currentController);
    if (ctrl != NULL) {
        ctrl->_38 = *pos;
    }
}

void stAdventure2::setDemoCameraParam(Vec3f* pos, Vec3f* target) {
    cmAdventureController* ctrl = dynamic_cast<cmAdventureController*>(CameraController::getInstance()->m_currentController);
    if (ctrl != NULL) {
        ctrl->_138 = *pos;
        ctrl->_38 = *target;
    }
}

void stAdventure2::setCameraAdvCameraOffset(Vec2f* offset) {
    cmAdventureController* ctrl = dynamic_cast<cmAdventureController*>(CameraController::getInstance()->m_currentController);
    if (ctrl != NULL) {
        ctrl->_F8 = offset->m_x;
        ctrl->_FC = offset->m_y;
        ctrl->unk8_mode = 2;
    }
}

void stAdventure2::stopCameraAdvCameraOffset() {
    cmAdventureController* ctrl = dynamic_cast<cmAdventureController*>(CameraController::getInstance()->m_currentController);
    if (ctrl != NULL) {
        ctrl->unk8_mode = 0;
    }
}

void stAdventure2::setFighterPos(int player, Vec3f* pos) {
    int entryId = g_ftManager->getEntryId(player);
    if (entryId != -1) {
        if (g_ftManager->isFighterEnableWarp(entryId)) {
            if (g_ftManager->getFighterOperationStatus(entryId) != 0) {
                g_ftManager->setWarpFighter(entryId, pos, 1.0f, 0);
            }
        }
    }
}

void stAdventure2::setFighterStartDamage(int player) {
    int entryId = g_ftManager->getEntryId(player);
    if (entryId != -1) {
        ftOwner* owner = g_ftManager->getOwner(entryId);
        owner->setStartDamage(owner->getDamage());
    }
}

bool stAdventure2::getFighterPos(int playerNo, Vec3f* outPos) {
    int entryId = g_ftManager->getEntryId(playerNo);
    if (entryId == -1) {
        return false;
    }
    Fighter* ft = g_ftManager->getFighter(entryId, -1);
    if (ft == NULL) {
        return false;
    }
    if (!g_ftManager->isFighterActivate(entryId, -1)) {
        return false;
    }
    if (!g_ftManager->isFighterPositionAccessable(entryId, -1)) {
        return false;
    }
    *outPos = soExternalValueAccesser::getPos(ft);
    return true;
}

static inline bool isSpecialRestartMode() {
    gmAdvData* adv = g_GameGlobal->m_advData;
    if (adv != NULL) {
        if (!(adv->m_mode != 1 && adv->m_mode != 2 && adv->m_mode != 3)) {
            return true;
        }
    }
    return false;
}

void stAdventure2::getFighterReStartPos(Vec3f* pos, int index) {
    pos->m_z = 0.0f;
    pos->m_y = 0.0f;
    pos->m_x = 0.0f;
    if (!isSpecialRestartMode()) {
        switch (m_unk52C) {
        case 0:
            if (!m_flag550_2 && index == 0) {
                *pos = m_rebirthPos;
            } else {
                if (!getFighterPos(0, pos)) {
                    *pos = m_rebirthPos;
                }
            }
            break;
        case 1:
            Stage::getFighterReStartPos(pos, index);
            break;
        case 2:
            *pos = m_unk56C;
            break;
        }
    } else {
        Stage::getFighterReStartPos(pos, index);
    }
}

void stAdventure2::createItem(itGenId genId, Vec3f* pos) {
    itManager::getInstance()->lotCreateItem(m_itemGenSheetKind, genId, pos, -1, -1, static_cast<itKind>(-1));
}

void stAdventure2::notifyEnemyEventOnHit(int createId, u32 taskId, u32 damage, gfTask::Category category) {
    stAdventureInfoMediation::getInstance()->getEmBoss()->notifyDamage(createId, damage);
    stAdventureInfoEmZako* zako = stAdventureInfoMediation::getInstance()->getEmZako();
    if (zako->isZako(createId)) {
        StageObject* obj = dynamic_cast<StageObject*>(gfTaskScheduler::getInstance()->getTaskById(category, taskId));
        if (obj != NULL) {
            if (category == gfTask::Category_Fighter) {
                Fighter* ft = dynamic_cast<Fighter*>(obj);
                if (ft != NULL) {
                    int playerNo = g_ftManager->getPlayerNo(ft->m_entryId);
                    if (playerNo != -1) {
                        zako->init(createId, playerNo);
                        zako->unk58 = true;
                    } else {
                        zako->unk58 = false;
                    }
                }
            } else if (category == gfTask::Category_Item || category == gfTask::Category_Weapon) {
                int entryId = g_ftManager->getEntryIdFromTaskId(soExternalValueAccesser::getTeamOwnerId(obj), NULL);
                int playerNo = g_ftManager->getPlayerNo(entryId);
                if (playerNo != -1) {
                    zako->init(createId, playerNo);
                    zako->unk58 = true;
                } else {
                    zako->unk58 = false;
                }
            } else {
                soDamageAttackerInfo info;
                soExternalValueAccesser::getAttackerInfo(obj, &info);
                if (info.m_indirectSoKind == StageObject_Fighter) {
                    Fighter* ft = dynamic_cast<Fighter*>(
                        gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, info.m_indirectTaskId));
                    if (ft != NULL) {
                        int playerNo = g_ftManager->getPlayerNo(ft->m_entryId);
                        if (playerNo != -1) {
                            zako->init(createId, playerNo);
                            zako->unk58 = true;
                        } else {
                            zako->unk58 = false;
                        }
                    }
                } else {
                    int entryId = g_ftManager->getEntryIdFromTaskId(soExternalValueAccesser::getTeamOwnerId(obj), NULL);
                    if (entryId != -1) {
                        zako->init(createId, g_ftManager->getPlayerNo(entryId));
                        zako->unk58 = true;
                    } else {
                        zako->unk58 = false;
                    }
                }
            }
        } else {
            zako->unk58 = false;
        }
    }
}

void stAdventure2::notifyEnemyEventCreateForLeftWnBossPackn(u32 createId) {
    stAdventureInfoMediation::getInstance()->getEmBoss()->setBossPacknWnLeft(createId);
}

void stAdventure2::notifyEnemyEventCreateForRightWnBossPackn(u32 createId) {
    stAdventureInfoMediation::getInstance()->getEmBoss()->setBossPacknWnRight(createId);
}

void stAdventure2::notifyEntryDoor() {
    setStageOutEffectInit(1);
    u32 groundNum = getGroundNum();
    for (u32 i = 0; i < groundNum; i++) {
        getGround(i)->setMotionRatio(0.0f);
    }
    if (stAdventureInfoMediation::getInstance() != NULL) {
        stAdventureInfoMediation::getInstance()->notifyStepJump();
    }
    m_unk1B8 = true;
}

void stAdventure2::unResistCameraSubjectOnPositionCenter() {
    m_cameraSubject.unRegist();
}

Rect2D* stAdventure2::getAIRange() {
    static Rect2D s_aiRange;
    s_aiRange = m_deadRange;
    float h = s_aiRange.m_up - s_aiRange.m_down;
    s_aiRange.m_up += h;
    s_aiRange.m_down -= h;
    return &s_aiRange;
}

float stAdventure2::getFighterDeadEffectSizeRate() {
    if (!isCameraLocked()) {
        Vec3f target;
        target = gfCameraManager::getManager()->getCamera(0)->m_targetPos;
        return (CameraController::getInstance()->unk15C - target.m_x) / 200.0f;
    }
    return m_fighterDeadEffectSizeRate;
}

float stAdventure2::getEnemyDeadEffectSizeRate() {
    if (!isCameraLocked()) {
        Vec3f target;
        target = gfCameraManager::getManager()->getCamera(0)->m_targetPos;
        return (CameraController::getInstance()->unk15C - target.m_x) / 250.0f;
    }
    return m_enemyDeadEffectSizeRate;
}

bool stAdventure2::isCameraLocked() {
    if (CameraController::getInstance() == NULL) {
        return false;
    }
    if (CameraController::getInstance()->m_currentControllerKind == CameraController::Adventure ||
        CameraController::getInstance()->m_currentControllerKind == CameraController::AdventureFree ||
        CameraController::getInstance()->m_currentControllerKind == CameraController::FighterTarget) {
        return false;
    }
    return true;
}

bool stAdventure2::isBossBattleMode() {
    return g_GameGlobal->m_advData->m_flags.m_unk0 != 0;
}

bool stAdventure2::isSimpleBossBattleMode() {
    return g_GameGlobal->m_advData->m_flags.m_unk1 != 0;
}

void stAdventure2::applyAdventureJumpTable(void* data, int size) {
    gfArchive* archive = new (Heaps::StageInstance) gfArchive();
    archive->setFileImage(data, size, Heaps::StageResource);
    m_jumpTable = NULL;
    m_jumpTableCount = 0;
    u32* table = static_cast<u32*>(archive->getData(Data_Type_Misc, static_cast<u32>(m_advData->m_itemGenKind) >> 24, 0xFFFE));
    if (table != NULL && table[0] == lbl_40_bss_148) {
        u32 offset = table[4];
        u32 count = table[1];
        m_jumpTable = reinterpret_cast<u8*>(table) + offset;
        m_jumpTableCount = count;
    }
    if (archive != NULL) {
        delete archive;
    }
}

int stAdventure2::getNowStepBgmID() {
    return getBgmID(m_advData->m_itemGenKind);
}

int stAdventure2::getBgmID(u32 step) {
    if (m_bgmTable != NULL) {
        for (u32 i = 0; i < m_bgmCount; i++) {
            if (step == m_bgmTable[i].m_stepId) {
                return m_bgmTable[i].m_bgmId;
            }
        }
    }
    return -1;
}

float stAdventure2::getBgmVolume() {
    if (m_bgmTable != NULL) {
        for (u32 i = 0; i < m_bgmCount; i++) {
            if (static_cast<u32>(m_advData->m_itemGenKind) == m_bgmTable[i].m_stepId) {
                return 0.01f * m_bgmTable[i].m_volume;
            }
        }
    }
    return 1.0f;
}

float stAdventure2::getBgmPlayOffsetFrame() {
    if (m_bgmTable != NULL) {
        for (u32 i = 0; i < m_bgmCount; i++) {
            if (static_cast<u32>(m_advData->m_itemGenKind) == m_bgmTable[i].m_stepId) {
                return m_bgmTable[i].m_playOffsetFrame;
            }
        }
    }
    return 0.0f;
}

bool stAdventure2::isNextStepBgmEqualNowStepBgmFromFlag() {
    return m_flag550_6;
}

void stAdventure2::notifyScrollLockStart() {
    if (m_versusItemDrop != NULL && m_versusItemDrop->isReady()) {
        m_versusItemDrop->start();
    }
}

void stAdventure2::notifyScrollLockEnd() {
    if (m_versusItemDrop != NULL && m_versusItemDrop->isStarted()) {
        m_versusItemDrop->end();
    }
}

void stAdventure2::updateStagePositions() {
    Stage::updateStagePositions();
    switch (m_unk52C) {
    case 0:
    case 1:
        break;
    case 2:
        Stage::getFighterReStartPos(&m_unk56C, 0);
        break;
    }
}

void stAdventure2::getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID) {
    *brres = NULL;
    *param = NULL;
    struct ItemPacTable {
        int resId;
        int itemKind;
    };
    ItemPacTable brresTable[6] = {
        { 11, 80 },
        { 12, 71 },
        { 13, 81 },
        { 13, 82 },
        { 14, 72 },
        { 15, 73 },
    };
    ItemPacTable paramTable[7] = {
        { 16, 80 },
        { 17, 71 },
        { 18, 81 },
        { 19, 82 },
        { 20, 72 },
        { 21, 73 },
        { 22, 70 },
    };
    int brresResId = -1;
    for (int i = 0; i < 6; i++) {
        if (itemID == brresTable[i].itemKind) {
            brresResId = brresTable[i].resId;
            break;
        }
    }
    if (brresResId == -1 && itemID != 70) {
        OSReport("\x8c\x9f\x8d\xf5\x91\xce\x8f\xdb\x83\x41\x83\x43\x83\x65\x83\x80\x8e\xed\x95\xca = [%d] \n", itemID);
    }
    int paramResId = -1;
    for (int i = 0; i < 7; i++) {
        if (itemID == paramTable[i].itemKind) {
            paramResId = paramTable[i].resId;
            break;
        }
    }
    if (paramResId == -1) {
        OSReport("\x8c\x9f\x8d\xf5\x91\xce\x8f\xdb\x83\x41\x83\x43\x83\x65\x83\x80\x8e\xed\x95\xca = [%d] \n", itemID);
    }
    gfArchive* brresArc;
    if (itemID == 70) {
        brresArc = m_figureArchives[variantID];
    } else {
        if (m_itemPacArchives[brresResId - 11] != NULL) {
            brresArc = m_itemPacArchives[brresResId - 11];
        } else {
            int size;
            void* data = m_commonFileData->getData(Data_Type_Misc, brresResId, &size, 0xFFFE);
            brresArc = new (Heaps::StageInstance) gfArchive();
            brresArc->setFileImage(data, size, Heaps::StageResource);
            m_itemPacArchives[brresResId - 11] = brresArc;
        }
    }
    gfArchive* paramArc;
    if (m_itemPacArchives[paramResId - 11] != NULL) {
        paramArc = m_itemPacArchives[paramResId - 11];
    } else {
        int size;
        void* data = m_commonFileData->getData(Data_Type_Misc, paramResId, &size, 0xFFFE);
        paramArc = new (Heaps::StageInstance) gfArchive();
        paramArc->setFileImage(data, size, Heaps::StageResource);
        m_itemPacArchives[paramResId - 11] = paramArc;
    }
    *brres = brresArc;
    *param = paramArc;
}

void stAdventure2::getItemPacEnemyFigure(gfArchive** archive) {
    if (m_enemyFigureArchive == NULL) {
        int size;
        void* data = m_commonFileData->getData(Data_Type_Misc, 23, &size, 0xFFFE);
        *archive = new (Heaps::StageInstance) gfArchive();
        (*archive)->setFileImage(data, size, Heaps::StageResource);
        m_enemyFigureArchive = *archive;
    } else {
        *archive = m_enemyFigureArchive;
    }
}

void stAdventure2::getItemGenPac(gfArchive** archive) {
    *archive = &m_itemSheetArchive;
}

static inline bool isCommonEnemyPac(EnemyKind enemyKind) {
    if (enemyKind == 0x17 || enemyKind == 0x1C || (enemyKind >= 0x1F && enemyKind <= 0x23)) {
        return true;
    }
    return false;
}

void stAdventure2::getEnemyPac(gfArchive** brres, gfArchive** param, gfArchive** enmCommon, gfArchive** primFaceBrres, EnemyKind enemyKind) {
    *brres = NULL;
    *param = NULL;
    *enmCommon = NULL;
    *primFaceBrres = NULL;
    int baseIdx = enemyKind * 2;
    int brresIdx = baseIdx + 1;
    gfArchive* brresArc = m_enemyArchives[brresIdx];
    if (brresArc == NULL) {
        int size;
        void* data;
        if (isCommonEnemyPac(enemyKind)) {
            data = m_commonFileData->getData(Data_Type_Misc, brresIdx, &size, 0xFFFE);
        } else {
            data = m_secondaryFileData->getData(Data_Type_Misc, brresIdx, &size, 0xFFFE);
        }
        brresArc = new (Heaps::StageInstance) gfArchive();
        brresArc->setFileImage(data, size, Heaps::StageResource);
        m_enemyArchives[brresIdx] = brresArc;
    }
    gfArchive* paramArc = m_enemyArchives[baseIdx];
    if (paramArc == NULL) {
        int size;
        void* data;
        if (isCommonEnemyPac(enemyKind)) {
            data = m_commonFileData->getData(Data_Type_Misc, baseIdx, &size, 0xFFFE);
        } else {
            data = m_secondaryFileData->getData(Data_Type_Misc, baseIdx, &size, 0xFFFE);
        }
        paramArc = new (Heaps::StageInstance) gfArchive();
        paramArc->setFileImage(data, size, Heaps::StageResource);
        m_enemyArchives[baseIdx] = paramArc;
    }
    if (m_enmCommonArchive == NULL) {
        int size;
        void* data = m_commonFileData->getData(Data_Type_Misc, 300, &size, 0xFFFE);
        m_enmCommonArchive = new (Heaps::StageInstance) gfArchive();
        m_enmCommonArchive->setFileImage(data, size, Heaps::StageResource);
    }
    *enmCommon = m_enmCommonArchive;
    if (isCommonEnemyPac(enemyKind)) {
        int faceId = (m_advData->unk618 + 19) % 20 + 200;
        if (m_primFaceArchive == NULL) {
            int size;
            void* data = m_commonFileData->getData(Data_Type_Misc, faceId, &size, 0xFFFE);
            m_primFaceArchive = new (Heaps::StageInstance) gfArchive();
            m_primFaceArchive->setFileImage(data, size, Heaps::StageResource);
        }
        *primFaceBrres = m_primFaceArchive;
    }
    *brres = brresArc;
    *param = paramArc;
}

void stAdventure2::getAdvRadarData(gfArchive** out1, gfArchive** out2) {
    *out1 = NULL;
    *out2 = NULL;
    int size1;
    *out1 = static_cast<gfArchive*>(m_commonFileData->getData(Data_Type_Misc, 7, &size1, 0xFFFE));
    int size2;
    *out2 = static_cast<gfArchive*>(m_commonFileData->getData(Data_Type_Model, 0, &size2, 0xFFFE));
}

void stAdventure2::setStageInEffectInit() {
    g_GameGlobal->m_advData->m_flags.m_unk5 = 0;
    g_ftManager->m_isHomerun = false;
    g_ftManager->unk6c_80 = true;
    Stage::setStageInEffectInit();
}

bool stAdventure2::initializeFighterAttackRatio(int playerNo, float ratio) {
    gmGlobalModeMelee* melee = g_GameGlobal->getGlobalVsMeleeCondition();
    int entryId = g_ftManager->getEntryId(playerNo);
    bool ret = false;
    if (g_ftManager->isValidEntryId(entryId) == true) {
        gmPlayerInitData* player = &melee->m_playersInitData[playerNo];
        ftOwner* owner = g_ftManager->getOwner(entryId);
        if (0.0f != player->m_attackRatio) {
            owner->getAttackRatio(player->m_attackRatio * ratio);
        } else {
            owner->getAttackRatio(ratio);
        }
        ret = true;
    }
    return ret;
}

void stAdventure2::renderOpa() {
    gfTask::renderOpa();
}

bool stAdventure2::isBamperVector() {
    return true;
}

bool stAdventure2::isAdventureStage() {
    return true;
}

// Explicit instantiations for stActTriggerCreteGimmick
template class stActTriggerCreteGimmick<const grAdvRebirthPosData, &stAdventure2::setRebirthPos>;
template class stActTriggerCreteGimmick<grAdventureChangeCameraPathData, &stAdventure2::changeGimmickCameraPath>;
template class stActTriggerCreteGimmick<grGimmickNodeVisibilityData, &stAdventure2::setGimmickNodeVisibilityEffect>;

template <class T, void (*Fn)(stAdventure2*, T*)>
void stAdventure2::createGimmickFunc(u32 resId, stDataMultiContainer* container) {
    stDataContainer* sub;
    u32 nodeIdx = 0;
    while ((sub = container->searchContainer(resId, nodeIdx++)) != NULL) {
        for (u32 i = 0; i < sub->getDataNum(); i++) {
            T* data = reinterpret_cast<T*>(sub->getData(i));
            (*Fn)(this, data);
        }
    }
}

inline void createGimmickChangeCameraPath(stAdventure2* stage, grAdventureChangeCameraPathData* data) {
    if (data->m_triggerId == 0) {
        stage->changeGimmickCameraPath(*data);
    } else {
        stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_None, reinterpret_cast<stTriggerData*>(&data->m_triggerId));
        trigger->setValidEdgeCallback(
            new (Heaps::StageInstance) stActTriggerCreteGimmick<grAdventureChangeCameraPathData, &stAdventure2::changeGimmickCameraPath>(*data));
    }
}

inline void createGimmickLadder(stAdventure2* stage, grGimmickAdventureLadderData* data) {
    data->m_motionPathData.m_motionRatio = data->m_difficultyRatios[GmGetAdventureGameLevel()];
    stage->stCommonGimmick::createGimmickLadder(data, stage->m_fileData);
}

inline void createGimmickCameraScroll(stAdventure2* stage, grGimmickCameraScroll* data) {
    stage->createGimmickCameraScroll(data);
}

inline void createGimmickTruckReactor(stAdventure2* stage, grAdventureTruckReactorData* data) {
    g_stTriggerMng->createTrigger(static_cast<Gimmick::AreaKind>(6), &data->m_triggerData)->setTruckAreaTrigger(data);
}

inline void createGimmickItemReactor(stAdventure2* stage, grAdventureItemReactorData* data) {
    if (adKeepManager::getInstance()->getKeepFlag(data->m_triggerId, data->m_keepKindByte) != 1) {
        stTrigger* trigger = g_stTriggerMng->createTrigger(static_cast<Gimmick::AreaKind>(6), &data->m_trigger);
        data->m_group = static_cast<gfArea::Group>(0x19);
        data->m_checkTargetGroup = 0x20000;
        trigger->setKeyAreaTrigger(data, Vec2f(data->m_pos.m_x, data->m_pos.m_y));
    }
}

inline void createGimmickFrameTrigger2(stAdventure2* stage, grGimmickFrameTriggerData2* data) {
    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_None, reinterpret_cast<stTriggerData*>(&data->unk4));
    trigger->setValidCallback(new (Heaps::StageInstance) stActTriggerFrameCB(data));
}

inline void createGimmickCatapult(stAdventure2* stage, grGimmickCatapultData* data) {
    stage->stCommonGimmick::createGimmickCatapult(data, stage->m_fileData);
}

// Explicit instantiations for createGimmickFunc
template void stAdventure2::createGimmickFunc<grAdventureChangeCameraPathData, &::createGimmickChangeCameraPath>(u32, stDataMultiContainer*);
template void stAdventure2::createGimmickFunc<grGimmickAdventureLadderData, &::createGimmickLadder>(u32, stDataMultiContainer*);
template void stAdventure2::createGimmickFunc<grGimmickCameraScroll, &::createGimmickCameraScroll>(u32, stDataMultiContainer*);
template void stAdventure2::createGimmickFunc<grAdventureTruckReactorData, &createGimmickTruckReactor>(u32, stDataMultiContainer*);
template void stAdventure2::createGimmickFunc<grAdventureItemReactorData, &createGimmickItemReactor>(u32, stDataMultiContainer*);
template void stAdventure2::createGimmickFunc<grGimmickFrameTriggerData2, &createGimmickFrameTrigger2>(u32, stDataMultiContainer*);
template void stAdventure2::createGimmickFunc<grGimmickCatapultData, &::createGimmickCatapult>(u32, stDataMultiContainer*);
