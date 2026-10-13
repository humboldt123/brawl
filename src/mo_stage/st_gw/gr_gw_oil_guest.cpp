#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <types.h>
#include <yk/yk_no_hit_normal.h>
#include <yk/yk_normal.h>

#include <st_gw/gr_gw_anim.h>
#include <st_gw/gr_gw_item.h>
#include <st_gw/gr_gw_oil_guest.h>

static grGWMarker sMarkerA(0xFF, 0);
static grGWMarker sMarkerB(0xFF, 1);

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

// MATCH-ONLY: the members of soSet are private
// MATCH-ONLY: a word based view of the bit fields of soCollisionAttackData (from +0x30). The shared header declares its
// single-bit flags as bool, which the compiler accesses one byte at a time; the original sets them with one
// read-modify-write per word.
struct grGWAttackBits {
    u32 m_nodeIndex : 9;
    u32 m_targetCategory : 10;
    u32 m_targetSituation : 3;
    u32 m_targetLr : 1;
    u32 m_targetPart : 4;
    u32 m_attribute : 5;
    u32 m_soundLevel : 2;
    u32 m_soundAttribute : 5;
    u32 m_setOffKind : 2;
    u32 m_noScale : 1;
    u32 m_isShieldable : 1;
    u32 m_isReflectable : 1;
    u32 m_isAbsorbable : 1;
    u32 m_subShield : 9;
    u32 _34 : 10;
    u32 m_serialHitFrame : 16;
    u32 m_isDirect : 1;
    u32 m_isInvalidInvincible : 1;
    u32 m_isInvalidXlu : 1;
    u32 m_lrCheck : 3;
    u32 m_isCatch : 1;
    u32 m_noTeam : 1;
    u32 m_noHitStop : 1;
    u32 m_noEffect : 1;
    u32 m_noTransaction : 1;
    u32 m_region : 5;
    u32 m_shapeType : 1;
    u32 m_isDeath100 : 1;
    u32 _3C : 30;
};

struct grGWSetView {
    void* m_elements;
    u32 m_size;
};

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grGWHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// MATCH-ONLY: the copy of a hit sphere to its simple form (word by word, as the original does it)
static inline void gwCopyHit(soCollisionHitData::Simple* dst, soCollisionHitData* src) {
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

grGWOilGuest::grGWOilGuest(const char* taskName) : grGW(taskName), m_snd() {
    m_dataGuest = NULL;
    m_sceneWork = NULL;
    m_sceneNo = 4;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_type = 0x10;
    m_played = 0;
    m_yakumonoMade = 0;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grGWOilGuest* grGWOilGuest::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWOilGuest* ground = new (Heaps::StageInstance) grGWOilGuest(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWOilGuest::~grGWOilGuest() {
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

void grGWOilGuest::update(float deltaFrame) {
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateScaleBase(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made when the guest is first updated; after that it follows the place of the guest.
void grGWOilGuest::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade == 1) {
        Vec3f pos;
        getNodePosition(&pos, 0, static_cast<u32>(0));
        setPos(&pos);
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
        }
    }
}

// The guest is shown while its scene plays, walks to the place of the guest data (the index changes every few frames) and is
// hidden while the oil spills on it (the guest data says it is angry).
void grGWOilGuest::updateActive(float deltaFrame) {
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
            disableHit(0, 0);
            m_dataGuest->unk60 = 0;
            m_state = 1;
            break;
        case 1:
            if (*m_sceneWork == m_sceneNo) {
                setMotion(0, true, true, &m_frameCount);
                setMotionFrame(m_frameCount, 0);
                m_frameLast = m_frameCount;
                if (m_motionRatio > 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_played = 0;
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
                m_state = 4;
            } else {
                if (m_dataGuest->unk60 == 1) {
                    if (m_type == 0) {
                        setVisibility(0);
                    }
                    disableHit(0, 0);
                    m_state = 5;
                } else {
                    if (!m_isVisible) {
                        setVisibility(1);
                    }
                    if (m_played == 0) {
                        if (getMotionFrame(0) > m_frameLast && m_motion == 0) {
                            setMotion(1, false, false, NULL);
                            m_played = 1;
                            enableHit(0, 0);
                        } else {
                            m_frameLast = getMotionFrame(0);
                        }
                    }
                    if (m_timer == 0.0f) {
                        float r = randf();
                        if (r < 0.25f) {
                            m_dataGuest->unk61 = 0;
                        } else if (r < 0.5f) {
                            m_dataGuest->unk61 = 1;
                        } else if (r < 0.75f) {
                            m_dataGuest->unk61 = 2;
                        } else {
                            m_dataGuest->unk61 = 3;
                        }
                        float r2 = randf();
                        m_timer = data[24] + (data[25] - data[24]) * r2;
                    }
                }
            }
            break;
        case 4:
            if (getMotionFrame(0) >= m_frameCount) {
                m_state = 0;
            }
            break;
        case 5:
            if (*m_sceneWork != m_sceneNo) {
                setMotion(0, false, true, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                disableHit(0, 0);
                m_state = 4;
            } else {
                if (m_dataGuest->unk60 == 0) {
                    setVisibility(1);
                    enableHit(0, 0);
                    m_state = 3;
                } else if (m_type == 2) {
                    if (m_dataGuest->unk61 < 4) {
                        setVisibility(1);
                    } else {
                        setVisibility(0);
                    }
                }
            }
            break;
        }
        m_pos = m_dataGuest->unk0[m_dataGuest->unk61];
    }
}

void grGWOilGuest::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = m_pos.m_x;
            data->m_pos.m_y = m_pos.m_y;
            data->m_pos.m_z = m_pos.m_z;
            Vec3f scale;
            gwVec3Scale(&scale, &m_scale, 0.9f);
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}

// The hit object of the guest: one capsule (from 20 units over the guest down to the guest, 10 wide) that fighters hit.
void grGWOilGuest::setHit() {
    m_hitData = new (Heaps::StageInstance) soCollisionHitData;
    m_hitSimple = new (Heaps::StageInstance) soCollisionHitData::Simple;
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grGWSetView);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = 0.0f;
    m_hitData->m_startOffsetPos.m_y = 20.0f;
    m_hitData->m_startOffsetPos.m_z = 0.0f;
    m_hitData->m_endOffsetPos.m_x = 0.0f;
    m_hitData->m_endOffsetPos.m_y = 0.0f;
    m_hitData->m_endOffsetPos.m_z = 0.0f;
    m_hitData->m_size = 10.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grGWHitByte*>(m_hitData)->m_shape = 1;
    gwCopyHit(m_hitSimple, m_hitData);
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = 0;
    // MATCH-ONLY: the members of soSet are private
    grGWSetView* set = reinterpret_cast<grGWSetView*>(m_hitSet);
    set->m_elements = m_hitSimple;
    set->m_size = 1;
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
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}

// The guest was hit (the oil spilled on it): it turns angry (the guest data says so) and makes a sound.
void grGWOilGuest::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    fn_27_26399C(m_yakumono);
    if (m_dataGuest != NULL) {
        if (m_dataGuest->unk60 != 1) {
            m_snd.playSE(static_cast<SndID>(0x1C94), 0, 0, -1);
            m_snd.setPos(&m_pos);
        }
        m_dataGuest->unk60 = 1;
    }
}

// ----- the angry guest -----

grGWOilGuestAnger::~grGWOilGuestAnger() {
    if (m_data != NULL) {
        delete m_data;
    }
    m_data = NULL;
}

// The angry guest has no hit spheres; its hit object only carries the attack.
void grGWOilGuestAnger::setHit() {
    m_data = new (Heaps::StageInstance) ykData;
    m_data->m_dataGroupNum = 0;
    m_data->m_dataGroups = NULL;
    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_data;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
}

// ----- the angry man -----

inline grGWOilGuestAngerM::grGWOilGuestAngerM(const char* taskName) : grGWOilGuestAnger(taskName) {
    m_attackOn = 0;
    m_dangerZone = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->clearAll();
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
}

grGWOilGuestAngerM* grGWOilGuestAngerM::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWOilGuestAngerM* ground = new (Heaps::StageInstance) grGWOilGuestAngerM(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWOilGuestAngerM::~grGWOilGuestAngerM() {
}

// The angry man comes to the place of the guest data; the AI keeps away from the place where he is.
void grGWOilGuestAngerM::updateActive(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(2, false, true, NULL);
            setVisibility(0);
            disableAttack(0);
            m_attackOn = 0;
            m_state = 1;
            break;
        case 1:
            if (*m_sceneWork == m_sceneNo) {
                setMotion(1, true, true, &m_frameCount);
                setMotionFrame(m_frameCount, 0);
                m_frameLast = m_frameCount;
                if (m_motionRatio > 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_state = 3;
            }
            break;
        case 3:
            if (*m_sceneWork != m_sceneNo) {
                setMotion(1, false, true, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                disableAttack(0);
                m_attackOn = 0;
                m_state = 4;
            } else {
                if (m_dataGuest->unk60 == 1) {
                    setMotion(0, true, true, &m_frameCount);
                    setVisibility(1);
                    setAttack();
                    m_timer = data[27];
                    m_state = 5;
                } else {
                    if (getMotionFrame(0) > m_frameLast && m_motion == 1) {
                        setMotion(2, false, false, NULL);
                    } else {
                        m_frameLast = getMotionFrame(0);
                    }
                }
            }
            break;
        case 4:
            if (getMotionFrameMatCol() >= m_frameCount) {
                if (m_dangerZone != -1) {
                    g_aiMgr->delDangerZone(m_dangerZone);
                    m_dangerZone = -1;
                }
                m_state = 0;
            }
            break;
        case 5:
            if (*m_sceneWork != m_sceneNo) {
                setMotion(1, false, true, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                disableAttack(0);
                m_attackOn = 0;
                m_state = 4;
            } else {
                if (m_dataGuest->unk60 == 0) {
                    setMotion(2, false, false, NULL);
                    setVisibility(0);
                    disableAttack(0);
                    m_attackOn = 0;
                    if (m_dataGuest->unk61 > 4) {
                        m_dataGuest->unk61 -= 4;
                    }
                    if (m_dangerZone != -1) {
                        g_aiMgr->delDangerZone(m_dangerZone);
                        m_dangerZone = -1;
                    }
                    m_state = 3;
                } else if (m_timer == 0.0f) {
                    m_dataGuest->unk60 = 0;
                } else {
                    Vec3f* pos = &m_dataGuest->unk0[m_dataGuest->unk61];
                    Vec2f min;
                    Vec2f max;
                    min.m_x = pos->m_x - 20.0f;
                    max.m_y = pos->m_y - 30.0f;
                    max.m_x = pos->m_x + 20.0f;
                    min.m_y = pos->m_y + 30.0f;
                    m_dangerZone = g_aiMgr->setDangerZone(&min, &max, m_dangerZone, false, false);
                }
            }
            break;
        }
        m_pos = m_dataGuest->unk0[m_dataGuest->unk61];
    }
}

void grGWOilGuestAngerM::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = m_pos.m_x;
            data->m_pos.m_y = m_pos.m_y;
            data->m_pos.m_z = m_pos.m_z;
            Vec3f scale;
            gwVec3Scale(&scale, &m_scale, 0.9f);
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}

// The animation is split in parts: 0 is the angry one (only the visibility), 1 the rest (visibility and color), 2 hides.
void grGWOilGuestAngerM::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    u32 prev = m_motion;
    if (prev == animId && force == 0) {
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
    m_motion = animId;
    if (animId == 2) {
        modelAnim->unbindVisibleAnim(sceneMdl);
        modelAnim->unbindMatColAnim(sceneMdl);
    } else {
        if (prev == 1) {
            if (animId == 0) {
                modelAnim->unbindMatColAnim(sceneMdl);
            }
        } else if (prev == 0) {
            if (animId == 1) {
                bool result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > 0);
                if (result) {
                    grGWSetColorAnim2(0, model, modelAnim, Heaps::StageInstance);
                }
            }
        } else if (animId == 1) {
            bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > 0);
            if (result) {
                grGWSetVisibilityAnim2(0, model, modelAnim, Heaps::StageInstance);
            }
            result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > 0);
            if (result) {
                grGWSetColorAnim2(0, model, modelAnim, Heaps::StageInstance);
            }
        } else if (animId == 0) {
            bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > 0);
            if (result) {
                grGWSetVisibilityAnim2(0, model, modelAnim, Heaps::StageInstance);
            }
        }
        gfModelAnimation::bind(sceneMdl, modelAnim);
        if (prev == 2) {
            modelAnim->setFrame(0.0);
        }
        modelAnim->setUpdateRate(1.0);
        modelAnim->setLoop(loop);
        if (frameCount != NULL) {
            *frameCount = modelAnim->getFrameCount();
        }
    }
}

// The angry man hurts with a fist: 13 percent, from 18 units over the guest, 10 wide, knocking fighters away.
void grGWOilGuestAngerM::setAttack() {
    if (m_attackOn != 1) {
        soCollisionAttackData attack;
        grGWAttackBits* bits = reinterpret_cast<grGWAttackBits*>(reinterpret_cast<u8*>(&attack) + 0x30);
        attack.m_reactionEffect = 70;
        attack.m_reactionFix = 0;
        attack.m_reactionAdd = 60;
        attack.m_power = 13;
        attack.m_vector = 70;
        bits->m_nodeIndex = 0;
        bits->m_attribute = soCollisionAttackData::Attribute_Normal;
        bits->m_targetSituation = 7;
        attack.m_size = 10.0f;
        bits->m_targetCategory = 0x3FF;
        bits->m_targetLr = 0;
        bits->m_targetPart = 0xF;
        attack.m_offsetPos.m_x = 0.0f;
        attack.m_offsetPos.m_y = 18.0f;
        attack.m_offsetPos.m_z = 0.0f;
        bits->m_setOffKind = 0;
        bits->m_noScale = 0;
        bits->m_soundLevel = soCollisionAttackData::Sound_Level_Large;
        bits->m_soundAttribute = soCollisionAttackData::Sound_Attribute_Punch;
        bits->m_isShieldable = 1;
        bits->m_isReflectable = 0;
        bits->m_isAbsorbable = 0;
        bits->m_serialHitFrame = 60;
        bits->m_isDirect = 0;
        bits->m_isInvalidInvincible = 0;
        bits->m_isInvalidXlu = 0;
        bits->m_lrCheck = soCollisionAttackData::Lr_Check_Pos;
        bits->m_isCatch = 0;
        bits->m_noTeam = 0;
        bits->m_noHitStop = 0;
        bits->m_noEffect = 0;
        bits->m_noTransaction = 0;
        bits->m_shapeType = soCollision::Shape_Capsule;
        m_yakumono->setAttack(0, 0, &attack);
        m_attackOn = 1;
    }
}

// The frame of the color animation (0 when the ground has none).
float grGWOilGuestAngerM::getMotionFrameMatCol() {
    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return 0.0f;
    }
    return modelAnim->m_anmObjMatClrRes->GetFrame();
}

// ----- the angry woman -----

inline grGWOilGuestAngerW::grGWOilGuestAngerW(const char* taskName) : grGWOilGuestAnger(taskName) {
    m_attackOn = 0;
    m_dangerZone = -1;
    m_timerTurn = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->clearAll();
    callback->m_numNodeCallbackData = 2;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
    callback->m_nodeCallbackDatas[1].m_flags |= 1;
}

grGWOilGuestAngerW* grGWOilGuestAngerW::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWOilGuestAngerW* ground = new (Heaps::StageInstance) grGWOilGuestAngerW(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWOilGuestAngerW::~grGWOilGuestAngerW() {
}

// The angry woman walks round the four places of the guest data (the index goes up by 4 and wraps) and the AI keeps away.
void grGWOilGuestAngerW::updateActive(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        m_timerTurn -= deltaFrame;
        if (m_timerTurn < 0.0f) {
            m_timerTurn = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, false, true, NULL);
            setVisibility(0);
            disableAttack(0);
            m_attackOn = 0;
            m_state = 1;
            break;
        case 1:
            if (*m_sceneWork == m_sceneNo) {
                setMotion(0, true, true, &m_frameCount);
                setMotionFrame(m_frameCount, 0);
                m_frameLast = m_frameCount;
                if (m_motionRatio > 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_state = 3;
            }
            break;
        case 3:
            if (*m_sceneWork != m_sceneNo) {
                setMotion(0, false, true, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_state = 4;
            } else {
                if (m_dataGuest->unk60 == 1) {
                    m_timer = data[27];
                    m_timerTurn = 120.0f;
                    m_state = 5;
                } else {
                    if (getMotionFrame(0) > m_frameLast && m_motion == 0) {
                        setMotion(1, false, false, NULL);
                    } else {
                        m_frameLast = getMotionFrame(0);
                    }
                }
            }
            break;
        case 4:
            if (getMotionFrame(0) >= m_frameCount) {
                if (m_dangerZone != -1) {
                    g_aiMgr->delDangerZone(m_dangerZone);
                    m_dangerZone = -1;
                }
                m_state = 0;
            }
            break;
        case 5:
            if (*m_sceneWork != m_sceneNo) {
                setMotion(0, false, true, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_state = 4;
            } else {
                if (m_dataGuest->unk60 == 0) {
                    setVisibility(0);
                    disableAttack(0);
                    m_attackOn = 0;
                    if (m_dataGuest->unk61 > 4) {
                        m_dataGuest->unk61 -= 4;
                    }
                    if (m_dangerZone != -1) {
                        g_aiMgr->delDangerZone(m_dangerZone);
                        m_dangerZone = -1;
                    }
                    m_state = 3;
                } else if (m_timer == 0.0f) {
                    m_dataGuest->unk60 = 0;
                } else {
                    if (m_timerTurn == 0.0f) {
                        m_dataGuest->unk61 += 4;
                        if (m_dataGuest->unk61 > 7) {
                            m_dataGuest->unk61 -= 8;
                        }
                        if (m_dataGuest->unk61 < 4) {
                            setVisibility(0);
                            disableAttack(0);
                            m_attackOn = 0;
                        } else {
                            setVisibility(1);
                            setAttack();
                        }
                        m_timerTurn = 120.0f;
                    }
                    Vec3f* pos = &m_dataGuest->unk0[m_dataGuest->unk61];
                    Vec2f min;
                    Vec2f max;
                    min.m_x = pos->m_x - 20.0f;
                    max.m_y = pos->m_y - 30.0f;
                    max.m_x = pos->m_x + 20.0f;
                    min.m_y = pos->m_y + 30.0f;
                    m_dangerZone = g_aiMgr->setDangerZone(&min, &max, m_dangerZone, false, false);
                }
            }
            break;
        }
        m_pos = m_dataGuest->unk0[m_dataGuest->unk61];
    }
}

// The model of the woman is placed at the calm place (the angry places are the same models 4 further); the second node (the
// angry one) follows the place of the guest data.
void grGWOilGuestAngerW::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = 0;
                calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            u32 index = m_dataGuest->unk61;
            if (index > 3) {
                index -= 4;
            }
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            Vec3f* pos = &m_dataGuest->unk0[index];
            data->m_pos.m_x = pos->m_x;
            float y = pos->m_y;
            data->m_pos.m_y = y;
            data->m_pos.m_z = pos->m_z;
            Vec3f scale;
            gwVec3Scale(&scale, &m_scale, 0.9f);
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
            float posY = m_pos.m_y;
            data = &calcWorldCallBack->m_nodeCallbackDatas[1];
            data->m_pos.m_x = 0.0f;
            data->m_pos.m_y = posY - y;
            data->m_pos.m_z = m_pos.m_z;
        }
    }
}

// The angry woman hurts with a pan: 13 percent, from 38 units over the guest, 10 wide, knocking fighters away.
void grGWOilGuestAngerW::setAttack() {
    if (m_attackOn != 1) {
        soCollisionAttackData attack;
        grGWAttackBits* bits = reinterpret_cast<grGWAttackBits*>(reinterpret_cast<u8*>(&attack) + 0x30);
        attack.m_reactionEffect = 70;
        attack.m_reactionFix = 0;
        attack.m_reactionAdd = 60;
        attack.m_power = 13;
        attack.m_vector = 70;
        bits->m_nodeIndex = 0;
        bits->m_attribute = soCollisionAttackData::Attribute_Normal;
        bits->m_targetSituation = 7;
        attack.m_size = 10.0f;
        bits->m_targetCategory = 0x3FF;
        bits->m_targetLr = 0;
        bits->m_targetPart = 0xF;
        attack.m_offsetPos.m_x = 0.0f;
        attack.m_offsetPos.m_y = 38.0f;
        attack.m_offsetPos.m_z = 0.0f;
        bits->m_setOffKind = 0;
        bits->m_noScale = 0;
        bits->m_soundLevel = soCollisionAttackData::Sound_Level_Large;
        bits->m_soundAttribute = soCollisionAttackData::Sound_Attribute_Punch;
        bits->m_isShieldable = 1;
        bits->m_isReflectable = 0;
        bits->m_isAbsorbable = 0;
        bits->m_serialHitFrame = 60;
        bits->m_isDirect = 0;
        bits->m_isInvalidInvincible = 0;
        bits->m_isInvalidXlu = 0;
        bits->m_lrCheck = soCollisionAttackData::Lr_Check_Pos;
        bits->m_isCatch = 0;
        bits->m_noTeam = 0;
        bits->m_noHitStop = 0;
        bits->m_noEffect = 0;
        bits->m_noTransaction = 0;
        bits->m_shapeType = soCollision::Shape_Capsule;
        m_yakumono->setAttack(0, 0, &attack);
        m_attackOn = 1;
    }
}
