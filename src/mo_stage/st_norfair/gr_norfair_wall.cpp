#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <gf/gf_camera.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>
#include <yk/yk_no_hit_normal.h>

#include <st_norfair/gr_norfair.h>

grNorfairWall::grNorfairWall(const char* taskName) : grNorfair(taskName), m_snd(), m_subject(0, 1) {
    m_posLimitWork = NULL;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_speed = 0.0f;
    m_speedPrev = 0.0f;
    m_target = 0.0f;
    m_type = 0;
    m_hasHit = 0;
    m_attackSet = 0;
    m_attackWork = NULL;
    m_soundStarted = 0;
    m_seHandle = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    m_subject.clear();
    m_dangerZone = -1;
    m_subject.m_state = 1;
}

grNorfairWall* grNorfairWall::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grNorfairWall* ground = new (Heaps::StageInstance) grNorfairWall(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grNorfairWall::~grNorfairWall() {
    if (m_attackWork != NULL) {
        delete m_attackWork;
    }
    m_attackWork = NULL;
}

void grNorfairWall::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateAI(deltaFrame);
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made the first time this is called.
void grNorfairWall::updateYakumono(float deltaFrame) {
    if (m_hasHit != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
        }
    }
}

// The AI keeps away from the part of the stage that the wall covers.
void grNorfairWall::updateAI(float deltaFrame) {
    Vec3f nodePos;
    Vec2f zoneB;
    Vec2f zoneA;
    getNodePosition(&nodePos, 0, m_nodeIndex);
    switch (m_type) {
    case 0:
        zoneA.m_x = m_posLimitWork[0].m_x;
        zoneB.m_x = nodePos.m_x + 50.0f;
        zoneA.m_y = m_posLimitWork[0].m_y;
        zoneB.m_y = m_posLimitWork[1].m_y;
        if (zoneB.m_x > 40.0f) {
            zoneB.m_x = 40.0f;
        }
        if (zoneA.m_x > zoneB.m_x) {
            zoneB.m_x = zoneA.m_x;
        }
        break;
    case 1:
        zoneA.m_x = nodePos.m_x - 50.0f;
        zoneA.m_y = m_posLimitWork[0].m_y;
        zoneB.m_x = m_posLimitWork[1].m_x;
        zoneB.m_y = m_posLimitWork[1].m_y;
        if (zoneA.m_x < -40.0f) {
            zoneA.m_x = -40.0f;
        }
        if (zoneA.m_x > zoneB.m_x) {
            zoneA.m_x = zoneB.m_x;
        }
        break;
    }
    m_dangerZone = g_aiMgr->setDangerZone(&zoneA, &zoneB, m_dangerZone, false, false);
}

// The wall comes in from its side of the stage when the event of the wall (1 left, 2 right) comes, speeds up towards a place
// of the stage, stops, waits and goes back. The camera follows it.
void grNorfairWall::updateMove(float deltaFrame) {
    stNorfairData* data = static_cast<stNorfairData*>(getStageData());
    if (data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            m_state = 1;
            break;
        case 1:
            if (*m_eventIDWork == 1 && m_type == 0) {
                m_state = 3;
                setAttackLeft();
            }
            if (*m_eventIDWork == 2 && m_type == 1) {
                m_state = 3;
                setAttackRight();
            }
            if (m_state == 3) {
                float r = randf();
                float range = data->unk20;
                m_speed = 0.0f;
                m_speedPrev = 0.0f;
                m_target = -range + (range * 2.0f) * r;
                m_subject.clear();
                m_subject.m_state = 0;
                m_subject.m_stateB = 0;
                gfCameraManager* cameraManager = gfCameraManager::getManager();
                if (cameraManager != NULL) {
                    Vec3f cameraPos;
                    cameraPos.m_x = cameraManager->m_cameras[0].m_targetPos.m_x;
                    cameraPos.m_y = cameraManager->m_cameras[0].m_targetPos.m_y;
                    cameraPos.m_z = cameraManager->m_cameras[0].m_targetPos.m_z;
                    m_subject.setPos(&cameraPos);
                    m_soundStarted = 0;
                }
            }
            break;
        case 3: {
            Vec3f nodePos;
            getNodePosition(&nodePos, 0, m_nodeIndex);
            float goal = data->unk1C + m_target;
            if (goal > nodePos.m_x) {
                m_speed += data->unk14 * deltaFrame;
            }
            if (goal < nodePos.m_x) {
                m_speed -= data->unk14 * deltaFrame;
            }
            if (fabs(m_speed) > data->unk18) {
                if (goal > nodePos.m_x) {
                    m_speed = data->unk18;
                }
                if (goal < nodePos.m_x) {
                    m_speed = data->unk18 * -1.0f;
                }
            }
            m_pos.m_x += m_speed * deltaFrame;
            Vec3f camPos;
            camPos.m_x = m_subject.m_pos.m_x;
            camPos.m_y = m_subject.m_pos.m_y;
            camPos.m_z = m_subject.m_pos.m_z;
            camPos.m_x = camPos.m_x - m_speed;
            if (m_type == 0) {
                float limitX = m_posLimitWork[1].m_x;
                if (camPos.m_x > limitX) {
                    camPos.m_x = limitX;
                }
            }
            if (m_type == 1 && camPos.m_x < m_posLimitWork[0].m_x) {
                camPos.m_x = m_posLimitWork[0].m_x;
            }
            m_subject.setPos(&camPos);
            if (m_speedPrev != 0.0f) {
                bool stopped = false;
                if (m_speedPrev > 0.0f && m_speed < 0.0f) {
                    stopped = true;
                }
                if (m_speedPrev < 0.0f && m_speed > 0.0f) {
                    stopped = true;
                }
                if (m_speed == 0.0f) {
                    stopped = true;
                }
                if (stopped == true) {
                    m_state = 4;
                    m_timer = data->unk24 + (data->unk28 - data->unk24) * randf();
                }
            }
            if (m_soundStarted == 0) {
                Vec3f soundPos;
                getNodePosition(&soundPos, 0, m_nodeIndex);
                if (m_speed > 0.0f && m_posLimitWork[0].m_x + 50.0f < soundPos.m_x) {
                    m_soundStarted = 1;
                }
                if (m_speed < 0.0f && soundPos.m_x < m_posLimitWork[1].m_x - 50.0f) {
                    m_soundStarted = 1;
                }
                if (m_soundStarted == 1) {
                    m_seHandle = m_snd.playSE(static_cast<SndID>(0x1BC6), 0, 0x3C, -1);
                }
            }
            m_speedPrev = m_speed;
            break;
        }
        case 4:
            if (m_timer == 0.0f) {
                m_speed = 0.0f;
                m_speedPrev = 0.0f;
                m_subject.m_state = 1;
                m_soundStarted = 0;
                m_state = 5;
            }
            break;
        case 5: {
            if (m_pos.m_x < 0.0f) {
                m_speed += data->unk14 * deltaFrame;
            }
            if (m_pos.m_x > 0.0f) {
                m_speed -= data->unk14 * deltaFrame;
            }
            if (fabs(m_speed) > data->unk18) {
                if (m_pos.m_x < 0.0f) {
                    m_speed = data->unk18;
                }
                if (m_pos.m_x > 0.0f) {
                    m_speed = data->unk18 * -1.0f;
                }
            }
            m_pos.m_x += m_speed * deltaFrame;
            if (m_speedPrev != 0.0f) {
                bool stopped = false;
                if (m_speedPrev > 0.0f && m_speed < 0.0f) {
                    stopped = true;
                }
                if (m_speedPrev < 0.0f && m_speed > 0.0f) {
                    stopped = true;
                }
                if (m_speed == 0.0f) {
                    stopped = true;
                }
                if (stopped == true) {
                    if (m_seHandle != -1) {
                        m_snd.stopSE(m_seHandle, 0x3C);
                    }
                    m_seHandle = -1;
                    if ((u8)(*m_eventIDWork - 1) < 2) {
                        *m_eventIDWork = 0;
                    }
                    m_state = 0;
                    disableAttack(0);
                    disableAttack(1);
                    disableAttack(2);
                    disableAttack(3);
                    disableAttack(4);
                    m_attackSet = 0;
                }
            }
            if (m_soundStarted == 0) {
                Vec3f soundPos;
                getNodePosition(&soundPos, 0, m_nodeIndex);
                if (m_speed > 0.0f && m_posLimitWork[1].m_x - 50.0f < soundPos.m_x) {
                    m_soundStarted = 1;
                }
                if (m_speed < 0.0f && soundPos.m_x < m_posLimitWork[0].m_x + 50.0f) {
                    m_soundStarted = 1;
                }
                if (m_soundStarted == 1) {
                    if (m_seHandle != -1) {
                        m_snd.stopSE(m_seHandle, 0x3C);
                    }
                    m_seHandle = -1;
                }
            }
            m_speedPrev = m_speed;
            break;
        }
        }
    }
}

void grNorfairWall::updateCallBack(float deltaFrame) {
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
            calcWorldCallBack->m_nodeCallbackDatas->m_offsetPos.m_x = m_pos.m_x;
            m_snd.setPos(&m_pos);
        }
    }
}

// The hit object of the wall: an attack module with five attack parts and no hit module.
void grNorfairWall::setHit() {
    m_attackWork = new (Heaps::StageInstance) ykData;
    m_attackWork->m_dataGroupNum = 0;
    m_attackWork->m_dataGroups = NULL;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_attackWork;

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 5, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

void grNorfairWall::setAttackLeft() {
    if (m_attackSet != 1) {
        Vec3f offset;
        offset.m_x = -90.0f;
        offset.m_y = -100.0f;
        offset.m_z = 0.0f;
        setAttackDetails(0, &offset);
        offset.m_x = -90.0f;
        offset.m_y = -50.0f;
        offset.m_z = 0.0f;
        setAttackDetails(1, &offset);
        offset.m_y = 0.0f;
        offset.m_x = -90.0f;
        offset.m_z = 0.0f;
        setAttackDetails(2, &offset);
        offset.m_x = -90.0f;
        offset.m_y = 50.0f;
        offset.m_z = 0.0f;
        setAttackDetails(3, &offset);
        offset.m_x = -100.0f;
        offset.m_y = 100.0f;
        offset.m_z = 0.0f;
        setAttackDetails(4, &offset);
        m_attackSet = 1;
    }
}

void grNorfairWall::setAttackRight() {
    if (m_attackSet != 1) {
        Vec3f offset;
        offset.m_x = 90.0f;
        offset.m_y = -100.0f;
        offset.m_z = 0.0f;
        setAttackDetails(0, &offset);
        offset.m_x = 90.0f;
        offset.m_y = -50.0f;
        offset.m_z = 0.0f;
        setAttackDetails(1, &offset);
        offset.m_y = 0.0f;
        offset.m_x = 90.0f;
        offset.m_z = 0.0f;
        setAttackDetails(2, &offset);
        offset.m_x = 90.0f;
        offset.m_y = 50.0f;
        offset.m_z = 0.0f;
        setAttackDetails(3, &offset);
        offset.m_x = 100.0f;
        offset.m_y = 100.0f;
        offset.m_z = 0.0f;
        setAttackDetails(4, &offset);
        m_attackSet = 1;
    }
}

// One hit sphere of the wall (size 90): it burns (power 14) and sends the fighter away from the wall.
void grNorfairWall::setAttackDetails(int index, Vec3f* offset) {
    soCollisionAttackData attack(1.0f);
    setAttackGimmickDetails(&attack, 90.0f, 1.0f, 1.0f, 1.0f,
        14, offset, 25, 66, 0, 60, m_nodeIndex,
        0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Fire, soCollisionAttackData::Sound_Level_Large,
        soCollisionAttackData::Sound_Attribute_Fire,
        false, false, false, true, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Forward,
        false, false, false, false, false, soCollisionAttackData::Region_None, false);
    m_yakumono->setAttack(index, 0, &attack);
    switch (m_type) {
    case 0:
        m_yakumono->setLr(1.0f);
        break;
    case 1:
        m_yakumono->setLr(-1.0f);
        break;
    }
}
