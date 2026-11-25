#define SO_ANIM_CMD_MODULE_IMPL_UNIT
#define SO_INSTANCE_UNIT_LOCAL_TEMPLATE

#include <ac/ac_anim_cmd_impl.h>
#include <so/anim/so_anim_cmd_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

void soAnimCmdModuleImpl::interpretCmd(s32 id, soModuleAccesser* moduleAccesser, float frame) {
    soAnimCmdControlUnit* unit = getControlUnit(id);
    if (unit != nullptr) {
        unit->m_animCmdInterpreter->interpretCmd(moduleAccesser, frame);
    }
}

void soAnimCmdModuleImpl::resetInterpreter(const soAttributeFlag& attr, soModuleAccesser* moduleAccesser, s32 index, bool keepFrame, float frame) {
    soAnimCmdAddressPackConv empty;
    if (attr.m_mask == -1) {
        s32 size = m_animCmdThreads->size();
        for (s32 i = 0; i < size; i++) {
            soAnimCmdControlUnit& unit = m_animCmdThreads->atIndex(i);
            if (index < 0) {
                unit.m_animCmdInterpreter->resetInterpreter(empty, moduleAccesser, frame);
            } else if (keepFrame == true) {
                unit.m_animCmdInterpreter->resetInterpreter(unit.getAddressPack(index), moduleAccesser, unit.m_animCmdInterpreter->m_currentFrame);
            } else {
                unit.m_animCmdInterpreter->resetInterpreter(unit.getAddressPack(index), moduleAccesser, frame);
            }
        }
    } else {
        soArrayVector<soAnimCmdControlUnit*, 20> arr;
        m_animCmdThreads->getAttributeArray(attr, arr);
        s32 size = arr.size();
        for (s32 i = 0; i < size; i++) {
            soAnimCmdControlUnit* unit = arr.at(i);
            if (index < 0) {
                unit->m_animCmdInterpreter->resetInterpreter(empty, moduleAccesser, frame);
            } else if (keepFrame == true) {
                unit->m_animCmdInterpreter->resetInterpreter(unit->getAddressPack(index), moduleAccesser, unit->m_animCmdInterpreter->m_currentFrame);
            } else {
                unit->m_animCmdInterpreter->resetInterpreter(unit->getAddressPack(index), moduleAccesser, frame);
            }
        }
    }
}

bool soAnimCmdModuleImpl::isInterpreterAnimCmd(const soAttributeFlag& attr, s32 index) {
    if (attr.m_mask == -1) {
        s32 size = m_animCmdThreads->size();
        for (s32 i = 0; i < size; i++) {
            if (m_animCmdThreads->atIndex(i).getAddressPack(index).addr2 == nullptr) {
                return false;
            }
        }
    } else {
        soArrayVector<soAnimCmdControlUnit*, 20> arr;
        m_animCmdThreads->getAttributeArray(attr, arr);
        s32 size = arr.size();
        for (s32 i = 0; i < size; i++) {
            if (arr.at(i)->getAddressPack(index).addr2 == nullptr) {
                return false;
            }
        }
    }
    return true;
}

void soAnimCmdModuleImpl::resetInterpreter(s32 id, soModuleAccesser* moduleAccesser, s32 index, float frame) {
    soAnimCmdControlUnit* unit = getControlUnit(id);
    if (unit != nullptr) {
        if (index < 0) {
            soAnimCmdAddressPackConv empty;
            unit->m_animCmdInterpreter->resetInterpreter(empty, moduleAccesser, frame);
        } else {
            unit->m_animCmdInterpreter->resetInterpreter(unit->getAddressPack(index), moduleAccesser, frame);
        }
    }
}

void soAnimCmdModuleImpl::resetInterpreter(s32 id, const acAnimCmdConv* cmd, soModuleAccesser* moduleAccesser) {
    soAnimCmdControlUnit* unit = getControlUnit(id);
    if (unit != nullptr) {
        unit->m_animCmdInterpreter->resetInterpreter(cmd, moduleAccesser, 1, 0.0f);
    }
}

void soAnimCmdModuleImpl::resetInterpreter(const soAttributeFlag& attr, const acAnimCmdConv* cmd, soModuleAccesser* moduleAccesser, bool runNow, u32 flag) {
    soArrayVector<soAnimCmdControlUnit*, 20> arr;
    m_animCmdThreads->getAttributeArray(attr, arr);
    for (s32 i = 0; i < arr.size(); i++) {
        arr.at(i)->m_animCmdInterpreter->resetInterpreter(cmd, moduleAccesser, flag, 0.0f);
        if (runNow == true) {
            arr.at(i)->m_animCmdInterpreter->interpretCmd(moduleAccesser, 0.0f);
        }
        break;
    }
}

void soAnimCmdModuleImpl::setAddressPackList(s32 id, soAnimCmdAddressPackArraySeparate* list) {
    soAnimCmdControlUnit* unit = getControlUnit(id);
    if (unit != nullptr) {
        unit->m_animCmdAddressPackArraySeparate = list;
    }
}

void soAnimCmdModuleImpl::stopInterpreter(const soAttributeFlag& attr, soModuleAccesser* moduleAccesser) {
    if (attr.m_mask == -1) {
        s32 size = m_animCmdThreads->size();
        for (s32 i = 0; i < size; i++) {
            m_animCmdThreads->atIndex(i).m_animCmdInterpreter->resetInterpreter(static_cast<const acAnimCmdConv*>(nullptr), moduleAccesser, 1, 0.0f);
        }
    } else {
        soArrayVector<soAnimCmdControlUnit*, 20> arr;
        m_animCmdThreads->getAttributeArray(attr, arr);
        s32 size = arr.size();
        for (s32 i = 0; i < size; i++) {
            arr.at(i)->m_animCmdInterpreter->resetInterpreter(static_cast<const acAnimCmdConv*>(nullptr), moduleAccesser, 1, 0.0f);
        }
    }
}

bool soAnimCmdModuleImpl::isAnimCmdEnd(s32 id) {
    soAnimCmdControlUnit* unit = getControlUnit(id);
    if (unit == nullptr) {
        return true;
    }
    return !unit->m_animCmdInterpreter->m_currentCmd;
}

float soAnimCmdModuleImpl::getAnimCmdFrame(s32 id) {
    soAnimCmdControlUnit* unit = getControlUnit(id);
    if (unit == nullptr) {
        return 0.0f;
    }
    return unit->m_animCmdInterpreter->m_currentFrame;
}

void soAnimCmdModuleImpl::getCmdAddressPack(soArray<soAnimCmdAddressPackConv>* outArr, s32 index, const soAttributeFlag& attr) {
    if (attr.m_mask == -1) {
        s32 size = m_animCmdThreads->size();
        for (s32 i = 0; i < size; i++) {
            outArr->push(m_animCmdThreads->atIndex(i).getAddressPack(index));
        }
    } else {
        soArrayVector<soAnimCmdControlUnit*, 20> arr;
        m_animCmdThreads->getAttributeArray(attr, arr);
        s32 size = arr.size();
        for (s32 i = 0; i < size; i++) {
            outArr->push(arr.at(i)->getAddressPack(index));
        }
    }
}

bool soAnimCmdModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 5) {
        return false;
    }
    switch (acmd->getType()) {
    case 0:
    case 1: {
        acCmdArg arg;
        if (!acmd->getArg(&arg, 0) || arg.getArgType() != 0) {
            return true;
        }
        s32 id = arg.getIntData();
        bool skip = true;
        if (m_animCmdThreads->isContain(id)) {
            if ((m_animCmdThreads->getAttribute(id).m_mask & 4) != 0) {
                skip = false;
            }
        }
        if (skip) {
            return true;
        }
        const acAnimCmdConv* cmdAddr = nullptr;
        if (acmd->getType() == 0) {
            acCmdArg arg1;
            if (!acmd->getArg(&arg1, 1) || arg1.getArgType() != 2) {
                return true;
            }
            cmdAddr = static_cast<const acAnimCmdConv*>(arg1.getAddressData());
        }
        resetInterpreter(id, cmdAddr, moduleAccesser);
        return true;
    }
    case 2: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        s32 id = acGetCmdArg(args, 0).getIntData();
        bool skip = true;
        if (m_animCmdThreads->isContain(id)) {
            bool bad = false;
            if ((m_animCmdThreads->getAttribute(id).m_mask & 8) == 0) {
                if ((m_animCmdThreads->getAttribute(id).m_mask & 16) == 0) {
                    bad = true;
                }
            }
            if (!bad) {
                skip = false;
            }
        }
        if (skip) {
            return true;
        }
        resetInterpreter(id, static_cast<const acAnimCmdConv*>(acGetCmdArg(args, 1).getAddressData()), moduleAccesser);
        return true;
    }
    case 3: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        s32 id = acGetCmdArg(args, 0).getIntData();
        bool skip = true;
        if (m_animCmdThreads->isContain(id)) {
            bool bad = false;
            if ((m_animCmdThreads->getAttribute(id).m_mask & 8) == 0) {
                if ((m_animCmdThreads->getAttribute(id).m_mask & 16) == 0) {
                    bad = true;
                }
            }
            if (!bad) {
                skip = false;
            }
        }
        if (skip) {
            return true;
        }
        resetInterpreter(id, moduleAccesser, acGetCmdArg(args, 1).getIntData(), 0.0f);
        return true;
    }
    case 4: {
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        s32 id = acGetCmdArg(args, 0).getIntData();
        bool skip = true;
        if (m_animCmdThreads->isContain(id)) {
            bool bad = false;
            if ((m_animCmdThreads->getAttribute(id).m_mask & 8) == 0) {
                if ((m_animCmdThreads->getAttribute(id).m_mask & 16) == 0) {
                    bad = true;
                }
            }
            if (!bad) {
                skip = false;
            }
        }
        if (skip) {
            return true;
        }
        resetInterpreter(id, static_cast<const acAnimCmdConv*>(nullptr), moduleAccesser);
        return true;
    }
    }
    return false;
}

bool soAnimCmdModuleImpl::isObserv(char group) {
    return group == 0xD;
}

void soAnimCmdModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    resetInterpreter(soAttributeFlag(4), moduleAccesser, -1, false, 0.0f);
    resetInterpreter(soAttributeFlag(1), moduleAccesser, statusKind, false, 0.0f);
}

u32 soAnimCmdModuleImpl::getInterpreterNum() {
    return m_animCmdThreads->size();
}

soAnimCmdModuleImpl::~soAnimCmdModuleImpl() { }
