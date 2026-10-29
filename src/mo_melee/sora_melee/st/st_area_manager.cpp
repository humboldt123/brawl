#include <gf/gf_task.h>
#include <sr/sr_common.h>
#include <st/st_area_manager.h>
#include <types.h>

template <class T> bool stAreaManager::search(T* areas, u32 areaId, u32* index) {
    u32 i;
    for (i = 0; i < m_size; i++) {
        if (areas[i].m_areaId != 0 && areaId == areas[i].m_areaId) {
            *index = i;
            return true;
        }
    }
    for (i = 0; i < m_size; i++) {
        if (areas[i].m_areaId == 0) {
            *index = i;
            return false;
        }
    }
    *index = m_size;
    return false;
}

stAreaManager::~stAreaManager() { }

void stAreaManager::clearAll() {
    m_size = 0;
    if (m_windAreas) {
        delete[] m_windAreas;
    }
    if (m_beltAreas) {
        delete[] m_beltAreas;
    }
    if (m_waterAreas) {
        delete[] m_waterAreas;
    }
    if (m_hitPointEffectAreas) {
        delete[] m_hitPointEffectAreas;
    }
    m_windAreas = nullptr;
    m_beltAreas = nullptr;
    m_waterAreas = nullptr;
    m_hitPointEffectAreas = nullptr;
}

u32 stAreaManager::setWind(stMovementAreaInfoData* data) {
    u32 index;
    search(m_windAreas, data->m_areaId, &index);
    if (index < m_size) {
        m_windAreas[index] = *data;
    }
    return index;
}

u32 stAreaManager::setBelt(stMovementAreaInfoData* data) {
    u32 index;
    search(m_beltAreas, data->m_areaId, &index);
    if (index < m_size) {
        m_beltAreas[index] = *data;
    }
    return index;
}

u32 stAreaManager::setWater(stWaterAreaInfoData* data) {
    u32 index;
    search(m_waterAreas, data->m_areaId, &index);
    if (index < m_size) {
        m_waterAreas[index] = *data;
    }
    return index;
}

u32 stAreaManager::setHitPointEffect(stHitPointEffectAreaInfoData* data) {
    u32 index;
    search(m_hitPointEffectAreas, data->m_areaId, &index);
    if (index < m_size) {
        m_hitPointEffectAreas[index] = *data;
    }
    return index;
}

void stAreaManager::setWindParam(stMovementAreaInfoData* data, u32 index) {
    if (index < m_size) {
        m_windAreas[index] = *data;
    }
}

void stAreaManager::setBeltParam(stMovementAreaInfoData* data, u32 index) {
    if (index < m_size) {
        m_beltAreas[index] = *data;
    }
}

void stAreaManager::setWaterParam(stWaterAreaInfoData* data, u32 index) {
    if (index < m_size) {
        m_waterAreas[index] = *data;
    }
}

void stAreaManager::erase(u32 areaId) {
    u32 index;
    if (m_size != 0) {
        if (search(m_windAreas, areaId, &index)) {
            m_windAreas[index].m_areaId = 0;
        }
        if (search(m_beltAreas, areaId, &index)) {
            m_beltAreas[index].m_areaId = 0;
        }
        if (search(m_waterAreas, areaId, &index)) {
            m_waterAreas[index].m_areaId = 0;
        }
        if (search(m_hitPointEffectAreas, areaId, &index)) {
            m_hitPointEffectAreas[index].m_areaId = 0;
        }
    }
}

bool stAreaManager::getMovementArea(u32 areaId, stMovementData* data) {
    u32 index;
    if (search(m_windAreas, areaId, &index)) {
        *data = m_windAreas[index].m_data;
        return true;
    }
    if (search(m_beltAreas, areaId, &index)) {
        *data = m_beltAreas[index].m_data;
        return true;
    }
    return false;
}

bool stAreaManager::getHitPointEffect(u32 areaId, stHitPointEffectData* data) {
    u32 index;
    if (search(m_hitPointEffectAreas, areaId, &index)) {
        *data = m_hitPointEffectAreas[index].m_data;
        return true;
    }
    return false;
}
