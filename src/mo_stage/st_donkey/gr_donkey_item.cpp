#include <ft/ft_manager.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <string.h>

#include <st_donkey/gr_donkey.h>

// HYPOTHESIS: the overload of ftManager::enumIncludeEntryId that counts the fighters inside a rectangle (the headers only
// know the one that takes an entry id; this is the same symbol of sora_melee, which tail-calls the entry manager).
extern "C" int enumIncludeEntryId__9ftManagerCFi(const ftManager* manager, Rect2D* area, int* out, int unk1, int unk2);
static inline int itemEnumIncludeEntryId(const ftManager* manager, Rect2D* area, int* out, int unk1, int unk2) {
    return enumIncludeEntryId__9ftManagerCFi(manager, area, out, unk1, unk2);
}

inline grDonkeyItem::grDonkeyItem(const char* taskName) : grDonkey(taskName) {
    m_posWork = NULL;
    m_stateWork = NULL;
    m_type = 8;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
    }
}

grDonkeyItem* grDonkeyItem::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDonkeyItem* ground = new (Heaps::StageInstance) grDonkeyItem(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDonkeyItem::~grDonkeyItem() {
}

void grDonkeyItem::update(float deltaFrame) {
    if (m_isUpdate) {
        updateArea();
        updateScaleBase(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// MATCH-ONLY: the rectangle of the area is built from a centre and a size inside a helper (the halving is not folded).
static inline void itemMakeArea(Rect2D* area, Vec3f pos, float offsetY, float width, float height) {
    float centerY = pos.m_y - offsetY;
    area->m_left = pos.m_x - 0.5f * width;
    area->m_right = pos.m_x + 0.5f * width;
    area->m_up = centerY + 0.5f * height;
    area->m_down = centerY - 0.5f * height;
}

// While the item is on offer (state 8) a fighter in the small area around it takes it (3 becomes 4 in the state byte).
void grDonkeyItem::updateArea() {
    switch (m_state) {
    case 8: {
        Rect2D area;
        int fighterIds[9];
        memset(fighterIds, 0, sizeof(fighterIds));
        itemMakeArea(&area, *m_posWork, -7.5f, 10.0f, 10.0f);
        if (itemEnumIncludeEntryId(g_ftManager, &area, fighterIds, 0, 1) > 0 && *m_stateWork == 3) {
            *m_stateWork = 4;
        }
        break;
    }
    }
}

// The life of an item: 0 hides it and waits, 1 shows it when the wait is over and offers it (state byte 3), 8 hides it
// when it is taken (4 or 5), 9 waits for the stage to confirm (6) and starts the next wait.
// The stage data holds the waits at 23 to 26.
void grDonkeyItem::updateActive(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setVisibility(0);
            m_state = 1;
            m_timer = data[23] + (data[24] - data[23]) * randf();
            break;
        case 1:
            if (m_timer == 0.0f) {
                *m_stateWork = 3;
                setVisibility(1);
                m_state = 8;
            }
            break;
        case 8:
            if (*m_stateWork >= 4 && *m_stateWork <= 5) {
                setVisibility(0);
                m_state = 9;
            }
            break;
        case 9:
            if (*m_stateWork == 6) {
                m_state = 1;
                m_timer = data[25] + (data[26] - data[25]) * randf();
            }
            break;
        }
    }
}

void grDonkeyItem::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f* posWork = m_posWork;
            grNodeCallbackData* data;
            if (posWork != NULL) {
                data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = posWork->m_x;
                data->m_pos.m_y = posWork->m_y;
                data->m_pos.m_z = posWork->m_z;
            }
            Vec3f scale;
            donkeyVec3Scale(&scale, &m_scaleBase, 0.9f);
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}

// ---- ItemScore ----

inline grDonkeyItemScore::grDonkeyItemScore(const char* taskName) : grDonkeyItem(taskName) {
}

grDonkeyItemScore* grDonkeyItemScore::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDonkeyItemScore* ground = new (Heaps::StageInstance) grDonkeyItemScore(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDonkeyItemScore::~grDonkeyItemScore() {
}

void grDonkeyItemScore::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The score of an item: it shows (with a sound) for one second when the item is taken and tells the stage when it is gone (6).
void grDonkeyItemScore::updateActive(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setVisibility(0);
            m_state = 1;
            break;
        case 1:
            if (*m_stateWork >= 4 && *m_stateWork <= 5) {
                setVisibility(1);
                m_snd.playSE(SndID(0x1B89), 0, 0, -1);
                m_snd.setPos(m_posWork);
                m_state = 8;
                m_timer = 60.0f;
            }
            break;
        case 8:
            if (m_timer == 0.0f) {
                *m_stateWork = 6;
                m_state = 0;
            }
            break;
        }
    }
}
