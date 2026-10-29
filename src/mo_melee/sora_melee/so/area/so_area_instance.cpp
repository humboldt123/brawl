#include <so/area/so_area_instance.h>
#include <so/area/so_area_module_impl.h>
#include <gf/gf_area.h>
#include <types.h>

void soAreaInstance::setOwnerId(int ownerId) {
    m_area->m_ownerId = ownerId;
}

void soAreaInstance::deactivate() {
    if (m_area->m_isRegist == true) {
        m_area->unRegist();
    }
}

bool soAreaInstance::isFlags(u16 mask) {
    return (m_data->m_shapeFlag.m_mask & mask) != 0;
}

u32 soAreaInstance::getId() {
    if (m_isCreated) {
        return m_area->m_id;
    }
    return 0;
}

void soAreaInstance::enable(bool isEnable, int frame) {
    if (isEnable) {
        m_area->m_isDrawFill = true;
    } else {
        m_area->m_isDrawFill = false;
    }
    m_isEnable = isEnable;
    m_enableFrame = frame;
}

void soAreaInstance::setWhole(bool isWhole) {
    if (!isWhole) {
        m_area->m_isDrawFill = false;
    }
}

void soAreaInstance::setCheckTargetGroup(int group) {
    m_area->m_checkTargetGroup = group;
}

void soAreaInstance::setCheckTargetLocalGroup(int group) {
    m_area->m_checkTargetLocalGroup = group;
}

void soAreaInstance::setTeamId(u32 teamId) {
    m_area->m_teamId = teamId;
}

u32 soAreaInstance::getTeamId() {
    return m_area->m_teamId;
}

void soAreaInstance::setShapeType(ShapeType shapeType) {
    m_shapeType = shapeType;
}
