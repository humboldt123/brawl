#include <st/st_trigger.h>
#include <st/st_trigger_actor_common.h>
#include <st/stage.h>
#include <types.h>

stActTriggerFrameExcelCB::stActTriggerFrameExcelCB(grGimmickFrameTriggerData* data) : unkC(0.0f), unk10(true), m_data(data) {
    m_trigger = g_stTriggerMng->createTrigger(Gimmick::Area_None, &data->unk10);
}

stActTriggerFrameExcelCB::~stActTriggerFrameExcelCB() { }

stActTriggerMotionBind::~stActTriggerMotionBind() { }

stActTriggerMotionStop::~stActTriggerMotionStop() { }

void stActTriggerMotionPathMoveStart::userProc() {
    unkC->startMove(unk10);
}

void stActTriggerMotionPathMoveStop::userProc() {
    unkC->setFrameUpdate(0.0f);
}

void stActTriggerMotionStop::userProc() {
    m_gimmick->setMotionRatio(0.0f);
}

void stActTriggerGroundValid::userProc() {
    unk10->setValidAttachedEffect(unkC);
    if (unkC == true) {
        unk10->setMotionFrame(0.0f, 0);
    }
}

void stActTriggerGfAreaValid::userProc() {
    if (unkC) {
        unk10->m_isDrawFill = true;
    } else {
        unk10->m_isDrawFill = false;
    }
}

void stActTriggerFrameExcelCB::userProc() {
    float x = m_data->unk0.m_x;
    if (x > 0.0f) {
        float delta = g_stGameFrame.m_frameDelta;
        m_data->unk0.m_x = x - delta;
    } else if (unk10) {
        if (m_trigger->m_isValid == 1) {
            if (unkC >= m_data->unk0.m_y) {
                if (m_data->unk0.m_z == 0.0f) {
                    unk10 = false;
                }
                m_trigger->setObserveFlag(false);
                unkC = 0.0f;
            }
        } else {
            if (unkC >= m_data->unk0.m_z) {
                if (m_data->unk0.m_y == 0.0f) {
                    unk10 = false;
                }
                m_trigger->setObserveFlag(true);
                unkC = 0.0f;
            }
        }
        float delta = g_stGameFrame.m_frameDelta;
        unkC += delta;
    }
}

stActTriggerFrameCB::stActTriggerFrameCB(stActTriggerFrameCBData* data) : unkC(0.0f), unk10(data) { }

stActTriggerFrameCB::~stActTriggerFrameCB() { }

void stActTriggerFrameCB::userProc() {
    if (unkC >= unk10->unk0) {
        g_stTriggerMng->setTriggerFlag(&unk10->unk8);
        stActTriggerFrameHalves trigger = unk10->parts;
        bool same;
        if (unk10->unk4 == trigger.id && unk10->unk7 == trigger.keep) {
            same = true;
        } else {
            same = false;
        }
        if (same) {
            unkC = 0.0f;
        }
    }
    float delta = g_stGameFrame.m_frameDelta;
    unkC += delta;
}

void stActTriggerMotionBind::userProc() {
    unk1C = m_gimmick->m_modelAnims[0]->getFrame();
    if (unk14 & 1) {
        m_gimmick->changeNodeAnim(unk10, 0);
    }
    if (unk14 & 2) {
        m_gimmick->changeVisibleAnim(unk10, 0);
    }
    if (unk14 & 4) {
        m_gimmick->changeTexAnim(unk10, 0);
    }
    if (unk14 & 8) {
        m_gimmick->changeTexSrtAnim(unk10, 0);
    }
    if (unk14 & 0x10) {
        m_gimmick->changeMatColAnim(unk10, 0);
    }
    if (unk14 & 0x20) {
        m_gimmick->changeShapeAnim(unk10, 0);
    }
    m_gimmick->m_modelAnims[0]->setFrame(unk1C);
    m_gimmick->setMotionRatio(m_motionRatio);
}
