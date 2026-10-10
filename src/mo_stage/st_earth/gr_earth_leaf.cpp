#include <ft/ft_external_value_accesser.h>
#include <ft/ft_manager.h>
#include <gr/gr_calc_world_callback.h>
#include <it/item.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <types.h>

#include <st_earth/gr_earth.h>

// HYPOTHESIS: two small constant objects (an 0xFF marker with an index) that the unit's headers instantiate.
struct grEarthLeafMarker {
    int m_a;
    int m_b;
    grEarthLeafMarker(int a, int b) : m_a(a), m_b(b) { }
};
static grEarthLeafMarker sMarkerA(0xFF, 0);
static grEarthLeafMarker sMarkerB(0xFF, 1);

grEarthLeaf::grEarthLeaf(const char* taskName) : grEarth(taskName), m_snd() {
    m_stateDown = 0;
    m_type = 3;
    m_stateReturn = 0;
    m_timerDown = 0.0f;
    m_downLimit = 0.0f;
    m_downAmount = 0.0f;
    m_posY = 0.0f;
    m_speedBack = 0.0f;
    m_decay = 0.0f;
    m_speedDown = 0.0f;
    m_speedY = 0.0f;
    m_timerReturn = 0.0f;
    m_weight = 0.0f;
    m_posOffsetWork = NULL;
    m_posBase.m_x = 0.0f;
    m_posBase.m_y = 0.0f;
    m_posBase.m_z = 0.0f;
    m_pelletData = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
}

grEarthLeaf* grEarthLeaf::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthLeaf* ground = new (Heaps::StageInstance) grEarthLeaf(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthLeaf::~grEarthLeaf() {
}

void grEarthLeaf::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateDown(deltaFrame);
        updateReturn(deltaFrame);
        updatePellet();
        updateCallBack(deltaFrame);
    }
}

// The leaf goes down when a fighter lands on it (the steps: 1 starts, 2 goes down fast, 3 slows down, 4 waits, 5 goes down
// further the heavier the fighter is).
void grEarthLeaf::updateDown(float deltaFrame) {
    if (m_posOffsetWork != NULL) {
        stEarthData* data = static_cast<stEarthData*>(getStageData());
        if (data != NULL) {
            m_timerDown = m_timerDown - deltaFrame;
            if (m_timerDown < 0.0f) {
                m_timerDown = 0.0f;
            }
            switch (m_stateDown) {
            case 3: {
                float rate = 1.0f - m_timerDown / 15.0f;
                if (rate < 0.0f) {
                    rate = 0.0f;
                }
                if (1.0f < rate) {
                    rate = 1.0f;
                }
                if (rate == 1.0f) {
                    m_stateDown = 4;
                    m_timerDown = 4.0f;
                }
                float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 8192.0f + 8192.0f))) * (1.0f / 256.0f));
                m_posOffsetWork->m_y = m_posBase.m_y + (m_downAmount * 0.65f) * sine;
                if (m_posOffsetWork->m_y < m_downLimit) {
                    m_posOffsetWork->m_y = m_downLimit;
                }
                break;
            }
            case 1: {
                Vec3f* offset = m_posOffsetWork;
                float y = offset->m_y;
                m_speedY = 0.0f;
                m_downAmount = y + (m_downLimit - y) * 0.5f;
                m_speedBack = data->unk7C;
                m_decay = data->unk84;
                m_stateReturn = 0;
                m_posBase.m_x = offset->m_x;
                m_posBase.m_y = offset->m_y;
                m_posBase.m_z = offset->m_z;
                Vec3f pos;
                getNodePosition(&pos, m_unk1, m_nodeIndex);
                m_snd.setPos(&pos);
                m_snd.playSE(static_cast<SndID>(0x1CD4), 0, 0, -1);
                m_stateDown = 2;
                m_timerDown = 8.0f;
                break;
            }
            case 2: {
                float rate = 1.0f - m_timerDown * 0.125f;
                if (rate < 0.0f) {
                    rate = 0.0f;
                }
                if (1.0f < rate) {
                    rate = 1.0f;
                }
                if (0.5f <= rate) {
                    m_stateDown = 3;
                    m_timerDown = 15.0f;
                }
                float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 16384.0f))) * (1.0f / 256.0f));
                m_posOffsetWork->m_y = m_posBase.m_y + (m_downAmount * 0.65f) * sine;
                if (m_posOffsetWork->m_y < m_downLimit) {
                    m_posOffsetWork->m_y = m_downLimit;
                }
                break;
            }
            case 5:
                if (m_downAmount + m_downAmount * 0.125f < m_posOffsetWork->m_y) {
                    float weight = m_weight - 70.0f;
                    if (weight < 0.0f) {
                        weight = 0.0f;
                    }
                    float rate = weight / 50.0f;
                    if (rate < 0.0f) {
                        rate = 0.0f;
                    }
                    if (1.0f < rate) {
                        rate = 1.0f;
                    }
                    m_speedDown = m_speedDown + deltaFrame * (data->unk78 * (rate + 0.5f));
                } else {
                    m_speedDown = m_speedDown - data->unk78 * deltaFrame;
                    if (m_speedDown < 0.0f) {
                        m_speedDown = 0.0f;
                    }
                }
                m_posOffsetWork->m_y = m_posOffsetWork->m_y - m_speedDown;
                if (m_posOffsetWork->m_y < m_downLimit) {
                    m_posOffsetWork->m_y = m_downLimit;
                }
                m_posY = m_posOffsetWork->m_y;
                break;
            case 4:
                if (m_timerDown == 0.0f) {
                    m_stateDown = 5;
                }
                break;
            }
        }
    }
}

// The leaf comes back up and bounces a few times when nobody is on it any more.
void grEarthLeaf::updateReturn(float deltaFrame) {
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    if (data != NULL) {
        float prev = m_timerReturn;
        m_timerReturn = prev - deltaFrame;
        if (m_timerReturn < 0.0f) {
            m_timerReturn = 0.0f;
        }
        if (m_timerReturn == 0.0f) {
            if (prev != 0.0f) {
                m_speedBack = data->unk7C;
                m_decay = data->unk84;
                m_seReturn = 1;
                Vec3f pos;
                getNodePosition(&pos, m_unk1, m_nodeIndex);
                m_snd.setPos(&pos);
                m_snd.playSE(static_cast<SndID>(0x1CD5), 0, 0, -1);
            }
            switch (m_stateReturn) {
            case 1:
                m_speedY = m_speedY * (m_decay * deltaFrame) - m_speedBack * deltaFrame;
                if (m_posOffsetWork->m_y <= 0.0f) {
                    m_stateReturn = 2;
                    m_speedBack = m_speedBack * m_decay;
                }
                break;
            case 0:
                if (m_posOffsetWork->m_y < m_posY * data->unk80) {
                    m_speedY = m_speedY + data->unk7C * deltaFrame;
                } else {
                    m_stateReturn = 1;
                }
                break;
            case 2:
                m_speedY = m_speedY * (m_decay * deltaFrame) + m_speedBack * deltaFrame;
                if (0.0f <= m_posOffsetWork->m_y) {
                    m_stateReturn = 1;
                    m_speedBack = m_speedBack * m_decay;
                    if (m_seReturn == 1) {
                        Vec3f pos;
                        getNodePosition(&pos, m_unk1, m_nodeIndex);
                        m_snd.setPos(&pos);
                        m_snd.playSE(static_cast<SndID>(0x1CD6), 0, 0, -1);
                        m_seReturn = 0;
                    }
                }
                break;
            }
            m_posOffsetWork->m_y = m_posOffsetWork->m_y + m_speedY;
            m_speedDown = 0.0f;
            m_stateDown = 0;
        }
    }
}

// The pellets that lie on this leaf (the type of the pellet is the one of the leaf) follow it.
void grEarthLeaf::updatePellet() {
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    if (data != NULL) {
        u8 count = data->unk3C;
        for (u32 i = 0; i != count; i++) {
            u8 type = m_type;
            if (type == 0 && m_pelletData[i].m_leaf == 0) {
                getPosPellet(m_pelletData[i].unk24, &m_pelletData[i].m_pos);
            } else if (type == 1 && m_pelletData[i].m_leaf == 1) {
                getPosPellet(m_pelletData[i].unk24, &m_pelletData[i].m_pos);
            } else if (type == 2 && m_pelletData[i].m_leaf == 2) {
                getPosPellet(m_pelletData[i].unk24, &m_pelletData[i].m_pos);
            }
        }
    }
}

void grEarthLeaf::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f* offset = m_posOffsetWork;
            if (offset != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_offsetPos.m_x = offset->m_x;
                data->m_offsetPos.m_y = offset->m_y;
                data->m_offsetPos.m_z = offset->m_z;
            }
        }
    }
}

// The place of the leaf (of the node of it); the mode 1 gives it without the move of the leaf.
void grEarthLeaf::getPos(Vec3f* pos, int mode) {
    if (pos != NULL) {
        pos->m_x = 0.0f;
        pos->m_y = 0.0f;
        pos->m_z = 0.0f;
        bool valid;
        if (m_unk1 == 0xFFFFFFFF || m_nodeIndex == -1) {
            valid = false;
        } else {
            valid = true;
        }
        if (valid) {
            nw4r::g3d::ScnMdlSimple* scnMdl = reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_sceneModels[m_unk1]);
            if (scnMdl != NULL) {
                Matrix mtx;
                if (scnMdl->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, m_nodeIndex)) {
                    Vec3f zero;
                    zero.m_x = 0.0f;
                    zero.m_y = 0.0f;
                    zero.m_z = 0.0f;
                    Vec3f world;
                    mtx.mulPos(&zero, &world);
                    pos->m_x = world.m_x;
                    pos->m_y = world.m_y;
                    pos->m_z = world.m_z;
                    if (mode == 1) {
                        pos->m_x = world.m_x - m_posOffsetWork->m_x;
                        pos->m_y = world.m_y - m_posOffsetWork->m_y;
                        pos->m_z = world.m_z - m_posOffsetWork->m_z;
                    }
                }
            }
        }
    }
}

// A fighter (the weight of it makes the leaf go down further) or a pellet landed on the leaf.
void grEarthLeaf::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFighter = 1;
    static u32 sOwnerItem = 4;
    if (isCollisionStatusOwnerTask(collStatus, reinterpret_cast<CollCategoryFlag*>(&sOwnerFighter)) == 1) {
        Fighter* fighter = static_cast<Fighter*>(gfTask::getTask(collStatus->m_taskId));
        if (fighter == NULL) {
            return;
        }
        m_weight = ftExternalValueAccesser::getWeight(fighter);
    }
    BaseItem* item;
    if ((isCollisionStatusOwnerTask(collStatus, reinterpret_cast<CollCategoryFlag*>(&sOwnerItem)) != 1)
        || ((item = static_cast<BaseItem*>(gfTask::getTask(collStatus->m_taskId))) != NULL
            && item->getParam(static_cast<itValueAccesser::ParamInt>(0x5B69)) != 0)) {
        float prev = m_timerReturn;
        m_timerReturn = 5.0f;
        if (prev == 0.0f) {
            m_stateDown = 1;
        }
    }
}

// The place on the leaf where the pellet lies: a line from the first to the second node of the leaf (by the kind of it), and
// the pellet is on it by the rate (0 - 1).
void grEarthLeaf::getPosPellet(float rate, Vec3f* pos) {
    if (pos != NULL) {
        pos->m_x = 0.0f;
        pos->m_y = 0.0f;
        pos->m_z = 0.0f;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (1.0f < rate) {
            rate = 1.0f;
        }
        Vec3f start;
        Vec3f end;
        switch (m_type) {
        case 1:
            getNodePosition(&start, 0, "ashibaPelletCP1");
            getNodePosition(&end, 0, "ashibaPelletCP2");
            break;
        case 0:
            getNodePosition(&start, 0, "ashibaPelletLP1");
            getNodePosition(&end, 0, "ashibaPelletLP2");
            break;
        case 2:
            getNodePosition(&start, 0, "ashibaPelletRP1");
            getNodePosition(&end, 0, "ashibaPelletRP2");
            break;
        default:
            return;
        }
        Vec3f dir;
        dir.m_x = end.m_x - start.m_x;
        dir.m_y = end.m_y - start.m_y;
        dir.m_z = end.m_z - start.m_z;
        float length = grEarthVecLength(&dir);
        float distance = rate * length;
        dir.normalize();
        start.m_x = start.m_x + dir.m_x * distance;
        start.m_y = start.m_y + dir.m_y * distance;
        start.m_z = start.m_z + dir.m_z * distance;
        pos->m_x = start.m_x;
        pos->m_y = start.m_y;
        pos->m_z = start.m_z;
        pos->m_x = start.m_x + m_posOffsetWork->m_x;
        pos->m_y = start.m_y + m_posOffsetWork->m_y;
        pos->m_z = (start.m_z + m_posOffsetWork->m_z) - 10.0f;
    }
}

float grEarthLeaf::getDownLimit() {
    return m_downLimit;
}




