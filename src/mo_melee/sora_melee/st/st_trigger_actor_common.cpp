#include <st/st_trigger_actor_common.h>
#include <st/st_trigger.h>
#include <types.h>

stActTriggerFrameExcelCB::stActTriggerFrameExcelCB(grGimmickFrameTriggerData* data) : unk8(0), unkC(0.0f), unk10(true), m_data(data) {
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
