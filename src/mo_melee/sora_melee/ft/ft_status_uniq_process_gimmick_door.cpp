#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_module_accesser.h>
#include <types.h>

void ftStatusUniqProcessGimmickDoor::notifyEvent(soGimmickEventArgs* eventInfo, soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: each gimmick owns a range of event kinds between two unnamed bounds.
    if (eventInfo->m_kind > 0x10 && eventInfo->m_kind < 0x12) {
    } else {
        ftStatusUniqProcessGimmick::notifyEvent(eventInfo, moduleAccesser);
    }
}
