#include <gm/gm_global.h>
#include <gm/gm_set_rule.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <mu/mu_rule.h>
#include <mu/selchar/mu_selchar.h>
#include <mu/selchar/mu_selchar_coin.h>
#include <mu/selchar/mu_selchar_hand.h>
#include <mu/selchar/mu_selchar_player_area.h>
#include <types.h>

// The menu helpers take three integers; this unit passes only the first one (see TIPS.md).
extern "C" int exchangeMuSelchkindToMuCharKind__6muMenuFiii(int selchKind);
extern "C" int isCharUsable__6muMenuFiii(int charKind);
extern "C" int isCharRefered__6muMenuFiii(int charKind);

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

void muSelCharTask::eraseHighScore(u32 msgIndex) {
    m_highScoreMsg->printf(msgIndex, "");
}

// Takes the picture of one of the two characters of a pair off the high score.
void muSelCharTask::eraseHighScorePairChar(int pairIdx) {
    static const int sPairCharObj[] = { 178, 179 };
    m_highScoreExpand->Remove(m_objs[sPairCharObj[pairIdx]]->m_sceneModel);
}

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
u8 muSelCharTask::getSelCharTypeHighScoreKind(int selCharType) {
    static const u8 sKumiteHighScoreKind[] = { 6, 7, 8, 9, 10, 11 };
    if (selCharType == 9 || selCharType == 13) {
        return sKumiteHighScoreKind[m_kumiteType];
    }
    return getSelCharTypeData(selCharType)->m_highScoreKind;
}
