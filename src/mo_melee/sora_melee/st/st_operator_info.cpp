#include <ft/ft_manager.h>
#include <gm/gm_global_mode_melee.h>
#include <if/if_mngr.h>
#include <snd/snd_system.h>
#include <st/operator/st_operator_info.h>
#include <types.h>

extern "C" {
int fn_80110310(int* trigger, int* padNo);
void fn_800E4D28(IfPlayer* player, int visible);
void fn_800E5174(IfPlayer* player, int mode);
bool fn_27_23AC10(stLoader* loader);
extern u8 lbl_27_bss_59D8;
}

// HYPOTHESIS: IfPlayer state read by the operator (offsets from the operator code only). IfMngr maps
// a player index to its IfPlayer slot through the bytes at 0x68 (unk68 in the local IfMngr header).
struct IfPlayerOperatorView {
    char _0[0x2b0];
    u32 m_2b0;
    char _2b4[0x9c];
    bool m_350;
    char _351[3];
    u32 m_354;
};

// HYPOTHESIS: the flag byte at 0x1C of gmPlayerInitData, whose third bit BrawlHeaders leaves unnamed.
struct gmPlayerInitDataFlagView {
    char _0[0x1c];
    bool m_1c_7 : 1;
    bool m_1c_6 : 1;
    bool m_1c_5 : 1;
};

// HYPOTHESIS: two (value, flag) pairs filled by the static initializer (same shape as other sora_melee units).
struct stOperatorInfoPair {
    u32 m_0;
    u32 m_4;
    stOperatorInfoPair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static stOperatorInfoPair g_stOperatorInfoPairA(0xff, 0);
static stOperatorInfoPair g_stOperatorInfoPairB(0xff, 1);

int stOperatorInfo::getPauseTriggerAndPadNo() {
    int trigger;
    int padNo;
    fn_80110310(&trigger, &padNo);
    return trigger;
}

void stOperatorInfo::setLocalPlayerPadMask() {
    if (m_c0 & 0x80) {
        m_cc = 0xff;
        return;
    }
    m_cc = 0;
    u8 mode = m_condition->m_meleeInitData.m_gameMode;
    if (mode != Game_Mode_Melee && mode != Game_Mode_Tournament) {
        for (int i = 0; i < 4; i++) {
            gmPlayerInitData* data = &m_condition->m_playersInitData[i];
            if (static_cast<u8>(data->_0x3C[1]) != 0) {
                m_cc |= 1 << (static_cast<u8>(data->_0x3C[1]) - 1);
                m_ec[static_cast<u8>(data->_0x3C[1])] = i;
            }
        }
    }
    if (m_cc == 0) {
        m_cc = 0xff;
    }
}

void stOperatorInfo::setLocalPlayerPadMaskWifi() {
    if (m_c0 & 0x80) {
        m_c8 = 0xff;
        m_cc = 0xff;
        return;
    }
    m_c8 = 0;
    m_cc = 0;
    for (int i = 0; i < 4; i++) {
        gmPlayerInitData* data = &m_condition->m_playersInitData[i];
        if (static_cast<u8>(data->_0x3C[1]) != 0) {
            m_c8 |= 1 << i;
            m_cc |= 1 << (static_cast<u8>(data->_0x3C[1]) - 1);
            m_ec[static_cast<u8>(data->_0x3C[1])] = i;
        }
    }
    if (m_cc == 0) {
        m_cc = 0xff;
    }
    if (m_c8 == 0) {
        m_c8 = 0xff;
    }
}

void stOperatorInfo::lockPlayer() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (m_loaderPlayers[i] && fn_27_23AC10(m_loaderPlayers[i])) {
            m_loaderPlayers[i]->lockEntity();
        }
    }
}

void stOperatorInfo::unlockPlayer() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (m_loaderPlayers[i] && fn_27_23AC10(m_loaderPlayers[i])) {
            m_loaderPlayers[i]->unlockEntity();
        }
    }
}

void stOperatorInfo::setPlayerCursorClear(u8 index) {
    IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[index])];
    if (reinterpret_cast<IfPlayerOperatorView*>(player)->m_354) {
        fn_800E5174(player, 0);
    }
    m_playersCursorStatus[index] = 0;
    m_playersNameCursorStatus[index] = 0;
}

void stOperatorInfo::setPlayerCursorStatus(u8 idx, u8 status) {
    // MATCH-ONLY: the index is widened before the float table store.
    int index = idx;
    if (status == 0 && status != m_playersCursorStatus[index] && m_playersNameCursorStatus[index] == 0) {
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[index])];
        if (reinterpret_cast<IfPlayerOperatorView*>(player)->m_354) {
            fn_800E5174(player, 0);
        }
        m_98[index] = 0.0f;
    }
    m_playersCursorStatus[index] = status;
}

void stOperatorInfo::setPlayerNameCursorStatus(u8 idx, u8 status) {
    // MATCH-ONLY: the index is widened before the float table store.
    int index = idx;
    if (status == 0 && status != m_playersNameCursorStatus[index] && m_playersCursorStatus[index] == 0) {
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[index])];
        if (reinterpret_cast<IfPlayerOperatorView*>(player)->m_354) {
            fn_800E5174(player, 0);
        }
        m_98[index] = 0.0f;
    }
    m_playersNameCursorStatus[index] = status;
}

void stOperatorInfo::setPlayerLoupeStatus(u8 index, u8 status) {
    if (status == 0 && status != m_playersLoupeStatus[index]) {
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[index])];
        if (reinterpret_cast<IfPlayerOperatorView*>(player)->m_350) {
            fn_800E4D28(player, 0);
        }
    }
    m_playersLoupeStatus[index] = status;
}

void stOperatorInfo::clearPlayerCursor() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        m_playersCursorStatus[i] = 0;
        m_playersNameCursorStatus[i] = 0;
        m_72[i] = 0;
        m_7c[i] = 0;
        m_98[i] = 0.0f;
        m_playersLoupeStatus[i] = 0;
    }
    if (g_IfMngr && g_ftManager) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (g_ftManager->getEntryId(i) != -1) {
                u8 playerId = m_condition->m_playersInitData[i].m_playerId;
                IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[playerId])];
                if (reinterpret_cast<IfPlayerOperatorView*>(player)->m_350) {
                    fn_800E4D28(player, 0);
                }
                player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[playerId])];
                if (reinterpret_cast<IfPlayerOperatorView*>(player)->m_2b0) {
                    reinterpret_cast<IfPlayerOperatorView*>(player)->m_2b0 = 0;
                }
            }
        }
    }
    if (m_114 >= 0) {
        g_sndSystem->stopSE(m_114, 0);
        m_114 = -1;
    }
}

void stOperatorInfo::checkOnamaeForcePut() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        gmPlayerInitData* data = &m_condition->m_playersInitData[i];
        if (data->m_state != 3) {
            bool forcePut = false;
            u8 playerId = data->m_playerId;
            if (reinterpret_cast<gmPlayerInitDataFlagView*>(data)->m_1c_5 && !data->m_isSpycloak) {
                forcePut = true;
            }
            m_72[playerId] = forcePut;
        }
    }
}

void stOperatorInfo::startEventCursor() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!m_condition->m_playersInitData[i].m_isSpycloak) {
            if (m_condition->m_playersInitData[i].m_state == 0) {
                m_7c[m_condition->m_playersInitData[i].m_playerId] = 180;
            }
            if (m_condition->m_playersInitData[i].m_state == 1) {
                m_7c[m_condition->m_playersInitData[i].m_playerId] = 300;
            }
        }
    }
}

void stOperatorInfo::processEnd() {
    if (m_119) {
        updatePlayerCursor(m_11a);
        m_119 = false;
    }
}

bool stOperatorInfo::isError() {
    return false;
}

u32 stOperatorInfo::getErrorLevel() {
    return 0;
}

u32 stOperatorInfo::getEndType() {
    return 1;
}

bool stOperatorInfo::isEnableOperation() {
    return true;
}

void stOperatorInfo::stopOperatorRequest() { }

void stOperatorInfo::startOperator() { }

void stOperatorInfo::processBegin() { }

stOperatorInfo::~stOperatorInfo() { }
