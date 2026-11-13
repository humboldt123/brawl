#define MU_WIFI_NETCOND_OUTLINE
#include <types.h>

#include <mu/wifi/mu_wifi_interface.h>
#include <gf/gf_scene.h>
#include <revolution/OS/OSError.h>
#include <revolution/OS/OSMutex.h>
#include <stdio.h>
#include <nt/nt_net_report.h>
#include <nt/network.h>
#include <nt/nt_offline.h>
#include <nt/nt_tr.h>
#include <nt/nt_report.h>
#include <string.h>
#include <nt/nt_match.h>
#include <nt/nt_etc_global.h>
#include <ip/ip_switch.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>

extern u32 g_NtAIDBitmap;

extern "C" void fn_800CF22C(muWifiInterfaceTask* self);

struct muWifiInterfaceTaskFields {
    char pad0[0x44];
    s32 unk44;
    s8 unk48;
    char pad49[0x27];
    char unk70[0xF5A];
    u8 unkFCA;
    char padFCB[0x7D];
    s32 unk1048;
    s32 unk104C;
};

struct NetworkFields {
    char pad0[0x38];
    s32 unk38;
};

struct ntFriendInfoFields {
    char pad0[0x608];
    u8 unk608;
};

void muWifiInterfaceTask::connect(bool arg0) {
    // BYTES: declaration order picks the register assignment
    s32 offset;
    Network* network;
    s32 i;
    muWifiInterfaceTaskFields* fields = reinterpret_cast<muWifiInterfaceTaskFields*>(this);
    fields->unk48 = arg0;
    fields->unk44 = 1;
    fields->unk1048 = 0;
    fields->unk104C = 0;
    reinterpret_cast<NtBase*>(this)->clearAppErrorStatus();
    network = &g_Network;
    fields->unkFCA &= 0xFFFFFFEF;
    network->create(true);
    i = 0;
    offset = 0;
    do {
        // MATCH-ONLY: char* arithmetic keeps the loaded value as the left add operand
        ntFriendInfoFields* entry = reinterpret_cast<ntFriendInfoFields*>(reinterpret_cast<char*>(*reinterpret_cast<s32*>(ntFriendInfo::getInstance())) + offset);
        if (entry->unk608 != 0) {
            entry->unk608 = 2;
        }
        i += 1;
        offset += 0x89;
    } while (i < 0x40);
    reinterpret_cast<NtTr*>(reinterpret_cast<NetworkFields*>(network)->unk38)->defineBroadcastArea(fields->unk70, 0x218, 0x138, 4);
    fn_800CF22C(this);
}

void netReport(const char* fmt, ...) {
}

// Function bodies below are taken from https://github.com/jj-link/brawl.

s32 muWifiInterfaceTask::exchangeDWCErrorCode(u32 arg0) {
    if ((arg0 >= 0x4E86U && arg0 <= 0x4E8DU) || (arg0 >= 0x4E8FU && arg0 <= 0x5207U)) {
        return 1;
    }
    if (arg0 == 0x4E85U || (arg0 >= 0x59D8U && arg0 <= 0x5DBFU)) {
        return 2;
    }
    if (arg0 == 0x4E8EU) {
        return 3;
    }
    if (arg0 == 0x7148U) {
        return 7;
    }
    if (arg0 == 0x7149U) {
        return 8;
    }
    if (arg0 == 0x4E84U || (arg0 >= 0xC350U && arg0 <= 0xEA5FU)) {
        return 4;
    }
    if ((arg0 - 0x10000U) == 0x3A2EU) {
        return 5;
    }
    return 6;
}

extern "C" void fn_80146574(Network* network);
extern "C" void fn_800CCF30(muWifiInterfaceTask* self, int arg1, int arg2, int arg3, int arg4);
extern "C" bool fn_8014592C(void* tr);
extern "C" u64 fn_80052250();

extern void* lbl_805A02A0;
extern const muWifiErrorMsg lbl_80408488[];

void muWifiInterfaceTask::destroy() {
    if (g_muWifiInterfaceTask != NULL) {
        fn_80146574(&g_Network);
        g_muWifiInterfaceTask->unlink();
        delete g_muWifiInterfaceTask;
        g_muWifiInterfaceTask = NULL;
        lbl_805A02A0 = NULL;
    }
}

void muWifiInterfaceTask::exit() {
    exit();
}

muWifiInterfaceTask::~muWifiInterfaceTask() { }

u32 muWifiInterfaceTask::getErrorMsgID(int index) {
    return lbl_80408488[index].m_msgId;
}

bool muWifiInterfaceTask::haveErrorCode(int index) {
    return lbl_80408488[index].m_haveErrorCode;
}

u32 muWifiInterfaceTask::getBadNameErrorMsgID() {
    return 0xC;
}

u64 muWifiInterfaceTask::getUserID() {
    NetworkUserData* userData = g_Network.m_userData;
    if (userData != NULL) {
        return userData->m_userId;
    }
    return 0;
}

bool muWifiInterfaceTask::disconnect() {
    bool connected;
    if (g_muWifiInterfaceTask != NULL && g_muWifiInterfaceTask->unk40 == 0) {
        connected = true;
    } else {
        connected = false;
    }
    if (!connected) {
        return false;
    }
    netReport("muWifiInterfaceTask::disconnect\n");
    g_Network.shutdown();
    unk40 = 1;
    return true;
}

bool muWifiInterfaceTask::isMyPlayerArea(int index) {
    if (index < 0 || index >= 4) {
        return false;
    }
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    muWifiPlayerArea* area = &m_playerAreas[index];
    return area->m_aid == aid && !area->unk5_b7;
}

u8 muWifiInterfaceTask::getTeamColor(int index) {
    int localPlayer = m_playerAreas[index].m_localPlayer;
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    return m_aidInfos[aid].m_localPlayers[localPlayer].m_teamColor;
}

u8 muWifiInterfaceTask::getAssignedTeamColor(int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0;
    }
    return info->m_assignedPlayers[index].m_teamColor;
}

u8 muWifiInterfaceTask::getAssignedHomerunRule() {
    return m_assignedHomerunRule;
}

u8 muWifiInterfaceTask::getAssignedKumiteType() {
    return m_assignedKumiteType;
}

u8* muWifiInterfaceTask::getAssignedMeleeRule() {
    return m_assignedMeleeRule;
}

u8 muWifiInterfaceTask::getAssignedDragoon() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0;
    }
    return info->unk78_b2;
}

void muWifiInterfaceTask::setAppNetworkError(int error, int detail) {
    if (!unkFCB_b7) {
        m_appNetworkError[0] = error;
        m_appNetworkError[1] = detail;
    }
}

void muWifiInterfaceTask::connectMatchingDaiRantou(int arg) {
    netReport("大乱闘のクライアントとしてマッチングを開始しました。\n");
    fn_800CCF30(this, 4, 0, 3, arg);
}

void muWifiInterfaceTask::startMatchingBattle100(int arg) {
    netReport("百人組み手のサーバとしてマッチングを開始しました。\n");
    fn_800CCF30(this, arg, 1, 2, -1);
}

void muWifiInterfaceTask::connectMatchingBattle100(int arg) {
    netReport("百人組み手のクライアントとしてマッチングを開始しました。\n");
    fn_800CCF30(this, 4, 1, 3, arg);
}

void muWifiInterfaceTask::startMatchingHomerun(int arg) {
    netReport("ホームランコンテストのサーバとしてマッチングを開始しました。\n");
    fn_800CCF30(this, arg, 2, 2, -1);
}

void muWifiInterfaceTask::connectMatchingHomerun(int arg) {
    netReport("ホームランコンテストのクライアントとしてマッチングを開始しました。\n");
    fn_800CCF30(this, 4, 2, 3, arg);
}

void muWifiInterfaceTask::startMatchingBossBattle(int arg) {
    netReport("ボスバトルのサーバとしてマッチングを開始しました。\n");
    fn_800CCF30(this, arg, 3, 2, -1);
}

void muWifiInterfaceTask::connectMatchingBossBattle(int arg) {
    netReport("ボスバトルのクライアントとしてマッチングを開始しました。\n");
    fn_800CCF30(this, 4, 3, 3, arg);
}

void muWifiInterfaceTask::clearAppError() {
    m_appNetworkError[0] = 0;
    m_appNetworkError[1] = 0;
    reinterpret_cast<NtBase*>(this)->clearAppErrorStatus();
}

bool muWifiInterfaceTask::isEndMatchingEnd() {
    return gUnk805a03d8 == 3;
}

void muWifiInterfaceTask::resetLimitTime(int seconds) {
    m_limitTime = seconds * 60;
    netReport("制限時間を%d秒に再設定しました\n", seconds);
}

bool muWifiInterfaceTask::testSendAIDInfoDone() {
    return fn_8014592C(lbl_805A03A8);
}

void muWifiInterfaceTask::clearPrepareLoadStage() {
    for (int i = 0; i < 4; i++) {
        m_aidInfos[i].unk56_b6 = false;
    }
    netReport("ステージ準備完了の通知をクリアしました。\n");
}

int muWifiInterfaceTask::getFriendAIDIndexAID(int aid) {
    return m_aidInfos[aid].m_friendAIDIndex;
}

void muWifiInterfaceTask::clearShortComment(int aid) {
    memset(m_aidInfos[aid].m_shortComment, 0, sizeof(m_aidInfos[aid].m_shortComment));
}

s32 muWifiInterfaceTask::getMeleeRestTime(u8* flag) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL || unkFCC_b24 < 3) {
        return -1;
    }
    if (flag != NULL) {
        *flag = info->unk56_b2;
    }
    return info->unk8;
}

u8 muWifiInterfaceTask::getRuleType() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL || unkFCC_b24 < 3) {
        return 0xFF;
    }
    return info->unk3;
}

u8 muWifiInterfaceTask::getPlayerArea(int index, u8* localPlayer) {
    *localPlayer = m_playerAreas[index].m_localPlayer;
    return m_playerAreas[index].m_aid;
}

int muWifiInterfaceTask::getSelCharMeleeKind() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0;
    }
    return info->unk7D_b2;
}

bool muWifiInterfaceTask::getPlayerLive(int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return true;
    }
    return m_playerLive[index] != 0xFF;
}

void muWifiInterfaceTask::initPlayerLive() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    for (int i = 0; i < 4; i++) {
        m_playerLive[i] = info->m_assignedPlayers[i].m_aid;
    }
}

void muWifiInterfaceTask::clearPlayerLive(u8 aid, u16 time) {
    for (int i = 0; i < 4; i++) {
        if (aid == m_playerLive[i]) {
            m_playerLive[i] = 0xFF;
            m_playerLiveTime[i] = time;
        }
    }
}

u64 muWifiInterfaceTask::getComposedCharaAcceptFlag() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        netReport("getComposedCharaAcceptFlag: 親がいません\n");
        return fn_80052250();
    }
    return info->unk30;
}

muWifiAIDInfo* muWifiInterfaceTask::getSafeParentAIDInfo() {
    u8 parentAID = Network::getParentAID();
    if (parentAID == 0xFF) {
        u8 aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        return &m_aidInfos[aid];
    }
    muWifiAIDInfo* info = &m_aidInfos[parentAID];
    if (!info->unk78_b7) {
        u8 aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        return &m_aidInfos[aid];
    }
    return info;
}

u8 muWifiInterfaceTask::getMyFriendStatusIcon() {
    return ntFriendInfo::getInstance()->unk0->unk450[0xDA];
}

u8 muWifiInterfaceTask::getFriendStatusIcon(int index) {
    return ntFriendInfo::getInstance()->unk0->unk5B3[index].unk8[0];
}

u8 muWifiInterfaceTask::getFriendStatusWiFiStatus(int index) {
    return ntFriendInfo::getInstance()->unk0->unk5B3[index].unk8[0x4D];
}

u8 muWifiInterfaceTask::getFriendStatusRoundTripTime(int index) {
    return ntFriendInfo::getInstance()->unk0->unk350[index].unk2[1];
}

u64 muWifiInterfaceTask::getMyRFLID() {
    u64 id;
    ntFriendInfo* info = ntFriendInfo::getInstance();
    memcpy(&id, &info->unk0->unk450[0x129], 8);
    return id;
}

void muWifiInterfaceTask::setMyRFLID(u64 id) {
    memcpy(&ntFriendInfo::getInstance()->unk0->unk450[0x129], &id, 8);
    ntFriendInfo::getInstance()->setDirtyFlag(true);
}

u8 muWifiInterfaceTask::getFriendStatusEntryPlayers(int index) {
    u8 players = ntFriendInfo::getInstance()->unk0->unk5B3[index].unk8[0x4E];
    if (players == 0) {
        players++;
    }
    return players;
}

int muWifiInterfaceTask::getNetcond() {
    int condition = gUnk805a03d8;
    if (condition < 3) {
        return 0;
    }
    if (condition == 3) {
        return 1;
    }
    if (condition == 4) {
        return 2;
    }
    if (condition == 7) {
        return 3;
    }
    netReport("Unknown Network::Netcond %d\n", condition);
    return 0;
}

void muWifiInterfaceTask::setFriendFlag(int index, bool flag) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    muWifiAIDInfo* info = &m_aidInfos[aid];
    info->unk59_b4 = (info->unk59_b4 & ~(1 << index)) | (flag << index);
}


int muWifiInterfaceTask::getNumNode() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 1;
    }
    int count = 0;
    for (int i = 0; i < 4; i++) {
        if ((int)info->unk68[i] == 1) {
            count++;
        }
    }
    return count;
}

wchar_t* muWifiInterfaceTask::getMyFriendStatusName(wchar_t* name) {
    u8* destination;
    const u8* source = &ntFriendInfo::getInstance()->unk0->unk450[0xA0];
    destination = (u8*)name;
    int count = 5;
    memset(name, 0, 12);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    return name;
}

wchar_t* muWifiInterfaceTask::getMyFriendStatusComment(wchar_t* name) {
    u8* destination;
    const u8* source = &ntFriendInfo::getInstance()->unk0->unk450[0xAA];
    destination = (u8*)name;
    int count = 20;
    memset(name, 0, 42);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    return name;
}

wchar_t* muWifiInterfaceTask::getMyShortComment(int index, wchar_t* name) {
    u8* destination;
    const u8* source = &ntFriendInfo::getInstance()->unk0->unk450[index * 0x28];
    destination = (u8*)name;
    int count = 20;
    memset(name, 0, 42);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    return name;
}

extern const char lbl_80457A10[0x38];

bool muWifiInterfaceTask::isPlayerAssignRecieved() {
    if (!((unkFC9 >> 4) & 1)) {
        return false;
    }
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        netReport(lbl_80457A10);
        NtMatching::setErrorStatus(12);
        return false;
    }
    if ((info->unk78_b3 >> 2) & 1) {
        u8 ownAid = Network::getMyAID();
        if (ownAid == 0xFF) { ownAid = 4; }
        m_aidInfos[ownAid].unk3 = info->unk3;
    }
    return (info->unk78_b3 >> 2) & 1;
}

bool muWifiInterfaceTask::isEqualServerGameMode() {
    u8 index = unkFD8;
    if (index >= 0x40) { return false; }
    u8 status = getFriendStatusWiFiStatus(index);
    switch (status) {
    case 19:
    case 23:
    case 25:
        return unk6CData == 0;
    case 20:
    case 26:
        return unk6CData == 1;
    case 21:
    case 27:
        return unk6CData == 2;
    default:
        netReport("isEqualServerGameMode:gameMode:%d  wifiStatus:%d\n", unk6CData, status);
        return false;
    }
}

bool muWifiInterfaceTask::needKeepAlive() {
    if (ipSwitch::getInstance()->m_source == 1) {
        return false;
    }
    if (g_GameGlobal->m_selCharData->unk6 != 0) {
        muWifiAIDInfo* info;
        u8 aid = Network::getParentAID();
        if (aid == 0xFF) {
            info = NULL;
        } else {
            info = &m_aidInfos[aid];
        }
        if (info == NULL) {
            return false;
        }
        if (info->unk55 == 4) {
            return false;
        }
    }
    return true;
}

extern const char lbl_80457AB0[0x64];
extern const char lbl_80457A48[0x2C];
extern const char lbl_80457A74[0x3C];

u8 muWifiInterfaceTask::getAssignedStageKind(u8* substage, u32* music) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0xFF;
    }
    if (!((info->unk78_b3 >> 2) & 1)) {
        return 0xFF;
    }
    *substage = info->unk10DData;
    *music = info->unk110;
    netReport(lbl_80457AB0, info->unk10C, *substage, *music);
    return info->unk10C;
}

void muWifiInterfaceTask::clearGlobalAIDStatusLive() {
    muWifiAIDInfo* info;
    Network::getMyAID();
    for (u8 aid = 0; aid < 5; aid++) {
        info = &m_aidInfos[aid];
        for (int player = 0; player < 4; player++) {
            info->unk68[player] = 0;
        }
    }
}

u8 muWifiInterfaceTask::getAssignedPlayerKind(int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    u8* kind;
    if (info == NULL) {
        netReport(lbl_80457A48);
        kind = NULL;
    } else if (!((info->unk78_b3 >> 2) & 1)) {
        netReport(lbl_80457A74);
        kind = NULL;
    } else {
        kind = &info->unkE4[index].unk0;
    }
    if (kind == NULL) {
        return 3;
    }
    return *kind;
}

bool muWifiInterfaceTask::getSelCharGameRule(void* rule, bool force) {
    muWifiAIDInfo* info = getSafeParentAIDInfo();
    bool changed = memcmp(unkFFCData, info->unk7DData, 4) != 0;
    if (force || changed) {
        for (int i = 0; i < 4; i++) { unkFFCData[i] = info->unk7DData[i]; }
        u8* output = (u8*)rule;
        for (int i = 0; i < 4; i++) { output[i] = info->unk7DData[i]; }
        u8 aid = Network::getMyAID();
        if (aid == 0xFF) { aid = 4; }
        muWifiAIDInfo* own = &m_aidInfos[aid];
        for (int i = 0; i < 4; i++) { own->unk64[i] = info->unk7DData[i]; }
        for (int i = 0; i < 4; i++) { own->unk7DData[i] = own->unk64[i]; }
        return true;
    }
    return false;
}

bool muWifiInterfaceTask::isEnoughRoomForFriendList(int player) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) { aid = 4; }
    if (aid == 4) { return false; }
    aid = Network::getMyAID();
    if (aid == 0xFF) { aid = 4; }
    if ((m_aidInfos[aid].unk56_b3 >> 1) & 1) { return false; }
    u8 other = m_playerAreas[player].m_aid;
    bool live;
    if (other >= 4) { live = false; }
    else { live = getSafeParentAIDInfo()->unk68[other] == 1; }
    if (!live) { return false; }
    muWifiAIDInfo* info = &m_aidInfos[other];
    aid = Network::getMyAID();
    if (aid == 0xFF) { aid = 4; }
    if (info->unk59_b4 & (1 << aid)) { return true; }
    return (info->unk57Data >> 6) & 1;
}


bool muWifiInterfaceTask::isExchangingFriendKey(int index) {
    u8 ownAid = Network::getMyAID();
    if (ownAid == 0xFF) { ownAid = 4; }
    if (ownAid == 4) { return false; }
    u8 aid = m_playerAreas[index].m_aid;
    bool live;
    if (aid >= 4) { live = false; }
    else { live = getSafeParentAIDInfo()->unk68[aid] == 1; }
    if (!live) { return false; }
    ownAid = Network::getMyAID();
    if (ownAid == 0xFF) { ownAid = 4; }
    int flags = m_aidInfos[ownAid].unk58_b0;
    if (flags & (1 << aid)) { return true; }
    return false;
}

u8 muWifiInterfaceTask::getLocalPlayerContNum(int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) { info = NULL; }
    else { info = &m_aidInfos[aid]; }
    __typeof__(info->unkE4[0])* player;
    if (info == NULL) { netReport(lbl_80457A48); player = NULL; }
    else if (!((info->unk78_b3 >> 2) & 1)) {
        netReport(lbl_80457A74); player = NULL;
    } else { player = &info->unkE4[index]; }
    if (player == NULL) { return 0; }
    aid = Network::getMyAID();
    if (aid == 0xFF) { aid = 4; }
    u8 localPlayer = player->unk2Data;
    for (int area = 0; area < 4; area++) {
        if (aid == m_playerAreas[area].m_aid && localPlayer == m_playerAreas[area].m_localPlayer) {
            return m_playerAreas[area].unk2Data;
        }
    }
    return 0;
}

bool muWifiInterfaceTask::isWifiPreloadCharacter() {
    u8 ownAid = Network::getMyAID();
    if (ownAid == 0xFF) { ownAid = 4; }
    if (ownAid == 4) { return false; }
    if (Network::getParentAID() == 0xFF) { return false; }
    bool ready = true;
    for (u8 aid = 0; aid < 4; aid++) {
        muWifiAIDInfo* info = &m_aidInfos[aid];
        bool live;
        if (aid >= 4) { live = false; }
        else { live = getSafeParentAIDInfo()->unk68[aid] == 1; }
        if (live && !info->unk57_b7) {
            ready = false;
            break;
        }
    }
    return ready;
}

extern u32 g_NtAIDBitmap;

wchar_t* muWifiInterfaceTask::getLocalPlayersOnamae(int index, wchar_t* name, u8* nameNo) {
    u8* destination;
    const u8* source;
    int count;
    muWifiAIDInfo* info;
    u8 player;
    u8 aid = m_playerAreas[index].m_aid;
    player = m_playerAreas[index].m_localPlayer;
    if (!(g_NtAIDBitmap & (1 << aid)) || !((unkFC9 >> 7) & 1)) {
        if (name != NULL) { *name = 0; }
        if (nameNo != NULL) {
            u8 ownAid = Network::getMyAID();
            if (ownAid == 0xFF) { ownAid = 4; }
            *nameNo = aid == ownAid ? m_aidInfos[aid].unk208[player] : 0x78;
        }
        return name;
    }
    info = &m_aidInfos[aid];
    if (name != NULL) {
        source = info->unk1EA[player];
        destination = (u8*)name;
        count = 5;
        memset(destination, 0, 12);
        while (--count >= 0) {
            int character = source[0];
            destination[0] = character;
            character |= source[1];
            destination[1] = source[1];
            source += 2;
            destination += 2;
            if (character == 0) { break; }
        }
    }
    if (nameNo != NULL) {
        u8 ownAid = Network::getMyAID();
        if (ownAid == 0xFF) { ownAid = 4; }
        *nameNo = aid == ownAid ? info->unk208[player] : 0x78;
    }
    return name;
}

wchar_t* muWifiInterfaceTask::getGlobalPlayersOnamae(int index, wchar_t* name, u8* nameNo) {
    int count;
    const u8* source;
    u8* destination;
    muWifiAIDInfo* info;
    u8 aid = m_playerAreas[index].m_aid;
    u8 player = m_playerAreas[index].m_localPlayer;
    if (!(g_NtAIDBitmap & (1 << aid)) || !((unkFC9 >> 7) & 1)) {
        if (name != NULL) { *name = 0; }
        if (nameNo != NULL) {
            u8 ownAid = Network::getMyAID();
            if (ownAid == 0xFF) { ownAid = 4; }
            *nameNo = aid == ownAid ? m_aidInfos[aid].unk208[player] : 0x78;
        }
    } else {
        info = &m_aidInfos[aid];
        if (name != NULL) {
            source = info->unk1EA[player];
            destination = (u8*)name;
            count = 5;
            memset(destination, 0, 12);
            while (--count >= 0) {
                int character = source[0];
                destination[0] = character;
                character |= source[1];
                destination[1] = source[1];
                source += 2;
                destination += 2;
                if (character == 0) { break; }
            }
        }
        if (nameNo != NULL) {
            u8 ownAid = Network::getMyAID();
            if (ownAid == 0xFF) { ownAid = 4; }
            *nameNo = aid == ownAid ? info->unk208[player] : 0x78;
        }
    }
    return name;
}

extern u8 lbl_805A0415;

bool NtBase::isParentInit() {
    u8 initialized = lbl_805A0415;
    lbl_805A0415 = 0;
    return initialized != 0;
}

bool NtBase::isParent() {
    return lbl_8059E742 == lbl_8059E745 && lbl_8059E742 != 0xFF;
}

s32 NtBase::isNetCondOnline() {
    s32 status = gUnk805a03d8;
    s32 online = 0;
    if (status >= 3 && status <= 6) {
        online = 1;
    }
    return online;
}

bool muWifiInterfaceTask::isStageSyncFlagSet(u32 flags) {
    if (!testSendAIDInfoDone()) {
        return false;
    }
    u8 combinedFlags = 0xFF;
    for (u32 aid = 0; aid < 4; aid++) {
        bool live;
        if ((u8)aid >= 4) {
            live = false;
        } else {
            live = getSafeParentAIDInfo()->unk68[(u8)aid] == 1;
        }
        u8 nodeFlags;
        if (live) {
            nodeFlags = m_aidInfos[(u8)aid].unk5A;
        } else {
            nodeFlags = 0xFF;
        }
        combinedFlags &= nodeFlags;
    }
    return (combinedFlags & flags) == flags;
}

extern const char lbl_80457C84[0x19];
extern const char lbl_80457CA0[0x20];
extern const char lbl_80459984[0x34];
extern int lbl_805A03E8;
extern int lbl_805A03F4;
extern mtRand lbl_805A0420;

int muWifiInterfaceTask::getNetworkErrorLevel() {
    int level = _getNetworkErrorLevel();
    if (level == 0) { return level; }
    if (!unkFCB_b7) {
        flushLogToSDCard();
        if (level != 1) {
            netReport(lbl_80457C84, level);
            return level;
        }
        unkFCB_b7 = true;
        if (NtBase::isNetCondOnline() && ntFriendInfoSave::isNotSaving()) {
            ntFriendInfoSave* save = new ((Heaps::HeapType)6) ntFriendInfoSave;
            if (save != NULL) { save->startSave((Heaps::HeapType)6, true); }
        }
    }
    if (!ntFriendInfoSave::isNotSaving()) {
        netReport(lbl_80457CA0);
        return 0;
    }
    netReport(lbl_80457C84, level);
    return level;
}

int muWifiInterfaceTask::_getNetworkErrorLevel() {
    int level = m_appNetworkError[1];
    if (level != 0) { return level; }
    if (&g_Network == NULL) { return 0; }
    int error = lbl_805A03E8;
    if (error == 0) { return 0; }
    switch (error) {
    case 1: case 2: case 3: return 1;
    case 4: return 2;
    case 6: return 1;
    case 7:
        switch (lbl_805A03F4) {
        case 0: return 0;
        case 1: return 2;
        case 2: case 3: case 6: return 1;
        case 7: return 3;
        default: return 1;
        }
    case 8: return 2;
    case 9: return 2;
    case 10: return 2;
    case 11: return 2;
    case 12: return 2;
    case 13: return 2;
    case 14: return 1;
    default: return 0;
    }
}

u8 muWifiInterfaceTask::getGlobalLiveAIDBitmap() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0;
    }
    u8 bitmap = 0;
    for (u32 index = 0; index < 4; index++) {
        if ((int)getSafeParentAIDInfo()->unk68[(u8)index] == 1) {
            bitmap |= 1 << (u8)index;
        }
    }
    return bitmap;
}

u8 muWifiInterfaceTask::getGlobalConnectAIDBitmap() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        return 0;
    }
    u8 bitmap = 0;
    for (u32 index = 0; index < 4; index++) {
        if (getSafeParentAIDInfo()->unk68[(u8)index] != 0) {
            bitmap |= 1 << (u8)index;
        }
    }
    return bitmap;
}

u8 muWifiInterfaceTask::getRuleMasterAID() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        info = &m_aidInfos[aid];
    }
    u8 master = info->unk79Data;
    bool live;
    if (master >= 4) {
        live = false;
    } else {
        live = getSafeParentAIDInfo()->unk68[master] == 1;
    }
    if (!live) {
        return 0xFF;
    }
    return info->unk79Data;
}

// HYPOTHESIS: these one-argument calls use the same exported names as sq_lib.cpp.
extern "C" int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int kind);
extern "C" int getNumCharColor__6muMenuFiii(int kind);
extern const char lbl_804599B8[0x20];
extern const char lbl_804599D8[0x38];

u8 muWifiInterfaceTask::getFreeCharColor(int character, int count) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    if (info == NULL) {
        netReport(lbl_804599B8);
        return 0;
    }
    __typeof__(info->unkE4[0])* players = info->unkE4;
    int selection;
    for (u32 color = 0; (int)(u8)color <
         ((selection = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(character)) == 0x28 ?
          1 : getNumCharColor__6muMenuFiii(selection)); color++) {
        int player;
        for (player = 0; player < count; player++) {
            if (character == players[player].unk3 && (u8)color == players[player].unk4) {
                break;
            }
        }
        if (player == count) {
            return color;
        }
    }
    netReport(lbl_804599D8, character);
    return 0;
}

extern s32 lbl_805A03F0;

int muWifiInterfaceTask::_getNetworkError(int* code) {
    *code = 0;
    if (m_appNetworkError[0] != 0) {
        return m_appNetworkError[0];
    }
    if (&g_Network == NULL) {
        return 0;
    }
    int error = lbl_805A03E8;
    if (error == 0) {
        return 0;
    }
    if (error == 1) {
        *code = lbl_805A03F0;
        return exchangeDWCErrorCode(*code);
    }
    if (!unkFCB_b7) {
        NtBase::setMatchingStatus(10);
    }
    if (error == 7) {
        *code = lbl_805A03F0;
        return exchangeDWCErrorCode(*code);
    }
    switch (error) {
    case 2:
    case 3: return 13;
    case 8: return 9;
    case 4: return 10;
    case 5: return 10;
    case 6: return 13;
    case 9: return 11;
    case 10: return 12;
    case 11: return 11;
    case 12: return 11;
    case 13: return 11;
    case 14: return 13;
    default: return 0;
    }
}

int muWifiInterfaceTask::selectRandomNetCharKind(u64 accepted) {
    int count = 0;
    for (int kind = 0; kind < 35; kind++) {
        if (accepted & (1ULL << kind)) { count++; }
    }
    if (count == 0) {
        netReport(lbl_80459984);
        count = 1;
    }
    int selected = (int)(count * lbl_805A0420.randf());
    for (int kind = 0; kind < 35; kind++) {
        if (accepted & (1ULL << kind)) {
            if (--selected < 0) { return kind; }
        }
    }
    return 0;
}

bool muWifiInterfaceTask::isCompleteCloseMatchingAllNode() {
    if (!fn_8014592C(lbl_805A03A8)) { return false; }
    for (u8 aid = 0; aid < 4; aid++) {
        bool live;
        if (aid >= 4) {
            live = false;
        } else {
            live = getSafeParentAIDInfo()->unk68[aid] == 1;
        }
        if (live && !(m_aidInfos[aid].unk56_b3 & 1)) { return false; }
    }
    return true;
}

bool muWifiInterfaceTask::isPreparedLoadAllStage() {
    if (!fn_8014592C(lbl_805A03A8)) { return false; }
    for (u8 aid = 0; aid < 4; aid++) {
        bool live;
        if (aid >= 4) {
            live = false;
        } else {
            live = getSafeParentAIDInfo()->unk68[aid] == 1;
        }
        if (live && !m_aidInfos[aid].unk56_b6) { return false; }
    }
    return true;
}


u16 muWifiInterfaceTask::getAssignedPlayerHandicap(int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) { info = NULL; }
    else { info = &m_aidInfos[aid]; }
    __typeof__(info->unkE4[0])* player;
    if (info == NULL) {
        netReport(lbl_80457A48);
        player = NULL;
    } else if (!((info->unk78_b3 >> 2) & 1)) {
        netReport(lbl_80457A74);
        player = NULL;
    } else {
        player = &info->unkE4[index];
    }
    if (player == NULL) { return 0; }
    if (m_assignedMeleeRule[4] == 0) { return 0; }
    return player->unk8;
}

extern const char lbl_80459884[0x3B];
extern const char lbl_804598C0[0xC4];

u64* muWifiInterfaceTask::takeItemSwitch(s8* appearance) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) { info = NULL; }
    else { info = &m_aidInfos[aid]; }
    if (info == NULL) {
        aid = Network::getMyAID();
        if (aid == 0xFF) { aid = 4; }
        info = &m_aidInfos[aid];
    }
    u64* items = info->unk120;
    s8 frequency = info->unk10F;
    netReport(lbl_80459884, items[0], items[1]);
    netReport(lbl_804598C0, frequency);
    *appearance = frequency;
    return items;
}

extern u32 g_NtAIDBitmap;
extern const char lbl_80458808[0x12];

wchar_t* muWifiInterfaceTask::getAIDName(u32 aid, wchar_t* name) {
    if (aid != 4 && (!(g_NtAIDBitmap & (1 << aid)) || !((unkFC9 >> 7) & 1))) {
        *name = 0;
        return name;
    }
    u8* destination;
    const u8* source = m_aidInfos[aid].unk1E0;
    destination = (u8*)name;
    int count = 5;
    memset(name, 0, 12);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) { break; }
    }
    netReport(lbl_80458808, aid);
    return name;
}

extern "C" void srand(unsigned int seed);

extern const char lbl_804588C0[0x44];
extern const char lbl_80458904[0x370];

u32 muWifiInterfaceTask::setGlobalRandSeed() {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) { info = NULL; }
    else { info = &m_aidInfos[aid]; }
    if (info == NULL) {
        aid = Network::getMyAID();
        if (aid == 0xFF) { aid = 4; }
        info = &m_aidInfos[aid];
        netReport(lbl_804588C0);
    }
    u32 seed = (info->unk130[0] << 24) + (info->unk130[1] << 16) +
               (info->unk130[2] << 8) + info->unk130[3];
    srand(seed);
    srandi(seed);
    netReport(lbl_80458904, seed);
    return seed;
}

// muWifiInterfaceTask::startMatchingCommon (the map name); src/sora/mu/mu_wifi_interface_task.cpp calls it by this stand-in name.
extern "C" void fn_800CCF30(muWifiInterfaceTask* self, int arg1, int arg2, int arg3, int arg4);

extern const char lbl_80457DC8[0x30];

void muWifiInterfaceTask::startMatchingDaiRantou(int arg) {
    netReport(lbl_80457DC8);
    __typeof__(unk1000[0])* rule = &unk1000[1];
    memset(rule, 0, sizeof(*rule));
    rule->unk1 = 0;
    rule->unk2 = 2;
    rule->unk3 = 2;
    rule->unk4 = 0;
    rule->unk5 = 10;
    rule->unk6 = 1;
    rule->unk7 = 0;
    rule->unk9 = 0;
    rule->unk8 = 0;
    rule->unkC = 0;
    rule->unkA = 0;
    unk1000[0] = unk1000[1];
    fn_800CCF30(this, arg, 0, 2, -1);
}

int muWifiInterfaceTask::getFriendAIDIndex(int index) {
    u8 aid = m_playerAreas[index].m_aid;
    bool live;
    if (aid >= 4) {
        live = false;
    } else {
        live = getSafeParentAIDInfo()->unk68[aid] == 1;
    }
    if (!live) {
        return -1;
    }
    return m_aidInfos[aid].m_friendAIDIndex;
}

wchar_t* muWifiInterfaceTask::getShortComment(unsigned int aid, int index, wchar_t* comment) {
    if (!(g_NtAIDBitmap & (1 << aid))) {
        *comment = 0;
        return comment;
    }
    muWifiAIDInfo* info = &m_aidInfos[aid];
    u8 ownAid = Network::getMyAID();
    if (ownAid == 0xFF) {
        ownAid = 4;
    }
    const void* comments;
    if (aid == ownAid) {
        comments = ntFriendInfo::getInstance()->unk0->unk450;
    } else {
        comments = info->m_shortComment;
    }
    u8* destination = (u8*)comment;
    const u8* source = (const u8*)comments + index * 0x28;
    int count = 20;
    memset(comment, 0, 42);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    return comment;
}

u8 muWifiInterfaceTask::getAssignedPlayerContNum(int index) {
    muWifiAIDInfo* info = getSafeParentAIDInfo();
    int count;
    for (count = 0; count < 4; count++) {
        if (info->unkE4[count].unk0 != 0) {
            break;
        }
    }
    if (index >= count) {
        return 0;
    }
    return index + 1;
}

bool muWifiInterfaceTask::isRequestedExchangeFriendKey(int index) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) { aid = 4; }
    if (aid == 4) { return false; }
    muWifiAIDInfo* info;
    aid = Network::getParentAID();
    if (aid == 0xFF) { info = NULL; }
    else { info = &m_aidInfos[aid]; }
    if (info == NULL) { return false; }
    if (!(info->unk78_b3 & 1)) { return false; }
    u8 playerAid = m_playerAreas[index].m_aid;
    if (playerAid == 0xFF) { return false; }
    aid = Network::getMyAID();
    if (aid == 0xFF) { aid = 4; }
    if (playerAid == aid) { return false; }
    bool live;
    if (playerAid >= 4) { live = false; }
    else { live = getSafeParentAIDInfo()->unk68[playerAid] == 1; }
    if (!live) { return false; }
    info = &m_aidInfos[playerAid];
    if (info->m_friendAIDIndex >= 0) { return false; }
    aid = Network::getMyAID();
    if (aid == 0xFF) { aid = 4; }
    if (info->unk58_b4 & (1 << aid)) { return true; }
    return false;
}

bool muWifiInterfaceTask::getWifiPlayerInfo(int index, int* character, u8* color, bool* connected) {
    muWifiAIDInfo* info = getSafeParentAIDInfo();
    __typeof__(info->unkE4[0])* player = &info->unkE4[index];
    *character = player->unk3;
    *color = player->unk4;
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) { aid = 4; }
    int kind = player->unk0;
    bool own = player->unk1Data == aid;
    if (kind != 0) { own = false; }
    *connected = kind != 3;
    if (*character == 0x3E) { *connected = false; }
    if (!NtBase::isNetCondOnline()) { *connected = false; }
    aid = Network::getMyAID();
    if (aid == 0xFF) { aid = 4; }
    if (aid == 4) { *connected = false; }
    if (Network::getParentAID() == 0xFF) { *connected = false; }
    muWifiAIDInfo* parentInfo;
    int count;
    aid = Network::getParentAID();
    if (aid == 0xFF) { parentInfo = NULL; }
    else { parentInfo = &m_aidInfos[aid]; }
    if (parentInfo == NULL) { count = 1; }
    else {
        count = 0;
        for (int node = 0; node < 4; node++) {
            if ((int)parentInfo->unk68[node] == 1) { count++; }
        }
    }
    if (count < 2) { *connected = false; }
    return own;
}

extern NtMatching* lbl_805A0434;
extern "C" bool fn_801474A0(NtMatching* matching, int index);

// HYPOTHESIS: testFriendKey forwards its argument to the matching manager.
extern "C" bool fn_800D1F04(muWifiInterfaceTask* self, int index) {
    return fn_801474A0(lbl_805A0434, index);
}

// HYPOTHESIS: getFriendStatusFriendCode forwards the friend index to the matching manager.
extern "C" u64 fn_800D2120(muWifiInterfaceTask* self, int index) {
    return lbl_805A0434->getRegisterFriendCode(index);
}

// HYPOTHESIS: isCloseMatching reads the close flag of the matching manager.
extern "C" u8 fn_800CCEB8(muWifiInterfaceTask* self) {
    return lbl_805A0434->unk33F6;
}

// HYPOTHESIS: endMatching stays under its fn_ name; it is not a member in the map.
extern "C" void fn_800CCEC4(muWifiInterfaceTask* self) {
    // HYPOTHESIS: the original passes the result register of disconnectAll on to clearAppErrorStatus.
    NtBase* base = reinterpret_cast<NtBase*>(g_Network.disconnectAll(false));
    muWifiInterfaceTaskFields* fields = reinterpret_cast<muWifiInterfaceTaskFields*>(self);
    fields->unk1048 = 0;
    fields->unk104C = 0;
    base->clearAppErrorStatus();
}

// HYPOTHESIS: putMyFriendStatusIcon stores the icon in the own friend record.
extern "C" void fn_800D24CC(muWifiInterfaceTask* self, u8 icon) {
    ntFriendInfo::getInstance()->unk0->unk450[0xDA] = icon;
    g_Network.m_userData->unk0[0x33F4] = 1;
    ntFriendInfo::getInstance()->setDirtyFlag(true);
}

extern "C" const u8* fn_800D2088(muWifiInterfaceTask* self, int index, u8* out) {
    ntFriendInfo* info = ntFriendInfo::getInstance();
    memcpy(out, &info->unk0->unk5B3[index].unk8[1], 0x4C);
    return out;
}

extern "C" void fn_800D2520(muWifiInterfaceTask* self, const u8* mii) {
    memcpy(&ntFriendInfo::getInstance()->unk0->unk450[0xDB], mii, 0x4C);
    g_Network.m_userData->unk0[0x33F4] = 1;
    ntFriendInfo::getInstance()->setDirtyFlag(true);
}

extern "C" void fn_800D2164(muWifiInterfaceTask* self, int index, u8 time) {
    ntFriendInfo::getInstance()->unk0->unk350[index].unk2[1] = time;
    ntFriendInfo::getInstance()->setDirtyFlag(true);
    netReport("友達%dの通信状態:%d\n", index, time);
}

// HYPOTHESIS: putMyFriendStatusComment copies a 20 character comment into the own friend record.
extern "C" void fn_800D2428(muWifiInterfaceTask* self, const u8* source) {
    u8* destination = &ntFriendInfo::getInstance()->unk0->unk450[0xAA];
    int count = 20;
    memset(destination, 0, 40);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    g_Network.m_userData->unk0[0x33F4] = 1;
    ntFriendInfo::getInstance()->setDirtyFlag(true);
}

// HYPOTHESIS: getAssignedPlayerAID reads one byte of the assigned player entry of the parent.
extern "C" u8 fn_800CB194(muWifiInterfaceTask* self, int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &self->m_aidInfos[aid];
    }
    u8* entry;
    if (info == NULL) {
        netReport(lbl_80457A48);
        entry = NULL;
    } else if (!((info->unk78_b3 >> 2) & 1)) {
        netReport(lbl_80457A74);
        entry = NULL;
    } else {
        entry = &info->unkE4[index].unk0;
    }
    if (entry == NULL) {
        return 0xFF;
    }
    return entry[1];
}

// HYPOTHESIS: getAssignedPlayerCharKind reads one byte of the assigned player entry of the parent.
extern "C" u8 fn_800CB4AC(muWifiInterfaceTask* self, int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &self->m_aidInfos[aid];
    }
    u8* entry;
    if (info == NULL) {
        netReport(lbl_80457A48);
        entry = NULL;
    } else if (!((info->unk78_b3 >> 2) & 1)) {
        netReport(lbl_80457A74);
        entry = NULL;
    } else {
        entry = &info->unkE4[index].unk0;
    }
    return entry != NULL ? entry[3] : 0x3E;
}

// HYPOTHESIS: getAssignedPlayerColor reads one byte of the assigned player entry of the parent.
extern "C" u8 fn_800CB568(muWifiInterfaceTask* self, int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &self->m_aidInfos[aid];
    }
    u8* entry;
    if (info == NULL) {
        netReport(lbl_80457A48);
        entry = NULL;
    } else if (!((info->unk78_b3 >> 2) & 1)) {
        netReport(lbl_80457A74);
        entry = NULL;
    } else {
        entry = &info->unkE4[index].unk0;
    }
    return entry != NULL ? entry[4] : 0;
}

extern "C" void fn_8010F960(u8 stage, u8* arg1, u8* arg2, int arg3);
extern "C" void fn_80145B98(NtTr* tr, int arg1);
extern "C" void fn_800D0AE0(muWifiInterfaceTask* self);

// HYPOTHESIS: the low bit of the byte at 0x56 of the AID info marks the stage as not yet sent.
struct FlagByte {
    u8 unk0_b7 : 7;
    u8 unk0_b0 : 1;
};

// HYPOTHESIS: setStageKind stores the stage of the own AID and flags the info for sending.
extern "C" void fn_800CA73C(muWifiInterfaceTask* self, int stage) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    u8* info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
    info[0x52] = stage;
    fn_8010F960(stage, info + 0x53, info + 0xC, 0);
    aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
    reinterpret_cast<FlagByte*>(&info[0x56])->unk0_b0 = 0;
    fn_800D0AE0(self);
    fn_80145B98(lbl_805A03A8, 0xFE);
}

extern const char lbl_804587CC[0x28];

// HYPOTHESIS: setStageSyncFlag stores the stage sync flag of the own AID and flags the info for sending.
extern "C" void fn_800D1E58(muWifiInterfaceTask* self, int flag) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    u8* info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
    info[0x5A] = flag;
    netReport(lbl_804587CC, flag);
    aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
    reinterpret_cast<FlagByte*>(&info[0x56])->unk0_b0 = 0;
    fn_800D0AE0(self);
    fn_80145B98(lbl_805A03A8, 0xFE);
}

extern const char lbl_80457928[0x2C];

int muWifiInterfaceTask::getDisconnectResult(int* errorIndex) {
    bool connected;
    *errorIndex = 0;
    if (g_muWifiInterfaceTask != NULL && g_muWifiInterfaceTask->unk40 == 0) {
        connected = true;
    } else {
        connected = false;
    }
    if (!connected) {
        netReport(lbl_80457928);
        return 2;
    }
    if (&g_Network != NULL && gUnk805a03d8 != 0) {
        return 1;
    }
    unk40 = 1;
    return 2;
}

extern "C" int fn_800CBCB0(muWifiInterfaceTask* task, int* detail);

int muWifiInterfaceTask::getConnectResult(int* errorIndex, int* errorCode, bool* isNewUser) {
    if (errorCode) { *errorCode = 0; }
    if (errorIndex) { *errorIndex = 0; }
    if (isNewUser) {
        *reinterpret_cast<u8*>(isNewUser) = *(reinterpret_cast<u8*>(lbl_805A0434) + 0x343C);
    }
    if (gUnk805a03d8 == 0) {
        return 0;
    }
    int error = fn_800CBCB0(this, errorCode);
    if (error != 0) {
        *errorIndex = error;
        return 3;
    }
    int status = gUnk805a03d8;
    int connecting = 0;
    if (status >= 1 && status < 3) {
        connecting = 1;
    }
    if (connecting) {
        return 1;
    }
    if (lbl_805A03F0 != 0) {
        if (errorCode) { *errorCode = lbl_805A03F0; }
        if (errorIndex) { *errorIndex = exchangeDWCErrorCode(*errorCode); }
        return 3;
    }
    if (isNewUser) {
        *reinterpret_cast<u8*>(isNewUser) = *(reinterpret_cast<u8*>(lbl_805A0434) + 0x343C);
    }
    unk40 = 0;
    fn_800CF22C(this);
    return 2;
}

extern const char lbl_8045879C[0x30];

// HYPOTHESIS: setContinueNextGame either ends the matching or flags the own AID as ready for the next game.
extern "C" void fn_800D1ADC(muWifiInterfaceTask* self, bool next) {
    if (!next) {
        // HYPOTHESIS: the original passes the result register of disconnectAll on to clearAppErrorStatus.
        NtBase* base = reinterpret_cast<NtBase*>(g_Network.disconnectAll(false));
        muWifiInterfaceTaskFields* fields = reinterpret_cast<muWifiInterfaceTaskFields*>(self);
        fields->unk1048 = 0;
        fields->unk104C = 0;
        base->clearAppErrorStatus();
    } else {
        u8 aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        u8* info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
        info[0x56] |= 0x20;
        aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
        reinterpret_cast<FlagByte*>(&info[0x56])->unk0_b0 = 0;
        fn_800D0AE0(self);
        fn_80145B98(lbl_805A03A8, 0xFE);
        aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        netReport(lbl_8045879C, aid);
    }
}

extern const char lbl_8045726C[0x14];
extern "C" void fn_800C8F68(const char* fmt, ...);

// HYPOTHESIS: the top five bits of the word at 0x7C of the AID info hold the wifi status.
struct WifiStatusWord {
    u32 unk0_b27 : 5;
    u32 unk0_b0 : 27;
};

// HYPOTHESIS: putMyWifiStatus sends the given wifi status of the own AID.
extern "C" void fn_800D221C(muWifiInterfaceTask* self, u8 status) {
    ntFriendInfo::getInstance()->unk0->unk450[0x127] = status;
    g_Network.m_userData->unk0[0x33F4] = 1;
    netReport(lbl_8045726C, status);
    fn_800C8F68(lbl_8045726C, status);
    if (NtBase::isParent()) {
        muWifiAIDInfo* info;
        u8 aid = Network::getParentAID();
        if (aid == 0xFF) {
            info = NULL;
        } else {
            info = &self->m_aidInfos[aid];
        }
        if (info != NULL) {
            reinterpret_cast<WifiStatusWord*>(reinterpret_cast<u8*>(info) + 0x7C)->unk0_b27 = status;
            aid = Network::getMyAID();
            if (aid == 0xFF) {
                aid = 4;
            }
            u8* own = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
            reinterpret_cast<FlagByte*>(&own[0x56])->unk0_b0 = 0;
            fn_800D0AE0(self);
            fn_80145B98(lbl_805A03A8, 0xFE);
        }
    }
}

// HYPOTHESIS: sendAIDInfo sets the sent flag of the own AID info and asks for a send.
void muWifiInterfaceTask::sendAIDInfo(bool flag, u8 mask) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    u8* info = reinterpret_cast<u8*>(&m_aidInfos[aid]);
    reinterpret_cast<FlagByte*>(&info[0x56])->unk0_b0 = flag;
    fn_800D0AE0(this);
    fn_80145B98(lbl_805A03A8, mask);
}

extern u8 lbl_805A02A8;

// HYPOTHESIS: updatePreloadCharacterStatus stores its argument in a global status byte; stays under its fn_ name.
extern "C" void fn_800DA0B4(u8 status) {
    lbl_805A02A8 = status;
}

// HYPOTHESIS: bit 1 of the byte at 0x56 of the AID info is a homerun-share flag.
struct FlagByteHomerun {
    u8 unk0_b7 : 1;
    u8 unk0_b6 : 1;
    u8 unk0_b0 : 6;
};

// HYPOTHESIS: resetShareInfoForHomerun clears the homerun flag and the byte at 0x5A of the own AID info, then flags it for sending.
extern "C" void fn_800CFE1C(muWifiInterfaceTask* self) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    u8* info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
    reinterpret_cast<FlagByteHomerun*>(&info[0x56])->unk0_b6 = 0;
    info[0x5A] = 0;
    aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
    reinterpret_cast<FlagByte*>(&info[0x56])->unk0_b0 = 1;
    fn_800D0AE0(self);
    fn_80145B98(lbl_805A03A8, 0xFE);
}

struct Slot8 {
    u8 unk0;
    u8 pad1[7];
};

// HYPOTHESIS: setTeamColor looks up the slot owned by the own AID in the table at 0x4C and stores the color in the own AID info.
extern "C" void fn_800CA5F8(muWifiInterfaceTask* self, int slot, u8 color) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    u8* entry = reinterpret_cast<u8*>(self) + slot * 8;
    if (entry[0x4C] == aid) {
        int index = entry[0x4D];
        aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        reinterpret_cast<Slot8*>(reinterpret_cast<u8*>(&self->m_aidInfos[aid]) + 0x3C)[index].unk0 = color;
        aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        u8* info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
        reinterpret_cast<FlagByte*>(&info[0x56])->unk0_b0 = 1;
        fn_800D0AE0(self);
        fn_80145B98(lbl_805A03A8, 0xFE);
    }
}

// HYPOTHESIS: putFriendStatusName clears a friend record and copies a 5 character name into it; returns the source.
extern "C" const u8* fn_800D26D0(muWifiInterfaceTask* self, int index, const u8* name) {
    const u8* source;
    int count;
    u8* destination = &ntFriendInfo::getInstance()->unk0->unk450[0x131] + index * 0x89;
    memset(destination, 0, 0x89);
    source = name;
    count = 5;
    memset(destination, 0, 10);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    ntFriendInfo::getInstance()->setDirtyFlag(true);
    return name;
}

u8 muWifiInterfaceTask::getAssignedCpuLevel(int index) {
    muWifiAIDInfo* info;
    u8 aid = Network::getParentAID();
    if (aid == 0xFF) {
        info = NULL;
    } else {
        info = &m_aidInfos[aid];
    }
    u8* level;
    if (info == NULL) {
        netReport(lbl_80457A48);
        level = NULL;
    } else if (!((info->unk78_b3 >> 2) & 1)) {
        netReport(lbl_80457A74);
        level = NULL;
    } else {
        level = &info->unkE4[index].unk0;
    }
    return level != NULL ? level[6] : 0;
}

wchar_t* muWifiInterfaceTask::getFriendStatusName(int index, wchar_t* name) {
    u8* destination;
    const u8* source = &ntFriendInfo::getInstance()->unk0->unk450[0x131] + index * 0x89;
    destination = (u8*)name;
    int count = 5;
    memset(name, 0, 12);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    return name;
}

wchar_t* muWifiInterfaceTask::getFriendStatusComment(int index, wchar_t* name) {
    u8* destination;
    const u8* source = &ntFriendInfo::getInstance()->unk0->unk450[0x13B] + index * 0x89;
    destination = (u8*)name;
    int count = 20;
    memset(name, 0, 42);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    return name;
}

void muWifiInterfaceTask::putShortComment(int aid, int index, const wchar_t* comment) {
    u8* destination = reinterpret_cast<u8*>(m_aidInfos[aid].m_shortComment) + index * 0x28;
    const u8* source = reinterpret_cast<const u8*>(comment);
    int count = 20;
    memset(destination, 0, 0x28);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
}

void muWifiInterfaceTask::putMyFriendStatusName(const wchar_t* name) {
    int count;
    const u8* source;
    u8* destination = &ntFriendInfo::getInstance()->unk0->unk450[0xA0];
    source = reinterpret_cast<const u8*>(name);
    count = 5;
    memset(destination, 0, 10);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    source = reinterpret_cast<const u8*>(name);
    destination = m_aidInfos[aid].unk1E0;
    count = 5;
    memset(destination, 0, 10);
    while (--count >= 0) {
        int character = source[0];
        destination[0] = character;
        character |= source[1];
        destination[1] = source[1];
        source += 2;
        destination += 2;
        if (character == 0) {
            break;
        }
    }
    g_Network.m_userData->unk0[0x33F4] = 1;
    ntFriendInfo::getInstance()->setDirtyFlag(true);
}

extern "C" char lbl_80457158[];

bool muWifiInterfaceTask::startMatchingAnybody(int arg) {
    char* base = lbl_80457158;
    const char* sequenceName = gfSceneManager::getInstance()->m_currentSequence->m_sequenceName;
    if (strcmp(sequenceName, base + 0xBC8) == 0) {
        netReport(base + 0xBD8);
        fn_800CCF30(this, 4, 5, 1, -1);
    } else if (strcmp(sequenceName, base + 0xC04) == 0) {
        netReport(base + 0xC18);
        fn_800CCF30(this, 4, 7, 1, -1);
    }
    (void)arg;
    return true;
}

extern const char lbl_8045881C[];

void muWifiInterfaceTask::setLocalPlayersOnamae(int player, const wchar_t* name, u8 nameNo) {
    u8 local = m_playerAreas[player].m_localPlayer;
    if (m_playerAreas[player].unk5_b7) {
        return;
    }
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    muWifiAIDInfo* info = &m_aidInfos[aid];
    if (name != NULL) {
        u8* destination = info->unk1EA[local];
        const u8* source = reinterpret_cast<const u8*>(name);
        int count = 5;
        memset(destination, 0, 10);
        while (--count >= 0) {
            int character = source[0];
            destination[0] = character;
            character |= source[1];
            destination[1] = source[1];
            source += 2;
            destination += 2;
            if (character == 0) {
                break;
            }
        }
    } else {
        *reinterpret_cast<u16*>(info->unk1EA[local]) = 0;
    }
    info->unk208[local] = nameNo;
    lbl_805A03A8->broadcastMyName();
}

void muWifiInterfaceTask::requestExchangeFriendKey(int playerArea) {
    u8 areaAid = m_playerAreas[playerArea].m_aid;
    u8 myAid = Network::getMyAID();
    if (myAid == 0xFF) {
        myAid = 4;
    }
    if (areaAid == myAid) {
        netReport(lbl_8045881C);
        return;
    }
    myAid = Network::getMyAID();
    if (myAid == 0xFF) {
        myAid = 4;
    }
    m_aidInfos[myAid].unk58_b4 |= 1 << areaAid;
    myAid = Network::getMyAID();
    if (myAid == 0xFF) {
        myAid = 4;
    }
    u8* info = reinterpret_cast<u8*>(&m_aidInfos[myAid]);
    reinterpret_cast<FlagByte*>(&info[0x56])->unk0_b0 = 0;
    fn_800D0AE0(this);
    fn_80145B98(lbl_805A03A8, areaAid);
}

// HYPOTHESIS: bit 6 of the own AID status tracks whether the matching word is below 0x40.
struct WifiRoomByte {
    u8 hi : 1;
    u8 room : 1;
    u8 lo : 6;
};

struct NtMatchingWord {
    char pad[0x3408];
    s32 value;
};

extern "C" void fn_800D353C(muWifiInterfaceTask* self) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    u8* info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
    unsigned room = reinterpret_cast<NtMatchingWord*>(lbl_805A0434)->value < 0x40;
    WifiRoomByte* bits = reinterpret_cast<WifiRoomByte*>(info + 0x57);
    unsigned oldRoom = bits->room;
    bits->room = room;
    if (oldRoom != room) {
        aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        info = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
        reinterpret_cast<FlagByte*>(info + 0x56)->unk0_b0 = 0;
        fn_800D0AE0(self);
        fn_80145B98(lbl_805A03A8, 0xFE);
    }
}

// HYPOTHESIS: the melee kind is the two-bit field shared with getSelCharMeleeKind.
struct MeleeKindBits {
    u8 unk4 : 4;
    u8 kind : 2;
    u8 unk0 : 2;
};

extern "C" void fn_800D846C(muWifiInterfaceTask* self, int kind) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    reinterpret_cast<MeleeKindBits*>(&self->m_aidInfos[aid].unk64[0])->kind = kind;
    muWifiAIDInfo* parent;
    aid = Network::getParentAID();
    if (aid == 0xFF) {
        parent = NULL;
    } else {
        parent = &self->m_aidInfos[aid];
    }
    if (parent != NULL) {
        reinterpret_cast<MeleeKindBits*>(&parent->unk7DData[0])->kind = kind;
    }
    aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    reinterpret_cast<u8*>(&self->m_aidInfos[aid])[0x56] |= 1;
    fn_800D0AE0(self);
    fn_80145B98(lbl_805A03A8, 0xFE);
}

// HYPOTHESIS: startWatching publishes wifi status 7, the same update as putMyWifiStatus.
extern "C" void fn_800CC440(muWifiInterfaceTask* self) {
    ntFriendInfo::getInstance()->unk0->unk450[0x127] = 7;
    g_Network.m_userData->unk0[0x33F4] = 1;
    netReport(lbl_8045726C, 7);
    fn_800C8F68(lbl_8045726C, 7);
    if (NtBase::isParent()) {
        muWifiAIDInfo* info;
        u8 aid = Network::getParentAID();
        if (aid == 0xFF) {
            info = NULL;
        } else {
            info = &self->m_aidInfos[aid];
        }
        if (info != NULL) {
            reinterpret_cast<WifiStatusWord*>(reinterpret_cast<u8*>(info) + 0x7C)->unk0_b27 = 7;
            aid = Network::getMyAID();
            if (aid == 0xFF) {
                aid = 4;
            }
            u8* own = reinterpret_cast<u8*>(&self->m_aidInfos[aid]);
            reinterpret_cast<FlagByte*>(&own[0x56])->unk0_b0 = 0;
            fn_800D0AE0(self);
            fn_80145B98(lbl_805A03A8, 0xFE);
        }
    }
}

extern const char lbl_80459830[];
extern const char lbl_8045986C[];

// HYPOTHESIS: the own node stores two item masks at offset 0x18 and a signed appearance at offset 2.
extern "C" void fn_800D88E0(muWifiInterfaceTask* self, const u64* items, s8 appearance) {
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    muWifiAIDInfo* info = &self->m_aidInfos[aid];
    u64* stored = reinterpret_cast<u64*>(info->unkC + 0x0C);
    stored[0] = items[0];
    stored[1] = items[1];
    reinterpret_cast<s8*>(info->unk0)[2] = appearance;
    netReport(lbl_80459830, (u32)items[0], items[0], items[1]);
    netReport(lbl_8045986C, reinterpret_cast<s8*>(info->unk0)[2]);
    aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    reinterpret_cast<FlagByte*>(reinterpret_cast<u8*>(&self->m_aidInfos[aid]) + 0x56)->unk0_b0 = 0;
    fn_800D0AE0(self);
    fn_80145B98(lbl_805A03A8, 0xFE);
}

void netMinReport(const char* fmt, ...) {
    va_list args;
    OSLockMutex(&NtReport::s_ReportMutex);
    va_start(args, fmt);
    OSVReport(fmt, args);
    OSUnlockMutex(&NtReport::s_ReportMutex);
}

// HYPOTHESIS: isPermitUploadWatching takes no arguments
extern "C" bool fn_800D99D8() {
    char* base = lbl_80457158;
    if (!NtBase::isParent()) {
        return false;
    }
    if (!nteGlobal::isWatchingEnableTime()) {
        netReport(base + 0x28E4);
        return false;
    }
    if (!nteGlobal::isPermitWatchingGlobal()) {
        netReport(base + 0x290C);
        return false;
    }
    netReport(base + 0x294C);
    return true;
}

// muWifiInterfaceTask::isReadyNextGame (the map name); kept under the stand-in name.
extern "C" bool fn_800D1BC8(muWifiInterfaceTask* self) {
    int detail;
    if (fn_800CBCB0(self, &detail)) { return true; }
    if (!fn_8014592C(lbl_805A03A8)) { return false; }
    for (u8 aid = 0; aid < 4; aid++) {
        bool live;
        if (aid >= 4) {
            live = false;
        } else {
            live = self->getSafeParentAIDInfo()->unk68[aid] == 1;
        }
        if (live && (s8)self->m_aidInfos[aid].unk55 == 4 && !((self->m_aidInfos[aid].unk56_b3 >> 2) & 1)) { return false; }
    }
    return true;
}

void muWifiInterfaceTask::setSelCharGameRule(const void* rule, bool send) {
    const u8* bytes = static_cast<const u8*>(rule);
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    muWifiAIDInfo* own = &m_aidInfos[aid];
    own->unk64[0] = bytes[0];
    own->unk64[1] = bytes[1];
    own->unk64[2] = bytes[2];
    own->unk64[3] = bytes[3];
    own->unk7DData[0] = bytes[0];
    own->unk7DData[1] = bytes[1];
    own->unk7DData[2] = bytes[2];
    own->unk7DData[3] = bytes[3];
    muWifiAIDInfo* parent;
    aid = Network::getParentAID();
    if (aid == 0xFF) {
        parent = NULL;
    } else {
        parent = &m_aidInfos[aid];
    }
    if (parent != NULL) {
        parent->unk7DData[0] = bytes[0];
        parent->unk7DData[1] = bytes[1];
        parent->unk7DData[2] = bytes[2];
        parent->unk7DData[3] = bytes[3];
    }
    if (send) {
        aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        reinterpret_cast<u8*>(&m_aidInfos[aid])[0x56] |= 1;
        fn_800D0AE0(this);
        fn_80145B98(lbl_805A03A8, 0xFE);
    }
}

extern const char lbl_80458C74[];
extern const char lbl_80458CA0[];

// muWifiInterfaceTask::sendInitialAIDInfo (the map name); kept under the stand-in name.
extern "C" void fn_800D53D4(muWifiInterfaceTask* self, u32 aid) {
    if (self->unkFCC_b24 == 4) {
        if (NtBase::isParent()) {
            netReport(lbl_80458C74);
            if (aid < 4) {
                u8 parentAid = Network::getParentAID();
                muWifiAIDInfo* parent;
                if (parentAid == 0xFF) {
                    u8 myAid = Network::getMyAID();
                    if (myAid == 0xFF) {
                        myAid = 4;
                    }
                    parent = &self->m_aidInfos[myAid];
                } else {
                    parent = &self->m_aidInfos[parentAid];
                    if (!parent->unk78_b7) {
                        u8 myAid = Network::getMyAID();
                        if (myAid == 0xFF) {
                            myAid = 4;
                        }
                        parent = &self->m_aidInfos[myAid];
                    }
                }
                parent->unk68[aid] = 3;
            }
        } else {
            netReport(lbl_80458CA0);
        }
        fn_80145B98(lbl_805A03A8, aid);
    }
}

// muWifiInterfaceTask::requestConnection (the map name); kept under the stand-in name.
extern "C" bool fn_800D54DC(muWifiInterfaceTask* self) {
    char* base = lbl_80457158;
    u8 aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    if (aid == 4) {
        netReport(base + 0x1B78);
        return false;
    }
    muWifiAIDInfo* parent;
    aid = Network::getParentAID();
    if (aid == 0xFF) {
        parent = NULL;
    } else {
        parent = &self->m_aidInfos[aid];
    }
    if (parent == NULL) {
        netReport(base + 0x1B90);
        return false;
    }
    self->unkFCC_b24 = 1;
    netReport(base + 0x1BB0);
    aid = Network::getMyAID();
    if (aid == 0xFF) {
        aid = 4;
    }
    reinterpret_cast<u8*>(&self->m_aidInfos[aid])[0x56] |= 1;
    fn_800D0AE0(self);
    fn_80145B98(lbl_805A03A8, 0xFE);
    return true;
}

bool muWifiInterfaceTask::isCancelApprovalCalled(u8* aid, int* player) {
    muWifiAIDInfo* parent;
    u8 parentAID = Network::getParentAID();
    if (parentAID == 0xFF) {
        parent = NULL;
    } else {
        parent = &m_aidInfos[parentAID];
    }
    if (parent == NULL) {
        return true;
    }
    u8 myAID = Network::getMyAID();
    if (myAID == 0xFF) {
        myAID = 4;
    }
    u8* self = reinterpret_cast<u8*>(this);
    u8* mine;
    mine = self + myAID * 0x218;
    if (mine[0xD2] == 0xFF) {
        return false;
    }
    u8* p = reinterpret_cast<u8*>(parent);
    u8 a = p[0x6C];
    u8 b = p[0x6D];
    u8 cur;
    u8 c = p[0x6E];
    if (a == 0xFF || ((cur = self[0xFCD]), cur != a) || self[0xFCE] != b) {
        if (*reinterpret_cast<s32*>(mine + 0xCC) != 0) {
            *reinterpret_cast<s32*>(mine + 0xCC) = 0;
            mine[0xD0] = 0xFF;
            mine[0xD1] = 0;
            mine[0xD2] = 0;
            myAID = Network::getMyAID();
            if (myAID == 0xFF) {
                myAID = 4;
            }
            mine = reinterpret_cast<u8*>(this) + myAID * 0x218;
            reinterpret_cast<s8*>(mine)[0xC6] &= ~1;
            fn_800D0AE0(this);
            fn_80145B98(lbl_805A03A8, 0xFE);
        }
        *aid = self[0xFCD];
        *player = self[0xFCE];
        self[0xFCD] = a;
        self[0xFCE] = b;
        return true;
    }
    if (mine[0xD0] == a && mine[0xD1] == b && mine[0xD2] == c) {
        return false;
    }
    *aid = cur;
    *player = self[0xFCE];
    self[0xFCD] = a;
    self[0xFCE] = b;
    return true;
}
