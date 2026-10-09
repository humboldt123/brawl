#include <gr/gr_calc_world_callback.h>
#include <mt/mt_prng.h>
#include <snd/snd_id.h>
#include <st_dolpic/gr_dolpic.h>

inline grDolpicKamome::grDolpicKamome(const char* taskName) : grDolpic(taskName) {
    m_mtxWork = NULL;
    m_seTimer = randf() * 120.0f + 60.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grDolpicKamome* grDolpicKamome::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDolpicKamome* ground = new (Heaps::StageInstance) grDolpicKamome(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDolpicKamome::~grDolpicKamome() { }

void grDolpicKamome::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase();
        updateSE(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The gull squawks every few seconds (one call in four is the other one).
void grDolpicKamome::updateSE(float deltaFrame) {
    float timer = m_seTimer - deltaFrame;
    m_seTimer = timer;
    if (timer < 0.0f) {
        m_seTimer = 0.0f;
    }
    if (m_seTimer == 0.0f) {
        SndID seId;
        float rnd = randf();
        // HYPOTHESIS: the thresholds of the variants are all the same (the compiler folds the comparisons)
        if (rnd < 0.25f) {
            seId = static_cast<SndID>(0x1B4E);
        } else if (rnd < 0.25f) {
            seId = static_cast<SndID>(0x1B4F);
        } else if (rnd < 0.25f) {
            seId = static_cast<SndID>(0x1B50);
        } else {
            seId = static_cast<SndID>(0x1B51);
        }
        m_snd.playSE(seId, 0, 0, -1);
        m_seTimer = randf() * 180.0f + 180.0f;
    }
}

// Moves the gull to the matrix the plaza publishes for it; the sound follows the gull.
void grDolpicKamome::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_matrix = *m_mtxWork;
            Vec3f pos;
            getNodePosition(&pos, 0, 1);
            m_snd.setPos(&pos);
        }
    }
}
