#include <st_pirates/gr_pirates_tower.h>
#include <st_pirates/gr_pirates_util.h>

grPiratesTower* grPiratesTower::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesTower* ground = new (Heaps::StageInstance) grPiratesTower(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesTower::~grPiratesTower() { }

// The eight gun ports of the tower hand their matrices to the cannons.
void grPiratesTower::processAnim() {
    Ground::processAnim();
    if (m_isUpdate && m_mtxGimmickWork != NULL) {
        getNodeMatrix(m_mtxGimmickWork + 8, 0, "Houdai1N");
        getNodeMatrix(m_mtxGimmickWork + 9, 0, "Houdai2N");
        getNodeMatrix(m_mtxGimmickWork + 10, 0, "Houdai3N");
        getNodeMatrix(m_mtxGimmickWork + 11, 0, "Houdai4N");
        getNodeMatrix(m_mtxGimmickWork + 12, 0, "Houdai5N");
        getNodeMatrix(m_mtxGimmickWork + 13, 0, "Houdai6N");
        getNodeMatrix(m_mtxGimmickWork + 14, 0, "Houdai7N");
        getNodeMatrix(m_mtxGimmickWork + 15, 0, "Houdai8N");
    }
}

void grPiratesTower::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        unk160 = 0;
        if (m_timer < *m_ctrlFrame) {
            unk160 = 1;
        }
        m_timer = *m_ctrlFrame;
        updateMotion(deltaFrame);
    }
}

// The tower is shown while the bombs rain (the event 7).
void grPiratesTower::updateMotion(float deltaFrame) {
    unk170 -= deltaFrame;
    if (unk170 < 0.0f) {
        unk170 = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(1, false, true, NULL);
        setVisibility(0);
        m_state = 1;
        break;
    case 1:
        if (unk160 == 1) {
            u32 next = 1;
            bool loop = true;
            switch (unk16D) {
            case 0:
                next = 1;
                break;
            }
            u8 event = *m_eventWork;
            if (event != unk16C) {
                u8 base = next;
                switch (event) {
                case 7:
                    next = 0;
                    setVisibility(1);
                    break;
                default:
                    next = 1;
                    setVisibility(0);
                    break;
                }
                if (next != base) {
                    loop = false;
                }
                unk16C = *m_eventWork;
            }
            if (next != 1) {
                setMotion(next, loop, true, &unk170);
                setMotionFrame(480.0f - *m_ctrlFrame, 0);
            }
        }
        break;
    }
}

void grPiratesTower::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    PIRATES_SET_MOTION(unk16D, 1, animId, loop, force, frameCount)
}
