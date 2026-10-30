#include <st/st_melee.h>
#include <so/so_world.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: stMelee::getPlayerPositionCore (unnamed); the last argument selects the position kind.
extern "C" bool fn_27_239664(int playerNo, Vec3f* pos, int kind);

void stMelee::setGravityNormal() {
    g_soWorld->m_gravityUp = 1.0f;
    g_soWorld->m_gravityDown = 1.0f;
}

void stMelee::setGravityHalf() {
    g_soWorld->m_gravityUp = 0.5f;
    g_soWorld->m_gravityDown = 0.5f;
}

void stMelee::updateWind2ndOnly() {
    if (m_wind2ndTrigger && m_wind2ndData) {
        m_wind2ndTrigger->setWindParam(m_wind2ndData, 0);
    }
}

void stMelee::processBegin() {
    stCommonGimmick::processBegin();
}

void stMelee::update(float deltaFrame) { }

bool stMelee::getPlayerPosition(int playerNo, Vec3f* pos) {
    return fn_27_239664(playerNo, pos, 0);
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

void stMelee::getItemGenPac(gfArchive** archive) {
    *archive = &m_itemSheetArchive;
}
