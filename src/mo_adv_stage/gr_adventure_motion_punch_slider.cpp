#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_motion_punch_slider.h>

grAdventureMotionPunchSlider* grAdventureMotionPunchSlider::create(int mdlIndex, const char* taskName) {
    grAdventureMotionPunchSlider* ground = new (Heaps::StageInstance) grAdventureMotionPunchSlider(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureMotionPunchSlider::~grAdventureMotionPunchSlider() { }

void grAdventureMotionPunchSlider::updateMove(float deltaFrame) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    Vec3f rot(0.0f, 0.0f, 0.0f);
    if (m_sliderMotionPath != NULL) {
        pos = m_sliderMotionPath->getPos();
        rot = m_sliderMotionPath->getRot();
    }
    m_calcWorldCallBack.m_nodeCallbackDatas[1].m_pos = pos;
    m_calcWorldCallBack.m_nodeCallbackDatas[1].m_rot = rot;
}
