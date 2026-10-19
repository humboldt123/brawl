#include <ai/ai_ip_buf.h>
#include <ai/ai_param.h>

// aiIpBuf: a 16-frame history of the stick and buttons a CPU character pressed.

aiIpBuf::aiIpBuf() {
    for (int i = 0; i < 16; i++) {
        m_stick[i] = Vec2f(0.0f, 0.0f);
        m_button[i] = 0;
    }
    m_index = 0;
}

void aiIpBuf::update(Vec2f* stick, u32* button) {
    m_index++;
    if (m_index >= 16) {
        m_index = 0;
    }
    m_stick[m_index] = *stick;
    m_button[m_index] = *button;
}

void aiIpBuf::getData(int framesBack, Vec2f* stick, u32* button) {
    if (framesBack < 0) {
        framesBack = 0;
    } else if (framesBack >= 16) {
        framesBack = 15;
    }
    framesBack = m_index - framesBack;
    if (framesBack < 0) {
        framesBack += 16;
    }
    if (stick != NULL) {
        *stick = m_stick[framesBack];
    }
    if (button != NULL) {
        *button = m_button[framesBack];
    }
}

Vec2f aiIpBuf::getStick(int framesBack) {
    Vec2f stick;
    getData(framesBack, &stick, NULL);
    return stick;
}

u32 aiIpBuf::getButton(int framesBack) {
    u32 button;
    getData(framesBack, NULL, &button);
    return button;
}

// aiParam: reads the per-character AI tuning (sizes, jump reach, attack flags).

static inline int aiClampRank(int rank) {
    rank = (s8)rank > 0 ? rank : 0;
    int value = 100;
    if ((s8)rank < 100) {
        value = rank;
    }
    return value;
}

static inline float aiRankToRate(u8 rank) {
    float rate = (s8)rank - 20;
    rate *= 0.5f;
    if (rate < 0.0f) {
        rate = 0.0f;
    } else if (rate > 30.0f) {
        rate = 30.0f;
    }
    return rate;
}

aiParam::aiParam() {
    m_par = NULL;
    m_atkData = NULL;
    m_rank = aiClampRank(100);
    m_rankRate = aiRankToRate(m_rank);
    m_disableCmds = 0;
}

void aiParam::setRank(int rank, u32 force, int update) {
    if ((s8)m_rank != (s8)rank || force == 1) {
        m_rank = aiClampRank(rank);
        m_rankRate = aiRankToRate(m_rank);
        if (m_par != NULL && update != 0) {
            par_work_update();
        }
    }
}

bool aiParam::check_revision(char* data) {
    if (data != NULL && data[0] == 'A' && data[1] == 'I' && data[2] == 'P' && data[3] == 'D'
        && *(u32*)(data + 4) == 12) {
        return true;
    }
    return false;
}

float aiParam::get_down_atk_r(aiChrIdx* chr) {
    if (!aiChrIdx::IsPlayer(chr)) {
        return aiMgr::getAiReferPar(0)->m_downAtkRate;
    }
    return aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind)->m_downAtkRate;
}

u8 aiParam::get_down_atk_f(aiChrIdx* chr) {
    if (!aiChrIdx::IsPlayer(chr)) {
        return aiMgr::getAiReferPar(0)->m_downAtkFlag;
    }
    return aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind)->m_downAtkFlag;
}

float aiParam::get_cliff_atk_r(aiChrIdx* chr) {
    if (!aiChrIdx::IsPlayer(chr)) {
        return aiMgr::getAiReferPar(0)->m_cliffAtkRate;
    }
    return aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind)->m_cliffAtkRate;
}

u8 aiParam::get_cliff_atk_f(aiChrIdx* chr) {
    if (!aiChrIdx::IsPlayer(chr)) {
        return aiMgr::getAiReferPar(0)->m_cliffAtkFlag;
    }
    return aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind)->m_cliffAtkFlag;
}

float aiParam::get_width(aiChrIdx* chr) {
    if (aiChrIdx::IsPlayer(chr)) {
        float scale = aiMgr::get_aiStat(chr)->m_scale.m_x;
        aiReferPar* par = aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind);
        return par->m_width * scale;
    }
    float scale = aiMgr::get_aiStat(chr)->m_scale.m_x;
    aiReferPar* par = aiMgr::getAiReferPar(0);
    return par->m_width * scale;
}

float aiParam::get_height(aiChrIdx* chr) {
    if (aiChrIdx::IsPlayer(chr)) {
        float scale = aiMgr::get_aiStat(chr)->m_scale.m_y;
        aiReferPar* par = aiMgr::getAiReferPar(aiMgr::get_aiStat(chr)->m_chrKind);
        return par->m_height * scale;
    }
    float scale = aiMgr::get_aiStat(chr)->m_scale.m_y;
    aiReferPar* par = aiMgr::getAiReferPar(0);
    return par->m_height * scale;
}

float aiParam::get_air_high(aiChrIdx* chr) {
    aiStat* stat = aiMgr::get_aiStat(chr);
    float high;
    if (!aiChrIdx::IsPlayer(chr)) {
        high = (stat->m_scale.m_y * 0.8f + 0.2f) * (aiMgr::getAiReferPar(0)->m_airHigh - 3.0f);
    } else {
        high = (stat->m_scale.m_y * 0.8f + 0.2f) * (aiMgr::getAiReferPar(stat->m_chrKind)->m_airHigh - 3.0f);
    }
    return high;
}

float aiParam::get_air_length(aiChrIdx* chr) {
    aiStat* stat = aiMgr::get_aiStat(chr);
    float length;
    if (!aiChrIdx::IsPlayer(chr)) {
        length = (stat->m_scale.m_x * 0.8f + 0.2f) * (aiMgr::getAiReferPar(0)->m_airLength - 3.0f);
    } else {
        length = (stat->m_scale.m_x * 0.8f + 0.2f) * (aiMgr::getAiReferPar(stat->m_chrKind)->m_airLength - 3.0f);
    }
    return length;
}

float aiParam::get_jump_high(aiJumpPar* par, aiStat* stat) {
    return par->m_jumpHigh * (stat->m_scale.m_y * 0.8f + 0.2f);
}

float aiParam::get_jump_high_sp(aiJumpPar* par, aiStat* stat) {
    return par->m_jumpHighSp * (stat->m_scale.m_y * 0.8f + 0.2f);
}

float aiParam::get_jump_length(aiJumpPar* par, aiStat* stat) {
    return par->m_jumpLength * (stat->m_scale.m_x * 0.8f + 0.2f);
}

void aiParam::set_disable_cmd(int cmd) {
    if ((u32)cmd >= 0x16) {
        return;
    }
    m_disableCmds |= 1 << cmd;
}

bool aiParam::chkCmdRevise(int cmd) {
    return m_par->m_reviseCmds & (1 << cmd);
}

bool aiParam::chkCmdFall(int cmd) {
    return m_par->m_fallCmds & (1 << cmd);
}

bool aiParam::chkCmdReflect(int cmd) {
    return m_par->m_reflectCmds & (1 << cmd);
}

tag_AI_ATK_DATA* tag_AI_ATK_DATA_HED::searchAtkData(u32 id) {
    int remaining = m_num;
    tag_AI_ATK_DATA* data = m_data;
    while (remaining != 0) {
        if (id == data->m_id) {
            return data;
        }
        data++;
        remaining--;
    }
    return NULL;
}
