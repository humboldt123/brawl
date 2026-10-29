#include <st/st_trigger_observe.h>
#include <gf/gf_area.h>
#include <math.h>
#include <st/stage.h>
#include <types.h>

stObsTriggerCB::stObsTriggerCB(bool* flag, bool unkC) : unk8(flag), unkC(unkC) { }

void stObsTriggerCB::resetTrigger() {
    if (!unkC) {
        *unk8 = false;
    }
}

void stObsNumTriggerCB2::userProc() {
    bool allOn = true;
    bool anyOn = false;
    bool allOff = true;
    bool anyOff = false;
    for (u32 i = 0; i < m_referenceNum; i++) {
        if (*m_referenceFlags[i] == true) {
            anyOn = true;
            allOff = false;
        } else {
            allOn = false;
            anyOff = true;
        }
    }
    bool result;
    if (unk28) {
        result = !unk29 ? allOn : allOff;
    } else {
        result = !unk29 ? anyOn : anyOff;
    }
    if (result) {
        *unk8 = true;
    }
}

stObsTriggerArea::stObsTriggerArea(bool* flag, bool unkC) : stObsTriggerCB(flag, unkC) { }

void stObsTriggerArea::userProc() { }

bool stObsTriggerArea::getAreaID(int* areaID) {
    if (m_area != nullptr) {
        *areaID = m_area->m_id;
        return true;
    }
    return false;
}

void stObsTriggerArea::setAreaSleep(bool isSleep) {
    if (isSleep) {
        m_area->m_isDrawFill = false;
    } else {
        m_area->m_isDrawFill = true;
    }
}

void stObsTriggerArea::setDefaultGimmickAreaData(stSimpleAreaData* simpleAreaData, stGimmickAreaData* gimmickAreaData) {
    bool useTwoBoundingPoints = simpleAreaData->m_useTwoBoundingPoints;
    // HYPOTHESIS: the default shape value is the gimmick group, overwritten by both shape branches.
    gimmickAreaData->m_shapeType = (gfArea::ShapeType)gfArea::Stage_Group_Gimmick_Normal;
    gimmickAreaData->m_group = gfArea::Stage_Group_Space_Normal;
    gimmickAreaData->m_checkTargetGroup = 0;
    gimmickAreaData->m_checkTargetLocalGroup = 0;
    gimmickAreaData->m_16 = 0;
    gimmickAreaData->m_18 = 0;
    gimmickAreaData->unk14 = 0;
    if (useTwoBoundingPoints == true) {
        float x1 = simpleAreaData->m_pos1.m_x;
        float y1 = simpleAreaData->m_pos1.m_y;
        float x2 = simpleAreaData->m_pos2.m_x;
        float y2 = simpleAreaData->m_pos2.m_y;
        gimmickAreaData->m_shapeType = gfArea::Shape_Rectangle;
        gimmickAreaData->m_offsetPos.m_x = (x1 + x2) / 2.0f;
        gimmickAreaData->m_offsetPos.m_y = (y1 + y2) / 2.0f;
        gimmickAreaData->m_range.m_y = fabs(y2 - y1);
        gimmickAreaData->m_range.m_x = fabs(x2 - x1);
    } else if (useTwoBoundingPoints == false) {
        float x = simpleAreaData->m_pos1.m_x;
        float y = simpleAreaData->m_pos1.m_y;
        float radius = simpleAreaData->m_pos2.m_y;
        gimmickAreaData->m_shapeType = gfArea::Shape_Circle;
        gimmickAreaData->m_offsetPos.m_x = x;
        gimmickAreaData->m_offsetPos.m_y = y;
        gimmickAreaData->m_range.m_x = radius;
        gimmickAreaData->m_range.m_y = radius;
    }
}

gfArea* stObsTriggerArea::getAreaPointer() {
    return m_area;
}

void stObsTriggerArea::setArea(gfArea* area) {
    m_area = area;
}

stGimmickAreaData* stObsTriggerArea::getAreaDataPointer() {
    return &m_gimmickAreaData;
}

stObsTriggerFrameCB::stObsTriggerFrameCB(bool* flag, float frame) : stObsTriggerCB(flag, false), unk10(frame), unk14(0.0f) { }

void stObsTriggerFrameCB::userProc() {
    if (unk14 >= unk10) {
        *unk8 = true;
    }
    float delta = g_stGameFrame.m_frameDelta;
    unk14 += delta;
}

stObsTriggerCameraReactionCB::~stObsTriggerCameraReactionCB() { }

stObsTriggerPlayerIncludeAreaCB::~stObsTriggerPlayerIncludeAreaCB() { }
