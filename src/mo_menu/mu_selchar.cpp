#include <memory.h>
#include <mt/mt_vector.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <mu/mu_rule.h>
#include <mu/selchar/mu_selchar.h>
#include <mu/selchar/mu_selchar_hand.h>
#include <mu/selchar/mu_selchar_player_area.h>
#include <types.h>

muSelCharCBCalcWorld::muSelCharCBCalcWorld() {
    m_calcWorldDone = false;
    m_next = nullptr;
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

u8 muSelCharTask::isTeamBattle() {
    return m_teamBattleType;
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
    m_readyButton->changeAnimN(3, 0x11, true, false);
}

void muSelCharTask::clickHomerunRule() {
    m_homerunRuleObj->changeAnimN(3, 1, true, false);
}

void muSelCharTask::clickKumiteRule() {
    m_kumiteRuleObj->changeAnimN(3, 1, true, false);
}

void muSelCharTask::eraseHighScore(u32 msgIndex) {
    m_highScoreMsg->printf(msgIndex, "");
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
