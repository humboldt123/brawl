#define SO_TURN_MODULE_IMPL_FULL
#include <so/turn/so_turn_module_impl.h>
#include <types.h>

void soTurnModuleImpl::activate() {
    end();
}

void soTurnModuleImpl::set(const soTurnData* data, bool changeLr, bool isExtern, float lr) {
    m_state = 1;
    m_turnData = data;
    m_changeLr = changeLr;
    unk24 = 0;
    m_lrReversed = false;
    m_lr = lr;
    m_isExtern = isExtern;
}

bool soTurnModuleImpl::isObserv(char group) {
    return group == 0x20;
}
