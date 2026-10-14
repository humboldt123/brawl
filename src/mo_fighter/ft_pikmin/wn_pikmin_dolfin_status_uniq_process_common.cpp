#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <wn/pikmin/wn_pikmin_dolfin_status_uniq_process_common.h>
#include <types.h>

// Photo-mode controller used by the Dolfin exit path (defined in sora_melee so_photo_call_back.cpp).
class soPhotoController {
public:
    bool setFreezeMode();
};

wnPikminDolfinStatusUniqProcessCommon g_wnPikminDolfinStatusUniqProcessCommon;

void wnPikminDolfinStatusUniqProcessCommon::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status < 4) {
        if (status >= 1) {
            return;
        }
    }
    soPhotoController* photo = 0;
    photo->setFreezeMode();
}
