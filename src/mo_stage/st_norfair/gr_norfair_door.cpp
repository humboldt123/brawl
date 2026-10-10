#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <gf/gf_model.h>
#include <gr/collision/gr_collision.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_norfair/gr_norfair.h>
#include <st_norfair/gr_norfair_anim.h>

// MATCH-ONLY: the members of soSet are private
struct grNorfairSetView {
    void* m_elements;
    u32 m_size;
};

// MATCH-ONLY: the copy of a hit sphere to its simple form (word by word, as the original does it)
static inline void norfairCopyHit(soCollisionHitData::Simple* dst, soCollisionHitData* src) {
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

grNorfairDoor::grNorfairDoor(const char* taskName) : grNorfair(taskName), m_snd() {
    m_posWork = NULL;
    m_posIndex = NULL;
    m_eventIDWork = NULL;
    m_stateWork = NULL;
    m_scaleY = 0.001f;
    m_motion = 3;
    m_frameCount = 0.0f;
    m_hasHit = 0;
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
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
}

grNorfairDoor* grNorfairDoor::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grNorfairDoor* ground = new (Heaps::StageInstance) grNorfairDoor(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    ground->setupMelee();
    return ground;
}

grNorfairDoor::~grNorfairDoor() {
    if (m_hitData != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitData); // MATCH-ONLY: raw storage (no element destructors)
    }
    m_hitData = NULL;
    if (m_hitSimple != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitSimple);
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

void grNorfairDoor::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made the first time this is called.
void grNorfairDoor::updateYakumono(float deltaFrame) {
    if (m_hasHit != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
        }
    }
}

void grNorfairDoor::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    fn_27_26399C(m_yakumono);
    setMotion(0, false, false, &m_frameCount);
    setEnableCollisionStatus(false);
    disableHit(0, 0);
    disableHit(1, 0);
    *m_stateWork = 6;
    m_snd.playSE(static_cast<SndID>(0x1BCC), 0, 0, -1);
    m_snd.setPos(&m_posWork[*m_posIndex]);
}

// The door opens when the event of the zone (the wave, event 4) comes and closes again after it. A hit opens it at once.
void grNorfairDoor::updateActive(float deltaFrame) {
    stNorfairData* data = static_cast<stNorfairData*>(getStageData());
    if (data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        m_frameCount = m_frameCount - deltaFrame;
        if (m_frameCount < 0.0f) {
            m_frameCount = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, false, true, NULL);
            setVisibility(0);
            setEnableCollisionStatus(false);
            disableHit(0, 0);
            disableHit(1, 0);
            m_state = 1;
            break;
        case 1:
            if (*m_eventIDWork == 4) {
                m_timer = data->unk48;
                m_scaleY = 0.001f;
                m_state = 12;
            }
            break;
        case 12:
            if (m_timer == 0.0f) {
                setVisibility(1);
                setEnableCollisionStatus(true);
                enableHit(0, 0);
                enableHit(1, 0);
                m_timer = data->unk4C;
                m_state = 6;
            }
            break;
        case 6:
            m_scaleY += deltaFrame * 0.041f;
            if (m_scaleY >= 1.0f) {
                m_scaleY = 1.0f;
            }
            if (m_timer == 0.0f) {
                setMotion(1, false, false, NULL);
                setEnableCollisionStatus(true);
                disableHit(0, 0);
                disableHit(1, 0);
                *m_stateWork = 7;
                m_snd.playSE(static_cast<SndID>(0x1BCD), 0, 0, -1);
                m_snd.setPos(&m_posWork[*m_posIndex]);
                m_state = 7;
            }
            break;
        case 7:
            if (*m_eventIDWork != 4) {
                m_timer = data->unk58;
                m_state = 8;
            }
            break;
        case 8:
            if (m_timer == 0.0f) {
                setMotion(0, false, true, &m_frameCount);
                setEnableCollisionStatus(false);
                *m_stateWork = 6;
                m_snd.playSE(static_cast<SndID>(0x1BCC), 0, 0, -1);
                m_snd.setPos(&m_posWork[*m_posIndex]);
                m_state = 9;
            }
            break;
        case 9:
            if (getMotionFrame(0) >= m_frameCount) {
                m_state = 0;
            }
            break;
        }
    }
}

void grNorfairDoor::updateCallBack(float deltaFrame) {
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
            Vec3f* posWork = m_posWork;
            if (posWork != NULL) {
                Vec3f* pos = &posWork[*m_posIndex];
                data->m_pos.m_x = pos->m_x;
                data->m_pos.m_y = pos->m_y;
                data->m_pos.m_z = pos->m_z;
            }
            float scaleY = m_scaleY;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = 1.0f;
            data->m_scale.m_y = scaleY;
            data->m_scale.m_z = 1.0f;
        }
    }
}

// The animations of the door are bound with all their parts, the character animation first.
void grNorfairDoor::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

    if (animId >= 3) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grNorfairSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grNorfairSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grNorfairSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grNorfairSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grNorfairSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

// The hit object of the door: two hit spheres, one on each side of it (at x = -20 and x = 20), and an attack module.
void grNorfairDoor::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData) * 2]);
    m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple) * 2]);
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grNorfairSetView);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData[0].m_startOffsetPos.m_x = -20.0f;
    m_hitData[0].m_startOffsetPos.m_y = 2.0f;
    m_hitData[0].m_startOffsetPos.m_z = 0.0f;
    m_hitData[0].m_endOffsetPos.m_x = -20.0f;
    m_hitData[0].m_endOffsetPos.m_y = 22.0f;
    m_hitData[0].m_endOffsetPos.m_z = 0.0f;
    m_hitData[0].m_size = 4.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grNorfairHitByte*>(&m_hitData[0])->m_shape = 1;
    norfairCopyHit(&m_hitSimple[0], &m_hitData[0]);
    m_hitSimple[0].m_height = soCollisionHitData::Height_Low;
    m_hitSimple[0].m_nodeIndex = m_nodeIndex;

    m_hitData[1].m_startOffsetPos.m_x = 20.0f;
    m_hitData[1].m_startOffsetPos.m_y = 2.0f;
    m_hitData[1].m_startOffsetPos.m_z = 0.0f;
    m_hitData[1].m_endOffsetPos.m_x = 20.0f;
    m_hitData[1].m_endOffsetPos.m_y = 22.0f;
    m_hitData[1].m_endOffsetPos.m_z = 0.0f;
    m_hitData[1].m_size = 4.0f;
    reinterpret_cast<grNorfairHitByte*>(&m_hitData[1])->m_shape = 1;
    norfairCopyHit(&m_hitSimple[1], &m_hitData[1]);
    m_hitSimple[1].m_height = soCollisionHitData::Height_Low;
    m_hitSimple[1].m_nodeIndex = m_nodeIndex;

    // MATCH-ONLY: the members of soSet are private
    grNorfairSetView* set = reinterpret_cast<grNorfairSetView*>(m_hitSet);
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
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 2, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}
