#include <so/area/so_area_module_impl.h>
#include <so/area/so_area_instance.h>
#include <types.h>

void soAreaModuleImpl::deactivate() {
    setWhole(false);
    int num = m_areaInstanceArray->size();
    for (int i = 0; i < num; i++) {
        if (m_areaInstanceArray->at(i).m_isCreated) {
            m_areaInstanceArray->at(i).deactivate();
        }
    }
}

void soAreaModuleImpl::setWhole(bool isWhole) {
    if (m_isWhole != isWhole) {
        int num = m_areaInstanceArray->size();
        for (int i = 0; i < num; i++) {
            if (m_areaInstanceArray->at(i).m_isCreated) {
                m_areaInstanceArray->at(i).setWhole(isWhole);
            }
        }
        m_isWhole = isWhole;
        m_areaContactLogArray->clear();
        m_enviromentElementChecker->clear();
    }
}

void soAreaModuleImpl::clearLog() {
    m_areaContactLogArray->clear();
}

void soAreaModuleImpl::check() {
    if (m_enviromentElementChecker->isEnable() == true) {
        m_enviromentElementChecker->clear();
        m_enviromentElementChecker->check(m_moduleAccesser);
    }
}

bool soAreaModuleImpl::isEnableArea(int index) {
    if (index < 0 || index >= m_areaInstanceArray->size()) {
        return false;
    }
    return m_areaInstanceArray->at(index).m_isEnable;
}

void soAreaModuleImpl::setAreaTargetGroupMask(u32 index, u32 mask) {
    m_areaInstanceArray->at(index).setCheckTargetGroup(mask);
}

void soAreaModuleImpl::setAreaTargetLocalGroupMask(u32 index, u32 mask) {
    m_areaInstanceArray->at(index).setCheckTargetLocalGroup(mask);
}

void soAreaModuleImpl::setAreaShapeType(u32 index, soAreaInstance::ShapeType shapeType) {
    m_areaInstanceArray->at(index).setShapeType(shapeType);
}

void soAreaModuleImpl::setAreaDbgColor(u32 index, GXColor color) {}

bool soAreaModuleImpl::isExistAreaInstance(int index) {
    if (index < 0 || index >= m_areaInstanceArray->size()) {
        return false;
    }
    return m_areaInstanceArray->at(index).m_area != NULL;
}

int soAreaModuleImpl::getAreaIndexFromId(int id) {
    int num = m_areaInstanceArray->size();
    for (int i = 0; i < num; i++) {
        if (id == m_areaInstanceArray->at(i).getId()) {
            return i;
        }
    }
    return -1;
}

s32 soAreaModuleImpl::getContactLogCount() {
    return m_areaContactLogArray->size();
}

soAreaContactLog* soAreaModuleImpl::getContactLog(s32 index) {
    return &m_areaContactLogArray->at(index);
}

u32 soAreaModuleImpl::getAreaContactCount(int areaId) {
    if (areaId == -1) {
        return m_areaContactLogArray->size();
    }
    u32 count = 0;
    int num = m_areaContactLogArray->size();
    for (int i = 0; i < num; i++) {
        if (areaId == m_areaContactLogArray->at(i).m_area) {
            count++;
        }
    }
    return count;
}

soAreaContactLog* soAreaModuleImpl::getAreaContactLog(int areaId, int index) {
    if (areaId == -1) {
        return &m_areaContactLogArray->at(index);
    }
    int count = 0;
    int num = m_areaContactLogArray->size();
    for (int i = 0; i < num; i++) {
        if (areaId == m_areaContactLogArray->at(i).m_area) {
            if (count == index) {
                return &m_areaContactLogArray->at(i);
            }
            count++;
        }
    }
    return NULL;
}

bool soAreaModuleImpl::isObserv(char unk1) {
    return unk1 == 0x14;
}

void soAreaModuleImpl::setLogOverThrough(bool logOverThrough) {
    m_logOverThrough = logOverThrough;
}

soAreaEnviromentElementChecker* soAreaModuleImpl::getEnviromentElementChecker() {
    return m_enviromentElementChecker;
}
