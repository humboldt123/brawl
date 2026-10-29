#include <ac/ac_anim_cmd_impl.h>
#include <gf/gf_slow_manager.h>
#include <gm/gm_global.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/event/so_event_system.h>
#include <so/link/so_link_module_impl.h>
#include <so/slow/so_slow_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

soSlowModuleImpl::soSlowModuleImpl(soModuleAccesser* acc) : soSlowModuleSimple(acc), soAnimCmdEventObserver(5, static_cast<soEventManager&>(acc->getEventManageModule()).getManageId()) {
    m_skipCount = 1;
    m_frame = 0;
    m_wholeSlowId.m_id = 0xFF;
    m_wholeFrame = 0;
    m_mag = 1;
}

soSlowModuleImpl::~soSlowModuleImpl() { }

void soSlowModuleImpl::activate() {
    soSlowModuleSimple::activate();
    m_skipCount = 1;
    m_wholeFrame = 0;
    m_frame = 0;
    m_mag = 1;
}

void soSlowModuleImpl::deactivate() {
    clearWhole();
}

void soSlowModuleImpl::clear() {
    soSlowModuleSimple::clear();
    m_frame = 0;
    m_mag = 1;
}

void soSlowModuleImpl::update(bool isUpdate) {
    if (isUpdate == true) {
        // HYPOTHESIS: link attribute 0xD makes the module follow the parent's slow motion.
        if (m_moduleAccesser->getLinkModule().chkAttribute(-1, (soLinkConnection::Attribute)0xD) == true) {
            m_moduleAccesser->getLinkModule().getLinkParentSlowInfo(&m_rate, &m_isSkip, -1);
        } else {
            int interval = m_mag / (int)g_GameGlobal->getSlowRate();
            if (interval < 1) {
                interval = 1;
            }
            if (m_frame > 0) {
                if (--m_frame <= 0) {
                    interval = 1;
                    m_mag = interval;
                    m_skipCount = interval;
                }
            }
            if (--m_skipCount <= 0) {
                m_skipCount = interval;
                m_isSkip = false;
            } else {
                m_isSkip = true;
            }
            m_rate = 1.0f / interval;
        }
    }
    if (m_wholeFrame > 0) {
        if (--m_wholeFrame <= 0) {
            clearWhole();
        }
    }
}

void soSlowModuleImpl::resetSkip() {
    if (m_mag != 1) {
        int interval = m_mag / (int)g_GameGlobal->getSlowRate();
        if (interval < 1) {
            interval = 1;
        }
        m_skipCount = interval;
    }
}

void soSlowModuleImpl::setWhole(int mag, int wholeFrame) {
    if (wholeFrame > m_wholeFrame) {
        m_wholeFrame = wholeFrame;
    }
    if (m_wholeSlowId.m_id == 0xFF) {
        m_wholeSlowId = gfSlowManager::requestSlow(mag);
    }
}

u8 soSlowModuleImpl::getWholeMag() {
    if (m_wholeSlowId.m_id != 0xFF) {
        u8 id = m_wholeSlowId.m_id;
        int rate = gfSlowManager::getSlowRate(id);
        return rate;
    }
    return 1;
}

void soSlowModuleImpl::clearWhole() {
    if (m_wholeSlowId.m_id != 0xFF) {
        u8 id = m_wholeSlowId.m_id;
        gfSlowManager::removeRequest(id);
        m_wholeSlowId.m_id = 0xFF;
    }
    m_wholeFrame = 0;
}

void soSlowModuleImpl::setWholeFrame(int wholeFrame) {
    m_wholeFrame = wholeFrame;
    if (wholeFrame <= 0) {
        clearWhole();
    }
}

// MATCH-ONLY: mirrors the inlined argument accessor (as getFrontArg in so_controller_module_impl.cpp).
static inline acCmdArg getCmdArg(const soArrayContractibleTable<const acCmdArgConv>& args, s32 index) {
    return acCmdArg(&args.at(index));
}

bool soSlowModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 2) {
        return false;
    }
    switch (acmd->getType()) {
    case 0: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        acCmdArg magArg = getCmdArg(args, 0);
        int mag = magArg.getIntData();
        acCmdArg frameArg = getCmdArg(args, 1);
        setWhole(mag, frameArg.getIntData());
        return true;
    }
    case 1:
        clearWhole();
        return true;
    }
    return false;
}

bool soSlowModuleImpl::isObserv(char group) {
    return group == 0x23;
}
