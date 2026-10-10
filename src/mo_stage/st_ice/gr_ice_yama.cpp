#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <cm/cm_quake.h>
#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <so/so_world.h>
#include <types.h>
#include <yk/yk_no_hit_normal.h>

#include <st_ice/gr_ice.h>

grIceYama::grIceYama(const char* taskName) : grIce(taskName), m_snd() {
    m_unk158 = 0;
    m_mtxGimmickWork = NULL;
    m_posBearWork = NULL;
    m_stateWork = NULL;
    m_stateBearWork = NULL;
    m_prevStage = 0xE;
    m_unk16D = 0;
    m_speed = 0.0f;
    m_limit = 0.0f;
    m_offsetX = 0.0f;
    m_offsetY = 0.0f;
    m_offsetZ = 0.0f;
    m_nodeKaiten = 0;
    m_nodeKoware = 0;
    m_nodeTSide[0] = 0;
    m_nodeTSide[1] = 0;
    m_nodeTSide[2] = 0;
    m_nodeTurara = 0;
    m_nodeBear[0] = 0;
    m_nodeBear[1] = 0;
    m_nodeBear[2] = 0;
    m_nodeBear[3] = 0;
    m_nodeBear[4] = 0;
    m_joint = NULL;
    m_hasHit = 0;
    m_attackSet = 0;
    m_attackWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    m_seHandleA = -1;
    m_seHandleB = -1;
    m_seState = 0xE;
    m_seTimer = 0.0f;
}

grIceYama* grIceYama::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIceYama* ground = new (Heaps::StageInstance) grIceYama(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceYama::~grIceYama() {
    if (m_attackWork != NULL) {
        delete m_attackWork;
    }
    m_attackWork = NULL;
}

void grIceYama::processAnim() {
    Ground::processAnim();
}

void grIceYama::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateJoint();
        updateYakumono(deltaFrame);
        updateState(deltaFrame);
        updateSE(deltaFrame);
        updateCallBack(deltaFrame);
        updateG3dProcCalcWorld();
        m_hasUpdatedG3dCalcWorld = false;
        if (m_mtxGimmickWork != NULL) {
            getNodeMatrix(&m_mtxGimmickWork[6], 0, m_nodeKaiten);
            getNodeMatrix(&m_mtxGimmickWork[7], 0, m_nodeKoware);
            getNodeMatrix(&m_mtxGimmickWork[8], 0, m_nodeTSide[0]);
            getNodeMatrix(&m_mtxGimmickWork[9], 0, m_nodeTSide[1]);
            getNodeMatrix(&m_mtxGimmickWork[10], 0, m_nodeTSide[2]);
            getNodeMatrix(&m_mtxGimmickWork[11], 0, m_nodeTurara);
        }
        if (m_posBearWork != NULL) {
            getNodePosition(&m_posBearWork[0], 0, m_nodeBear[0]);
            getNodePosition(&m_posBearWork[1], 0, m_nodeBear[1]);
            getNodePosition(&m_posBearWork[2], 0, m_nodeBear[2]);
            getNodePosition(&m_posBearWork[3], 0, m_nodeBear[3]);
            getNodePosition(&m_posBearWork[4], 0, m_nodeBear[4]);
        }
    }
}

bool grIceYama::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeKaiten, 0, "P_Kaitenyuka");
    getNodeIndex(&m_nodeKoware, 0, "P_kowareyuka");
    getNodeIndex(&m_nodeTSide[0], 0, "P_TSide1");
    getNodeIndex(&m_nodeTSide[1], 0, "P_TSide2");
    getNodeIndex(&m_nodeTSide[2], 0, "P_TSide3");
    getNodeIndex(&m_nodeTurara, 0, "P_Turara");
    getNodeIndex(&m_nodeBear[0], 0, "P_WhiteBare1");
    getNodeIndex(&m_nodeBear[1], 0, "P_WhiteBare2");
    getNodeIndex(&m_nodeBear[2], 0, "P_WhiteBare3");
    getNodeIndex(&m_nodeBear[3], 0, "P_WhiteBare4");
    getNodeIndex(&m_nodeBear[4], 0, "P_WhiteBare5");
    return result;
}

// The joint of the collision that belongs to the top of the mountain is found once (the avalanche changes its material).
void grIceYama::updateJoint() {
    if (m_joint == NULL && m_collision != NULL) {
        u32 node;
        if (getNodeIndex(&node, 0, "M_top_out1")) {
            u16 count = m_collision->m_jointLen;
            for (u32 i = 0; i != count; i++) {
                grCollisionJoint* joint = m_collision->getJoint(i);
                if (joint == NULL) {
                    return;
                }
                if (joint->m_ground == this && joint->_0x4E == 0 && node == joint->m_nodeIndex) {
                    m_joint = joint;
                    return;
                }
            }
        }
    }
}

// The hit object is made the first time this is called.
void grIceYama::updateYakumono(float deltaFrame) {
    if (m_hasHit != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
        }
    }
}

// The steps of the mountain follow the part of the scene (the state of the stage): 1 - the mountain goes up with the
// shaking and the avalanche sounds, 2 - the avalanche with an attack (the snow crashes down), 3 - the bears' ledge moves
// like a seesaw, 4 - the mountain slows down.
void grIceYama::updateState(float deltaFrame) {
    stIceData* data = static_cast<stIceData*>(getStageData());
    if (data == NULL) {
        return;
    }
    float timer = m_timer - deltaFrame;
    m_timer = timer;
    if (timer < 0.0f) {
        m_timer = 0.0f;
    }
    if (*m_stateWork != m_prevStage) {
        m_state = 0;
    }
    switch (*m_stateWork) {
    case 3: {
        u8 state = m_state;
        if (state == 0x10) {
            float speed = m_speed + data->unkA4 * deltaFrame;
            m_speed = speed;
            if (data->unkA8 < speed) {
                m_speed = data->unkA8;
            }
            float y = m_offsetY - m_speed;
            m_offsetY = y;
            if (m_limit < __fabs(y)) {
                setCollisionAttr(10);
                float random = randf();
                m_state = 0x11;
                m_timer = data->unkAC + (data->unkB0 - data->unkAC) * random;
            }
        } else if (state < 0x10) {
            if (state == 1) {
                if (*m_stateBearWork == 6) {
                    m_limit = data->unk9C;
                    m_snd.playSE(static_cast<SndID>(0x1C88), 0, 0, -1);
                    m_state = 0x10;
                }
                if (*m_stateBearWork == 7) {
                    m_limit = data->unkA0;
                    m_snd.playSE(static_cast<SndID>(0x1C88), 0, 0, -1);
                    m_state = 0x10;
                }
            } else if (state == 0) {
                if (m_seHandleA != -1) {
                    m_snd.stopSE(m_seHandleA, 0x1E);
                    m_seHandleA = -1;
                }
                m_snd.playSE(static_cast<SndID>(0x1C7F), 0, 0, -1);
                m_seHandleB = m_snd.playSE(static_cast<SndID>(0x1C84), 0, 0, -1);
                cmRemoveQuake(1);
                Vec3f quake;
                quake.m_x = 0.0f;
                quake.m_y = 0.0f;
                quake.m_z = 0.0f;
                cmReqQuake(cmQuake::Amplitude_XL, &quake);
                setGravity(1.0f, 1.0f);
                disableAttack(0);
                m_attackSet = 0;
                m_limit = 0.0f;
                m_speed = 0.0f;
                m_offsetX = 0.0f;
                m_offsetY = 0.0f;
                m_offsetZ = 0.0f;
                m_state = 1;
            }
        } else if (state == 0x12) {
            float y = m_offsetY + data->unkB4 * deltaFrame;
            m_offsetY = y;
            if (0.0f <= y) {
                m_limit = 0.0f;
                m_speed = 0.0f;
                m_offsetX = 0.0f;
                m_offsetY = 0.0f;
                m_offsetZ = 0.0f;
                if (*m_stateBearWork != 0) {
                    *m_stateBearWork = 5;
                }
                setCollisionAttr(0xE);
                m_state = 1;
            }
        } else if (state < 0x12 && m_timer == 0.0f) {
            m_state = 0x12;
        }
        break;
    }
    case 1: {
        {
            u8 state = m_state;
            if (state == 3) {
                if (getMotionFrame(0) >= 1620.0f) {
                    m_snd.playSE(static_cast<SndID>(0x1C7C), 0, 0, -1);
                    Vec3f quake;
                    quake.m_x = 0.0f;
                    quake.m_y = 0.0f;
                    quake.m_z = 0.0f;
                    cmReqQuake(cmQuake::Amplitude_M, &quake);
                    m_state = 1;
                }
            } else if (state < 3 && state == 0) {
                disableAttack(0);
                m_attackSet = 0;
                setGravity(1.0f, 1.0f);
                m_state = 3;
            }
        }
        break;
    }
    case 2: {
        {
            u8 state = m_state;
            if (state != 1) {
                if (state == 0) {
                    u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x480001));
                    g_ecMgr->setParent(effect, m_sceneModels[0], "A_M_top", 0);
                    cmRemoveQuake(1);
                    Vec3f quake;
                    quake.m_x = 0.0f;
                    quake.m_y = 0.0f;
                    quake.m_z = 0.0f;
                    cmReqQuake(cmQuake::Amplitude_S, &quake);
                    m_snd.playSE(static_cast<SndID>(0x1C7D), 0, 0, -1);
                    setAttack();
                    m_seState = 2;
                    m_seTimer = 625.0f;
                    m_state = 2;
                } else if (state < 3 && getMotionFrame(0) >= 1840.0f) {
                    m_seHandleA = m_snd.playSE(static_cast<SndID>(0x1C7E), 0, 0, -1);
                    Vec3f quake;
                    quake.m_x = 0.0f;
                    quake.m_y = 0.0f;
                    quake.m_z = 0.0f;
                    cmReqQuake(cmQuake::Amplitude_M, &quake);
                    setGravity(0.5f, 0.5f);
                    m_state = 1;
                }
            }
        }
        break;
    }
    case 4: {
        if (m_state != 1) {
            if (m_state != 0) {
                m_prevStage = *m_stateWork;
                return;
            }
            disableAttack(0);
            m_attackSet = 0;
            if (m_seHandleB != -1) {
                m_snd.stopSE(m_seHandleB, 0x1E);
                m_seHandleB = -1;
            }
            m_snd.playSE(static_cast<SndID>(0x1C8D), 0, 0, -1);
            Vec3f quake;
            quake.m_x = 0.0f;
            quake.m_y = 0.0f;
            quake.m_z = 0.0f;
            cmReqQuake(cmQuake::Amplitude_L, &quake);
            setGravity(1.25f, 1.25f);
            setCollisionAttr(0xE);
            m_seState = 4;
            m_seTimer = 400.0f;
            m_state = 1;
        }
        float y = m_offsetY + data->unkB4 * deltaFrame;
        m_offsetY = y;
        if (0.0f <= y) {
            m_limit = 0.0f;
            m_speed = 0.0f;
            m_offsetX = 0.0f;
            m_offsetY = 0.0f;
            m_offsetZ = 0.0f;
            m_state = 4;
        }
        break;
    }
    }
    m_prevStage = *m_stateWork;
}

// The volume of the sound effects (not of the music) goes down as the avalanche comes and back up after it.
void grIceYama::updateSE(float deltaFrame) {
    stIceData* data = static_cast<stIceData*>(getStageData());
    if (data != NULL) {
        float timer = m_seTimer - deltaFrame;
        m_seTimer = timer;
        if (timer < 0.0f) {
            m_seTimer = 0.0f;
        }
        u8 state = m_seState;
        if (state != 3) {
            if (state < 3) {
                if (1 < state && g_Stage->m_stageParam != NULL) {
                    float rate = 1.0f - m_seTimer / 625.0f;
                    if (rate - 0.0f < 0.0f) {
                        rate = 0.0f;
                    }
                    float clamped = 1.0f;
                    if (rate - 1.0f < 0.0f) {
                        clamped = rate;
                    }
                    if (clamped == 1.0f) {
                        m_seState = 0xE;
                    }
                    float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(clamped * 16384.0f))) * (1.0f / 256.0f));
                    g_sndSystem->setEffectVol(0, 1, g_Stage->m_stageParam->m_effectVol * 0.01f * (1.0f - sine));
                }
            } else if (state < 5 && g_Stage->m_stageParam != NULL) {
                float rate = 1.0f - m_seTimer / 400.0f;
                if (rate - 0.0f < 0.0f) {
                    rate = 0.0f;
                }
                float clamped = 1.0f;
                if (rate - 1.0f < 0.0f) {
                    clamped = rate;
                }
                if (clamped == 1.0f) {
                    m_seState = 0xE;
                }
                float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(clamped * 16384.0f))) * (1.0f / 256.0f));
                g_sndSystem->setEffectVol(0, 1, g_Stage->m_stageParam->m_effectVol * 0.01f * sine);
            }
        }
    }
}

void grIceYama::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_offsetPos.m_x = m_offsetX;
            data->m_offsetPos.m_y = m_offsetY;
            data->m_offsetPos.m_z = m_offsetZ;
            Vec3f pos;
            pos.m_x = 0.0f;
            pos.m_y = 0.0f;
            pos.m_z = 0.0f;
            m_snd.setPos(&pos);
        }
    }
}

// The hit object of the mountain: two attack parts and nothing else (the attack is the avalanche).
void grIceYama::setHit() {
    m_attackWork = new (Heaps::StageInstance) ykData;
    m_attackWork->m_dataGroupNum = 0;
    m_attackWork->m_dataGroups = NULL;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_attackWork;

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 2, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

// The avalanche (power 18, it throws the fighter away from the mountain).
void grIceYama::setAttack() {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        u32 node = getNodeIndex(0, "P_TSide2");
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = 0.0f;
        offset.m_z = 0.0f;
        setAttackGimmickDetails(&attack, 2.5f, 1.0f, 1.0f, 1.0f,
            18, &offset, 361, 100, 70, 70, node,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Large, soCollisionAttackData::Sound_Attribute_Kick,
            false, false, false, true, false, false, 0, 60,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, true);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackSet = 1;
    }
}

void grIceYama::setGravity(float up, float down) {
    g_soWorld->m_gravityUp = up;
    g_soWorld->m_gravityDown = down;
}

// All the lines of the joint get the material (the top of the mountain is snow, ice during the fall).
void grIceYama::setCollisionAttr(u8 material) {
    if (m_joint != NULL) {
        u16 count = reinterpret_cast<u16*>(m_joint)[1];
        for (u32 i = 0; i != count; i++) {
            grCollisionLine* line = m_joint->getLine(i);
            if (line != NULL) {
                line->m_materialType = static_cast<grCollisionLine::MaterialType>(material);
            }
        }
    }
}
