#include <ft/ft_manager.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <string.h>

#include <st_dxyorster/gr_dxyorster.h>
#include <st_dxyorster/gr_dxyorster_anim.h>

grDxYorster* grDxYorster::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxYorster* ground = new (Heaps::StageInstance) grDxYorster(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxYorster::grDxYorster(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grDxYorster::~grDxYorster() { }

// ---- Bg ----

// HYPOTHESIS: the overload of ftManager::enumIncludeEntryId that counts the fighters inside a rectangle (the headers only
// know the one that takes an entry id; this is the same symbol of sora_melee, which tail-calls the entry manager).
extern "C" int enumIncludeEntryId__9ftManagerCFi(const ftManager* manager, Rect2D* area, int* out, int unk1, int unk2);
static inline int bgEnumIncludeEntryId(const ftManager* manager, Rect2D* area, int* out, int unk1, int unk2) {
    return enumIncludeEntryId__9ftManagerCFi(manager, area, out, unk1, unk2);
}

static inline bool dxYorsterNearZero(float v) {
    bool result = false;
    if ((float)fabs(v) < 1e-5f) {
        result = true;
    }
    return result;
}

// MATCH-ONLY: the rectangle of the area is built from a centre and a size inside a helper (the halving is not folded).
static inline void dxYorsterMakeArea(Rect2D* area, Vec3f pos, float offsetY, float width, float height) {
    float centerY = pos.m_y - offsetY;
    area->m_left = pos.m_x - 0.5f * width;
    area->m_right = pos.m_x + 0.5f * width;
    area->m_up = centerY + 0.5f * height;
    area->m_down = centerY - 0.5f * height;
}

grDxYorsterBg* grDxYorsterBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxYorsterBg* ground = new (Heaps::StageInstance) grDxYorsterBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxYorsterBg::~grDxYorsterBg() { }

void grDxYorsterBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateArea(deltaFrame);
    }
}

// Keeps a timer for each half of the island that runs for 5 frames after a fighter stood at the matching block node.
void grDxYorsterBg::updateArea(float deltaFrame) {
    m_timerL -= deltaFrame;
    if (m_timerL < 0.0f) {
        m_timerL = 0.0f;
    }
    m_timerR -= deltaFrame;
    if (m_timerR < 0.0f) {
        m_timerR = 0.0f;
    }
    if (m_timerL == 0.0f) {
        *m_stateLWork = 1;
    } else {
        *m_stateLWork = 0;
    }
    if (m_timerR == 0.0f) {
        *m_stateRWork = 1;
    } else {
        *m_stateRWork = 0;
    }
    u8 unset = 0;
    if (dxYorsterNearZero(m_posGimmickWork[3].m_x) && dxYorsterNearZero(m_posGimmickWork[3].m_y) &&
        dxYorsterNearZero(m_posGimmickWork[3].m_z)) {
        unset = 1;
    }
    if (unset != 1) {
        Rect2D area;
        int fighterIds[9];
        memset(fighterIds, 0, sizeof(fighterIds));
        dxYorsterMakeArea(&area, m_posGimmickWork[3], 10.0f, 10.0f, 20.0f);
        if (bgEnumIncludeEntryId(g_ftManager, &area, fighterIds, 0, 1) > 0) {
            m_timerL = 5.0f;
        }
        memset(fighterIds, 0, sizeof(fighterIds));
        dxYorsterMakeArea(&area, m_posGimmickWork[5], 10.0f, 10.0f, 20.0f);
        if (bgEnumIncludeEntryId(g_ftManager, &area, fighterIds, 0, 1) > 0) {
            m_timerR = 5.0f;
        }
    }
}
