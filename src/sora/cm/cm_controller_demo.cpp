#include <cm/cm_controller_demo.h>
#include <cm/cm_subject.h>
#include <nw4r/ut.h>
#include <types.h>

extern nw4r::ut::LinkList<cmSubject, 0> lbl_8049DEDC;

cmDemoController::cmDemoController() : gfCameraController() {
    m_targetType = 0;
    unkC = 0.0f;
    Rect2D range = { -1.0f, 1.0f, 1.0f, -1.0f };
    unk20.m_left = range.m_left;
    unk20.m_right = range.m_right;
    unk20.m_up = range.m_up;
    unk20.m_down = range.m_down;
    unk14 = 0.0f;
    unk18 = 0.0f;
    unk4C = -1;
    unk50 = -1;
    unk54 = 30.0f;
    unk58 = 1.0f;
    unk48 = 1.0471976f;
    unk10 = 0;
}

cmDemoController::~cmDemoController() { }

cmSubject* cmDemoController::getFighterSubject(int playerNo) {
    if (playerNo < 0) {
        return NULL;
    }
    nw4r::ut::LinkList<cmSubject, 0>* list = &lbl_8049DEDC;
    if (!list->IsEmpty()) {
        for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = list->GetBeginIter(); it != list->GetEndIter(); ++it) {
            if ((u8)(it->m_stateB + 0xFE) <= 1 && playerNo == (int)it->m_playerNo) {
                return &*it;
            }
        }
    }
    return NULL;
}

bool cmDemoController::isForceCamControl3D() {
    return m_targetType == 9;
}
