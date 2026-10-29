#include <so/shake/so_shake_module_simple.h>
#include <so/so_module_accesser.h>
#include <types.h>

soShakeModuleSimple::soShakeModuleSimple(soModuleAccesser* moduleAccesser, soArray<soShakeTerm>* terms, void* shakeData)
    : m_moduleAccesser(moduleAccesser) { }

soShakeModuleSimple::~soShakeModuleSimple() { }

bool soShakeModuleSimple::getOffset(Vec2f* offset) {
    return m_moduleAccesser->getLinkModule().chkLinkShake(offset);
}

bool soShakeModuleSimple::isShake() {
    return false;
}

void soShakeModuleSimple::update(bool) { }

void soShakeModuleSimple::stopKind(int kind) { }

void soShakeModuleSimple::stop() { }

void soShakeModuleSimple::extend(int kind, int frame) { }

void soShakeModuleSimple::req(int kind, int frame, bool flag, const Vec2f* offset) { }

void soShakeModuleSimple::activate() { }
