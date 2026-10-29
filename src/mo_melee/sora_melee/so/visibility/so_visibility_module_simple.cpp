#include <ac/ac_anim_cmd_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/event/so_event_system.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_module_hyp.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <types.h>

soVisibilityModuleSimple::soVisibilityModuleSimple(soModuleAccesser* acc, soVisibilityData* data, int unk)
    : soAnimCmdEventObserver(5, static_cast<soEventManager&>(acc->getEventManageModule()).getManageId()) {
    m_whole = true;
    m_moduleAccesser = acc;
}

soVisibilityModuleSimple::~soVisibilityModuleSimple() { }

void soVisibilityModuleSimple::activate() {
    m_whole = true;
}

void soVisibilityModuleSimple::setWhole(u8 whole) {
    m_whole = whole;
}

bool soVisibilityModuleSimple::isVisible() {
    return m_whole && m_moduleAccesser->getLinkModule().chkLinkVisibility();
}

void soVisibilityModuleSimple::update() {
    bool visible = false;
    if (getDebugModule(m_moduleAccesser).unk1C()) {
        visible = isVisible();
    }
    m_moduleAccesser->getModelModule().setVisibility(visible);
    if (m_moduleAccesser->getEffectModule().isSyncVisibility() == true) {
        m_moduleAccesser->getEffectModule().setWhole(visible);
    }
}

bool soVisibilityModuleSimple::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 4) {
        return false;
    }
    switch (acmd->getType()) {
    case 0:
    case 1:
        return true;
    case 2: {
        acCmdArg arg;
        if (acmd->getArgNum() < 1) {
            return true;
        }
        acmd->getArg(&arg, 0);
        if (arg.getArgType() != 3) {
            return true;
        }
        setWhole(arg.getBoolData());
        return true;
    }
    }
    return false;
}

bool soVisibilityModuleSimple::isObserv(char group) {
    return group == 0xB;
}

u8 soVisibilityModuleSimple::getWhole() {
    return m_whole;
}

int soVisibilityModuleSimple::get(u32) {
    return 0;
}

void soVisibilityModuleSimple::hideAll() { }

void soVisibilityModuleSimple::setDefaultAll() { }

void soVisibilityModuleSimple::setStatusDefault(u32, int, bool) { }

void soVisibilityModuleSimple::set(u32, int) { }

void soVisibilityModuleSimple::setData(soVisibilityData*) { }
