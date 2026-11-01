#include <ec/ec_mgr.h>
#include <ef/ef_screen.h>
#include <ft/ft_external_value_accesser.h>
#include <ft/ft_manager.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <snd/snd_system.h>
#include <so/so_external_value_accesser.h>
#include <so/so_world.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <types.h>

void stMelee::setPlayerPositionIndexSerial() {
    m_playerPositionIndex[0] = 0;
    m_playerPositionIndex[1] = 1;
    m_playerPositionIndex[2] = 2;
    m_playerPositionIndex[3] = 3;
}

void stMelee::setPlayerPositionIndexFixed(int index) {
    m_playerPositionIndex[0] = index;
    m_playerPositionIndex[1] = index;
    m_playerPositionIndex[2] = index;
    m_playerPositionIndex[3] = index;
}

stMelee::~stMelee() {
    g_soWorld->m_gravityUp = 1.0f;
    g_soWorld->m_gravityDown = 1.0f;
    g_efScreen->m_notify = NULL;
    if (m_wind2ndData != NULL) {
        delete m_wind2ndData;
    }
}

void stMelee::setGravityNormal() {
    g_soWorld->m_gravityUp = 1.0f;
    g_soWorld->m_gravityDown = 1.0f;
}

void stMelee::setGravityHalf() {
    g_soWorld->m_gravityUp = 0.5f;
    g_soWorld->m_gravityDown = 0.5f;
}

grCollision* stMelee::createCollisionSelf(stCollisionWork* collisionWork, Ground* ground, const char* name1, const char* name2, u16 unk) {
    grCollision* collision;
    if (collisionWork == NULL) {
        return NULL;
    }
    grCollData* collData = new (Heaps::StageResource) grCollData;
    if (collData == NULL) {
        return NULL;
    }
    initCollData_fromCollWork(collData, collisionWork, name1, name2, unk);
    collision = grCollision::create(this, collData, ground, 1);
    setCollision(collision);
    delete collData;
    return collision;
}

void stMelee::updateWind2ndOnly() {
    if (m_wind2ndTrigger && m_wind2ndData) {
        m_wind2ndTrigger->setWindParam(m_wind2ndData, 0);
    }
}

void stMelee::zoomOutCamera(float verticalRotationFactor, float horizontalRotationFactor) {
    cmStageParam* param = m_cameraParam1;
    if (param != NULL) {
        m_savedVerticalRotationFactor = param->m_verticalRotationFactor;
        m_savedHorizontalRotationFactor = param->m_horizontalRotationFactor;
        param->m_verticalRotationFactor = verticalRotationFactor;
        param->m_horizontalRotationFactor = horizontalRotationFactor;
    }
}

void stMelee::zoomInCamera() {
    cmStageParam* param = m_cameraParam1;
    if (param != NULL) {
        param->m_verticalRotationFactor = m_savedVerticalRotationFactor;
        param->m_horizontalRotationFactor = m_savedHorizontalRotationFactor;
    }
}

void stMelee::processBegin() {
    stCommonGimmick::processBegin();
}

void stMelee::processUpdate() {
    if (m_isPaused != true) {
        float deltaFrame = g_GameGlobal->getGameFrameDelta();
        deltaFrame *= g_GameGlobal->m_stageData->m_motionRatio;
        deltaFrame *= g_GameGlobal->m_stageData->m_motionSubRatio;
        g_ecMgr->pushsetCurrentGroup(0x2000000);
        update(deltaFrame);
        g_ecMgr->popCurrentGroup();
    }
}

void stMelee::update(float deltaFrame) { }

bool stMelee::getPlayerPositionCore(int playerNo, Vec3f* pos, int kind) {
    ftManager* manager = g_ftManager;
    int entryId = manager->getEntryId(playerNo);
    if (entryId == -1) {
        return false;
    }
    if (!manager->isFighterActivate(entryId, -1)) {
        return false;
    }
    if (!manager->isFighterPositionAccessable(entryId, -1)) {
        return false;
    }
    Fighter* fighter = manager->getFighter(entryId, -1);
    if (fighter != NULL) {
        Vec3f result;
        switch (kind) {
            case 0:
                result = soExternalValueAccesser::getPos(fighter);
                break;
            case 1:
                result = ftExternalValueAccesser::getHipPos(fighter);
                break;
            case 2:
                result = ftExternalValueAccesser::getCursorPos(fighter);
                break;
        }
        *pos = result;
        return true;
    }
    return false;
}

bool stMelee::getPlayerPositionEntryId(int entryId, Vec3f* pos) {
    if (entryId == -1) {
        return false;
    }
    ftManager* manager = g_ftManager;
    if (!manager->isFighterActivate(entryId, -1)) {
        return false;
    }
    if (!manager->isFighterPositionAccessable(entryId, -1)) {
        return false;
    }
    Fighter* fighter = manager->getFighter(entryId, -1);
    if (fighter != NULL) {
        Vec3f result;
        result = soExternalValueAccesser::getPos(fighter);
        *pos = result;
        return true;
    }
    return false;
}

bool stMelee::getPlayerPosition(int playerNo, Vec3f* pos) {
    return getPlayerPositionCore(playerNo, pos, 0);
}

bool stMelee::getPlayerHipPosition(int playerNo, Vec3f* pos) {
    return getPlayerPositionCore(playerNo, pos, 1);
}

bool stMelee::getPlayerCursorPosition(int playerNo, Vec3f* pos) {
    return getPlayerPositionCore(playerNo, pos, 2);
}

void stMelee::getFighterStartPos(Vec3f* startPos, int fighterIndex) {
    // HYPOTHESIS: index 30 asks for a random restart point (compared unsigned in the original).
    if (fighterIndex == 30u) {
        u32 last = m_lastReStartPosIndex;
        u32 index;
        do {
            index = randi(4);
            m_lastReStartPosIndex = index;
        } while (index == last);
        Stage::getFighterReStartPos(startPos, index);
        startPos->m_y += 20.0f;
    } else {
        Stage::getFighterStartPos(startPos, m_playerPositionIndex[fighterIndex]);
    }
}

void stMelee::getFighterReStartPos(Vec3f* startPos, int fighterIndex) {
    // HYPOTHESIS: index 30 asks for a random restart point (compared unsigned in the original).
    if (fighterIndex == 30u) {
        u32 last = m_lastReStartPosIndex;
        u32 index;
        do {
            index = randi(4);
            m_lastReStartPosIndex = index;
        } while (index == last);
        Stage::getFighterReStartPos(startPos, index);
        startPos->m_y += 20.0f;
    } else {
        Stage::getFighterReStartPos(startPos, fighterIndex);
    }
}

int stMelee::playSeBasic(SndID id, float pan) {
    s32 handle = g_sndSystem->prepareSE(id, -1, -1);
    if (handle == -1) {
        return -1;
    }
    g_sndSystem->setPan(handle, pan);
    g_sndSystem->startSE(handle, 0, 0);
    return handle;
}

void stMelee::stopSeBasic(int handle, float fadeFrames) {
    if (handle != -1) {
        g_sndSystem->stopSE(static_cast<s32>(handle), static_cast<s32>(60.0f * fadeFrames));
    }
}

void stMelee::setSePan(int handle, float pan) {
    if (handle != -1) {
        g_sndSystem->setPan(handle, pan);
    }
}

bool stMelee::checkChangeScene() {
    return false;
}

void stMelee::resetChangeScene() { }

void stMelee::setChangeSceneNumber(s32 n) { }

void stCollisionWork::initialize() {
    m_vtxDatas = NULL;
    m_lineDatas = NULL;
    m_jointDatas = NULL;
    m_vtxLen = 0;
    m_isClosed = false;
}

void stCollisionWork::destroy() {
    // BYTES: the three tables are freed with a plain __dla (no array cookie).
    if (m_vtxDatas != NULL) {
        delete[] reinterpret_cast<u8*>(m_vtxDatas);
    }
    if (m_lineDatas != NULL) {
        delete[] reinterpret_cast<u8*>(m_lineDatas);
    }
    if (m_jointDatas != NULL) {
        delete[] reinterpret_cast<u8*>(m_jointDatas);
    }
    m_vtxDatas = NULL;
    m_lineDatas = NULL;
    m_jointDatas = NULL;
}

bool stCollisionWork::isValid() {
    return m_vtxLen != 0;
}

void stMelee::getItemGenPac(gfArchive** archive) {
    *archive = &m_itemSheetArchive;
}
