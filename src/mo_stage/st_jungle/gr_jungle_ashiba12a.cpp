#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba12A::grJungleAshiba12A(const char* taskName) : grJungleAshiba(taskName), m_snd() {
    m_move.m_x = 0.0f;
    m_move.m_y = 0.0f;
    m_move.m_z = 0.0f;
}

grJungleAshiba12A* grJungleAshiba12A::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba12A* ground = new (Heaps::StageInstance) grJungleAshiba12A(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba12A::~grJungleAshiba12A() {
}

void grJungleAshiba12A::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    updateMove(deltaFrame);
}

// The platform rises out of the side of the stage, stays, and sinks again; the stage asks for it with requestMove.
void grJungleAshiba12A::updateMove(float deltaFrame) {
    stJungleData* data = static_cast<stJungleData*>(getStageData());
    if (data != NULL) {
        float timer = m_timer;
        m_timer = timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setVisibility(0);
            setEnableCollisionStatus(false);
            if (m_collision != NULL) {
                m_collision->setDisable();
            }
            m_state = 1;
            break;
        case 2:
            if (m_timer == 0.0f) {
                setVisibility(1);
                setEnableCollisionStatus(true);
                if (m_collision != NULL) {
                    m_collision->setEnable();
                }
                m_state = 6;
            }
            break;
        case 6: {
            float move = m_move.m_y + data->unk2C * deltaFrame;
            m_move.m_y = move;
            float top = m_posWork[0].m_y;
            float bottom = m_posWork[1].m_y;
            if (bottom < top + move) {
                m_timer = 15.0f;
                m_move.m_y = bottom - top;
                m_state = 7;
            }
            break;
        }
        case 7:
            if (m_timer <= 4.0f) {
                g_ecMgr->setDrawPrio(1);
                u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x390003));
                g_ecMgr->setDrawPrio(-1);
                g_ecMgr->setParent(effect, m_sceneModels[0], "StgJungle00LV12", 0);
                m_state = 8;
            }
            break;
        case 8:
            if (m_timer == 0.0f) {
                m_state = 0;
            }
            break;
        }
    }
}

void grJungleAshiba12A::updateCallBack(float deltaFrame) {
    grJungleAshiba::updateCallBack(deltaFrame);
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
        data->m_flags |= 0x10;
        data->m_offsetPos.m_x = m_move.m_x;
        data->m_offsetPos.m_y = m_move.m_y;
        data->m_offsetPos.m_z = m_move.m_z;
        Vec3f pos;
        getNodePosition(&pos, 0, data->m_nodeIndex);
        m_snd.setPos(&pos);
    }
}

void grJungleAshiba12A::requestMove() {
    if (m_state == 1) {
        g_ecMgr->setDrawPrio(1);
        u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x390003));
        g_ecMgr->setDrawPrio(-1);
        g_ecMgr->setParent(effect, m_sceneModels[0], "StgJungle00LV12", 0);
        m_snd.playSE(static_cast<SndID>(0x1B90), 0, 0, -1);
        m_move.m_x = 0.0f;
        m_move.m_y = 0.0f;
        m_move.m_z = 0.0f;
        m_timer = 4.0f;
        m_state = 2;
    }
}
