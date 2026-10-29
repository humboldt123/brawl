#include <so/link/so_link_module_impl.h>
#include <so/slow/so_slow_module_simple.h>
#include <so/so_module_accesser.h>
#include <types.h>

soSlowModuleSimple::soSlowModuleSimple(soModuleAccesser* moduleAccesser) : m_moduleAccesser(moduleAccesser), m_rate(1.0f), m_isSkip(false) { }

soSlowModule::~soSlowModule() { }

soSlowModuleSimple::~soSlowModuleSimple() { }

void soSlowModuleSimple::activate() {
    m_isSkip = false;
    m_rate = 1.0f;
    clear();
}

void soSlowModuleSimple::clear() { }

void soSlowModuleSimple::update(bool isUpdate) {
    if (isUpdate == true) {
        if (m_moduleAccesser->getLinkModule().isLinkParentSlow(-1) == true) {
            m_moduleAccesser->getLinkModule().getLinkParentSlowInfo(&m_rate, &m_isSkip, -1);
        } else {
            m_isSkip = false;
            m_rate = 1.0f;
        }
    }
}

int soSlowModuleSimple::getWholeFrame() {
    return 0;
}

void soSlowModuleSimple::setWholeFrame(int wholeFrame) { }

u8 soSlowModuleSimple::getWholeMag() {
    return 1;
}

void soSlowModuleSimple::clearWhole() { }

void soSlowModuleSimple::setWhole(int, int wholeFrame) { }

int soSlowModuleSimple::getFrame() {
    return 0;
}

int soSlowModuleSimple::getMag() {
    return 1;
}

void soSlowModuleSimple::set(int mag, int frame) { }

void soSlowModuleSimple::resetSkip() { }

void soSlowModuleSimple::deactivate() { }
