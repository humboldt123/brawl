#define MU_SELCHAR_UNIT_DECLS
#include <gm/gm_global.h>
#include <gm/gm_set_rule.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <mu/mu_msg.h>
#include <mu/menu.h>
#include <mu/mu_object.h>
#include <mu/mu_rule.h>
#include <mu/selchar/mu_selchar.h>
#include <mu/selchar/mu_selchar_coin.h>
#include <mu/selchar/mu_selchar_hand.h>
#include <mu/selchar/mu_selchar_player_area.h>
#include <types.h>
#include <snd/snd_system.h>
#include <string.h>
#include <gf/gf_pad_system.h>
#include <stdio.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <mu/mu_notice_wnd.h>
#include <nt/network.h>

// The menu helpers take three integers; this unit passes only the first one (see TIPS.md).
extern "C" int exchangeMuSelchkindToMuCharKind__6muMenuFiii(int selchKind);
extern "C" int isCharUsable__6muMenuFiii(int charKind);
extern "C" int isCharRefered__6muMenuFiii(int charKind);
extern "C" void setCharRefered__6muMenuFiii(int charKind);

muSelCharCBCalcWorld::muSelCharCBCalcWorld() {
    m_calcWorldDone = false;
    m_next = nullptr;
}

int muSelCharTask::getSelCharType() {
    return g_GameGlobal->m_selCharData->m_selCharType;
}

// The handicap setting of the rules.
u8 muSelCharTask::getGlobalHandiType() {
    return g_GameGlobal->m_setRule->m_handicap;
}

void muSelCharTask::setHomerunRule(int rule) {
    m_homerunRule = rule;
}

void muSelCharTask::setKumiteType(int type) {
    m_kumiteType = type;
}

void muSelCharTask::setQueueMeleeDropRule(int rule) {
    m_queueMeleeDropRule = rule;
}

bool muSelCharTask::isTeamBattle() {
    return m_isTeamBattle;
}

muSelCharPlayerArea* muSelCharTask::getPlayerArea(int areaIdx) {
    return m_selCharPlayerAreas[areaIdx];
}

int muSelCharTask::getRuleNumStock() {
    return m_ruleNumStock;
}

int muSelCharTask::getHomerunRule() {
    return m_homerunRule;
}

int muSelCharTask::getKumiteType() {
    return m_kumiteType;
}

int muSelCharTask::getQueueMeleeDropRule() {
    return m_queueMeleeDropRule;
}

int muSelCharTask::getDifficulty() {
    return m_difficulty;
}

u8 muSelCharTask::isEntried2P() {
    return m_isEntried2P;
}

muSelCharCoin* muSelCharTask::getCoin(int areaIdx) {
    return m_selCharPlayerAreas[areaIdx]->m_selCharCoin;
}

muSelCharHand* muSelCharTask::getHand(int areaIdx) {
    return m_selCharPlayerAreas[areaIdx]->m_selCharHand;
}

int muSelCharTask::getRuleTaskState() {
    return m_ruleTaskState;
}

void muSelCharTask::setDifficulty(int difficulty) {
    m_difficulty = difficulty;
}

u8 muSelCharTask::isTeamBattleTouch() {
    return m_teamBattleTouch;
}

int muSelCharTask::getMaxDiff() {
    return m_maxDiff;
}

void muSelCharCBCalcWorld::ExecCallback_DRAW_XLU(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    if (m_next) {
        m_next->ExecCallback_DRAW_XLU(timing, obj, param, info);
    }
}

void muSelCharCBCalcWorld::ExecCallback_DRAW_OPA(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    if (m_next) {
        // BUG: the opaque draw is passed on as the translucent draw of the next callback (the call to DRAW_OPA does not match).
        m_next->ExecCallback_DRAW_XLU(timing, obj, param, info);
    }
}

void muSelCharCBCalcWorld::ExecCallback_CALC_VIEW(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    if (m_next) {
        m_next->ExecCallback_CALC_VIEW(timing, obj, param, info);
    }
}

void muSelCharCBCalcWorld::ExecCallback_CALC_MAT(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    if (m_next) {
        m_next->ExecCallback_CALC_MAT(timing, obj, param, info);
    }
}

void muSelCharCBCalcWorld::ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    if (m_next) {
        m_next->ExecCallback_CALC_WORLD(timing, obj, param, info);
    }
    if (timing == nw4r::g3d::ScnObj::CALLBACK_TIMING_C) {
        m_calcWorldDone = true;
    }
}

// Which part of the screen a height is in.
int muSelCharTask::getScreenArea(Vec2f pos) {
    if (pos.m_y >= 16.0f) {
        return 0;
    }
    if (pos.m_y < -5.4f) {
        return 2;
    }
    return 1;
}

void muSelCharTask::setReadyButtonDecided() {
    m_objs[Obj_ReadyButton]->changeAnimN(3, 0x11, true, false);
}

void muSelCharTask::clickHomerunRule() {
    m_objs[Obj_HomerunRule]->changeAnimN(3, 1, true, false);
}

void muSelCharTask::clickKumiteRule() {
    m_objs[Obj_KumiteRule]->changeAnimN(3, 1, true, false);
}

// Clears the five lines of the high score.
void muSelCharTask::eraseAllHighScore() {
    for (int i = 0; i < 5; i++) {
        m_highScoreMsg->printf(i + 2, "");
    }
}

// Called out of line from the total high-score screen.
#pragma dont_inline on
void muSelCharTask::eraseHighScore(u32 msgIndex) {
    m_highScoreMsg->printf(msgIndex, "");
}
#pragma dont_inline reset

// Takes the picture of one of the two characters of a pair off the high score.
// Called out of line from the pair high-score screens.
#pragma dont_inline on
void muSelCharTask::eraseHighScorePairChar(int pairIdx) {
    static const int sPairCharObj[] = { 178, 179 };
    m_highScoreExpand->Remove(m_objs[sPairCharObj[pairIdx]]->m_sceneModel);
}
#pragma dont_inline reset

void muSelCharTask::addObj(int obj) {
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_objs[obj]->m_sceneModel);
}

void muSelCharTask::removeObj(int obj) {
    m_scnGroup->Remove(m_objs[obj]->m_sceneModel);
}

// The area of the player whose hand holds the number of a rule (-1: none).
int muSelCharTask::findRuleNumHoldPlayer() {
    for (int i = 0; i < m_numPlayerAreas; i++) {
        if (m_selCharPlayerAreas[i]->m_selCharHand->m_mode == 4) {
            return i;
        }
    }
    return -1;
}

// Lets the rules screen take the controls (or not).
void muSelCharTask::setRuleControlEnable(u8 enable) {
    m_ruleControlEnable = enable;
    if (m_ruleTask) {
        m_ruleTask->setCtrlFlg(enable ? 0xFF : 0);
    }
}

// The place of a character in the list of the screen (-1: not on the screen). The characters that share a picture (the two
// forms of a character, the Pokemon Trainer and his Pokemon) are found by the first of them.
int muSelCharTask::getCharListIdx(int charKind) {
    int i;
    int idx = -1;
    for (i = 0; i < m_charListNum; i++) {
        int kind = m_charList[i];
        switch (charKind) {
        case 3:
        case 4:
            if (kind == 3) {
                idx = i;
            }
            break;
        case 14:
        case 15:
            if (kind == 14) {
                idx = i;
            }
            break;
        case 27:
        case 28:
        case 29:
        case 30:
            if (kind == 27) {
                idx = i;
            }
            break;
        default:
            if (kind == charKind) {
                idx = i;
            }
            break;
        }
        if (idx >= 0) {
            break;
        }
    }
    return idx;
}

// The center of the picture of a character.
bool muSelCharTask::getFacePos(Vec2f* pos, int charKind) {
    Rect2D rect;
    if (!getFaceRect(&rect, charKind)) {
        return false;
    }
    pos->m_x = (rect.m_right + rect.m_left) / 2.0f;
    pos->m_y = (rect.m_down + rect.m_up) / 2.0f;
    return true;
}

// The corners of the picture of a character.
bool muSelCharTask::getFaceRect(Rect2D* rect, int charKind) {
    int idx = getCharListIdx(charKind);
    if (idx < 0) {
        return false;
    }
    m_objs[Obj_FaceBase + idx]->getRect3D(rect, "colL", "colR");
    return true;
}

// Puts the cursor (the number of a player on a coin) on the picture of a character.
void muSelCharTask::setCursor(int charKind, MuObject* cursor) {
    cursor->m_sceneModel->DetachFromParent();
    int idx = getCharListIdx(charKind);
    if (idx >= 0) {
        m_expands[Expand_FaceBase + idx]->PushBack(cursor->m_sceneModel, "pos00");
    }
}

void muSelCharTask::dispCanceled() {
    MuObject* obj = m_objs[2];
    obj->setFrameMatCol(15.0f);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
}

void muSelCharTask::setReadyButtonPointed(u8 pointed) {
    if (m_readyButtonPointed != pointed) {
        if (m_readyState == 2) {
            m_objs[Obj_ReadyButton]->changeAnimN(pointed ? 2 : 1, 0x11, true, false);
        }
        m_readyButtonPointed = pointed;
    }
}

void muSelCharTask::setVisibleDiff(bool visible) {
    m_scnGroup->Remove(m_diffExpand);
    if (visible) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_diffExpand);
    }
}

void muSelCharTask::setVisibleStock(bool visible) {
    m_scnGroup->Remove(m_stockExpand);
    if (visible) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_stockExpand);
    }
}

// The level of the computers: the arrows only when there is more than one level.
void muSelCharTask::initDiffObj() {
    nw4r::g3d::ScnMdlExpand* expand = m_diffExpand;
    if (m_maxDiff > 1) {
        expand->PushBack(m_objs[Obj_DiffUp]->m_sceneModel, "pos00");
        expand->PushBack(m_objs[Obj_DiffDown]->m_sceneModel, "pos01");
    }
    m_objs[Obj_DiffLevel]->setFrameVisible(m_maxDiff);
    if (getSelCharType() == 7) {
        m_objs[Obj_DiffLevel]->setFrameTex(1.0f);
    }
}

void muSelCharTask::initStockObj() {
    nw4r::g3d::ScnMdlExpand* expand = m_stockExpand;
    expand->PushBack(m_objs[Obj_StockUp]->m_sceneModel, "pos00");
    expand->PushBack(m_objs[Obj_StockDown]->m_sceneModel, "pos01");
}

// HYPOTHESIS: the priorities of the coins are read and not used (the order is not changed any more).
void muSelCharTask::arrangeCoinPrio() {
    for (int i = 0; i < m_numPlayerAreas; i++) {
        if (m_selCharPlayerAreas[i]->m_playerKind != 0) {
            m_selCharPlayerAreas[i]->m_selCharCoin->getCoinDrawPrio();
        }
    }
}

// Whether a character is shown on the screen: 0 not at all, 1 as a picture that cannot be picked yet, 2 normally.
// HYPOTHESIS: the meaning of the values and of the flag (it skips the test of the unlocked characters).
int muSelCharTask::getCharApparState(int selchKind, bool refer) {
    int charKind = exchangeMuSelchkindToMuCharKind__6muMenuFiii(selchKind);
    if (!refer && !isCharUsable__6muMenuFiii(charKind)) {
        return 0;
    }
    if (!isCharRefered__6muMenuFiii(charKind)) {
        return 1;
    }
    return 2;
}

// The longest time of a battle in minutes (the shortest is 1 minute for kinds 1 and 0x12, 0 for no limit otherwise).
int muSelCharTask::getRuleTimeMax() {
    int max = 99;
    if (getSelCharType() == 0x12) {
        max = 60;
    }
    return max;
}

int muSelCharTask::getRuleTimeMin() {
    switch (getSelCharType()) {
    case 1:
    case 0x12:
        return 1;
    }
    return 0;
}

static const muSelCharTypeData sSelCharTypeData[] = {
    { 0x0004, 1, 1, 0x10, 0, 0x0000, -1 },
    { 0x0004, 5, 5, 0x10, 0, 0x0000, 8262 },
    { 0x0004, 3, 3, 0x10, 0, 0x0000, 8262 },
    { 0x0004, 1, 1, 0x10, 0, 0x0000, -1 },
    { 0x00B4, 1, 1, 0x40, 1, 0x0000, 8240 },
    { 0x00B4, 2, 14, 0x40, 2, 0x0000, 8241 },
    { 0x00B4, 12, 13, 0x40, 3, 0x0000, 8243 },
    { 0x00B4, 6, 7, 0x40, 4, 0x0000, 8245 },
    { 0x00B4, 8, 9, 0x40, 5, 0x0000, 8246 },
    { 0x00B4, 10, 11, 0x40, 6, 0x0000, 8262 },
    { 0x00B4, 5, 5, 0x40, 12, 0x0000, 8244 },
    { 0x00B4, 3, 4, 0x40, 0, 0x0000, 8262 },
    { 0x0004, 1, 1, 0x30, 0, 0x0000, -1 },
    { 0x00B7, 6, 6, 0x40, 6, 0x0000, 8262 },
    { 0x00B7, 7, 7, 0x40, 5, 0x0000, 8246 },
    { 0x00B7, 5, 5, 0x40, 3, 0x0000, 8243 },
    { 0x00B7, 1, 1, 0x20, 0, 0x0000, 8247 },
    { 0x00B7, 2, 2, 0x20, 0, 0x0000, 8248 },
    { 0x00B7, 3, 3, 0x00, 0, 0x0000, 8262 },
};

const muSelCharTypeData* muSelCharTask::getSelCharTypeData(int selCharType) {
    return &sSelCharTypeData[selCharType];
}

// The high score of a kind of the select screen; the Multi-Man Brawl (kinds 9 and 13) has one for each of its kinds.
int muSelCharTask::getSelCharTypeHighScoreKind(int selCharType) {
    static const u8 sKumiteHighScoreKind[] = { 6, 7, 8, 9, 10, 11 };
    if (selCharType == 9 || selCharType == 13) {
        return sKumiteHighScoreKind[m_kumiteType];
    }
    return getSelCharTypeData(selCharType)->m_highScoreKind;
}

int muSelCharHand::getMode(int* arg) {
    if (arg) {
        *arg = m_modeArg;
    }
    return m_mode;
}

extern const u8 lbl_10_rodata_2C8[0x28];

void muSelCharTask::createWifiModel(nw4r::g3d::ResFile* resFile) {
    createObjResFile(lbl_10_rodata_2C8, 5, resFile, nullptr, false);
}

extern MuAnimIdxData lbl_10_data_610[5][2];

void muSelCharTask::dispDiffLevel(int level, bool enabled) {
    lbl_10_data_610[level][!enabled].m_flags = 1;
    m_objs[Obj_DiffLevel]->setAnimIdx(&lbl_10_data_610[level][!enabled], false);
}


void muSelCharTask::ruleCall() {
    SndID sound = (SndID)0x202B;
    switch (unk5D4) {
    case 0:
        sound = (SndID)0x202B;
        break;
    case 1:
        sound = (SndID)0x202D;
        break;
    case 2:
        sound = (SndID)0x202C;
        break;
    }
    g_sndSystem->playSEwithFixEffect(sound, 1.0f, 1.0f);
}

muSelCharTask* muSelCharTask::create(void* data) {
    muSelCharTask* task = new ((HeapType)0x2A) muSelCharTask;
    task->initProc(data);
    return task;
}

void muSelCharTask::sendSystemAllCharKind() {
    for (int area = 0; area < m_numPlayerAreas; area++) {
        m_selCharPlayerAreas[area]->sendSystemCharKind();
    }
}

int muSelCharTask::getRuleDispType() {
    int dispType = (getSelCharTypeData(getSelCharType())->m_unk4 >> 5) & 7;
    if (m_unk5D0) {
        dispType = 3;
    }
    if (getSelCharType() == 12 && isWifiGameMaster()) {
        dispType = 0;
    }
    return dispType;
}

int muSelCharTask::getCollKindPlayerArea(Vec2f* pos, int* areaIdx, int* arg, float* rect) {
    for (int i = 0; i < m_numPlayerAreas; i++) {
        int collKind = m_selCharPlayerAreas[i]->getCollKind(pos, arg, rect);
        if (collKind) {
            *areaIdx = i;
            return collKind;
        }
    }
    return 0;
}

void muSelCharTask::setRuleNumHoldCursorVisible(bool visible) {
    MuObject* obj = m_objs[Obj_RuleNumHold];
    m_scnGroup->Remove(obj->m_sceneModel);
    if (visible) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, obj->m_sceneModel);
        obj->setFrameMatCol(0.0f);
        obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    }
}

void muSelCharTask::setFaceIconMotion(int faceIdx, int motion) {
    static MuAnimNameData sMotions[] = {
        { 10.0f, 10.0f, 0, NULL, 0x11 },
        { 0.0f, 9.0f, 0, NULL, 1 },
        { 10.0f, 30.0f, 0, NULL, 0x10 },
        { 10.0f, 20.0f, 0, NULL, 1 },
    };
    MuObject* obj = m_objs[Obj_FaceBase + faceIdx];
    char animName[64];
    const nw4r::g3d::ResMdlData* mdl = obj->m_resMdl.ptr();
    strcpy(animName, mdl->m_toName != 0 ? reinterpret_cast<const char*>(mdl) + mdl->m_toName : NULL);
    strcat(animName, "__0");
    sMotions[motion].m_name = animName;
    obj->setAnimName(&sMotions[motion], false);
    obj->m_scnMdl->SetPriorityDrawOpa(0x49);
    obj->m_scnMdl->SetPriorityDrawXlu(0x49);
}

int muSelCharTask::findFreeWiiControllerNo() {
    gfPadStatus status;
    for (int controller = 4; controller < 8; controller++) {
        g_gfPadSystem->getSysPadStatus(controller, &status);
        if (status.m_error == 0 && status.m_controllerType != 0) {
            int area;
            for (area = 0; area < m_numPlayerAreas; area++) {
                if (m_selCharPlayerAreas[area]->m_controllerNo == controller) {
                    break;
                }
            }
            if (area >= m_numPlayerAreas) {
                return controller;
            }
        }
    }
    return -1;
}

void muSelCharTask::dispWifiPrompt(u32 message) {
    MuObject* obj = m_objs[187];
    m_highScoreMsg->printIndex(7, message, NULL);
    m_scnGroup->Remove(obj->m_sceneModel);
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    obj->setFrameNode(0.0f);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    g_sndSystem->playSE((SndID)0x1EF3, -1, 0, 0, -1);
}

int muSelCharTask::getCollCharKind(Vec2f* pos) {
    for (int faceIdx = 0; faceIdx < m_charListNum; faceIdx++) {
        Rect2D rect;
        m_objs[Obj_FaceBase + faceIdx]->getRect3D(&rect, "colL", "colR");
        if (rect.m_left <= pos->m_x && rect.m_right >= pos->m_x &&
            rect.m_down <= pos->m_y && rect.m_up >= pos->m_y) {
            return m_charList[faceIdx];
        }
    }
    return 0x28;
}

void muSelCharTask::dispHighScoreDiffLevel(int level) {
    MuObject* obj = m_objs[177];
    nw4r::g3d::ScnMdlExpand* expand;
    const char* node;
    if (getSelCharType() == 7) {
        expand = m_highScoreExpand;
        node = "pos04";
    } else {
        expand = m_diffExpand;
        node = "pos02";
    }
    expand->Remove(obj->m_sceneModel);
    if (level >= 0) {
        obj->setFrameVisible(level + 1);
        expand->PushBack(obj->m_sceneModel, node);
    }
}

extern char lbl_10_data_6E8[8];

// Called out of line from the pair high-score screens.
#pragma dont_inline on
void muSelCharTask::setHighScoreStyle(int style) {
    MuObject* object;
    nw4r::g3d::ScnMdlExpand* expand = m_highScoreExpand;
    object = m_objs[176];
    m_scnGroup->Remove(expand);
    if (style != 0) {
        float frame = (float)style - 1.0f;
        object->setFrameVisible(frame);
        object->setFrameNode(frame);
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, expand);
    }
}
#pragma dont_inline reset

void muSelCharTask::dispNumStock(int count, bool enabled) {
    int index;
    nw4r::g3d::ScnMdlExpand* expand = m_stockExpand;
    lbl_10_data_610[count - 1][!enabled].m_flags = 3;
    m_objs[167]->setAnimIdx(&lbl_10_data_610[count - 1][!enabled], false);
    for (index = 0; index < 5; index++) {
        expand->Remove(m_objs[168 + index]->m_sceneModel);
    }
    char node[8];
    for (index = 0; index < count; index++) {
        sprintf(node, lbl_10_data_6E8, index + 2);
        expand->PushBack(m_objs[168 + index]->m_sceneModel, node);
    }
}

int muSelCharTask::strcatHighScoreUnit(char* text, int capacity, int kind) {
    int message = -1;
    switch (kind) {
    case 2:
        if (g_GameGlobal->getLanguage() == 1) message = 26;
        else message = 25;
        break;
    case 3: message = 27; break;
    case 5: message = 28; break;
    case 6: message = 29; break;
    case 4: message = 30; break;
    }
    int length = strlen(text);
    if (message >= 0) {
        int size;
        const char* unit = m_highScoreMsg->getMsgData(message, &size);
        memcpy(text + length, unit, size);
        length += size;
        text[length] = 0;
        length++;
    }
    return length;
}

void muSelCharTask::sendWifiGameRule(bool force, bool send) {
    __typeof__(unk637) rule;
    memset(&rule, 0, sizeof(rule));
    rule.unk0_6 = unk5D4;
    rule.unk0_4 = m_handiMode;
    rule.unk0_2 = m_isTeamBattle != 0;
    rule.unk2 = unk5D8;
    rule.unk3 = m_ruleNumStock;
    rule.unk1_5 = m_kumiteType;
    rule.unk1_4 = m_homerunRule;
    if (force || memcmp(&rule, &unk637, sizeof(rule)) != 0) {
        g_muWifiInterfaceTask->setSelCharGameRule(&rule, send);
        unk637 = rule;
    }
}

int muSelCharTask::isPushedUnusedControllerStartButton() {
    bool used[8];
    memset(used, 0, sizeof(used));
    muSelCharPlayerArea* player;
    for (int area = 0; area < m_numPlayerAreas; area++) {
        player = getPlayerArea(area);
        int controller = player->m_controllerNo;
        if (controller >= 0) {
            used[controller] = true;
        }
        controller = player->unk426Flag ? player->unk42C : -1;
        if (controller >= 0) {
            used[controller] = true;
        }
    }
    for (int controller = 0; controller < 8; controller++) {
        if (!used[controller]) {
            gfPadStatus status;
            g_gfPadSystem->getSysPadStatus(controller, &status);
            if (status.m_error == 0 && (status.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_START)) {
                return controller;
            }
        }
    }
    return -1;
}

bool muSelCharTask::canWifiEntry() {
    int area;
    bool canEntry = false;
    int players = 0;
    int available = 0;
    muWifiInterfaceTask* wifi = g_muWifiInterfaceTask;
    if (getSelCharType() == 12) {
        for (area = 0; area < m_numPlayerAreas; area++) {
            muSelCharPlayerArea* player = m_selCharPlayerAreas[area];
            switch (player->_0) {
            case 0:
                if (player->m_playerKind != 2) {
                    players++;
                } else {
                    available++;
                }
                break;
            case 1:
                int state = player->getWifiStateByWifiCond();
                if (state == 0 && wifi->testRequestApproval(area)) {
                    available++;
                }
                if (state == 1 || state == 2) {
                    players++;
                }
                break;
            case 2:
            default:
                break;
            }
        }
        if (available > 0 && players < 3) {
            canEntry = true;
        }
    }
    return canEntry;
}

extern const u32 lbl_10_rodata_4F8[6];

void muSelCharTask::dispKumiteRule(int type) {
    int motion = 0;
    m_highScoreMsg->printIndex(1, lbl_10_rodata_4F8[type], NULL);
    int selCharType = getSelCharType();
    int group;
    if (isSelCharTypeIn(selCharType, 0, 4)) {
        group = 0;
    } else if (isSelCharTypeIn(selCharType, 4, 12)) {
        group = 1;
    } else {
        bool inGroup = isSelCharTypeIn(selCharType, 12, 16);
        group = 3;
        if (inGroup) {
            group = 2;
        }
    }
    if (group == 2 && !isWifiGameMaster()) {
        motion = 4;
    }
    m_objs[Obj_KumiteRule]->changeAnimN(motion, 0x10, false, false);
}

void muSelCharTask::wifiEntryPlayer() {
    muSelCharPlayerArea* player;
    int controller;
    int area;
    int entryArea = -1;
    bool replaceCpu = false;
    if (canWifiEntry()) {
        controller = isPushedUnusedControllerStartButton();
        if (controller >= 0) {
            for (area = 0; area < m_numPlayerAreas; area++) {
                player = getPlayerArea(area);
                if (player->_0 == 1 && player->getWifiStateByWifiCond() == 0) {
                    entryArea = area;
                    break;
                }
            }
            if (entryArea < 0) {
                for (area = 0; area < m_numPlayerAreas; area++) {
                    player = getPlayerArea(area);
                    if (player->_0 == 0 && player->m_playerKind == 2) {
                        entryArea = area;
                        replaceCpu = true;
                        break;
                    }
                }
            }
            if (entryArea >= 0) {
                player = m_selCharPlayerAreas[entryArea];
                if (replaceCpu) {
                    player->wifiExitCpuPlayer();
                }
                player->wifiRequestPlayerEntry(controller);
            }
        }
    }
}

void muSelCharTask::processDefault() {
    u8 selected;
    int i;
    unk64C--;
    if (isSelCharTypeOnline(getSelCharType())) wifiMain();
    int ruleState = m_ruleTaskState;
    if (ruleState != 0) mainRule();
    faceIconMain();
    for (i = 0; i < m_numPlayerAreas; i++) m_selCharPlayerAreas[i]->main();
    switch (unk40) {
    case 0:
        if (ruleState == 0) selectingMain();
        break;
    case 1:
        if (--unk5F8 <= 0) {
            setToGlobal();
            if (isSelCharTypeOnline(getSelCharType())) g_muWifiInterfaceTask->noticeEndOfCharSelect();
            setState(2, 0, 0);
        }
        break;
    case 3:
        unk40 = 4;
        break;
    case 6: {
        int result;
        if (unk60C->m_step != 3) result = 0;
        else {
            selected = unk60C->m_selected;
            closeWifiDlg();
            result = 2;
            if (selected) result = 1;
        }
        switch (result) {
        case 1: setState(4, 0, 0); break;
        case 2: setState(0, 0, 0); break;
        }
        break;
    }
    case 7: {
        int result;
        if (unk60C->m_step != 3) result = 0;
        else {
            selected = unk60C->m_selected;
            closeWifiDlg();
            result = 2;
            if (selected) result = 1;
        }
        if (result != 0) {
            if (unk644 == 3) setState(4, 0, 0);
            else setState(5, 0, 0);
        }
        break;
    }
    case 8: {
        u8 aid;
        int player;
        if (g_muWifiInterfaceTask->isCancelApprovalCalled(&aid, &player) && aid == unk635Byte && player == unk636Byte) {
            closeWifiDlg();
            setState(0, 0, 0);
        } else {
            int result;
            if (unk60C->m_step != 3) result = 0;
            else {
                selected = unk60C->m_selected;
                closeWifiDlg();
                result = 2;
                if (selected) result = 1;
            }
            if (result != 0) {
                g_muWifiInterfaceTask->answerRequestedEntry(result == 1, unk635Byte, unk636Byte);
                setState(0, 0, 0);
            }
        }
        break;
    }
    }
}

void muSelCharTask::setRuleNumStock(int count) {
    u8 aid;
    m_ruleNumStock = count;
    int type = getSelCharType();
    int group;
    if (isSelCharTypeIn(type, 0, 4)) group = 0;
    else if (isSelCharTypeIn(type, 4, 12)) group = 1;
    else {
        bool inGroup = isSelCharTypeIn(type, 12, 16);
        group = 3;
        if (inGroup) group = 2;
    }
    if (group == 2) {
        muWifiInterfaceTask& wifi = *g_muWifiInterfaceTask;
        int master = wifi.unkFCA_b5;
        if (!master) {
            aid = Network::getMyAID();
            if (aid == 0xFF) aid = 4;
            u8 masterAid = wifi.getRuleMasterAID();
            master = masterAid == aid;
        }
        if (master) sendWifiGameRule(false, true);
    }
}

extern const u8 lbl_10_rodata_4F0[4][2];

void muSelCharTask::appearHomerunRule() {
    int rule = m_homerunRule;
    int disabled = 0;
    int type = getSelCharType();
    int group;
    if (isSelCharTypeIn(type, 0, 4)) group = 0;
    else if (isSelCharTypeIn(type, 4, 12)) group = 1;
    else {
        bool inGroup = isSelCharTypeIn(type, 12, 16);
        group = 3;
        if (inGroup) group = 2;
    }
    if (group == 2 && !isWifiGameMaster()) disabled = 1;
    int motion;
    if (rule == 0) motion = lbl_10_rodata_4F0[disabled][0];
    else motion = lbl_10_rodata_4F0[disabled][1];
    m_objs[Obj_HomerunRule]->changeAnimN(motion, 0x10, false, false);
    MuObject* object = m_objs[Obj_HomerunRule];
    object->changeAnimN(2, 1, true, false);
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, object->m_sceneModel);
}

extern const u8 lbl_10_rodata_118[8];

int muSelCharTask::bButtonHoldProc() {
    float frame = 0.0f;
    int player = -1;
    int count = 0;
    for (int area = 0; area < m_numPlayerAreas; area++) {
        int held = m_selCharPlayerAreas[area]->m_selCharHand->m_bButtonCnt;
        if (held > count) {
            count = held;
            player = area;
        }
    }
    if (player >= 0) {
        int controller = m_selCharPlayerAreas[player]->getControllerKind();
        if (controller < 4) frame = lbl_10_rodata_118[controller];
    }
    MuObject* object = m_objs[3];
    m_scnGroup->Remove(object->m_sceneModel);
    if (count > 0) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, object->m_sceneModel);
        object->setFrameVisible(frame);
        object->setFrameNode(count);
    }
    if (count >= 30) return player;
    return -1;
}

extern const u8 lbl_10_rodata_4E0[4];
extern const u8 lbl_10_rodata_4E8[4][2];

void muSelCharTask::setReadyState(int state) {
    MuObject* object = m_objs[Obj_ReadyButton];
    m_scnGroup->Remove(object->m_sceneModel);
    if (state != 0) {
        m_scnGroup->Insert(m_scnGroup->sceneItemsCount, object->m_sceneModel);
        object->changeAnimN(state != 1, 0x11, true, false);
    }
    u32 motion = lbl_10_rodata_4E0[state];
    object = m_objs[1];
    int type = getSelCharType();
    int group;
    if (isSelCharTypeIn(type, 0, 4)) group = 0;
    else if (isSelCharTypeIn(type, 4, 12)) group = 1;
    else {
        bool inGroup = isSelCharTypeIn(type, 12, 16);
        group = 3;
        if (inGroup) group = 2;
    }
    int animation;
    bool play;
    if (motion <= 1) {
        animation = lbl_10_rodata_4E8[group][0];
        play = motion == 1;
    } else {
        animation = lbl_10_rodata_4E8[group][1];
        play = true;
    }
    object->changeAnimN(animation, 0x18, play, false);
    m_readyState = state;
}

extern const u8 lbl_10_rodata_4F0[4][2];
extern const u8 lbl_10_rodata_4E8[4][2];

void muSelCharTask::assignTeam() {
    int count = 0;
    for (int area = 0; area < m_numPlayerAreas; area++) {
        if (m_selCharPlayerAreas[area]->m_playerKind != 0) {
            count++;
        }
    }
    int* assignment;
    int countAssigned;
    int team;
    int teamSize;
    for (teamSize = count - 1; teamSize > 0; teamSize--) {
        if (count % teamSize == 0) {
            break;
        }
    }
    if (teamSize <= 0) {
        teamSize = 1;
    }
    int teams[4];
    assignment = teams;
    countAssigned = 0;
    for (team = 0; team < 3 && countAssigned < 4; team++) {
        for (int member = 0; member < teamSize && countAssigned < 4;) {
            *assignment = team;
            member++;
            assignment++;
            countAssigned++;
        }
    }
    int* assignedTeam = teams;
    for (int area = 0; area < m_numPlayerAreas; area++) {
        muSelCharPlayerArea* player = m_selCharPlayerAreas[area];
        if (player->m_playerKind != 0) {
            player->setTeamColor(*assignedTeam, player->m_teamSet);
            assignedTeam++;
        }
    }
}

void muSelCharTask::dispHomerunRule(int rule) {
    int mode = 0;
    int selCharType = getSelCharType();
    int group;
    if (isSelCharTypeIn(selCharType, 0, 4)) {
        group = 0;
    } else if (isSelCharTypeIn(selCharType, 4, 12)) {
        group = 1;
    } else {
        bool inGroup = isSelCharTypeIn(selCharType, 12, 16);
        group = 3;
        if (inGroup) {
            group = 2;
        }
    }
    if (group == 2 && !isWifiGameMaster()) {
        mode = 1;
    }
    m_objs[Obj_HomerunRule]->changeAnimN(rule == 0 ? lbl_10_rodata_4F0[mode][0] : lbl_10_rodata_4F0[mode][1], 0x10, false, false);
}

bool muSelCharTask::isWifiMode() {
    int selCharType = getSelCharType();
    int group;
    if (isSelCharTypeIn(selCharType, 0, 4)) {
        group = 0;
    } else if (isSelCharTypeIn(selCharType, 4, 12)) {
        group = 1;
    } else {
        bool inGroup = isSelCharTypeIn(selCharType, 12, 16);
        group = 3;
        if (inGroup) {
            group = 2;
        }
    }
    if (group == 0) goto offline;
    selCharType = getSelCharType();
    if (isSelCharTypeIn(selCharType, 0, 4)) {
        group = 0;
    } else if (isSelCharTypeIn(selCharType, 4, 12)) {
        group = 1;
    } else {
        bool inGroup = isSelCharTypeIn(selCharType, 12, 16);
        group = 3;
        if (inGroup) {
            group = 2;
        }
    }
    if (group != 1) goto online;
offline:
    return false;
online:
    return true;
}

// HYPOTHESIS: kind 4 keeps the stock in the three bits of the select data. Otherwise rule byte 2
// chooses the stock count (1) or the time in minutes.
void muSelCharTask::setRuleNumToGlobal() {
    gmSelCharData* sel = g_GameGlobal->m_selCharData;
    gmSetRule* rule = g_GameGlobal->m_setRule;
    int type = sel->m_selCharType;
    if (type == 4) {
        sel->unk2E_b4 = m_ruleNumStock;
        return;
    }
    int ruleKind = reinterpret_cast<u8*>(rule)[2];
    if (ruleKind == 1) {
        rule->m_stockCount = m_ruleNumStock;
    } else {
        rule->m_timeMinutes = unk5D8;
    }
}

int muSelCharTask::isWifiGameMaster() {
    u8 aid;
    muWifiInterfaceTask& wifi = *g_muWifiInterfaceTask;
    int master = wifi.unkFCA_b5;
    if (!master) {
        aid = Network::getMyAID();
        if (aid == 0xFF) {
            aid = 4;
        }
        u8 masterAid = wifi.getRuleMasterAID();
        master = masterAid == aid;
    }
    return master;
}

extern const u8 lbl_10_rodata_2F4[16];
void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* obj, u32 timing);
void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* obj, u32 op);

// Puts the world-matrix callback in front of the callback on each listed model.
void muSelCharTask::bindCalcWorldFlag() {
    nw4r::g3d::ScnMdl* model;
    for (int index = 0; index < 15; ++index) {
        int obj;
        if (index >= 0 && index < 15) {
            obj = lbl_10_rodata_2F4[index];
        } else {
            obj = Obj_Num;
        }
        MuObject* object = m_objs[obj];
        if (object) {
            model = object->getSceneModel();
            m_calcWorldCBs[index].m_next = model->m_callback;
            model->m_callback = &m_calcWorldCBs[index];
            ScnObj_EnableCallbackExecOp(model, 4);
            ScnObj_EnableCallbackTiming(model, 1);
        }
    }
}

int muSelCharTask::dispTargetTotalHighScore() {
    int style = 1;
    int index = 0;
    int allClear = 0;
    do {
        if (isCharHighScoreCleared(index) == 0) {
            break;
        }
        index++;
    } while (index < 0x23);
    if (index >= 0x23) {
        allClear = 1;
    }
    if (allClear != 0) {
        style = 2;
    }
    setHighScoreStyle(style);
    if (allClear != 0) {
        int sum = 0;
        for (int c = 0; c < 0x23; c++) {
            sum += getCharHighScore(c, m_difficulty, 0, 0);
        }
        dispHighScore(3, sum, 1);
    } else {
        eraseHighScore(3);
    }
    return allClear;
}

// HYPOTHESIS: byte flag, then each character as a word and its color as a byte.
struct muSelCharScorePair {
    u8 m_valid;
    char _pad1[3];
    int m_charKind0;
    u8 m_color0;
    char _pad2[3];
    int m_charKind1;
    u8 m_color1;
};

void muSelCharTask::dispOnlinePairHighScore() {
    setHighScoreStyle(3);
    muSelCharScorePair chars;
    int style;
    int level;
    int score = getOnlinePairHighScore(&style, &level, &chars);
    dispHighScore(4, score, style);
    dispHighScoreDiffLevel(level);
    int valid = chars.m_valid;
    if (valid != 0) {
        dispHighScorePairChar(0, chars.m_charKind0, chars.m_color0);
        dispHighScorePairChar(1, chars.m_charKind1, chars.m_color1);
    } else {
        eraseHighScorePairChar(0);
        eraseHighScorePairChar(1);
    }
}

void muSelCharTask::dispPairHighScore() {
    setHighScoreStyle(3);
    muSelCharScorePair chars;
    int style;
    int level;
    int score = getPairHighScore(m_difficulty, &style, &level, &chars);
    dispHighScore(4, score, style);
    int valid = chars.m_valid;
    if (valid != 0) {
        dispHighScorePairChar(0, chars.m_charKind0, chars.m_color0);
        dispHighScorePairChar(1, chars.m_charKind1, chars.m_color1);
    } else {
        eraseHighScorePairChar(0);
        eraseHighScorePairChar(1);
    }
    if (getSelCharType() != 7) {
        dispHighScoreDiffLevel(level);
    }
}

// Marks a character as seen and takes the "not seen" picture off its icon.
void muSelCharTask::setReferFlag(int selchKind) {
    int referred = isCharRefered__6muMenuFiii(exchangeMuSelchkindToMuCharKind__6muMenuFiii(selchKind));
    int state = 1;
    if (referred != 0) {
        state = 2;
    }
    if (state != 2) {
        setCharRefered__6muMenuFiii(exchangeMuSelchkindToMuCharKind__6muMenuFiii(selchKind));
        int idx = getCharListIdx(selchKind);
        // HYPOTHESIS: 0x70 is the first "not seen" picture; expand 1 holds those pictures.
        m_expands[1]->Remove(m_objs[idx + 0x70]->m_sceneModel);
    }
}

bool muSelCharTask::closeWifiDlg() {
    if (unk60C != nullptr) {
        unk60C->exit();
        unk60C = nullptr;
        setRuleControlEnable(1);
        if (m_ruleTaskState == 0) {
            for (int i = 0; i < m_numPlayerAreas; i++) {
                m_selCharPlayerAreas[i]->m_selCharHand->setBanVisible(false);
            }
            for (int i = 0; i < m_numPlayerAreas; i++) {
                m_selCharPlayerAreas[i]->m_selCharCoin->setBanVisible(false);
            }
        }
        return true;
    }
    return false;
}

void muSelCharTask::faceIconMain() {
    // BYTES: a cursor stepped by one pointer keeps the picture in r31 and the cursor in r30.
    MuObject* icon;
    muSelCharTask* cursor = this;
    int i = 0;
    while (i < m_charListNum) {
        icon = cursor->m_objs[Obj_FaceBase];
        if (icon->m_modelAnim->m_anmObjChrRes->GetUpdateRate() <= 0.0f) {
            icon->m_scnMdl->SetPriorityDrawOpa(i + 0x17);
            icon->m_scnMdl->SetPriorityDrawXlu(i + 0x17);
        }
        cursor = reinterpret_cast<muSelCharTask*>(reinterpret_cast<char*>(cursor) + 4);
        i++;
    }
}

sndSystem* sndSystem::getInstance() {
    return g_sndSystem;
}

MuSelchkind muSelCharPlayerArea::getCharKind() {
    return m_charKind;
}

int muSelCharPlayerArea::getDirRepeat() {
    return m_menuController.m_trigger;
}

int gfPadStatus::isButtonAOn() {
    return (m_buttonsCurrentFrame2.bits >> 8) & 1;
}

int gfPadStatus::isButtonAOnEdge() {
    return (m_buttonsPressedThisFrame.bits >> 8) & 1;
}

int gfPadStatus::isButtonAOnRepeat() {
    return (m_buttonsPressedThisFrame2.bits >> 8) & 1;
}

void muSelCharTask::removeCursor(MuObject* cursor) {
    cursor->m_sceneModel->DetachFromParent();
}

void muSelCharTask::readyStateMain() {
    if (m_readyState == 1) {
        MuObject* object = m_objs[Obj_ReadyButton];
        if (object->isNodeAnimFinished()) {
            m_readyState = 2;
            object->changeAnimN(m_readyButtonPointed ? 2 : 1, 0x11, true, false);
            object = m_objs[1];
            int selCharType = getSelCharType();
            int group;
            if (isSelCharTypeIn(selCharType, 0, 4)) {
                group = 0;
            } else if (isSelCharTypeIn(selCharType, 4, 12)) {
                group = 1;
            } else {
                bool inGroup = isSelCharTypeIn(selCharType, 12, 16);
                group = 3;
                if (inGroup) {
                    group = 2;
                }
            }
            object->changeAnimN(lbl_10_rodata_4E8[group][1], 0x18, true, false);
        }
    }
}

void muSelCharTask::setVisibleAllPlayersHandi(bool visible) {
    for (int i = 0; i < m_numPlayerAreas; i++) {
        m_selCharPlayerAreas[i]->setHandiVisible(visible);
    }
    if (!visible) {
        int i;
        int* modeArg = nullptr;
        int arg;
        modeArg = &arg;
        for (i = 0; i < m_numPlayerAreas; i++) {
            muSelCharHand* hand = getHand(i);
            if (modeArg != nullptr) {
                arg = hand->m_modeArg;
            }
            // HYPOTHESIS: mode 5 is the one that opens the handicap list.
            if (hand->m_mode == 5) {
                getHand(i)->setMode(0, 0);
                m_selCharPlayerAreas[arg]->closeHandiList();
            }
        }
    }
}

// HYPOTHESIS: the style of the total high score for each kind of high score, in rows of six bytes.
extern const u8 lbl_10_rodata_128[][6];

void muSelCharTask::dispVersusHighScore() {
    const u8* styles;
    int offset = getSelCharTypeHighScoreKind(getSelCharType()) * 6;
    styles = lbl_10_rodata_128[0];
    setHighScoreStyle(6);
    int c;
    int sum = 0;
    for (c = 0; c < 0x23; c++) {
        sum += getCharHighScore(c, m_difficulty, 0, 0);
    }
    dispHighScore(3, sum, offset[styles]);
}

extern "C" int exchangeSelchkind2SelCharNarrationSndID__6muMenuFiii(int kind);
extern "C" int exchangeMuSelchkind2MuStockchkind__6muMenuFiii(int kind);

// The original passes the area index along; the function itself takes nothing.
extern "C" void arrangeCoinPrio__13muSelCharTaskFv(muSelCharTask* task, int areaIdx);

void muSelCharTask::decideCharKind(int areaIdx, int charKind) {
    muSelCharCoin* coin;
    muSelCharPlayerArea* area = m_selCharPlayerAreas[areaIdx];
    coin = area->m_selCharCoin;
    area->dispCharPicEffect();
    setFaceIconMotion(getCharListIdx(charKind), 2);
    coin->dispEffect();
    if (charKind != 0x29) {
        g_sndSystem->playSE((SndID)exchangeSelchkind2SelCharNarrationSndID__6muMenuFiii(
                                area->exchangeCharKindDetail(area->m_charKind)), -1, 0, 0, -1);
    }
    if (area->m_playerKind == 1 && charKind != 0x29) {
        int controllerNo = area->m_controllerNo;
        if (controllerNo >= 0) {
            gfPadType::PadType kind = area->getControllerKind();
            if (kind != 0 && kind != 4) {
                unsigned int stock = exchangeMuSelchkind2MuStockchkind__6muMenuFiii(area->exchangeCharKindDetail(area->m_charKind));
                if (stock != -1) {
                    g_sndSystem->playSERem(controllerNo - 4, (SndID)stock, -1, 0);
                }
            }
        }
    }
    arrangeCoinPrio__13muSelCharTaskFv(this, areaIdx);
    area->sendSystemCharKind();
}
