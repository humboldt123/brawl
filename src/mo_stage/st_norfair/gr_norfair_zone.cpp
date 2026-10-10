#include <ai/ai_mgr.h>
#include <ft/ft_manager.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <string.h>
#include <types.h>

#include <st_norfair/gr_norfair.h>
#include <st_norfair/gr_norfair_anim.h>

// ftManager::enumIncludeEntryId (sora_melee): lists the fighters in an area, with their places. BrawlHeaders declares another
// form of the symbol.
extern "C" int enumIncludeEntryId__9ftManagerCFi(ftManager* manager, Rect2D* area, int* entryIds, Vec3f* positions, int count);

grNorfairZone::grNorfairZone(const char* taskName) : grNorfair(taskName), m_snd(), m_subject(0, 1) {
    m_posWork = NULL;
    m_posLimitWork = NULL;
    m_posIndex = NULL;
    m_stateWork = NULL;
    m_inOutWork = NULL;
    memset(m_inTimer, 0, sizeof(m_inTimer));
    m_scaleY = 0.001f;
    m_motion = 1;
    m_frameCount = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
    m_subject.clear();
    m_subject.m_state = 1;
    m_dangerZone[0] = -1;
    m_dangerZone[1] = -1;
    m_dangerZone[2] = -1;
    m_dangerZone[3] = -1;
}

grNorfairZone* grNorfairZone::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grNorfairZone* ground = new (Heaps::StageInstance) grNorfairZone(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grNorfairZone::~grNorfairZone() {
}

void grNorfairZone::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateArea(deltaFrame);
        updateActive(deltaFrame);
        updateAI(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// A fighter in the zone is told to the stage (and stays in it for five frames after he left the area).
void grNorfairZone::updateArea(float deltaFrame) {
    m_inTimer[0] = m_inTimer[0] - deltaFrame;
    if (m_inTimer[0] < 0.0f) {
        m_inTimer[0] = 0.0f;
    }
    if (m_inTimer[0] == 0.0f) {
        m_inOutWork[0] = 0;
    }
    m_inTimer[1] = m_inTimer[1] - deltaFrame;
    if (m_inTimer[1] < 0.0f) {
        m_inTimer[1] = 0.0f;
    }
    if (m_inTimer[1] == 0.0f) {
        m_inOutWork[1] = 0;
    }
    m_inTimer[2] = m_inTimer[2] - deltaFrame;
    if (m_inTimer[2] < 0.0f) {
        m_inTimer[2] = 0.0f;
    }
    if (m_inTimer[2] == 0.0f) {
        m_inOutWork[2] = 0;
    }
    m_inTimer[3] = m_inTimer[3] - deltaFrame;
    if (m_inTimer[3] < 0.0f) {
        m_inTimer[3] = 0.0f;
    }
    if (m_inTimer[3] == 0.0f) {
        m_inOutWork[3] = 0;
    }
    switch (m_state) {
    case 7: {
        int entryIds[9];
        Vec3f positions[9];
        memset(entryIds, 0, sizeof(entryIds));
        memset(positions, 0, sizeof(positions));
        float width = 35.0f;
        float height = 20.0f;
        Rect2D area;
        Vec3f* zonePos = &m_posWork[*m_posIndex];
        float centerY = 10.0f + zonePos->m_y;
        area.m_left = zonePos->m_x - 0.5f * width;
        area.m_right = zonePos->m_x + 0.5f * width;
        area.m_up = centerY + 0.5f * height;
        area.m_down = centerY - 0.5f * height;
        int count = enumIncludeEntryId__9ftManagerCFi(g_ftManager, &area, entryIds, positions, 1);
        if (0 < count) {
            int* entryId = entryIds;
            Vec3f* position = positions;
            for (int i = 0; i != count; i++) {
                int playerNo = g_ftManager->getPlayerNo(*entryId);
                if (playerNo < 4 && position->m_x >= area.m_left && position->m_x <= area.m_right && position->m_y >= area.m_down
                    && position->m_y <= area.m_up) {
                    m_inOutWork[playerNo] = 1;
                    m_inTimer[playerNo] = 5.0f;
                }
                entryId++;
                position++;
            }
        }
        break;
    }
    }
}

void grNorfairZone::updateActive(float deltaFrame) {
    stNorfairData* data = static_cast<stNorfairData*>(getStageData());
    if (data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, false, true, NULL);
            setVisibility(0);
            setEnableCollisionStatus(false);
            *m_stateWork = 4;
            m_state = 1;
            break;
        case 1:
            if (*m_eventIDWork == 4) {
                m_timer = data->unk48;
                m_scaleY = 0.001f;
                m_state = 12;
            }
            break;
        case 12:
            if (m_timer == 0.0f) {
                setVisibility(1);
                setEnableCollisionStatus(true);
                *m_stateWork = 5;
                m_snd.playSE(static_cast<SndID>(0x1BCA), 0, 0, -1);
                m_snd.setPos(&m_posWork[*m_posIndex]);
                m_subject.m_state = 0;
                m_state = 7;
            }
            break;
        case 7:
            m_scaleY += deltaFrame * 0.041f;
            if (m_scaleY >= 1.0f) {
                m_scaleY = 1.0f;
            }
            if (*m_eventIDWork != 4) {
                m_timer = data->unk5C;
                m_state = 9;
            }
            break;
        case 9:
            if (m_timer == 0.0f) {
                setMotion(0, false, true, &m_frameCount);
                setEnableCollisionStatus(false);
                *m_stateWork = 4;
                m_snd.playSE(static_cast<SndID>(0x1BCB), 0, 0, -1);
                m_snd.setPos(&m_posWork[*m_posIndex]);
                m_subject.m_state = 1;
                m_state = 10;
            }
            break;
        case 10:
            if (getMotionFrame(0) >= m_frameCount) {
                m_state = 0;
            }
            break;
        }
    }
}

// The fighters keep out of the parts of the stage next to the zone while it is open: the AI is told four danger zones.
void grNorfairZone::updateAI(float deltaFrame) {
    Vec2f zoneB;
    Vec2f zoneA;
    switch (m_state) {
    case 7:
        zoneA.m_x = m_posLimitWork[0].m_x;
        zoneA.m_y = m_posLimitWork[0].m_y;
        zoneB.m_x = m_posLimitWork[1].m_x;
        zoneB.m_y = m_posWork[*m_posIndex].m_y + 15.0f;
        m_dangerZone[0] = g_aiMgr->setDangerZone(&zoneA, &zoneB, m_dangerZone[0], false, false);
        zoneA.m_x = m_posLimitWork[0].m_x;
        zoneA.m_y = m_posLimitWork[0].m_y;
        zoneB.m_x = m_posWork[*m_posIndex].m_x - 30.0f;
        zoneB.m_y = m_posLimitWork[1].m_y;
        m_dangerZone[1] = g_aiMgr->setDangerZone(&zoneA, &zoneB, m_dangerZone[1], false, false);
        zoneA.m_x = m_posWork[*m_posIndex].m_x + 30.0f;
        zoneA.m_y = m_posLimitWork[0].m_y;
        zoneB.m_x = m_posLimitWork[1].m_x;
        zoneB.m_y = m_posLimitWork[1].m_y;
        m_dangerZone[2] = g_aiMgr->setDangerZone(&zoneA, &zoneB, m_dangerZone[2], false, false);
        zoneA.m_x = m_posLimitWork[0].m_x;
        zoneA.m_y = m_posWork[*m_posIndex].m_y - 10.0f;
        zoneB.m_x = m_posLimitWork[1].m_x;
        zoneB.m_y = m_posLimitWork[1].m_y;
        m_dangerZone[3] = g_aiMgr->setDangerZone(&zoneA, &zoneB, m_dangerZone[3], false, false);
        break;
    default:
        if (m_dangerZone[0] != -1) {
            g_aiMgr->delDangerZone(m_dangerZone[0]);
        }
        if (m_dangerZone[1] != -1) {
            g_aiMgr->delDangerZone(m_dangerZone[1]);
        }
        if (m_dangerZone[2] != -1) {
            g_aiMgr->delDangerZone(m_dangerZone[2]);
        }
        if (m_dangerZone[3] != -1) {
            g_aiMgr->delDangerZone(m_dangerZone[3]);
        }
        m_dangerZone[0] = -1;
        m_dangerZone[1] = -1;
        m_dangerZone[2] = -1;
        m_dangerZone[3] = -1;
        break;
    }
}

void grNorfairZone::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            Vec3f* posWork = m_posWork;
            if (posWork != NULL) {
                Vec3f* pos = &posWork[*m_posIndex];
                data->m_pos.m_x = pos->m_x;
                data->m_pos.m_y = pos->m_y;
                data->m_pos.m_z = pos->m_z;
            }
            float scaleY = m_scaleY;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = 1.0f;
            data->m_scale.m_y = scaleY;
            data->m_scale.m_z = 1.0f;
            m_subject.setPos(&m_posWork[*m_posIndex]);
            m_subject.m_stateB = 0;
        }
    }
}

// The animation of the zone (the character animation only).
void grNorfairZone::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    m_motion = animId;

    if (animId >= 1) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grNorfairSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->m_anmObjChrRes->SetFrame(0.0f);
    modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    modelAnim->setLoopNode(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
    }
}
