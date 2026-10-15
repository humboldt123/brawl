#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <string.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_gw/gr_gw_chef_etc.h>
#include <st_gw/gr_gw_item.h>

static grGWMarker sMarkerA(0xFF, 0);
static grGWMarker sMarkerB(0xFF, 1);

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

// MATCH-ONLY: the members of soSet are private
struct grGWChefSetView {
    void* m_elements;
    u32 m_size;
};

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grGWChefHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// MATCH-ONLY: the copy of a hit sphere to its simple form (word by word, as the original does it)
static inline void gwChefCopyHit(soCollisionHitData::Simple* dst, soCollisionHitData* src) {
    u32* srcWords = reinterpret_cast<u32*>(src);
    u32* dstWords = reinterpret_cast<u32*>(dst);
    for (int w = 0; w < 6; w += 3) {
        u32 w1 = srcWords[w + 1];
        u32 w0 = srcWords[w];
        dstWords[w] = w0;
        dstWords[w + 1] = w1;
        dstWords[w + 2] = srcWords[w + 2];
    }
    dst->m_size = src->m_size;
    reinterpret_cast<u8*>(dst)[0x1C] = reinterpret_cast<u8*>(src)[0x1C];
}

grGWChefEtc::grGWChefEtc(const char* taskName) : grGW(taskName) {
    m_sceneWork = NULL;
    m_sceneNo = 4;
    memset(m_nodeMeal, 0, sizeof(m_nodeMeal));
    memset(m_nodeChef, 0, sizeof(m_nodeChef));
    memset(m_meal, 0, sizeof(m_meal));
    m_timerChef = 0.0f;
    m_hpChef = 0.0f;
    m_posChef = 0;
    m_stateChef = 0;
    m_attackOnChef = 0;
    m_posLimitWork = NULL;
    m_posEnd.m_x = 0.0f;
    m_posEnd.m_y = 0.0f;
    m_posEnd.m_z = 0.0f;
    m_posStart.m_x = 0.0f;
    m_posStart.m_y = 0.0f;
    m_posStart.m_z = 0.0f;
    m_posNow.m_x = 0.0f;
    m_posNow.m_y = 0.0f;
    m_posNow.m_z = 0.0f;
    m_rot.m_x = 0.0f;
    m_rot.m_y = 0.0f;
    m_rot.m_z = 0.0f;
    m_flying = 0;
    m_nodeFly = -1;
    m_played = 0;
    m_yakumonoMade = 0;
    _27e[0] = 0;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
    m_dangerZone[0] = -1;
    m_dangerZone[1] = -1;
    m_dangerZone[2] = -1;
    m_dangerZone[3] = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->clearAll();
    callback->m_numNodeCallbackData = 2;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
    callback->m_nodeCallbackDatas[1].m_flags |= 1;
    callback->m_nodeCallbackDatas[1].m_flags |= 2;
    callback->m_nodeCallbackDatas[1].m_flags |= 4;
}

grGWChefEtc* grGWChefEtc::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWChefEtc* ground = new (Heaps::StageInstance) grGWChefEtc(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWChefEtc::~grGWChefEtc() {
    if (m_hitData != NULL) {
        delete[] m_hitData;
    }
    m_hitData = NULL;
    if (m_hitSimple != NULL) {
        delete[] m_hitSimple;
    }
    m_hitSimple = NULL;
    if (m_hitSet != NULL) {
        delete m_hitSet;
    }
    m_hitSet = NULL;
    if (m_dataGroup != NULL) {
        delete m_dataGroup;
    }
    m_dataGroup = NULL;
    if (m_data != NULL) {
        delete m_data;
    }
    m_data = NULL;
}

void grGWChefEtc::update(float deltaFrame) {
    grGW::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateScaleBase(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The nodes of the model are looked up and the hit object is made the first time the ground is updated.
void grGWChefEtc::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade != 1) {
        m_nodeCooking = getNodeIndex(0, "_CHIF_COOKING");
        m_nodeMeal[0] = getNodeIndex(0, "Chif_A_Cooking_01");
        m_nodeMeal[1] = getNodeIndex(0, "Chif_A_Cooking_02");
        m_nodeMeal[2] = getNodeIndex(0, "Chif_A_Cooking_03");
        m_nodeMeal[3] = getNodeIndex(0, "Chif_A_Cooking_04");
        m_nodeMeal[4] = getNodeIndex(0, "Chif_A_Cooking_05");
        m_nodeMeal[5] = getNodeIndex(0, "Chif_B_Cooking_01");
        m_nodeMeal[6] = getNodeIndex(0, "Chif_B_Cooking_02");
        m_nodeMeal[7] = getNodeIndex(0, "Chif_B_Cooking_03");
        m_nodeMeal[8] = getNodeIndex(0, "Chif_B_Cooking_04");
        m_nodeMeal[9] = getNodeIndex(0, "Chif_B_Cooking_05");
        m_nodeMeal[10] = getNodeIndex(0, "Chif_C_Cooking_01");
        m_nodeMeal[11] = getNodeIndex(0, "Chif_C_Cooking_02");
        m_nodeMeal[12] = getNodeIndex(0, "Chif_C_Cooking_03");
        m_nodeMeal[13] = getNodeIndex(0, "Chif_C_Cooking_04");
        m_nodeMeal[14] = getNodeIndex(0, "Chif_C_Cooking_05");
        m_nodeMeal[15] = getNodeIndex(0, "Chif_D_Cooking_01");
        m_nodeMeal[16] = getNodeIndex(0, "Chif_D_Cooking_02");
        m_nodeMeal[17] = getNodeIndex(0, "Chif_D_Cooking_03");
        m_nodeMeal[18] = getNodeIndex(0, "Chif_D_Cooking_04");
        m_nodeMeal[19] = getNodeIndex(0, "Chif_D_Cooking_05");
        m_nodeChara = getNodeIndex(0, "_CHIF_CHARA");
        m_nodeChef[0][0] = getNodeIndex(0, "Chif_A_Body");
        m_nodeChef[0][1] = getNodeIndex(0, "_CHIF_A_HAND_01");
        m_nodeChef[0][2] = getNodeIndex(0, "_CHIF_A_HAND_02");
        m_nodeChef[1][0] = getNodeIndex(0, "Chif_B_Body");
        m_nodeChef[1][1] = getNodeIndex(0, "Chif_B_Hand_01");
        m_nodeChef[1][2] = getNodeIndex(0, "Chif_B_Hand_02");
        m_nodeChef[2][0] = getNodeIndex(0, "Chif_C_Body");
        m_nodeChef[2][1] = getNodeIndex(0, "Chif_C_Hand_01");
        m_nodeChef[2][2] = getNodeIndex(0, "Chif_C_Hand_02");
        m_nodeChef[3][0] = getNodeIndex(0, "Chif_D_Body");
        m_nodeChef[3][1] = getNodeIndex(0, "Chif_D_Hand_01");
        m_nodeChef[3][2] = getNodeIndex(0, "Chif_D_Hand_02");
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
        }
    }
}

void grGWChefEtc::updateActive(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, false, true, NULL);
            setVisibility(0);
            if (m_attackOnChef == 1) {
                disableAttack(4);
            }
            if (m_meal[0].m_attackOn == 1) {
                disableAttack(0);
            }
            if (m_meal[1].m_attackOn == 1) {
                disableAttack(1);
            }
            if (m_meal[2].m_attackOn == 1) {
                disableAttack(2);
            }
            if (m_meal[3].m_attackOn == 1) {
                disableAttack(3);
            }
            if (m_nodeFly != -1) {
                setNodeVisibility(true, 0, m_nodeFly, true, false);
                m_flying = 1;
                m_posNow.m_x = 0.0f;
                m_posNow.m_y = 0.0f;
                m_posNow.m_z = 0.0f;
                m_rot.m_x = 0.0f;
                m_rot.m_y = 0.0f;
                m_rot.m_z = 0.0f;
            }
            setNodeVisibility(false, 0, m_nodeChara, true, true);
            setNodeVisibility(false, 0, m_nodeCooking, true, true);
            disableHit(0, 0);
            disableHit(1, 0);
            disableHit(2, 0);
            disableHit(3, 0);
            m_state = 1;
            break;
        case 1:
            if (*m_sceneWork == m_sceneNo) {
                selectPosChef();
                m_timerChef = data[28];
                m_hpChef = data[30];
                m_stateChef = 6;
                m_attackOnChef = 0;
                m_meal[0].m_timer = data[33];
                m_meal[0].m_state = 2;
                m_meal[0].m_stage = 0;
                m_meal[0].m_attackOn = 0;
                m_meal[1].m_timer = data[33];
                m_meal[1].m_state = 2;
                m_meal[1].m_stage = 0;
                m_meal[1].m_attackOn = 0;
                m_meal[2].m_timer = data[33];
                m_meal[2].m_state = 2;
                m_meal[2].m_stage = 0;
                m_meal[2].m_attackOn = 0;
                m_meal[3].m_timer = data[33];
                m_meal[3].m_state = 2;
                m_meal[3].m_stage = 0;
                m_meal[3].m_attackOn = 0;
                setMotion(0, true, true, &m_frameCount);
                setMotionFrame(m_frameCount, 0);
                m_frameLast = m_frameCount;
                if (m_motionRatio > 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_played = 0;
                m_nodeFly = -1;
                m_state = 3;
            }
            break;
        case 3:
            if (*m_sceneWork != m_sceneNo) {
                setMotion(0, false, true, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                disableHit(0, 0);
                disableAttack(0);
                m_meal[0].m_attackOn = 0;
                disableHit(1, 0);
                disableAttack(1);
                m_meal[1].m_attackOn = 0;
                disableHit(2, 0);
                disableAttack(2);
                m_meal[2].m_attackOn = 0;
                disableHit(3, 0);
                disableAttack(3);
                m_meal[3].m_attackOn = 0;
                disableHit(m_posChef, 0);
                m_state = 4;
            } else {
                if (!m_isVisible) {
                    setVisibility(1);
                }
                if (m_played == 0) {
                    if (getMotionFrame(0) > m_frameLast) {
                        setMotion(1, false, false, NULL);
                        m_played = 1;
                    } else {
                        m_frameLast = getMotionFrame(0);
                    }
                }
                updateChef(deltaFrame);
                updateMeal(deltaFrame);
            }
            break;
        case 4:
            if (getMotionFrame(0) >= m_frameCount) {
                if (m_dangerZone[0] != -1) {
                    g_aiMgr->delDangerZone(m_dangerZone[0]);
                }
                if (m_dangerZone[1] != -1) {
                    g_aiMgr->delDangerZone(m_dangerZone[1]);
                }
                if (m_dangerZone[2] != -1) {
                    g_aiMgr->delDangerZone(m_dangerZone[2]);
                }
                if (m_dangerZone[3] != -1) {
                    g_aiMgr->delDangerZone(m_dangerZone[3]);
                }
                m_dangerZone[0] = -1;
                m_dangerZone[1] = -1;
                m_dangerZone[2] = -1;
                m_dangerZone[3] = -1;
                m_state = 0;
            }
            break;
        }
    }
}

// The chef moves to another place now and then; after the player hit him enough he flies away (state 8) along a line.
void grGWChefEtc::updateChef(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timerChef -= deltaFrame;
        if (m_timerChef < 0.0f) {
            m_timerChef = 0.0f;
        }
        switch (m_stateChef) {
        case 6:
            if (m_timerChef == 0.0f) {
                selectPosChef();
                if (m_attackOnChef == 1) {
                    disableAttack(4);
                    m_attackOnChef = 0;
                }
                m_timerChef = data[28];
            }
            break;
        case 7:
            if (m_timerChef == 0.0f) {
                u32 pos = m_posChef;
                setNodeVisibility(false, 0, m_nodeChef[pos][1], true, false);
                setNodeVisibility(true, 0, m_nodeChef[pos][2], true, false);
                m_timerChef = data[28];
                m_stateChef = 6;
            }
            break;
        case 8: {
            m_rot.m_z = m_rot.m_z + 25.0f;
            if (m_rot.m_z > 360.0f) {
                m_rot.m_z = m_rot.m_z - 360.0f;
            }
            float t = 1.0f - m_timerChef / 120.0f;
            if (t < 0.0f) {
                t = 0.0f;
            }
            if (t > 1.0f) {
                t = 1.0f;
            }
            if (t == 1.0f) {
                setNodeVisibility(false, 0, m_nodeFly, true, false);
                *m_sceneWork = 4;
            }
            Vec3f dir = m_posEnd - m_posStart;
            float len = gwLength(dir);
            dir.normalize();
            float dist = len * t;
            m_flying = 1;
            m_posNow.m_x = m_posStart.m_x + dir.m_x * dist;
            m_posNow.m_y = m_posStart.m_y + dir.m_y * dist;
            m_posNow.m_z = m_posStart.m_z + dir.m_z * dist;
            break;
        }
        }
    }
}

// The pans: each one burns through its five models (stage 0 - 4), hurting fighters on the way, and then blinks and starts again.
void grGWChefEtc::updateMeal(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        for (u8 i = 0; i < 4; i++) {
            m_meal[i].m_timer -= deltaFrame;
            if (m_meal[i].m_timer < 0.0f) {
                m_meal[i].m_timer = 0.0f;
            }
            switch (m_meal[i].m_state) {
            case 2:
                if (m_meal[i].m_timer == 0.0f) {
                    float r = randf();
                    if (r >= data[34]) {
                        m_meal[i].m_timer = data[33];
                    } else {
                        setNodeVisibility(true, 0, m_nodeMeal[i * 5 + m_meal[i].m_stage], true, false);
                        Vec3f pos;
                        getNodePosition(&pos, 0, m_nodeMeal[i * 5 + m_meal[i].m_stage]);
                        Vec2f min;
                        Vec2f max;
                        min.m_y = 20.0f + pos.m_y;
                        min.m_x = pos.m_x - 20.0f;
                        max.m_y = pos.m_y - 20.0f;
                        max.m_x = 20.0f + pos.m_x;
                        m_dangerZone[i] = g_aiMgr->setDangerZone(&min, &max, m_dangerZone[i], false, false);
                        m_meal[i].m_timer = data[31];
                        m_meal[i].m_state = 4;
                    }
                }
                break;
            case 3:
                if (m_meal[i].m_timer == 0.0f) {
                    if (m_meal[i].m_stage == 0) {
                        m_meal[i].m_timer = data[32];
                        m_meal[i].m_state = 4;
                    } else {
                        setNodeVisibility(false, 0, m_nodeMeal[i * 5 + m_meal[i].m_stage], true, false);
                        disableAttack(i);
                        m_meal[i].m_attackOn = 0;
                        m_meal[i].m_stage--;
                        setNodeVisibility(true, 0, m_nodeMeal[i * 5 + m_meal[i].m_stage], true, false);
                        setAttackMeal(i);
                        m_meal[i].m_attackOn = 1;
                        Vec3f pos;
                        getNodePosition(&pos, 0, m_nodeMeal[i * 5 + m_meal[i].m_stage]);
                        Vec2f min;
                        Vec2f max;
                        min.m_y = 20.0f + pos.m_y;
                        min.m_x = pos.m_x - 20.0f;
                        max.m_y = pos.m_y - 20.0f;
                        max.m_x = 20.0f + pos.m_x;
                        m_dangerZone[i] = g_aiMgr->setDangerZone(&min, &max, m_dangerZone[i], false, false);
                        m_meal[i].m_timer = data[31];
                    }
                }
                break;
            case 4:
                if (m_played == 1) {
                    setAttackMeal(i);
                    m_meal[i].m_attackOn = 1;
                }
                if (m_meal[i].m_timer == 0.0f) {
                    if (m_meal[i].m_stage == 4 && isTurnMeal(i) == 1) {
                        m_meal[i].m_timer = data[31];
                        m_meal[i].m_state = 3;
                    } else {
                        setNodeVisibility(false, 0, m_nodeMeal[i * 5 + m_meal[i].m_stage], true, false);
                        disableAttack(i);
                        m_meal[i].m_attackOn = 0;
                        m_meal[i].m_stage++;
                        if (m_meal[i].m_stage == 5) {
                            disableAttack(i);
                            m_meal[i].m_state = 5;
                            m_meal[i].m_stage = 4;
                            m_meal[i].m_repeat = 15;
                            m_meal[i].m_timer = 4.0f;
                            m_meal[i].m_attackOn = 0;
                        } else {
                            setNodeVisibility(true, 0, m_nodeMeal[i * 5 + m_meal[i].m_stage], true, false);
                            setAttackMeal(i);
                            m_meal[i].m_attackOn = 1;
                            Vec3f pos;
                            getNodePosition(&pos, 0, m_nodeMeal[i * 5 + m_meal[i].m_stage]);
                            Vec2f min;
                            Vec2f max;
                            min.m_y = 20.0f + pos.m_y;
                            min.m_x = pos.m_x - 20.0f;
                            max.m_y = pos.m_y - 20.0f;
                            max.m_x = 20.0f + pos.m_x;
                            m_dangerZone[i] = g_aiMgr->setDangerZone(&min, &max, m_dangerZone[i], false, false);
                            m_meal[i].m_timer = data[31];
                        }
                    }
                }
                break;
            case 5:
                if (m_meal[i].m_timer <= 2.0f) {
                    setNodeVisibility(false, 0, m_nodeMeal[i * 5 + m_meal[i].m_stage], true, false);
                } else {
                    setNodeVisibility(true, 0, m_nodeMeal[i * 5 + m_meal[i].m_stage], true, false);
                }
                if (m_dangerZone[i] != -1) {
                    g_aiMgr->delDangerZone(m_dangerZone[i]);
                    m_dangerZone[i] = -1;
                }
                if (m_meal[i].m_timer == 0.0f) {
                    if (m_meal[i].m_repeat == 0) {
                        m_meal[i].m_stage = 0;
                        m_meal[i].m_timer = data[33];
                        m_meal[i].m_state = 2;
                    } else {
                        m_meal[i].m_repeat--;
                        m_meal[i].m_timer = 4.0f;
                    }
                }
                break;
            }
        }
    }
}

void grGWChefEtc::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_numNodeCallbackData = 1;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            calcWorldCallBack->m_numNodeCallbackData = 1;
            Vec3f scale;
            gwVec3Scale(&scale, &m_scale, 0.9f);
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
            if (m_flying == 1) {
                calcWorldCallBack->m_numNodeCallbackData = 2;
                calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex = m_nodeFly;
                data = calcWorldCallBack->m_nodeCallbackDatas;
                data[1].m_pos.m_x = m_posNow.m_x;
                data[1].m_pos.m_y = m_posNow.m_y;
                data[1].m_pos.m_z = m_posNow.m_z;
                data = calcWorldCallBack->m_nodeCallbackDatas;
                data[1].m_rot.m_x = m_rot.m_x;
                data[1].m_rot.m_y = m_rot.m_y;
                data[1].m_rot.m_z = m_rot.m_z;
                data = calcWorldCallBack->m_nodeCallbackDatas;
                data[1].m_scale.m_x = m_scale.m_x * 0.9f;
                data[1].m_scale.m_y = m_scale.m_y * 0.9f;
                data[1].m_scale.m_z = m_scale.m_z * 0.9f;
                m_flying = 0;
            }
        }
    }
}

// The hit object: a capsule for each of the four chefs (the chef of the current place is the only one that is on), and the
// attacks of the pans and of the chef.
void grGWChefEtc::setHit() {
    m_hitData = new (Heaps::StageInstance) soCollisionHitData[4];
    m_hitSimple = new (Heaps::StageInstance) soCollisionHitData::Simple[4];
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grGWChefSetView);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;
    Vec3f origin;
    getNodePosition(&origin, 0, static_cast<u32>(2));

    for (int i = 0; i < 4; i++) {
        m_hitData[i].m_startOffsetPos.m_x = 0.0f;
        m_hitData[i].m_startOffsetPos.m_y = 8.0f;
        m_hitData[i].m_startOffsetPos.m_z = 0.0f;
        m_hitData[i].m_endOffsetPos.m_x = 0.0f;
        m_hitData[i].m_endOffsetPos.m_y = -8.0f;
        m_hitData[i].m_endOffsetPos.m_z = 0.0f;
        m_hitData[i].m_size = 12.0f;
        // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
        reinterpret_cast<grGWChefHitByte*>(&m_hitData[i])->m_shape = 1;
        gwChefCopyHit(&m_hitSimple[i], &m_hitData[i]);
        m_hitSimple[i].m_height = soCollisionHitData::Height_Low;
        m_hitSimple[i].m_nodeIndex = m_nodeChef[i][0];
    }
    // MATCH-ONLY: the members of soSet are private
    grGWChefSetView* set = reinterpret_cast<grGWChefSetView*>(m_hitSet);
    set->m_elements = m_hitSimple;
    set->m_size = 4;
    m_dataGroup->m_hitDataSimpleSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(m_hitSet);
    m_dataGroup->m_hitGroupIndex = 0;
    m_data->m_dataGroups = m_dataGroup;
    m_data->m_dataGroupNum = 1;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_data;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 5, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 4, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}

// The attack of the chef (his second hand): 10 percent, knocks fighters away at an angle of 90 degrees.
void grGWChefEtc::setAttackChef(u32 index) {
    soCollisionAttackData attack(1.0f);
    Vec3f offset;
    offset.m_x = 0.0f;
    offset.m_y = 0.0f;
    offset.m_z = 0.0f;
    setAttackGimmickDetails(&attack, 10.0f, 1.0f, 1.0f, 1.0f, 0, &offset, 90, 70, 100, 70, m_nodeChef[m_posChef][2], 0x3FF, 7,
                            false, 15, soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Small,
                            soCollisionAttackData::Sound_Attribute_None, false, false, false, true, false, false, 0, 60, false,
                            false, false, soCollisionAttackData::Lr_Check_Pos, false, false, false, false, false,
                            soCollisionAttackData::Region_None, true);
    m_yakumono->setAttack(index, 0, &attack);
}

// The attack of a pan: 4 percent, from the node of the stage of the pan.
void grGWChefEtc::setAttackMeal(u32 index) {
    soCollisionAttackData attack(1.0f);
    Vec3f offset;
    offset.m_x = 0.0f;
    offset.m_y = 0.0f;
    offset.m_z = 0.0f;
    setAttackGimmickDetails(&attack, 4.0f, 1.0f, 1.0f, 1.0f, 8, &offset, 70, 70, 0, 50, m_nodeMeal[index * 5 + m_meal[index].m_stage],
                            0x3FF, 7, false, 15, soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Medium,
                            soCollisionAttackData::Sound_Attribute_Kick, false, false, false, true, false, false, 0, 60, false,
                            false, false, soCollisionAttackData::Lr_Check_Pos, false, false, false, false, false,
                            soCollisionAttackData::Region_None, true);
    m_yakumono->setAttack(index, 0, &attack);
}

// The chef goes to a place next to the one he is at (the places are in a row); the pan and the hit are moved with him.
void grGWChefEtc::selectPosChef() {
    u32 pos = m_posChef;
    setNodeVisibility(false, 0, m_nodeChef[pos][0], true, false);
    setNodeVisibility(false, 0, m_nodeChef[pos][1], true, false);
    setNodeVisibility(false, 0, m_nodeChef[pos][2], true, false);
    disableHit(m_posChef, 0);
    float r = randf();
    switch (m_posChef) {
    case 0:
        if (r >= 0.5f) {
            m_posChef = 1;
        } else {
            m_posChef = 0;
        }
        break;
    case 1:
        if (r >= 1.0f / 3.0f) {
            if (r >= 2.0f / 3.0f) {
                m_posChef = 2;
            } else {
                m_posChef = 1;
            }
        } else {
            m_posChef = 0;
        }
        break;
    case 2:
        if (r >= 1.0f / 3.0f) {
            if (r >= 2.0f / 3.0f) {
                m_posChef = 3;
            } else {
                m_posChef = 2;
            }
        } else {
            m_posChef = 1;
        }
        break;
    case 3:
        if (r >= 0.5f) {
            m_posChef = 3;
        } else {
            m_posChef = 2;
        }
        break;
    }
    pos = m_posChef;
    setNodeVisibility(true, 0, m_nodeChef[pos][0], true, false);
    setNodeVisibility(true, 0, m_nodeChef[pos][1], true, false);
    if (m_played == 1) {
        enableHit(m_posChef, 0);
    }
}

// The pan of the place that the chef is at is turned (the chef flips the meal); this is true once for each time.
bool grGWChefEtc::isTurnMeal(u32 index) {
    float* data = static_cast<float*>(getStageData());
    if (data == NULL) {
        return false;
    } else if (m_stateChef == 6) {
        if (m_posChef == index) {
            m_stateChef = 7;
            m_timerChef = data[31];
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

// A pan burned a fighter: the flames of that pan go out for a while.
void grGWChefEtc::onInflict(soCollisionLog* log, u32 unk, float damage) {
    u8 index = reinterpret_cast<u8*>(log)[0x25];
    m_meal[index].m_state = 5;
    disableAttack(index);
    m_meal[index].m_timer = 4.0f;
    m_meal[index].m_attackOn = 0;
    m_meal[index].m_repeat = 15;
}

// The chef was hit: when his strength is used up he flies away from his place to a random place of the border of the camera.
void grGWChefEtc::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    float power = damage->unk4;
    fn_27_26399C(m_yakumono);
    m_hpChef = m_hpChef - power;
    if (m_hpChef < 0.0f) {
        m_stateChef = 8;
        m_timerChef = 120.0f;
        disableHit(m_posChef, 0);
        if (damage->unk7c == -1.0f) {
            Vec3f* limit = m_posLimitWork;
            float r = randf();
            m_posEnd.m_x = limit[0].m_x;
            m_posEnd.m_z = 0.0f;
            m_posEnd.m_y = limit[0].m_y - limit[1].m_y * r;
        } else {
            Vec3f* limit = m_posLimitWork;
            float r = randf();
            m_posEnd.m_x = limit[1].m_x;
            m_posEnd.m_z = 0.0f;
            m_posEnd.m_y = limit[0].m_y - limit[1].m_y * r;
        }
        m_nodeFly = m_nodeChef[m_posChef][0];
        getNodePosition(&m_posNow, 0, m_nodeFly);
        m_posStart.m_x = m_posNow.m_x;
        m_posStart.m_y = m_posNow.m_y;
        m_posStart.m_z = m_posNow.m_z;
        setNodeVisibility(false, 0, m_nodeChef[m_posChef][1], true, false);
        setNodeVisibility(false, 0, m_nodeChef[m_posChef][2], true, false);
    }
}
