#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Shared base for the catch-wait status (defined in the sora_melee ft status code).
// HYPOTHESIS: the base only overrides exitStatus; its destructor is out of line in the original.
class ftStatusUniqProcessCatchWait : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCatchWait() __attribute__((never_inline)) { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftDonkeyStatusUniqProcessCatchWait : public ftStatusUniqProcessCatchWait {
public:
    virtual ~ftDonkeyStatusUniqProcessCatchWait() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// Exiting into status 0x120 keeps the catch-wait handling; any other exit defers to the base.
void ftDonkeyStatusUniqProcessCatchWait::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x120) {
        return ftStatusUniqProcessCatchWait::exitStatus(moduleAccesser, nextStatus);
    }
}

ftDonkeyStatusUniqProcessCatchWait g_ftDonkeyStatusUniqProcessCatchWait;
