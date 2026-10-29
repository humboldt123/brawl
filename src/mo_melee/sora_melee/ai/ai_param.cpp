#include <ai/ai_act.h>
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

// HYPOTHESIS: aiParam::chk_situation (condition id, own and target character), still unnamed.
extern "C" int fn_27_21E24C(aiParam*, u8, aiChrIdx*, aiChrIdx*);

int aiParam::select_situation(int start, int end, aiChrIdx* chr, aiChrIdx* target) {
    int i;
    int skip = 0;
    for (i = start; i < end; i++) {
        u8* entry = (u8*)m_par + m_par->m_situationList[i];
        if (skip > 0) {
            if (entry[2] & 8) {
                skip--;
            }
            if (skip > 0) {
                if (entry[3] == 0) {
                    skip++;
                }
                continue;
            }
        }
        int result;
        u8 flags;
        for (u32 j = 0; j < 2; j++) {
            result = fn_27_21E24C(this, entry[j], chr, target);
            if (j == 0) {
                flags = entry[2];
                if (flags & 1) {
                    result = !result;
                }
            } else {
                flags = entry[2];
                if (flags & 2) {
                    result = !result;
                }
            }
            if (result) {
                if (flags & 4) {
                    break;
                }
            } else if (!(flags & 4)) {
                break;
            }
        }
        if (entry[3] == 0) {
            if (!result) {
                skip = 1;
            }
        } else if (result) {
            break;
        }
    }
    return i;
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

int ai_act_id2cmd(aiAct::actId id) {
    if (id > 0x6030) {
        if (id < 0x6040) {
            return id - 0x6031;
        }
        int cmd = id - 0x6034;
        if (cmd < 0x16) {
            return cmd;
        }
    }
    return 0;
}

tag_AI_ATK_DATA* tag_AI_ATK_DATA_HED::searchAtkData(int id) {
    for (u32 i = 0; i < m_num; i++) {
        if (id == m_data[i].m_id) {
            return &m_data[i];
        }
    }
    return NULL;
}

int get_motnum_chr(ftKind kind) {
    switch (kind) {
    case Fighter_Mario:
        return 0x1DE;
    case Fighter_DonkeyKong:
        return 0x204;
    case Fighter_Link:
        return 0x1E4;
    case Fighter_Samus:
        return 0x1EC;
    case Fighter_Yoshi:
        return 0x1E8;
    case Fighter_Kirby:
        return 0x31D;
    case Fighter_Fox:
        return 0x1F2;
    case Fighter_Pikachu:
        return 0x1E6;
    case Fighter_Luigi:
        return 0x1E2;
    case Fighter_CaptainFalcon:
        return 0x1EB;
    case Fighter_Ness:
        return 0x1EA;
    case Fighter_Bowser:
        return 0x1E3;
    case Fighter_Peach:
        return 0x1E1;
    case Fighter_Zelda:
        return 0x1E0;
    case Fighter_Sheik:
        return 0x1E6;
    case Fighter_Popo:
    case Fighter_Nana:
        return 0x1F2;
    case Fighter_Marth:
        return 0x1F5;
    case Fighter_MrGameAndWatch:
        return 0x202;
    case Fighter_Falco:
        return 0x1EC;
    case Fighter_Ganondorf:
        return 0x1E8;
    case Fighter_Wario:
        return 0x213;
    case Fighter_MetaKnight:
        return 0x1EC;
    case Fighter_Pit:
        return 0x1F4;
    case Fighter_ZeroSuitSamus:
        return 0x1EB;
    case Fighter_Olimar:
        return 0x1E0;
    case Fighter_Lucas:
        return 0x1EB;
    case Fighter_DiddyKong:
        return 0x1FB;
    case Fighter_Charizard:
        return 0x1DD;
    case Fighter_Squirtle:
        return 0x1E4;
    case Fighter_Ivysaur:
        return 0x1DD;
    case Fighter_KingDedede:
        return 0x217;
    case Fighter_Lucario:
        return 0x1EB;
    case Fighter_Ike:
        return 0x1F4;
    case Fighter_ROB:
        return 0x1E7;
    case Fighter_Jigglypuff:
        return 0x1F5;
    case Fighter_ToonLink:
        return 0x1E4;
    case Fighter_Wolf:
        return 0x1EE;
    case Fighter_Snake:
        return 0x1FE;
    case Fighter_Sonic:
        return 0x1F1;
    case Fighter_GigaBowser:
        return 0x1E4;
    case Fighter_WarioMan:
        return 0x213;
    case Fighter_Alloy_Red:
        return 0x1CF;
    case Fighter_Alloy_Blue:
        return 0x1CF;
    case Fighter_Alloy_Yellow:
        return 0x1CF;
    case Fighter_Alloy_Green:
        return 0x1CF;
    default:
        return -1;
    }
}
