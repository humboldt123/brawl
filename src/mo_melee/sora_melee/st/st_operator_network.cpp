#include <ip/ip_switch.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <nt/network.h>
#include <revolution/OS/OSError.h>
#include <st/operator/st_operator_network.h>
#include <types.h>

extern "C" {
// HYPOTHESIS: muWifiInterfaceTask::getNetworkError (map name), the error code with its detail.
int fn_800CBCB0(muWifiInterfaceTask* task, int* detail);
}

// HYPOTHESIS: the operator that receives the network stop and exit callbacks.
static stOperatorNetwork* unk_bss_operator;

void stNetworkOperatorStopCallback(u16 frame, u8 result) {
    if (unk_bss_operator != NULL) {
        unk_bss_operator->stopControllerCompleteCallback(result);
    }
}

void stNetworkOperatorAbortCallback(u8 aid, u16 frame, u8 result) {
    if (unk_bss_operator != NULL) {
        unk_bss_operator->abortControllerCompleteCallback(aid, result);
    }
}

void stOperatorNetworkMelee::processBegin() {
    if (m_isOnline) {
        unk64 = fn_800CBCB0(g_muWifiInterfaceTask, &unk68);
        if (unk64 != 0) {
            unk48 = false;
            m_isOnline = false;
            if (unk64 <= 6) {
                m_errorValue = 2;
                OSReport("stOperatorNetworkMelee::processBegin() ERROR_CODE_FATAL !!!");
            } else {
                m_errorValue = 1;
                OSReport("stOperatorNetworkMelee::processBegin() ERROR_CODE_MATCHING_BROKEN !!!");
            }
            NtShared::ntSetStopCallback(NULL);
            NtShared::ntSetExitCallback(NULL);
        }
    }
}

void stOperatorNetworkMelee::checkNoContest() {
    if (!m_isNoContest && m_isOnline && unk_bss_operator != NULL) {
        m_isNoContest = unk5C == 2;
    }
}

void stOperatorNetworkMelee::setNullPadForNetworkOut(bool isNull) {
    ipSwitch::getInstance()->m_flags.nullNetworkOutput = isNull;
}
