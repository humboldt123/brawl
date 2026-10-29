#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define SO_GIMMICK_EVENT_PRESENTER_INLINE
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_gimmick_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessGimmick::ftStatusUniqProcessGimmick(soModuleAccesser* moduleAccesser) :
    soGimmickEventObserver(-1, -1), m_moduleAccsr(moduleAccesser) { }

void ftStatusUniqProcessGimmick::initStatus(soModuleAccesser* moduleAccesser) {
    soGimmickEventObserver::addObserver(moduleAccesser->getWorkManageModule().getInt(0x10000016), -1);
}

void soGimmickEventObserver::addObserver(short manageId, s8 p2) {
    addObserverSub(manageId, this, p2);
}

void ftStatusUniqProcessGimmick::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (m_sendID != -1) {
        soGimmickEventArgs_Exit args;
        presentEvent(&args, moduleAccesser);
        removeObserver(m_manageID);
    }
}

void ftStatusUniqProcessGimmick::notifyEvent(soGimmickEventArgs* eventInfo, soModuleAccesser* moduleAccesser) {
    switch (eventInfo->m_kind) {
    case Gimmick::Event_Disable:
        removeObserver(m_manageID);
        moduleAccesser->getStatusModule().changeStatusRequest(0xE, moduleAccesser);
        break;
    }
}

void ftStatusUniqProcessGimmick::presentEvent(soGimmickEventArgs* eventInfo, soModuleAccesser* moduleAccesser) {
    soGimmickEventPresenter presenter(m_manageID, m_sendID);
    presenter.presentEventGimmick(eventInfo, moduleAccesser, -1);
}

void ftStatusUniqProcessGimmick::notifyEventGimmick(soGimmickEventArgs* eventInfo, int* taskId) {
    notifyEvent(eventInfo, m_moduleAccsr);
}
