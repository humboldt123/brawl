#include <cm/cm_controller_ai.h>
#include <gm/gm_global.h>
#include <st/stage.h>
#include <types.h>

// HYPOTHESIS: GameGlobal::getGameFrameDelta (main.dol, unnamed).
extern "C" float fn_8004E94C(GameGlobal* global);

void Stage::processBegin() {
    g_stGameFrame.m_frameDelta = fn_8004E94C(g_GameGlobal);
    g_stGameFrame.m_isJust = g_GameGlobal->isJustGameFrame();
    if (m_shadow) {
        m_shadow->clearShadowObject();
    }
    switch (unk40) {
        case 0:
            unk40 = 1;
        case 1:
            if (loading()) {
                createObj();
                unk40 = 2;
            }
            break;
        case 3:
            unk40 = 4;
            break;
        case 4:
            process();
            break;
    }
}

void Stage::processUpdate() { }

void Stage::processMapCorrection() { }

void Stage::renderPre() {
    if (m_shadow) {
        m_shadow->clearShadowObject();
    }
}

bool Stage::loading() {
    return true;
}

void Stage::startLoadLocalData() { }

bool Stage::isLoadLocalData() {
    return true;
}

int Stage::entryLocalData() {
    return 1;
}

void Stage::removeLocalData() { }

void Stage::createObj() { }

void Stage::process() { }

int Stage::getZoneLightSetIndex() {
    return 0x14;
}

u32 Stage::getGroundNum() {
    u32 num = 0;
    for (Ground* ground = m_headGround; ground; ground = ground->m_nextGround) {
        num++;
    }
    return num;
}

Ground* Stage::getGround(int index) {
    Ground* ground = m_headGround;
    while (index != 0 && ground) {
        ground = ground->m_nextGround;
        index--;
    }
    return ground;
}

void Stage::setCollision(grCollision* coll) { }

void Stage::clearShadow() {
    if (m_shadow) {
        m_shadow->clearShadowObject();
    }
}

void Stage::renderDebug() { }

void Stage::renderDebugPositions() { }

bool Stage::isReStartSamePoint() {
    return false;
}

u8 Stage::getItemPosCount() {
    if (m_stagePositions == NULL) {
        return 0;
    }
    return m_stagePositions->m_itemPosCount;
}

u8 Stage::getPokeTrainerPosCount() {
    return m_pokeTrainerPosCount;
}

void Stage::enableDevil() {
    m_isDevil = true;
    g_cmAIController->unk94_80 = true;
}

void Stage::disableDevil() {
    m_isDevil = false;
    g_cmAIController->unk94_80 = false;
}

bool Stage::isDevil() {
    return m_isDevil;
}

void Stage::getLucarioFinalTechniquePosition(Vec3f* pos) {
    pos->m_x = 0.0f;
    pos->m_y = 100.0f;
    pos->m_z = 0.0f;
}

bool Stage::isEnd() {
    return false;
}

bool Stage::isEventEnd(int param1, int* eventState, int* eventDecision) {
    return false;
}

GXColor Stage::getFinalTechniqColor() {
    GXColor color = { 0x14, 0x00, 0x04, 0xB8 };
    return color;
}

void Stage::notifyEventInfoReady() { }

void Stage::notifyEventInfoGo() { }

void Stage::getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID) { }

void Stage::getItemGenPac(gfArchive** archive) { }

void Stage::getItemPacEnemyFigure(gfArchive** archive) { }

void Stage::getEnemyPac(gfArchive** brres, gfArchive** param, gfArchive** enmCommon, gfArchive** primFaceBrres, EnemyKind enemyKind) { }

void Stage::getAdvRadarData(gfArchive**, gfArchive**) { }

void Stage::stAdventureEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId) { }

bool Stage::startAppear() {
    return false;
}

bool Stage::isAppear() {
    return false;
}

bool Stage::isStartAppearTimming() {
    return true;
}

void Stage::endAppear() { }

void Stage::setAppearKind(u8 kind) { }

void* Stage::getMadeinAiData() {
    return NULL;
}

bool Stage::isBamperVector() {
    return false;
}

void Stage::getBamperVector(Vec3f* vec) {
    if (vec) {
        vec->m_x = 0.0f;
        vec->m_y = 0.0f;
        vec->m_z = 0.0f;
    }
}

int Stage::getZoneState() {
    return 0;
}

void Stage::getZonePos(Vec3f* pos) { }

float Stage::getMagmaHeight() {
    return 0.0f;
}

float Stage::getAcidHeight() {
    return 0.0f;
}

u8 Stage::getIteamDropStatus() {
    return 1;
}

void Stage::setVision(u8) { }

IfSmashAppearTask* Stage::getAppearTask() {
    return NULL;
}

void Stage::forceStopAppear() { }

void Stage::updateWind2ndOnly() { }

grGimmickWindData2nd* Stage::getWind2ndOnlyData() {
    return NULL;
}

bool Stage::createWind2ndOnly() {
    return false;
}
