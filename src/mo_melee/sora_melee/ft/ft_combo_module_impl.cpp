#include <ac/ac_anim_cmd_impl.h>
#include <ft/ft_combo_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

ftComboModuleImpl::ftComboModuleImpl(soModuleAccesser* moduleAccesser) :
    soStatusEventObserver(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId(), -1),
    soAnimCmdEventObserver(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId(), (s8)-1),
    m_moduleAccesser(moduleAccesser) {
}

ftComboModuleImpl::~ftComboModuleImpl() { }

void ftComboModuleImpl::activate() {
    clearSetting();
}

void ftComboModuleImpl::update() {
    if (m_moduleAccesser->getSlowModule().isSkip() != true) {
        if (m_frame > 0 && --m_frame <= 0) {
            clearSetting();
        }
    }
}

void ftComboModuleImpl::set(int kind) {
    if (m_kind != kind) {
        clearSetting();
    }
    bool isContinue;
    if (m_frame >= 0 && m_statusKind == m_moduleAccesser->getStatusModule().getStatusKind()) {
        isContinue = true;
    } else {
        isContinue = false;
    }
    if (isContinue == true) {
        m_count++;
    } else {
        m_count = 1;
    }
    switch (kind) {
    case 1:
        switch (m_count) {
        case 1:
            m_frame = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xBDA, 0);
            break;
        case 2:
            m_frame = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xBDB, 0);
            break;
        }
        break;
    case 2:
        switch (m_count) {
        case 1:
            m_frame = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xBDC, 0);
            break;
        case 2:
            m_frame = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xBDD, 0);
            break;
        }
        if (m_frame == 0) {
            m_frame = -1;
        }
        break;
    case 3:
        switch (m_count) {
        case 1:
            m_frame = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xBDE, 0);
            break;
        case 2:
            break;
        }
        if (m_frame == 0) {
            m_frame = -1;
        }
        break;
    }
    m_statusKind = m_moduleAccesser->getStatusModule().getStatusKind();
    m_kind = kind;
}

void ftComboModuleImpl::clearSetting() {
    m_kind = -1;
    m_count = 0;
    m_frame = -1;
    m_statusKind = -1;
}

void ftComboModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (m_frame >= 0) {
        if (statusKind == 0) {
            return;
        }
        if (statusKind == 1) {
            return;
        }
        if (statusKind == 0x24) {
            return;
        }
        if (statusKind == 0x27) {
            return;
        }
        if (statusKind == 0x2C) {
            return;
        }
    }
    clearSetting();
}

typedef soArrayContractibleTable<const acCmdArgConv> ArgTable;

static inline acCmdArg getFrontArg(const ArgTable& args) {
    return acCmdArg(&args.at(0));
}

bool ftComboModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 2) {
        return false;
    }
    switch (acmd->getType()) {
    case 0: {
        if (acmd->getArgNum() < 1) {
            return true;
        }
        ArgTable args = acmd->getArgList();
        set(getFrontArg(args).getIntData());
        return true;
    }
    case 1:
        clearSetting();
        return true;
    }
    return false;
}

bool ftComboModuleImpl::isObserv(char unk1) {
    return unk1 == 0x13;
}

int ftComboModuleImpl::getCount() {
    return m_count;
}
