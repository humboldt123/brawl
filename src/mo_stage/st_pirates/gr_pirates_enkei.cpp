#include <gr/gr_calc_world_callback.h>
#include <st_pirates/gr_pirates_enkei.h>
#include <st_pirates/gr_pirates_util.h>

grPiratesEnkei* grPiratesEnkei::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesEnkei* ground = new (Heaps::StageInstance) grPiratesEnkei(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesEnkei::grPiratesEnkei(const char* taskName) : grPirates(taskName) {
    unk160 = 0;
    m_mtxWork = NULL;
    m_eventWork = NULL;
    unk16C = 0;
    unk170 = 0.0f;
    unk174 = 0.0f;
    unk178 = 0.0f;
    m_angleDelta = 0.0f;
    unk180 = 9;
    unk184 = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grPiratesEnkei::~grPiratesEnkei() { }

void grPiratesEnkei::update(float deltaFrame) {
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

// The view drifts around the ship: the angle turns by a fixed step each frame (it stands still while the tornado rises).
void grPiratesEnkei::updateCallBack(float deltaFrame) {
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
                if (!(*m_eventWork == 9 && *static_cast<float*>(unk15C) < 1500.0f && *static_cast<float*>(unk15C) > 480.0f)) {
                    unk174 += m_angleDelta;
                    if (unk174 < 360.0f) {
                        unk174 += 360.0f;
                    }
                    if (unk174 > 360.0f) {
                        unk174 -= 360.0f;
                    }
                }
                Matrix mtx = *m_mtxWork;
                float angle = unk174;
                mtx.rotY(angle * 0.017453292f);
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = mtx;
            }
        }
    }
}

// Follows the weather of the stage (the same table as the sea and the sky).
void grPiratesEnkei::updateMotion(float deltaFrame) {
    unk184 -= deltaFrame;
    if (unk184 < 0.0f) {
        unk184 = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(0, true, true, &unk184);
        m_state = 1;
    case 1: {
        if (unk160 != 1) {
            return;
        }
        u32 next = 9;
        bool loop = true;
        u8 current = unk180;
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
        if (event != unk16C) {
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
            unk16C = event;
        }
        if (next != 9) {
            setMotion(next, loop, true, &unk184);
            setMotionFrame(480.0f - *m_ctrlFrame, 0);
        }
        break;
    }
    }
}

void grPiratesEnkei::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    PIRATES_SET_MOTION(unk180, 9, animId, loop, force, frameCount)
}
