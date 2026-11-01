#include <gr/gr_gimmick_spring.h>
#include <snd/snd_system.h>
#include <types.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

struct grGimmickSpringEventArgs : soGimmickSpringEventArgs {
    grGimmickSpringEventArgs(Gimmick::EventKind kind, int sendID) : soGimmickSpringEventArgs(kind, sendID) {}
};

extern f32 lbl_27_rodata_4410;

void grGimmickSpring::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    switch (m_state) {
    case State_On: {
        m_modelAnims[0]->m_anmObjShpRes->SetFrame(m_animFrame);
        // MATCH-ONLY: the lengths convert to float as signed ints.
        if (m_animFrame > (f32)(s32)m_animOnLength) {
            grGimmickSpringEventArgs args(Gimmick::Spring_Event_Shoot, 0);
            getTopNode(&args.m_topPos);
            args.m_bounce = m_springData->m_bounce;
            args.m_rot = m_springData->m_rot;
            m_yakumono->presentEventGimmick(&args, -1);
            setMotionOff();
        } else {
            grGimmickSpringEventArgs args(Gimmick::Spring_Event_Pos, 0);
            getTopNode(&args.m_topPos);
            m_yakumono->presentEventGimmick(&args, -1);
        }
        break;
    }
    case State_Off: {
        m_modelAnims[0]->m_anmObjShpRes->SetFrame(m_animFrame);
        if (m_animFrame > (f32)(s32)m_animOffLength) {
            m_modelAnims[0]->unbindShapeAnim(m_sceneModels[0]);
            // MATCH-ONLY: the reset value is loaded before the state byte is stored.
            f32 rest = lbl_27_rodata_4410;
            m_state = State_Rest;
            m_animFrame = rest;
        }
        break;
    }
    }
    m_animFrame += deltaFrame;
    grGimmick::updateCallback(0U);
}


extern f32 lbl_27_rodata_4410;

void grGimmickSpring::setMotionOff() {
    m_modelAnims[0]->unbindShapeAnim(m_sceneModels[0]);
    changeNodeAnim(0, 0);
    changeShapeAnim(0, 0);
    g_sndSystem->playSE(static_cast<SndID>(0x226E), -1, 0, 0, -1);
    // MATCH-ONLY: the frame constant is loaded before the state store to match the original schedule.
    f32 animFrame = lbl_27_rodata_4410;
    m_state = State_Off;
    m_animFrame = animFrame;
}


bool grGimmickSpring::setSleepArea(bool unk1) {
    bool result = grYakumono::setSleepArea(unk1);
    if (unk1 == 1) {
        soGimmickEventArgs_Disable args;
        m_yakumono->presentEventGimmick(&args, -1);
    }
    return result;
}


bool grGimmickSpring::disableArea() {
    bool result = grYakumono::disableArea();
    soGimmickEventArgs_Disable args;
    m_yakumono->presentEventGimmick(&args, -1);
    return result;
}


struct grGimmickSpringPosArgs : soGimmickSpringEventArgs {
    inline grGimmickSpringPosArgs() : soGimmickSpringEventArgs(Gimmick::Spring_Event_Pos, 0) {}
};

void grGimmickSpring::presentPosEvent() {
    grGimmickSpringPosArgs args;
    getTopNode(&args.m_topPos);
    m_yakumono->presentEventGimmick(&args, -1);
}

grGimmickSpring::~grGimmickSpring() {
}
