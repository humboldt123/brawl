// soAnimCmdModule::interpretCmd1 takes the frame last (see so/anim/so_anim_cmd_module_impl.h).
#define SO_ANIM_CMD_INTERPRET_CMD1_FRAME_LAST
#include <em/ai/em_ai_unit.h>
#include <so/anim/so_anim_cmd_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// The work variables the decide anim cmds talk through. HYPOTHESIS: names.
static const u32 WorkFlag_AIDecided = 0x22000004; // set by the decide anim cmds to take the unit
static const u32 WorkInt_AIArea = 0x20000000;     // the area the unit is decided for

emAIUnit::emAIUnit() {
    m_param = NULL;
    m_flag = 0;
}

emAIUnit::~emAIUnit() { }

void emAIUnit::create(emAIUnitParam* param) {
    m_param = param;
    m_flag = param->m_initFlag;
}

void emAIUnit::reset() {
    m_flag = m_param->m_initFlag;
}

void emAIUnit::resetFlag() {
    m_flag = m_param->m_initFlag;
}

void emAIUnit::setFlag(int flag) {
    m_flag = flag;
}

int emAIUnit::getFlag() {
    return m_flag;
}

bool emAIUnit::execAnimCmdDecide(soModuleAccesser* acc, int animCmdGroup, int areaIndex) {
    soAnimCmdModule& animCmd = acc->getAnimCmdModule();
    if (m_param->m_decideAnimCmd == NULL) {
        return true;
    }
    acc->getWorkManageModule().offFlag(WorkFlag_AIDecided);
    acc->getWorkManageModule().setInt(areaIndex, WorkInt_AIArea);
    animCmd.resetInterpreter3(animCmdGroup, m_param->m_decideAnimCmd, acc);
    animCmd.interpretCmd1(animCmdGroup, acc, 1.0f);
    if (acc->getWorkManageModule().isFlag(WorkFlag_AIDecided) == true) {
        return true;
    }
    animCmd.resetInterpreter3(animCmdGroup, NULL, acc);
    return false;
}

void emAIUnit::execAnimCmdExec(soModuleAccesser* acc, int animCmdGroup) {
    soAnimCmdModule& animCmd = acc->getAnimCmdModule();
    if (m_param->m_execAnimCmd == NULL) {
        return;
    }
    animCmd.resetInterpreter3(animCmdGroup, m_param->m_execAnimCmd, acc);
    animCmd.interpretCmd1(animCmdGroup, acc, 1.0f);
    animCmd.resetInterpreter3(animCmdGroup, NULL, acc);
}

int emAIUnit::getPriorty() {
    return m_param->m_priority;
}

int emAIUnit::getStatusKind() {
    return m_param->m_statusKind;
}

bool emAIUnit::isTargetKind(u32 mask, int area) {
    return (1 << area) & mask;
}

bool emAIUnit::isTargetKind1(int area) {
    return isTargetKind(m_param->m_targetAreaMask, area);
}

bool emAIUnit::isSetInfo() {
    return m_param->m_setInfo;
}
