#include <so/event/so_event_manage_module_impl.h>
#include <types.h>

// soEventSystem::addManager and soEventSystem::eraseManager (sora_melee map), still unnamed in so_event_system.
extern "C" s16 fn_27_90238(soEventSystem* system, soEventManager& manager);
extern "C" void fn_27_90D80(soEventSystem* system, s16 manageId);

soEventManageModuleImpl::soEventManageModuleImpl(void* unitList) :
    m_unitList(static_cast<soInstanceManager<soEventUnit*>*>(unitList)), m_manageId(-1) {
    m_manageId = fn_27_90238(soEventSystem::getInstance(), *this);
}

soEventManageModuleImpl::~soEventManageModuleImpl() {
    fn_27_90D80(soEventSystem::getInstance(), m_manageId);
}

bool soEventManageModuleImpl::isExist(short id) {
    return m_unitList->isContain(id);
}

soEventUnit* soEventManageModuleImpl::getEventUnit(short id) {
    return m_unitList->at(id);
}

s16 soEventManageModuleImpl::addUnit(soEventUnit* unit) {
    return m_unitList->add(unit, unit->getEventId());
}

void soEventManageModuleImpl::eraseUnit(u32 id) {
    if (m_unitList->isContain(id)) {
        m_unitList->erase(id);
    }
}

bool soEventManageModuleImpl::isNullUnit(u32 id) {
    if (!m_unitList->isContain(id)) {
        return true;
    }
    soEventUnit* unit = m_unitList->at(id);
    if (unit == nullptr) {
        return true;
    }
    return unit->isNull();
}
