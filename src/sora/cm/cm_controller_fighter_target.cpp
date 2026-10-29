#include <cm/cm_controller_fighter_target.h>
#include <cm/cm_subject.h>
#include <nw4r/ut.h>
#include <types.h>

extern nw4r::ut::LinkList<cmSubject, 0> lbl_8049DEDC;

cmFighterTargetController::cmFighterTargetController() : gfCameraController() {
    unk8 = 0;
    m_targetIndex = 0;
    unk90 = 1.0f;
    unk10[0].unk0.m_x = unk10[0].unk0.m_y = unk10[0].unk0.m_z = 0.0f;
    unk10[0].unkC.m_x = unk10[0].unkC.m_y = 0.0f;
    unk10[0].unk14.m_x = unk10[0].unk14.m_y = 0.0f;
    unk10[0].unk1C = 200.0f;
    unk10[1].unk0.m_x = unk10[1].unk0.m_y = unk10[1].unk0.m_z = 0.0f;
    unk10[1].unkC.m_x = unk10[1].unkC.m_y = 0.0f;
    unk10[1].unk14.m_x = unk10[1].unk14.m_y = 0.0f;
    unk10[1].unk1C = 200.0f;
    unk10[2].unk0.m_x = unk10[2].unk0.m_y = unk10[2].unk0.m_z = 0.0f;
    unk10[2].unkC.m_x = unk10[2].unkC.m_y = 0.0f;
    unk10[2].unk14.m_x = unk10[2].unk14.m_y = 0.0f;
    unk10[2].unk1C = 200.0f;
    unk10[3].unk0.m_x = unk10[3].unk0.m_y = unk10[3].unk0.m_z = 0.0f;
    unk10[3].unkC.m_x = unk10[3].unkC.m_y = 0.0f;
    unk10[3].unk14.m_x = unk10[3].unk14.m_y = 0.0f;
    unk10[3].unk1C = 200.0f;
}

Vec3f cmFighterTargetController::getFighterPos() {
    u32 index = 0;
    bool foundFirst = false;
    cmSubject* target = NULL;
    cmSubject* first = NULL;
    nw4r::ut::LinkList<cmSubject, 0>* list = &lbl_8049DEDC;
    if (!list->IsEmpty()) {
        for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = list->GetBeginIter(); it != list->GetEndIter(); ++it) {
            if ((u8)(it->m_stateB + 0xFE) <= 1) {
                if (!foundFirst) {
                    first = &*it;
                    foundFirst = true;
                }
                if (index == m_targetIndex) {
                    target = &*it;
                    break;
                }
                index++;
            }
        }
    }
    if (target) {
        return target->m_pos;
    }
    m_targetIndex = 0;
    if (first) {
        return first->m_pos;
    }
    return Vec3f(0.0f, 0.0f, 0.0f);
}
