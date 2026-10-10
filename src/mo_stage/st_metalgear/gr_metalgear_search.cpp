#include <ec/ec_mgr.h>
#include <ft/ft_manager.h>
#include <gf/gf_model.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <types.h>

#include <st_metalgear/gr_metalgear.h>

// ftManager::enumIncludeEntryId (sora_melee): lists the fighters in an area. BrawlHeaders declares another form of the symbol.
extern "C" int enumIncludeEntryId__9ftManagerCFi(ftManager* manager, Rect2D* area, int* entryIds, int unk, int count);

// sndSystem::stopSE (main, unnamed): stops a sound over the given frames
extern "C" void fn_80075F1C(sndSystem* sndSystem, int handle, int frames);

// stMelee: the position of a player, his hip (D0) or his cursor (D8) (sora_melee, unnamed)
extern "C" bool fn_27_2398D0(int playerNo, Vec3f* pos);
extern "C" bool fn_27_2398D8(int playerNo, Vec3f* pos);

// The search light finds a fighter, follows him and loses him.
grMetalgearSearch* grMetalgearSearch::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMetalgearSearch* ground = new (Heaps::StageInstance) grMetalgearSearch(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMetalgearSearch::grMetalgearSearch(const char* taskName) : grMetalgear(taskName), m_snd() {
    m_posGimmickWork = NULL;
    m_stateWork = NULL;
    m_stateExclamationWork = NULL;
    m_move.m_x = 0.0f;
    m_move.m_y = 0.0f;
    m_move.m_z = 0.0f;
    m_first = 1;
    m_tgtWork = NULL;
    m_seHandle = -1;
}

grMetalgearSearch::~grMetalgearSearch() {
}

void grMetalgearSearch::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateArea(deltaFrame);
        updateActive(deltaFrame);
    }
}

// While the light moves on its own (state 9), a fighter under it (a small area around the light) is picked as the target.
void grMetalgearSearch::updateArea(float deltaFrame) {
    if (*m_stateWork == 6 && m_state == 9) {
        int entryIds[9];
        memset(entryIds, 0, sizeof(entryIds));
        Rect2D area;
        area.m_up = m_posGimmickWork[2].m_y - 2.5f;
        area.m_right = m_posGimmickWork[2].m_x;
        area.m_left = area.m_right - 17.5f;
        area.m_right = area.m_right + 17.5f;
        area.m_down = area.m_up + 17.5f;
        area.m_up = area.m_up - 17.5f;
        int count = enumIncludeEntryId__9ftManagerCFi(g_ftManager, &area, entryIds, 0, 1);
        if (0 < count) {
            int pick = (int)((float)count * randf());
            if (pick < 0) {
                pick = 0;
            }
            int last = count - 1;
            if (pick < last) {
                last = pick;
            }
            int playerNo = g_ftManager->getPlayerNo(entryIds[last]);
            Vec3f pos;
            if (getPlayerPosition(playerNo, &pos, 0) != 0) {
                *m_tgtWork = playerNo;
                *m_stateWork = 4;
            }
        }
    }
}

void grMetalgearSearch::updateActive(float deltaFrame) {
    stMetalgearData* data = static_cast<stMetalgearData*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_timer = m_timer - deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0: {
        if (m_first == 1) {
            float r = randf();
            m_first = 0;
            m_timer = data->unk74 + (data->unk78 - data->unk74) * r;
        } else {
            m_timer = data->unk7C + (data->unk80 - data->unk7C) * randf();
        }
        float sine;
        float cosine;
        nw4r::math::SinCosFIdx(&sine, &cosine, 0.00390625f * (float)(s16)(int)(randf() * 65535.0f));
        m_move.m_x = cosine * data->unk8C;
        float speed = data->unk8C;
        m_move.m_z = 0.0f;
        m_move.m_y = sine * speed;
        *m_stateWork = 6;
        m_state = 1;
        break;
    }
    case 1:
        if (m_timer == 0.0f) {
            float r = randf();
            m_state = 8;
            m_timer = data->unk98 + (data->unk9C - data->unk98) * r;
        }
        break;
    case 8:
        if (m_timer == 0.0f) {
            float r = randf();
            m_state = 9;
            m_timer = data->unk98 + (data->unk9C - data->unk98) * r;
        } else {
            u32 rootNode = 0;
            getNodePosition(&m_posGimmickWork[3], 0, rootNode);
            Vec3f* pos = m_posGimmickWork;
            float moveY = m_move.m_y;
            float moveZ = m_move.m_z;
            pos[3].m_x = pos[3].m_x + m_move.m_x;
            pos[3].m_y = pos[3].m_y + moveY;
            pos[3].m_z = pos[3].m_z + moveZ;
        }
        break;
    case 9:
        if (m_timer == 0.0f) {
            float r = randf();
            m_state = 8;
            m_timer = data->unk98 + (data->unk9C - data->unk98) * r;
        } else if (*m_stateWork == 4) {
            m_timer = 30.0f;
            m_snd.playSE(snd_se_stage_Metalgear_03, 0, 0, -1);
            m_snd.setPos(&m_posGimmickWork[2]);
            m_state = 10;
        } else {
            u32 rootNode = 0;
            getNodePosition(&m_posGimmickWork[3], 0, rootNode);
            Vec3f* pos = m_posGimmickWork;
            float moveY = m_move.m_y;
            float moveZ = m_move.m_z;
            pos[3].m_x = pos[3].m_x + m_move.m_x;
            pos[3].m_y = pos[3].m_y + moveY;
            pos[3].m_z = pos[3].m_z + moveZ;
        }
        break;
    case 10: {
        Vec3f pos;
        if (*m_stateExclamationWork != 4 && getPlayerPosition(*m_tgtWork, &pos, 1) == 1) {
            *m_stateExclamationWork = 4;
            pos.m_y = pos.m_y + 5.0f;
            Vec3f* work = m_posGimmickWork;
            work[10].m_x = pos.m_x;
            work[10].m_y = pos.m_y;
            work[10].m_z = pos.m_z;
        }
        if (m_timer == 0.0f) {
            m_seHandle = g_sndSystem->playSE(snd_se_stage_Metalgear_04, 0, 30, 0, -1);
            float r = randf();
            m_state = 11;
            m_timer = data->unk90 + (data->unk94 - data->unk90) * r;
        }
        break;
    }
    case 11:
        if (m_timer == 0.0f) {
            m_timer = 90.0f;
            *m_stateExclamationWork = 6;
            if (m_seHandle != -1) {
                fn_80075F1C(g_sndSystem, m_seHandle, 15);
            }
            m_state = 12;
        } else if (getPlayerPosition(*m_tgtWork, &m_posGimmickWork[3], 0) != 1) {
            *m_tgtWork = -1;
            *m_stateWork = 5;
            *m_stateExclamationWork = 6;
            if (m_seHandle != -1) {
                fn_80075F1C(g_sndSystem, m_seHandle, 30);
            }
            m_state = 12;
        }
        break;
    case 12:
        if (m_timer == 0.0f) {
            m_state = 13;
            m_timer = 300.0f;
        }
        break;
    case 13:
        if (m_timer == 0.0f) {
            m_state = 0;
        } else {
            Vec3f* pos = m_posGimmickWork;
            pos[3].m_x = 0.0f;
            pos[3].m_y = 0.0f;
            pos[3].m_z = 200.0f;
        }
        break;
    }
}

// The place of a player: kind 1 is his cursor, any other kind his hip (both are small functions of the melee stage in sora_melee).
bool grMetalgearSearch::getPlayerPosition(int playerNo, Vec3f* pos, int kind) {
    if (kind == 1) {
        return fn_27_2398D8(playerNo, pos);
    }
    return fn_27_2398D0(playerNo, pos);
}
