#include <ft/ft_manager.h>
#include <gf/gf_model.h>
#include <math.h>
#include <memory.h>
#include <types.h>

#include <st_famicom/gr_famicom.h>

// ftManager::enumIncludeEntryId (sora_melee): lists the fighters in an area, with their places. BrawlHeaders declares another
// form of the symbol.
extern "C" int enumIncludeEntryId__9ftManagerCFi(ftManager* manager, Rect2D* area, int* entryIds, Vec3f* positions, int count);

// HYPOTHESIS: inline helper of the original (a tolerance test).
static inline bool famicomIsNearZero(float value) {
    bool result = false;
    if ((float)fabs(value) < 1e-5f) {
        result = true;
    }
    return result;
}

grFamicomBg::grFamicomBg(const char* taskName) : grFamicom(taskName) {
    m_posYukaWork = NULL;
    m_posPowWork = NULL;
    m_posEnemyWork = NULL;
    m_posBallWork = NULL;
    m_posLimitWork = NULL;
    m_landingWork = NULL;
    m_landingFlgWork = NULL;
    m_stateAreaWork = NULL;
    memset(m_nodeYuka, 0, sizeof(m_nodeYuka));
    m_nodePow = 0;
    memset(m_nodeEnemy, 0, sizeof(m_nodeEnemy));
    memset(m_nodeBall, 0, sizeof(m_nodeBall));
    memset(m_landingTimer, 0, sizeof(m_landingTimer));
}

grFamicomBg* grFamicomBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grFamicomBg* ground = new (Heaps::StageInstance) grFamicomBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grFamicomBg::~grFamicomBg() {
}

void grFamicomBg::processAnim() {
    Ground::processAnim();
    if (m_posYukaWork != NULL) {
        Vec3f* pos = m_posYukaWork;
        bool isZero = false;
        if (famicomIsNearZero(pos[0].m_x) && famicomIsNearZero(pos[0].m_y) && famicomIsNearZero(pos[0].m_z)) {
            isZero = true;
        }
        if (isZero == true) {
            getNodePosition(&m_posYukaWork[0], 0, m_nodeYuka[0]);
            getNodePosition(&m_posYukaWork[1], 0, m_nodeYuka[1]);
            m_posYukaWork[1].m_x -= 2.5f;
            getNodePosition(&m_posYukaWork[2], 0, m_nodeYuka[2]);
            m_posYukaWork[2].m_x += 2.5f;
            getNodePosition(&m_posYukaWork[3], 0, m_nodeYuka[3]);
            getNodePosition(&m_posYukaWork[4], 0, m_nodeYuka[4]);
            getNodePosition(&m_posYukaWork[5], 0, m_nodeYuka[5]);
            getNodePosition(&m_posYukaWork[6], 0, m_nodeYuka[6]);
        }
    }
    if (m_posPowWork != NULL) {
        Vec3f* pos = m_posPowWork;
        bool isZero = false;
        if (famicomIsNearZero(pos->m_x) && famicomIsNearZero(pos->m_y) && famicomIsNearZero(pos->m_z)) {
            isZero = true;
        }
        if (isZero == true) {
            getNodePosition(m_posPowWork, 0, m_nodePow);
        }
    }
    if (m_posEnemyWork != NULL) {
        Vec3f* pos = m_posEnemyWork;
        bool isZero = false;
        if (famicomIsNearZero(pos->m_x) && famicomIsNearZero(pos->m_y) && famicomIsNearZero(pos->m_z)) {
            isZero = true;
        }
        if (isZero == true) {
            getNodePosition(&m_posEnemyWork[0], 0, m_nodeEnemy[0]);
            getNodePosition(&m_posEnemyWork[1], 0, m_nodeEnemy[1]);
            getNodePosition(&m_posEnemyWork[2], 0, m_nodeEnemy[2]);
            getNodePosition(&m_posEnemyWork[3], 0, m_nodeEnemy[3]);
        }
    }
    if (m_posBallWork != NULL) {
        Vec3f* pos = m_posBallWork;
        bool isZero = false;
        if (famicomIsNearZero(pos->m_x) && famicomIsNearZero(pos->m_y) && famicomIsNearZero(pos->m_z)) {
            isZero = true;
        }
        if (isZero == true) {
            getNodePosition(&m_posBallWork[0], 0, m_nodeBall[0]);
            getNodePosition(&m_posBallWork[1], 0, m_nodeBall[1]);
            getNodePosition(&m_posBallWork[2], 0, m_nodeBall[2]);
            getNodePosition(&m_posBallWork[3], 0, m_nodeBall[3]);
        }
    }
}

void grFamicomBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateArea(deltaFrame);
    }
}

// Finds out, for each of the four floors, whether fighters stand on it (and for how long), and on which side of the stage
// most of them are (the green balls roll towards them).
#define FAMICOM_BG_AREA(index, centerY)                                                                              \
    memset(entryIds, 0, sizeof(entryIds));                                                                           \
    memset(positions, 0, sizeof(positions));                                                                         \
    result = false;                                                                                                  \
    area.m_left = centerX - width * 0.5f;                                                                            \
    area.m_right = centerX + width * 0.5f;                                                                           \
    area.m_up = height * 0.5f + centerY;                                                                             \
    area.m_down = centerY - height * 0.5f;                                                                           \
    count = enumIncludeEntryId__9ftManagerCFi(g_ftManager, &area, entryIds, positions, 1);                           \
    if (0 < count) {                                                                                                 \
        m_landingTimer[index] = 5.0f;                                                                                \
        checkLR(count, positions, &result);                                                                          \
        if (result == true) {                                                                                        \
            *m_landingFlgWork = 1;                                                                                   \
        } else {                                                                                                     \
            *m_landingFlgWork = 0;                                                                                   \
        }                                                                                                            \
    }

void grFamicomBg::updateArea(float deltaFrame) {
    if (m_landingTimer[0] > 0.0f) {
        m_landingTimer[0] = m_landingTimer[0] - deltaFrame;
    }
    if (m_landingTimer[1] > 0.0f) {
        m_landingTimer[1] = m_landingTimer[1] - deltaFrame;
    }
    if (m_landingTimer[2] > 0.0f) {
        m_landingTimer[2] = m_landingTimer[2] - deltaFrame;
    }
    if (m_landingTimer[3] > 0.0f) {
        m_landingTimer[3] = m_landingTimer[3] - deltaFrame;
    }
    if (m_landingTimer[0] > 0.0f) {
        m_landingWork[0] = m_landingWork[0] + deltaFrame;
    } else {
        m_landingWork[0] = 0.0f;
    }
    if (m_landingTimer[1] > 0.0f) {
        m_landingWork[1] = m_landingWork[1] + deltaFrame;
    } else {
        m_landingWork[1] = 0.0f;
    }
    if (m_landingTimer[2] > 0.0f) {
        m_landingWork[2] = m_landingWork[2] + deltaFrame;
    } else {
        m_landingWork[2] = 0.0f;
    }
    if (m_landingTimer[3] > 0.0f) {
        m_landingWork[3] = m_landingWork[3] + deltaFrame;
    } else {
        m_landingWork[3] = 0.0f;
    }
    if (*m_stateAreaWork == 0) {
        float centerX = 0.0f;
        float height = 25.0f;
        float width = m_posLimitWork[1].m_x - m_posLimitWork[0].m_x;
        bool result;
        Rect2D area;
        int entryIds[9];
        Vec3f positions[9];
        int count;
        FAMICOM_BG_AREA(0, 12.0f)
        FAMICOM_BG_AREA(1, 45.0f)
        FAMICOM_BG_AREA(2, 80.0f)
        FAMICOM_BG_AREA(3, 120.0f)
    }
}

bool grFamicomBg::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeYuka[0], 0, "yukaD_Position");
    getNodeIndex(&m_nodeYuka[1], 0, "yukaF_Position");
    getNodeIndex(&m_nodeYuka[2], 0, "yukaG_Position");
    getNodeIndex(&m_nodeYuka[3], 0, "yukaC_Position");
    getNodeIndex(&m_nodeYuka[4], 0, "yukaE_Position");
    getNodeIndex(&m_nodeYuka[5], 0, "yukaA_Position");
    getNodeIndex(&m_nodeYuka[6], 0, "yukaB_Position");
    getNodeIndex(&m_nodePow, 0, "powBlockPosition");
    getNodeIndex(&m_nodeEnemy[0], 0, "EnemyOutPointA");
    getNodeIndex(&m_nodeEnemy[1], 0, "EnemyOutPointB");
    getNodeIndex(&m_nodeEnemy[2], 0, "EnemyInPointA");
    getNodeIndex(&m_nodeEnemy[3], 0, "EnemyInPointB");
    getNodeIndex(&m_nodeBall[0], 0, "GreenBallPositionA");
    getNodeIndex(&m_nodeBall[1], 0, "GreenBallPositionB");
    getNodeIndex(&m_nodeBall[2], 0, "GreenBallPositionC");
    getNodeIndex(&m_nodeBall[3], 0, "GreenBallPositionD");
    return result;
}

// Counts the fighters on each side of the stage: result is false when most of them are on the right.
void grFamicomBg::checkLR(int count, Vec3f* positions, bool* result) {
    if (positions == NULL) {
        return;
    }
    if (result == NULL) {
        return;
    }
    *result = false;
    if (count == 0) {
        return;
    }
    int right = 0;
    int left = 0;
    for (int i = count; i != 0; i--) {
        if (positions->m_x > 0.0f) {
            right++;
        } else {
            left++;
        }
        positions++;
    }
    if (right > left) {
        *result = false;
        return;
    }
    *result = true;
}
