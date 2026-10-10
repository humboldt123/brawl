#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_earth/gr_earth.h>
#include <st_earth/gr_earth_anim.h>
#include <st_earth/gr_earth_hit.h>

grEarthPelletFlower::grEarthPelletFlower(const char* taskName) : grEarth(taskName), m_snd() {
    m_pelletData = NULL;
    m_motion = 6;
    m_frameCount = 0.0f;
    m_started = 0;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grEarthPelletFlower* grEarthPelletFlower::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthPelletFlower* ground = new (Heaps::StageInstance) grEarthPelletFlower(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthPelletFlower::~grEarthPelletFlower() {
    if (m_hitData != NULL) {
        delete m_hitData;
    }
    m_hitData = NULL;
    if (m_hitSimple != NULL) {
        delete m_hitSimple;
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

void grEarthPelletFlower::processAnim() {
    Ground::processAnim();
}

void grEarthPelletFlower::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateState(deltaFrame);
        updateMotion();
        updateCallBack();
        updateG3dProcCalcWorld();
        m_hasUpdatedG3dCalcWorld = false;
        if (m_pelletData != NULL) {
            getNodeMatrix(&m_pelletData->m_mtx, 0, "PerettoLocateN");
        }
    }
}

// The hit object is made once (when the ground has its model).
void grEarthPelletFlower::updateYakumono(float deltaFrame) {
    if (m_started != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_started = 1;
        }
    }
}

// The steps of the flower: 0 is closed (the data of the pellet is cleared), 1 waits for the stage to make the pellet (2), 0x13
// shows it and reacts to the state of the pellet (1, 2 or 3 = it was hit, 4 = it is gone), 0x1A is over.
void grEarthPelletFlower::updateState(float deltaFrame) {
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    if (m_pelletData != NULL && data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0x13:
            switch (m_pelletData->unk08) {
            case 3:
                setMotion(4, false, true, &m_frameCount);
                disableHit(0, 0);
                disableHit(1, 0);
                m_pelletData->unk08 = 0;
                break;
            case 1:
                setMotion(1, false, true, &m_frameCount);
                disableHit(0, 0);
                disableHit(1, 0);
                m_snd.setPos(&m_pelletData->m_pos);
                m_snd.playSE(static_cast<SndID>(0x1CC6), 0, 0, -1);
                m_snd.playSE(static_cast<SndID>(0x1CC7), 0, 0, -1);
                m_pelletData->unk08 = 0;
                break;
            case 2:
                setMotion(3, false, true, &m_frameCount);
                disableHit(0, 0);
                disableHit(1, 0);
                m_snd.setPos(&m_pelletData->m_pos);
                m_snd.playSE(static_cast<SndID>(0x1CC4), 0, 0, -1);
                m_pelletData->unk08 = 0;
                break;
            case 0:
                break;
            default:
                if (m_pelletData->unk08 < 5) {
                    setMotion(2, false, true, &m_frameCount);
                    disableHit(0, 0);
                    disableHit(1, 0);
                    u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x4E0004));
                    g_ecMgr->setParent(effect, m_sceneModels[0], "TopN", 0);
                    m_state = 0x1A;
                }
                break;
            }
            break;
        case 1:
            if (m_pelletData->m_state == 2) {
                setMotion(0, false, true, &m_frameCount);
                setVisibility(1);
                m_snd.setPos(&m_pelletData->m_pos);
                m_snd.playSE(static_cast<SndID>(0x1CC5), 0, 0, -1);
                u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x4E0003));
                fn_8005FF8C(g_ecMgr, effect, 0x14);
                g_ecMgr->setParent(effect, m_sceneModels[0], "TopN", 1);
                m_state = 0x13;
            }
            break;
        case 0:
            setMotion(6, false, true, NULL);
            setVisibility(0);
            disableHit(0, 0);
            disableHit(1, 0);
            m_pelletData->unk08 = 0;
            m_state = 1;
            break;
        }
    }
}

// The animations of the flower: when the closing one (2) is over, the pellet is made again; any other animation is followed by
// the idle one and the hit spheres are on.
void grEarthPelletFlower::updateMotion() {
    switch (m_motion) {
    case 2:
        if (m_frameCount <= getMotionFrame(0)) {
            m_snd.setPos(&m_pelletData->m_pos);
            m_snd.playSE(static_cast<SndID>(0x1CC9), 0, 0, -1);
            m_pelletData->m_state = 0;
            m_state = 0;
        }
        break;
    default:
        if (m_motion < 5) {
            if (m_frameCount <= getMotionFrame(0)) {
                setMotion(5, true, true, NULL);
                enableHit(0, 0);
                enableHit(1, 0);
                m_pelletData->unk08 = 3;
            }
        }
        break;
    }
}

void grEarthPelletFlower::updateCallBack() {
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
            stEarthPelletData* pellet = m_pelletData;
            if (pellet != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = pellet->m_pos.m_x;
                data->m_pos.m_y = pellet->m_pos.m_y;
                data->m_pos.m_z = pellet->m_pos.m_z;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_rot.m_y = m_pelletData->unk20;
            }
        }
    }
}

// The hit object of the flower: two hit spheres (a capsule of size 2 and a sphere of size 6) and the damage module. It has no
// attack.
void grEarthPelletFlower::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = new (Heaps::StageInstance) soCollisionHitData[2];
    m_hitSimple = new (Heaps::StageInstance) soCollisionHitData::Simple[2];
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grEarthSetView);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData[0].m_startOffsetPos.m_x = 0.0f;
    m_hitData[0].m_startOffsetPos.m_y = 2.0f;
    m_hitData[0].m_startOffsetPos.m_z = 10.0f;
    m_hitData[0].m_endOffsetPos.m_x = 0.0f;
    m_hitData[0].m_endOffsetPos.m_y = 22.0f;
    m_hitData[0].m_endOffsetPos.m_z = 10.0f;
    m_hitData[0].m_size = 2.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grEarthHitByte*>(&m_hitData[0])->m_shape = 1;
    grEarthCopyHit(&m_hitSimple[0], &m_hitData[0]);
    m_hitSimple[0].m_height = soCollisionHitData::Height_Low;
    m_hitSimple[0].m_nodeIndex = 0;
    m_hitData[1].m_startOffsetPos.m_x = 0.0f;
    m_hitData[1].m_startOffsetPos.m_y = 30.0f;
    m_hitData[1].m_startOffsetPos.m_z = 10.0f;
    m_hitData[1].m_endOffsetPos.m_x = 0.0f;
    m_hitData[1].m_endOffsetPos.m_y = 0.0f;
    m_hitData[1].m_endOffsetPos.m_z = 0.0f;
    m_hitData[1].m_size = 6.0f;
    reinterpret_cast<grEarthHitByte*>(&m_hitData[1])->m_shape = 0;
    grEarthCopyHit(&m_hitSimple[1], &m_hitData[1]);
    m_hitSimple[1].m_height = soCollisionHitData::Height_Low;
    m_hitSimple[1].m_nodeIndex = 0;

    // MATCH-ONLY: the members of soSet are private
    grEarthSetView* set = reinterpret_cast<grEarthSetView*>(m_hitSet);
    set->m_elements = m_hitSimple;
    set->m_size = 2;
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
    typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 2, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
    fn_27_263800(yakumono, 1, 0);
}

// The animations of the flower are bound with all their parts: the one of the index is looked for in the file, in every kind
// of animation.
void grEarthPelletFlower::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion != animId || force != 0) {
        nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
        if (sceneMdl != NULL) {
            gfModelAnimation* modelAnim = *m_modelAnims;
            if (modelAnim != NULL) {
                nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
                if (model.IsValid()) {
                    modelAnim->unbindNodeAnim(sceneMdl);
                    modelAnim->unbindVisibleAnim(sceneMdl);
                    modelAnim->unbindTexAnim(sceneMdl);
                    modelAnim->unbindTexSrtAnim(sceneMdl);
                    modelAnim->unbindMatColAnim(sceneMdl);
                    m_motion = animId;
                    if (animId < 6) {
                        GR_EARTH_BIND_ANIMS(animId, model, modelAnim)
                        gfModelAnimation::bind(sceneMdl, modelAnim);
                        modelAnim->setFrame(0.0);
                        modelAnim->setUpdateRate(1.0);
                        modelAnim->setLoop(loop);
                        if (frameCount != NULL) {
                            *frameCount = modelAnim->getFrameCount();
                        }
                    }
                }
            }
        }
    }
}

// A hit takes the hit points of the part that was hit (the second hit sphere is the flower, the first the pellet): the
// pellet is gone (4) when one of the two is used up.
void grEarthPelletFlower::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (m_pelletData != NULL) {
        float amount = damage->unk4;
        fn_27_26399C(m_yakumono);
        if (damage->unk35 == 0) {
            m_pelletData->unk0C = m_pelletData->unk0C - amount;
            if (m_pelletData->unk0C < 0.0f) {
                m_pelletData->unk0C = 0.0f;
            }
        } else {
            m_pelletData->unk10 = m_pelletData->unk10 - amount;
            if (m_pelletData->unk10 < 0.0f) {
                m_pelletData->unk10 = 0.0f;
            }
        }
        stEarthPelletData* pellet = m_pelletData;
        if (pellet->unk0C == 0.0f || pellet->unk10 == 0.0f) {
            pellet->unk08 = 4;
        } else {
            pellet->unk08 = 1;
        }
    }
}

void grEarthPelletFlower::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}
