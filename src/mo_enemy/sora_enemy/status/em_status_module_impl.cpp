// The constructor of soStatusModuleImpl is declared only for the module builders; with FT_REL_LINK_EXTERN its change
// request queue has the out-of-line destructor of sora_melee, which the inline soStatusModuleImpl destructor calls.
#define FT_MODULE_BUILDER
#define FT_REL_LINK_EXTERN
#include <em/enemy.h>
#include <em/status/em_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stop/so_stop_module_impl.h>
#include <types.h>

// The status work flags the AI check looks at. HYPOTHESIS: names (WorkFlag_Front is 0x12000003 in the Enemy unit).
static const u32 WorkFlag_AIStatus = 0x12000004;
static const u32 WorkFlag_AIStatusEx = 0x22000000;

// A weak copy of this unit (the destructor below calls it).
soStatusModuleImpl::~soStatusModuleImpl() { }

emStatusModuleImpl::emStatusModuleImpl(soModuleAccesser* acc, soArrayContractibleTable<const soStatusData>* statusTable,
                                       void* uniqProcs, soGeneralWorkSimple* statusWork, soTransitionModule* transition,
                                       void* changeRequests, void* preCheckData, int unk1, u8 unk2)
    : soStatusModuleImpl(acc, statusTable, static_cast<soArray<soStatusUniqProcess*>*>(uniqProcs), statusWork,
                         transition, changeRequests, preCheckData, unk1, unk2) {
    initAIUpdatePri();
}

emStatusModuleImpl::~emStatusModuleImpl() { }

bool emStatusModuleImpl::checkTransition(soModuleAccesser* moduleAccesser) {
    Enemy& enemy = dynamic_cast<Enemy&>(moduleAccesser->getStageObject());
    bool changed = false;
    if (m_aiUpdatePriMin == -50 && m_aiUpdatePriMax == 250) {
        enemy.getAIModule().update(-100, -100);
        if (soStatusModuleImpl::checkTransition(moduleAccesser) == true) {
            return true;
        }
        if (moduleAccesser->getWorkManageModule().isFlag(WorkFlag_AIStatus) == true ||
            moduleAccesser->getWorkManageModule().isFlag(WorkFlag_AIStatusEx) == true) {
            changed = enemy.getAIModule().update(-250, -150);
        } else if (!moduleAccesser->getStopModule().isStop()) {
            changed = enemy.getAIModule().update(m_aiUpdatePriMin, m_aiUpdatePriMax);
        }
    } else {
        changed = enemy.getAIModule().update(m_aiUpdatePriMin, m_aiUpdatePriMax);
    }
    if (changed == true) {
        return true;
    }
    return soStatusModuleImpl::checkTransition(moduleAccesser);
}

void emStatusModuleImpl::setAIUpdatePri(int min, int max) {
    m_aiUpdatePriMin = min;
    m_aiUpdatePriMax = max;
}

void emStatusModuleImpl::initAIUpdatePri() {
    m_aiUpdatePriMin = -50;
    m_aiUpdatePriMax = 250;
}

// The inline methods of soStatusModuleImpl (weak copies of this unit, which the vtable points at).

bool soStatusModuleImpl::isObserv(char group) {
    return group == 2;
}

void soStatusModuleImpl::connectStatusDataList(void* list) {
    m_statusDataArr->connect((soArrayContractibleTable<const soStatusData>*)list);
}

void* soStatusModuleImpl::getLastStatusTransitionInfo() {
    return m_transitionModule->getLastTransitionInfo();
}

void soStatusModuleImpl::clearTransitionTermAll(int groupID) {
    m_transitionModule->clearTransitionTermAll(groupID);
}

void soStatusModuleImpl::enableTransitionTermAll(int groupID) {
    m_transitionModule->enableTermAll(groupID);
}

bool soStatusModuleImpl::isEnableTransitionTermGroup(int groupID) {
    return m_transitionModule->isEnableTermGroup(groupID);
}

void soStatusModuleImpl::unableTransitionTermGroup(int groupID) {
    m_transitionModule->unableTermGroup(groupID);
}

void soStatusModuleImpl::enableTransitionTermGroup(int groupID) {
    m_transitionModule->enableTermGroup(groupID);
}

bool soStatusModuleImpl::isChanged() {
    return m_isChanged;
}

void soStatusModuleImpl::startWatchChange() {
    m_isChanged = false;
}

bool soStatusModuleImpl::isCollisionAttackOccer() {
    return m_isCollisionAttackOccer;
}

int soStatusModuleImpl::getStatusKind() {
    return m_statusKind;
}
