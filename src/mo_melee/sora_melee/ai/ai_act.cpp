#include <ai/ai_act.h>
#include <ai/ai_cl_block.h>
#include <ai/ai_input.h>

extern "C" {
// HYPOTHESIS: a zero vector in the DOL's read-only data; its name is not known.
extern const Vec3f lbl_8041F3E8;
}

aiAct::aiAct(aiInput* input, aiStat* stat) {
    for (int i = 0; i < 24; i++) {
        m_unk0[i] = 0.0f;
    }
    for (int i = 0; i < 4; i++) {
        m_unkD4[i] = 0;
    }
    m_input = input;
    m_stat = stat;
    m_id = m_prevId = m_unk7A = m_unk7E = 0;
    m_unk80 = 0;
    m_script = m_unk64 = NULL;
    m_unk68 = m_unk6C = NULL;
    m_unk81 = 0;
    m_unk82 = 0;
    m_unk84 = 0.0f;
    m_unk88 = 0.0f;
    m_unk8C = 0.0f;
    m_unk90 = 0;
    m_unk91 = 0;
    m_unk92 = 0;
    m_unk93 = 0;
    m_unk94 = 0;
    m_unk98 = -1.0f;
    m_reflectDone = 0;
    m_reflectRate = 1.0f;
    m_defendAirRate = 0.0f;
    m_unkA8 = 0;
    m_unkAC = 0;
    m_unkB0 = m_unkBC = m_unkC8 = lbl_8041F3E8;
    m_unkDC = 1.0f;
    m_goalPos.m_x = m_goalPos.m_y = m_goalEdgeL.m_x = m_goalEdgeL.m_y = m_goalEdgeR.m_x = m_goalEdgeR.m_y = 0.0f;
    m_goalUnk48 = m_viaUnk48 = m_unk10C = 0;
    m_goalJointId = m_viaJointId = m_unk108 = 0;
    m_unk110 = 0.0f;
}

void aiAct::script_reload(void* script) {
    s8 target[16];
    target[0] = -1;
    act_change(0, target, 0, 0);
    m_script = script;
}

aiAct::~aiAct() { }

void aiAct::set_id(u16 id) {
    if (m_id != 0) {
        m_prevId = m_id;
    }
    m_id = id;
    m_input->swingChkReset(0);
}

void aiAct::set_goal_inf(AiClBlock* goal, AiClBlock* via, float x, float y) {
    m_goalPos.m_x = x;
    m_goalPos.m_y = y;
    if (goal != NULL) {
        m_goalEdgeL = goal->m_edgeL;
        m_goalEdgeR = goal->m_edgeR;
        if (goal->m_joint == NULL) {
            m_goalJointId = 0;
            m_goalUnk48 = 0;
        } else {
            m_goalUnk48 = goal->m_unk48;
            m_goalJointId = goal->m_joint->m_id;
        }
    } else {
        m_goalJointId = 0;
    }
    if (via != NULL) {
        if (via->m_joint == NULL) {
            m_viaJointId = 0;
            m_viaUnk48 = 0;
        } else {
            m_viaUnk48 = via->m_unk48;
            m_viaJointId = via->m_joint->m_id;
        }
    } else {
        m_viaJointId = 0;
    }
}

void aiAct::setReflectDone() {
    m_reflectDone = 180;
    m_reflectRate *= 0.85f;
}

void aiAct::addDefendAirRate(float amount) {
    m_defendAirRate = m_defendAirRate + amount;
}

tag_AI_SCRIPT* tag_AI_SCRIPT_PACK::get_script(tag_AI_SCRIPT_PACK* common, u32 id) {
    int i;
    tag_AI_SCRIPT* script;
    if ((id & 0xFFFF800F) != 0) {
        for (i = 0; i < 2; i++) {
            tag_AI_SCRIPT_PACK* pack = i == 0 ? this : common;
            if (pack != NULL) {
                for (u32 j = 0; j < pack->m_numScripts; j++) {
                    script = (tag_AI_SCRIPT*)((u8*)pack + pack->m_offsets[j]);
                    if (script != NULL && script->m_id == id) {
                        return script;
                    }
                }
            }
        }
    }
    u32 key = id & 0x7FF0;
    for (i = 0; i < 2; i++) {
        tag_AI_SCRIPT_PACK* pack = i == 0 ? this : common;
        if (pack != NULL) {
            for (u32 j = 0; j < pack->m_numScripts; j++) {
                script = (tag_AI_SCRIPT*)((u8*)pack + pack->m_offsets[j]);
                if (script != NULL && (script->m_id & ~0x8000) == key) {
                    return script;
                }
            }
        }
    }
    return NULL;
}

// Skips to the end of the conditional block that starts at this command: to its else (when this command is not an
// else itself) or to the end of the block, counting the nested blocks on the way.
tag_AI_SCRIPT_CMD* tag_AI_SCRIPT_CMD::if_skip() {
    tag_AI_SCRIPT_CMD* cmd = this;
    u8 first = cmd->m_type;
    u8 type;
    int depth = first == 8;
    u8 prev = 0;
    while (true) {
        type = cmd->m_type;
        if ((type == 6 || type == 7) && prev != 8) {
            depth++;
        }
        if (first != 8 && type == 8 && depth == 1) {
            return cmd;
        }
        if (type == 9 && --depth <= 0) {
            return cmd;
        }
        prev = type;
        cmd = (tag_AI_SCRIPT_CMD*)((u8*)cmd + cmd->m_size);
    }
}
