#include <memory.h>
#include <string.h>
#include <types.h>

#include <st_homerun/gr_homerun.h>

grHomerunLoopFloor* grHomerunLoopFloor::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHomerunLoopFloor* ground = new (Heaps::StageInstance) grHomerunLoopFloor(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

inline grHomerunLoopFloor::grHomerunLoopFloor(const char* taskName) : grHomerunLoop(taskName) {
    m_index = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grHomerunLoopFloor::~grHomerunLoopFloor() { }

void grHomerunLoopFloor::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateScroll(deltaFrame);
    }
}

// The floor piece is placed behind the ground strip of the same index (once the strips know their positions).
void grHomerunLoopFloor::updateScroll(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0: {
        if (homerunIsZero(m_posGroundWork[0]) == true) {
            return;
        }
        m_posFloorWork[m_index] = m_posGroundWork[m_index];
        if (m_index == 0) {
            m_posFloorWork[m_index].m_x = m_posGroundWork[m_index].m_x - 125.0f;
            m_posFloorWork[m_index].m_x += 37.5f;
        } else {
            m_posFloorWork[m_index] = m_posFloorWork[m_index - 1];
            m_posFloorWork[m_index].m_x += 75.0f;
        }
        m_state = 1;
    }
        // FALL-THROUGH
    case 1:
        m_state = 2;
        break;
    }
}

void grHomerunLoopFloor::fixpos() {
    grHomerunLoop::fixpos();
    if (m_isUpdate) {
        fixposScroll();
        fixposCallBack();
        if (*m_scrollRateWork > 0.0f) {
            updateG3dProcCalcWorld();
            m_hasUpdatedG3dCalcWorld = false;
        }
    }
}

// The twelve floor pieces form a row: the piece that left the screen is moved behind the furthest one.
void grHomerunLoopFloor::fixposScroll() {
    switch (m_state) {
    case 2:
        if (*m_scrollRateWork > 0.0f) {
            m_posFloorWork[m_index].m_x -= *m_scrollWork;
        }
        Vec3f* floor = m_posFloorWork;
        if (floor[m_index].m_x < *m_posLimitWork - 187.5f) {
            float maxX = 0.0f;
            u8 maxIndex = 0;
            for (u8 i = 0; i < 12; i++) {
                if (floor[i].m_x > maxX) {
                    maxX = floor[i].m_x;
                    maxIndex = i;
                }
            }
            floor[m_index].m_x = 75.0f + floor[maxIndex].m_x;
            if (m_index == 0 && *m_scrollRateWork > 0.0f) {
                m_posFloorWork[m_index].m_x -= *m_scrollWork;
            }
        }
    }
}

void grHomerunLoopFloor::fixposCallBack() {
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
            Vec3f* floor = &m_posFloorWork[m_index];
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data[0].m_pos.m_x = floor->m_x;
            data[0].m_pos.m_y = floor->m_y;
            data[0].m_pos.m_z = floor->m_z;
        }
    }
}

