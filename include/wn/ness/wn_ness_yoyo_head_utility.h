#pragma once

#include <so/so_module_accesser.h>
#include <types.h>

// The three yo-yo head statuses end their position fix the same way: the position is pulled back to the corrected ground
// position on the axes the head touches the ground on, then the node transform is refreshed.
inline void wnNessYoyoHeadFixGroundPos(soModuleAccesser* moduleAccesser) {
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    Vec3f pos = posture.getPos();
    Vec3f corrected = ground.getCorrectPos(0);
    if (ground.isTouch((grCollStatus::TouchMask)9, 0)) {
        pos.m_y = corrected.m_y;
    }
    if (ground.isTouch((grCollStatus::TouchMask)6, 0)) {
        pos.m_x = corrected.m_x;
    }
    posture.setPos(&pos);
    moduleAccesser->getStageObject().updateNodeSRT();
}
