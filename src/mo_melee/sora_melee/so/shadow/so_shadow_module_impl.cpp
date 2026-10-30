#include <so/so_module_accesser.h>
#include <so/shadow/so_shadow_module_impl.h>
#include <st/stage.h>
#include <types.h>

// stShadow::eraseShadowObject (sora_melee map 0x228C04), still unnamed in st_shadow.
extern "C" void fn_27_228C04(stShadow* shadow, nw4r::g3d::ScnMdl* scnMdl);

soShadowModuleImpl::soShadowModuleImpl(soModuleAccesser* moduleAccesser, float size, int node) :
    soStatusEventObserver(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId(), -1),
    soAnimCmdEventObserver(5, static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId()),
    soModelEventObserver(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId(), -1) {
    m_moduleAccesser = moduleAccesser;
    _0x2C = size;
    _0x30 = 4.0f;
    _0x34 = 100.0f;
    _0x38 = 0.0f;
    _0x3C = 1.5f;
    _0x40 = node;
    m_active = true;
    m_activeStatus = false;
    m_groundRef = false;
    m_groundPokemon = false;
    m_ignoreMode = 3;
    m_dispProjection = false;
}

soShadowModuleImpl::~soShadowModuleImpl() { }

void soShadowModuleImpl::activate() {
    m_activeStatus = true;
    m_dispProjection = false;
}

void soShadowModuleImpl::deactivate() {
    m_activeStatus = false;
}

void soShadowModuleImpl::setIgnoreMode(int mode) {
    m_ignoreMode = mode;
}

bool soShadowModuleImpl::isObserv(char group) {
    return group == 0x19;
}

void soShadowModuleImpl::setActiveStatus(int status) {
    m_activeStatus = status;
}

void soShadowModuleImpl::setSize(float size) {
    _0x2C = size;
}

void soShadowModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                                 soModuleAccesser* moduleAccesser) {
    setActiveStatus(true);
}

void soShadowModuleImpl::notifyEventDestructInstance(soModuleAccesser* moduleAccesser) {
    stShadow* shadow = g_Stage->m_shadow;
    fn_27_228C04(shadow, moduleAccesser->getModelModule().getScnMdl(true));
}

bool soShadowModuleImpl::isDispProjection() {
    return m_dispProjection;
}

void soShadowModuleImpl::setGroundPokemon(int groundPokemon) {
    m_groundPokemon = groundPokemon;
}

void soShadowModuleImpl::setSizeMul(float sizeMul) {
    _0x3C = sizeMul;
}

void soShadowModuleImpl::setGroundRef(int groundRef) {
    m_groundRef = groundRef;
}

void soShadowModuleImpl::setYOffset(float yOffset) {
    _0x38 = yOffset;
}

void soShadowModuleImpl::setSizeMinMax(float sizeMin, float sizeMax) {
    _0x30 = sizeMin;
    _0x34 = sizeMax;
}

void soShadowModuleImpl::setNode(int node) {
    _0x40 = node;
}

void soShadowModuleImpl::setActive(bool active) {
    m_active = active;
}
