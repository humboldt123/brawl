#define SO_AREA_ENVIROMENT_ELEMENT_CHECKER_IMPL_CT
#include <so/area/so_area_enviroment_element_checker_impl.h>
#include <types.h>

soAreaEnviromentElementCheckerImpl::soAreaEnviromentElementCheckerImpl() : m_waterAreaId(0), m_waterAreaIndex(-1), m_onMovementGroundArea(0) {}

soAreaEnviromentElementCheckerImpl::~soAreaEnviromentElementCheckerImpl() {}

void soAreaEnviromentElementCheckerImpl::unable() {
    soEnable::disable();
    clear();
}

void soAreaEnviromentElementCheckerImpl::clearWaterInfo() {
    m_waterAreaId = 0;
    m_waterAreaIndex = -1;
}

void soAreaEnviromentElementCheckerImpl::clear() {
    clearWaterInfo();
    m_onMovementGroundArea = 0;
}

bool soAreaEnviromentElementCheckerImpl::isWater() {
    return m_waterAreaId != 0;
}

bool soAreaEnviromentElementCheckerImpl::isWaterAreaIndex(int index) {
    if (m_waterAreaIndex < 0) {
        return false;
    }
    return m_waterAreaIndex == index;
}

u32 soAreaEnviromentElementCheckerImpl::getOnMovementGroundArea() {
    return m_onMovementGroundArea;
}

int soAreaEnviromentElementCheckerImpl::getWaterAreaId() {
    return m_waterAreaId;
}

bool soAreaEnviromentElementCheckerImpl::isEnable() {
    return soEnable::isEnable();
}

void soAreaEnviromentElementCheckerImpl::enable() {
    soEnable::enable();
}
