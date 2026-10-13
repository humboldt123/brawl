#include <st_dxbigblue/gr_dxbigblue_tyukei.h>
#include <st_dxbigblue/gr_dxbigblue_util.h>
#include <st_dxbigblue/st_dxbigblue_data.h>

grDxBigBlueTyukei* grDxBigBlueTyukei::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grDxBigBlueTyukei* ground = new (Heaps::StageInstance) grDxBigBlueTyukei(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grDxBigBlueTyukei::~grDxBigBlueTyukei() { }

// Picks a side, a speed and the first bob when it comes in; then flies across.
void grDxBigBlueTyukei::updateActive(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
            case 0:
                setVisibility(0);
                setEnableCollisionStatus(false);
                m_state = 1;
                break;
            case 1:
                if (m_flyDataWork->m_state != 10) {
                    if (randf() < 0.5f) {
                        m_flyDataWork->m_pos.m_x = m_posLimitWork[0] - data->unk68;
                        m_dir = 1;
} else {
                        m_flyDataWork->m_pos.m_x = m_posLimitWork[3] + data->unk68;
                        m_dir = 0;
                                        }
                    m_speed = data->unkFC + (data->unk100 - data->unkFC) * randf();
                    m_flyDataWork->unk28 = data->unk104 + (data->unk108 - data->unk104) * randf();
                    m_speedTimer = data->unk10C + (data->unk110 - data->unk10C) * randf();
                    m_bobTimer = data->unk114 + (data->unk118 - data->unk114) * randf();
                    m_state = 2;
                }
                break;
            case 2:
                if (m_flyDataWork->m_state == 10) {
                    m_state = 0;
                } else {
                    if (!m_isVisible) {
                        setVisibility(1);
                        setEnableCollisionStatus(true);
                    }
                    updateMove(deltaFrame);
                }
                break;
        }
    }
}

void grDxBigBlueTyukei::updateMove(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        m_speedTimer -= deltaFrame;
        if (m_speedTimer < 0.0f) {
            m_speedTimer = 0.0f;
        }
        m_bobTimer -= deltaFrame;
        if (m_bobTimer < 0.0f) {
            m_bobTimer = 0.0f;
        }
        if (m_speedTimer == 0.0f) {
            m_speed = data->unkFC + (data->unk100 - data->unkFC) * randf();
            m_speedTimer = data->unk10C + (data->unk110 - data->unk10C) * randf();
        }
        if (m_bobTimer == 0.0f) {
            m_flyDataWork->unk28 = data->unk104 + (data->unk108 - data->unk104) * randf();
            m_flyDataWork->unk28 = -m_flyDataWork->unk28;
            m_bobTimer = data->unk114 + (data->unk118 - data->unk114) * randf();
        }
        if (m_dir == 1) {
            m_flyDataWork->m_pos.m_x += m_speed * deltaFrame;
            if (m_flyDataWork->m_pos.m_x > m_posLimitWork[3] + data->unk68) {
                m_flyDataWork->m_state = 10;
            }
        } else {
            m_flyDataWork->m_pos.m_x -= m_speed * deltaFrame;
            if (m_flyDataWork->m_pos.m_x < m_posLimitWork[0] - data->unk68) {
                m_flyDataWork->m_state = 10;
            }
        }
    }
}
