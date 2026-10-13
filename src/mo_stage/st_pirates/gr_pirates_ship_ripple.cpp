#include <gr/gr_calc_world_callback.h>
#include <st_pirates/gr_pirates_ship_ripple.h>
#include <st_pirates/gr_pirates_util.h>

grPiratesShipRipple* grPiratesShipRipple::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesShipRipple* ground = new (Heaps::StageInstance) grPiratesShipRipple(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesShipRipple::grPiratesShipRipple(const char* taskName) : grPirates(taskName) {
    unk160 = 0;
    unk161 = 0;
    m_mtxWork = NULL;
    m_eventWork = NULL;
    unk16C = 0;
    unk16D = 2;
    unk170 = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grPiratesShipRipple::~grPiratesShipRipple() { }

void grPiratesShipRipple::update(float deltaFrame) {
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

// The ripple plays its waves; when the ship runs aground (the event 6) it plays the wave animation once more.
void grPiratesShipRipple::updateMotion(float deltaFrame) {
    unk170 -= deltaFrame;
    if (unk170 < 0.0f) {
        unk170 = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(0, true, true, NULL);
        m_state = 1;
    case 1: {
        if (unk160 == 1) {
            u32 next = 2;
            bool loop = true;
            switch (unk16D) {
            case 1:
                if (--unk161 == 0) {
                    next = 0;
                }
                break;
            }
            u8 event = *m_eventWork;
            if (event != unk16C) {
                u8 base = next;
                switch (event) {
                case 6:
                    unk161 = 3;
                    next = 1;
                    break;
                }
                if (next != base) {
                    loop = false;
                }
                unk16C = *m_eventWork;
            }
            if (next != 2) {
                setMotion(next, loop, true, &unk170);
                setMotionFrame(480.0f - *m_ctrlFrame, 0);
            }
        }
        break;
    }
    }
}

// The model follows the first matrix of the scene.
void grPiratesShipRipple::updateCallBack(float deltaFrame) {
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

void grPiratesShipRipple::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    PIRATES_SET_MOTION_CASES(unk16D, 2, animId, loop, force, frameCount,
        case 0:
            PIRATES_BIND_CHR(0, model, modelAnim)
            PIRATES_BIND_TEXSRT(0, model, modelAnim)
            break;
        case 1:
            PIRATES_BIND_CHR(1, model, modelAnim)
            PIRATES_BIND_VIS(0, model, modelAnim)
            PIRATES_BIND_TEXSRT(1, model, modelAnim)
            break;
        default:
            return;,
        )
}
