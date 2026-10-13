#include <gr/gr_calc_world_callback.h>
#include <st_pirates/gr_pirates_sea_sky.h>
#include <st_pirates/gr_pirates_util.h>

grPiratesSeaSky* grPiratesSeaSky::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesSeaSky* ground = new (Heaps::StageInstance) grPiratesSeaSky(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesSeaSky::grPiratesSeaSky(const char* taskName) : grPirates(taskName) {
    unk160 = 0;
    m_mtxWork = NULL;
    m_mtxGimmickWork = NULL;
    m_eventWork = NULL;
    unk170 = 0;
    unk171 = 9;
    unk174 = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grPiratesSeaSky::~grPiratesSeaSky() { }

void grPiratesSeaSky::processAnim() {
    Ground::processAnim();
    if (m_isUpdate && m_mtxGimmickWork != NULL) {
        getNodeMatrix(m_mtxGimmickWork + 5, 0, "EnkeiN");
        getNodeMatrix(m_mtxGimmickWork + 6, 0, "Suimen");
    }
}

void grPiratesSeaSky::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        unk160 = 0;
        if (m_timer < *m_ctrlFrame) {
            unk160 = 1;
        }
        m_timer = *m_ctrlFrame;
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Follows the weather of the stage: the animation that comes after the current one when the loop starts again, or the
// one that the new weather asks for.
void grPiratesSeaSky::updateMotion(float deltaFrame) {
    unk174 -= deltaFrame;
    if (unk174 < 0.0f) {
        unk174 = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(0, true, true, &unk174);
        m_state = 1;
    case 1: {
        if (unk160 != 1) {
            return;
        }
        u32 next = 9;
        bool loop = true;
        u8 current = unk171;
        switch (current) {
        case 3:
        case 8:
            next = 1;
            break;
        case 4:
        case 6:
            next = 2;
            break;
        case 5:
        case 7:
            next = 0;
            break;
        }
        u8 event = *m_eventWork;
        if (event != unk170) {
            u8 base = next;
            switch (event) {
            case 1:
                switch (current) {
                case 1:
                    next = 5;
                    break;
                case 2:
                    next = 7;
                    break;
                }
                break;
            case 2:
                switch (current) {
                case 0:
                    next = 3;
                    break;
                case 1:
                    break;
                case 2:
                    next = 8;
                    break;
                }
                break;
            case 3:
            case 5:
                switch (current) {
                case 0:
                    next = 4;
                    break;
                case 1:
                    next = 6;
                    break;
                }
                break;
            }
            if (next != base) {
                loop = false;
            }
            unk170 = event;
        }
        if (next != 9) {
            setMotion(next, loop, true, &unk174);
            setMotionFrame(480.0f - *m_ctrlFrame, 0);
        }
        break;
    }
    }
}

// The model follows the first matrix of the scene.
void grPiratesSeaSky::updateCallBack(float deltaFrame) {
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
            if (m_mtxWork != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *m_mtxWork;
            }
        }
    }
}

void grPiratesSeaSky::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    PIRATES_SET_MOTION(unk171, 9, animId, loop, force, frameCount)
}
