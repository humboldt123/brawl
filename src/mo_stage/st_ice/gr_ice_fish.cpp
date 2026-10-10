#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves

#include <ai/ai_mgr.h>
#include <ec/ec_mgr.h>
#include <gf/gf_memory_util.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <st/st_trigger.h>
#include <types.h>

#include <st_ice/gr_ice.h>
#include <st_ice/gr_ice_anim.h>

grIceFish::grIceFish(const char* taskName) : grIce(taskName), m_snd() {
    m_attackWork = NULL;
    m_effectHandle = 0;
    m_posWork = NULL;
    m_posXWork = NULL;
    m_limitWork = NULL;
    m_stateWork = NULL;
    m_motion = 1;
    m_motionTimer = 0.0f;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_started = 0;
    m_attackSet = 0;
    m_sePlayed = 0;
    m_dangerZone = -1;
    m_eatState = 0;
    m_trigger = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grIceFish* grIceFish::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIceFish* ground = new (Heaps::StageInstance) grIceFish(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceFish::~grIceFish() {
    if (m_attackWork != NULL) {
        delete m_attackWork;
    }
    m_attackWork = NULL;
}

void grIceFish::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The area of the mouth sees the fighters (the events 0x27: a fighter is in the mouth, 0x33: the area is gone).
void grIceFish::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    switch (eventInfo->m_kind) {
    case Gimmick::Event_Exit:
        m_eatState = 0;
        return;
    case Gimmick::Chappy_Event_In:
        m_eatState = 1;
        m_snd.playSE(static_cast<SndID>(0x1C89), 0, 0, -1);
        return;
    default:
        return;
    }
}

// The tail attacks and the mouth eats only while the fish is in the air: the area of the mouth is on between the frames 24
// and 40, the tail hits from 40 to 80.
void grIceFish::updateYakumono(float deltaFrame) {
    if (m_started == 1) {
        if (m_state == 7) {
            if (24.0f < getMotionFrame(0)) {
                if (getMotionFrame(0) < 40.0f || 80.0f < getMotionFrame(0)) {
                    if (m_attackSet == 1) {
                        disableAttack(0);
                    }
                    m_attackSet = 0;
                    if (m_trigger != NULL) {
                        m_trigger->setAreaSleep(true);
                    }
                } else {
                    setAttackTail();
                }
            } else if (m_trigger != NULL) {
                m_trigger->setAreaSleep(false);
            }
        } else {
            if (m_attackSet == 1) {
                disableAttack(0);
            }
            m_attackSet = 0;
            if (m_trigger != NULL) {
                m_trigger->setAreaSleep(true);
            }
        }
        if (m_eatState == 1) {
            presentPosEvent();
        }
        return;
    }
    createEatArea();
    if (m_yakumono != NULL) {
        m_started = 1;
    }
}

// The fish waits under the water. When the stage says so it jumps up at the place it chose (the splash, the danger zone
// for the AI), flies in an arc and goes back down.
void grIceFish::updateActive(float deltaFrame) {
    float timer = m_timer - deltaFrame;
    m_timer = timer;
    if (timer < 0.0f) {
        m_timer = 0.0f;
    }
    timer = m_motionTimer - deltaFrame;
    m_motionTimer = timer;
    if (timer < 0.0f) {
        m_motionTimer = 0.0f;
    }
    u8 state = m_state;
    if (state == 6) {
        m_pos.m_x = *m_posXWork;
        m_pos.m_y = m_posWork[0].m_y + 70.0f;
        m_pos.m_z = 0.0f;
        g_ecMgr->setPos(m_effectHandle, &m_pos);
        if (m_timer == 0.0f) {
            setMotion(0, false, true, &m_motionTimer);
            setVisibility(1);
            u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x480006));
            g_ecMgr->setParent(effect, m_sceneModels[0], "StgIceFish_TransN", 0);
            m_state = 7;
        }
    } else if (state < 6) {
        if (state == 1) {
            if (*m_stateWork == 5) {
                m_pos.m_x = *m_posXWork;
                m_pos.m_y = m_posWork[0].m_y + 70.0f;
                m_pos.m_z = 0.0f;
                m_effectHandle = g_ecMgr->setEffect(static_cast<EfID>(0x480007), &m_pos);
                Vec2f corner1;
                Vec2f corner2;
                corner1.m_x = m_pos.m_x - 30.0f;
                corner1.m_y = m_pos.m_y + 30.0f;
                corner2.m_x = m_pos.m_x + 30.0f;
                corner2.m_y = m_pos.m_y - 30.0f;
                m_dangerZone = g_aiMgr->setDangerZone(&corner1, &corner2, m_dangerZone, 0, 0);
                m_timer = 45.0f;
                m_state = 6;
            }
        } else if (state == 0) {
            setMotion(1, false, true, NULL);
            setVisibility(0);
            m_sePlayed = 0;
            m_pos.m_x = 0.0f;
            m_pos.m_y = m_limitWork[1].m_y - 100.0f;
            m_pos.m_z = 0.0f;
            m_state = 1;
        }
    } else if (state < 8) {
        if (m_motionTimer == 0.0f) {
            if (m_dangerZone != -1) {
                g_aiMgr->delDangerZone(m_dangerZone);
                m_dangerZone = -1;
            }
            *m_stateWork = 0xE;
            m_state = 0;
        }
        if (m_sePlayed == 0 && 10.0f <= getMotionFrame(0)) {
            m_snd.playSE(static_cast<SndID>(0x1C8B), 0, 0, -1);
            m_sePlayed = 1;
        }
    }
}

void grIceFish::updateCallBack(float deltaFrame) {
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
            data->m_pos.m_x = m_pos.m_x;
            data->m_pos.m_y = m_pos.m_y;
            data->m_pos.m_z = m_pos.m_z;
            m_snd.setPos(&m_pos);
        }
    }
}

// The attack of the tail (power 15, it throws the fighter up and to the sides).
void grIceFish::setAttackTail() {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        u32 node;
        getNodeIndex(&node, 0, "StgIceFish_TailNd");
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = 0.0f;
        offset.m_z = 0.0f;
        setAttackGimmickDetails(&attack, 15.0f, 1.0f, 1.0f, 1.0f,
            15, &offset, 90, 140, 0, 50, node,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Large, soCollisionAttackData::Sound_Attribute_Kick,
            false, false, false, true, false, false, 0, 60,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, false);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackSet = 1;
    }
}

void grIceFish::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_motion = animId;

    if (animId == 0) {
        bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > 0);
        if (result) {
            grIceSetVisibilityAnim2(0, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > 0);
        if (result) {
            grIceSetChrAnim2(0, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > 0);
        if (result) {
            grIceSetTexPatAnim2(0, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > 0);
        if (result) {
            grIceSetTexSrtAnim2(0, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > 0);
        if (result) {
            grIceSetColorAnim2(0, model, modelAnim, Heaps::StageInstance);
        }

        gfModelAnimation::bind(sceneMdl, modelAnim);
        modelAnim->setFrame(0.0);
        modelAnim->setUpdateRate(1.0);
        modelAnim->setLoop(loop);

        if (frameCount != NULL) {
            *frameCount = modelAnim->getFrameCount();
        }
    }
}

// The area of the mouth: a square (20 x 20) next to the node of the head, watched by a trigger of the kind 0x2F.
void grIceFish::createEatArea() {
    gfMemFill(&m_ykData, 0, 0xC);
    m_areaData.m_group = static_cast<gfArea::Group>(0x15);
    m_areaData.m_shapeType = static_cast<soAreaInstance::ShapeType>(0);
    m_areaData.m_0x4 = 0;
    m_areaData.m_0x8 = 0;
    m_areaData.m_shapeFlag.m_mask = 1;
    m_areaData.m_nodeIndex = 0xC;
    m_areaData.m_offsetPos.m_x = 5.0f;
    m_areaData.m_offsetPos.m_y = 0.0f;
    m_areaData.m_range.m_x = 20.0f;
    m_areaData.m_range.m_y = 20.0f;
    setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, true);
    m_trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Peripheral_Lock, -1);
    m_trigger->setObserveYakumono(m_yakumono);
}

// The place of the mouth is given to the area of the fighters that are eaten.
void grIceFish::presentPosEvent() {
    Vec3f pos;
    getNodePosition(&pos, 0, m_areaData.m_nodeIndex);
    soGimmickChappyEventArgs_Pos args(pos);
    m_yakumono->presentEventGimmick(&args, -1);
}
