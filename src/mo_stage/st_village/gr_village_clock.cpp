#include <memory.h>
#include <revolution/OS.h>
#include <types.h>

#include <st_village/gr_village.h>

grVillageClock::grVillageClock(const char* taskName) : grVillage(taskName) {
    m_nodeHour = 0;
    m_rotHour.m_x = 0.0f;
    m_rotHour.m_y = 0.0f;
    m_rotHour.m_z = 0.0f;
    m_rotMinute.m_x = 0.0f;
    m_rotMinute.m_y = 0.0f;
    m_rotMinute.m_z = 0.0f;
    m_nodeMinute = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 2;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x20;
    callback->m_nodeCallbackDatas[1].m_flags |= 0x20;
}

grVillageClock* grVillageClock::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageClock* ground = new (Heaps::StageInstance) grVillageClock(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageClock::~grVillageClock() {
}

void grVillageClock::update(float deltaFrame) {
    grVillage::update(deltaFrame);
    if (m_isUpdate) {
        updateClock(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hands show the time of the console (twelve hours).
void grVillageClock::updateClock(float deltaFrame) {
    // MATCH-ONLY: the constants are pooled in the order of the hour hand's calculation
    float unusedMinute = 60.0f;
    float unusedDay = 720.0f;
    float unusedTurn = -360.0f;
    OSCalendarTime time;
    OSTicksToCalendarTime(OSGetTime(), &time);
    int hour = time.hour;
    if (hour >= 12) {
        hour -= 12;
    }
    float rotHour = (((float)time.min + (float)hour * 60.0f) / 720.0f) * -360.0f;
    float rotMinute = ((float)time.min / 60.0f) * -360.0f;
    m_rotHour.m_z = rotHour;
    m_rotMinute.m_z = rotMinute;
}

void grVillageClock::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeHour;
                calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex = m_nodeMinute;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_offsetRot.m_x = m_rotHour.m_x;
            data->m_offsetRot.m_y = m_rotHour.m_y;
            data->m_offsetRot.m_z = m_rotHour.m_z;
            data = &calcWorldCallBack->m_nodeCallbackDatas[1];
            data->m_offsetRot.m_x = m_rotMinute.m_x;
            data->m_offsetRot.m_y = m_rotMinute.m_y;
            data->m_offsetRot.m_z = m_rotMinute.m_z;
        }
    }
}

bool grVillageClock::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeHour, 0, "obj_offices_kh_j");
    getNodeIndex(&m_nodeMinute, 0, "obj_offices_km_j");
    return result;
}
