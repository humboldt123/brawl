#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <cm/cm_quake.h>
#include <ec/ec_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <snd/snd_system.h>
#include <types.h>
#include <so/damage/so_damage.h>
#include <yk/yk_normal.h>

#include <st_mansion/gr_mansion_pillar.h>
#include <st_mansion/gr_mansion_util.h>

// MATCH-ONLY: the copy of a hit sphere to its simple form (word by word, as the original does it)
static inline void mansionCopyHit(soCollisionHitData::Simple* dst, soCollisionHitData* src) {
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

grMansionPillar* grMansionPillar::create(int modelIndex, const char* nodeName, const char* taskName) {
    grMansionPillar* ground = new (Heaps::StageInstance) grMansionPillar(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(modelIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grMansionPillar::grMansionPillar(const char* taskName) : grMansion(taskName), unk1A8() {
    unk158 = 0;
    unk15C = 0.0f;
    unk160 = 0;
    unk164 = NULL;
    unk168 = NULL;
    unk16C = NULL;
    unk170 = 0.0f;
    unk174 = 0.0f;
    unk178 = 0.0f;
    unk17C = 0.0f;
    unk180 = 0.875f;
    unk184 = 0;
    unk188 = 0.0f;
    unk18C = 1;
    unk190 = 0.0f;
    unk194 = NULL;
    unk198 = NULL;
    unk19C = NULL;
    unk1A0 = NULL;
    unk1A4 = NULL;
    unk1B0 = 0;
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(0x15);
    if (g_GameGlobal->m_modeMelee == NULL) {
        return;
    }
    if (mansionMeleeBytes()->m_mode != MANSION_MODE_EVENT) {
        return;
    }
    if (mansionMeleeBytes()->m_event == MANSION_EVENT_ID) {
        unk1B0 = 1;
    }
}

grMansionPillar::~grMansionPillar() {
    if (unk194 != NULL) {
        delete unk194;
    }
    unk194 = NULL;
    if (unk198 != NULL) {
        delete unk198;
    }
    unk198 = NULL;
    if (unk19C != NULL) {
        delete unk19C;
    }
    unk19C = NULL;
    if (unk1A0 != NULL) {
        delete unk1A0;
    }
    unk1A0 = NULL;
    if (unk1A4 != NULL) {
        delete unk1A4;
    }
    unk1A4 = NULL;
}

void grMansionPillar::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    setNode();
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateShake(deltaFrame);
        updateDestroy(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made when the pillar is first updated (the stage data is there); after that it follows the place of the
// pillar.
void grMansionPillar::updateYakumono(float deltaFrame) {
    if (unk164 != NULL && getStageData() != NULL) {
        if (unk158 == 1) {
            setPos(unk164->m_x, unk164->m_y, unk164->m_z);
        } else {
            setHit();
            if (m_yakumono != NULL) {
                setPos(unk164->m_x, unk164->m_y, unk164->m_z);
                disableHit(0, 0);
                setHP();
                unk158 = 1;
            }
        }
    }
}

// MATCH-ONLY: the angle is multiplied at run time (degrees to radians)
static inline float mansionDegToRad(float deg) {
    return deg * 0.017453292f;
}

// While the pillar shakes (after a hit) the model is moved a bit every third frame, in a direction that is drawn at random.
void grMansionPillar::updateShake(float deltaFrame) {
    unk170 -= deltaFrame;
    if (unk170 <= 0.0f) {
        unk170 = 0.0f;
    }
    if (unk170 > 0.0f) {
        u32 frame = static_cast<u32>(unk170);
        if (static_cast<float>(frame % 3) == 0.0f) {
            float sin;
            float cos;
            float deg = 90.0f;
            float toRad = 0.017453292f;
            mtSinCosf(toRad * deg, &sin, &cos);
            float amount = 0.5f + randf() * 0.8f;
            unk174 = amount * cos;
            unk178 = amount * sin;
            unk17C = 0.0f;
        }
    } else {
        unk174 = 0.0f;
        unk178 = 0.0f;
        unk17C = 0.0f;
    }
}

// The pillar waits to be broken (the hit points are gone), falls (with a crash) and comes back later.
void grMansionPillar::updateDestroy(float deltaFrame) {
    unk154 -= deltaFrame;
    if (unk154 < 0.0f) {
        unk154 = 0.0f;
    }
    unk188 -= deltaFrame;
    if (unk188 < 0.0f) {
        unk188 = 0.0f;
    }
    switch (unk150) {
    case 0:
        setMotion(0, false, true, &unk190);
        setVisibility(1);
        setHP();
        unk150 = 1;
        break;
    case 1:
        setMotionFrame(0.0f, 0);
        enableHit(0, 0);
        unk150 = 2;
        break;
    case 2:
        setMotionFrame(0.0f, 0);
        if (*unk168 == 1) {
            disableHit(0, 0);
            u32 effect = g_ecMgr->setEffect(ef_ptc_stg_mansion_pillar);
            g_ecMgr->setParent(effect, m_sceneModels[0], "StgManshon00pillar01", true);
            Vec3f offset(0.0f, 0.0f, 0.0f);
            cmReqQuake(cmQuake::Amplitude_XL, &offset);
            float random = randf();
            if (random < 0.25f) {
                unk1A8.playSE(snd_se_stage_Mansion_08, 0, 0, -1);
            } else if (random < 0.5f) {
                unk1A8.playSE(snd_se_stage_Mansion_09, 0, 0, -1);
            } else if (random < 0.75f) {
                unk1A8.playSE(snd_se_stage_Mansion_10, 0, 0, -1);
            } else {
                unk1A8.playSE(snd_se_stage_Mansion_11, 0, 0, -1);
            }
            unk150 = 4;
        }
        break;
    case 4:
        unk190 -= deltaFrame;
        if (unk190 < 0.0f) {
            unk190 = 0.0f;
        }
        if (unk190 == 0.0f) {
            setVisibility(0);
            unk150 = 5;
        }
        break;
    case 5:
        if (*unk168 == 4) {
            setMotion(0, false, true, &unk190);
            setVisibility(1);
            unk150 = 6;
        }
        break;
    case 6:
        setMotionFrame(0.0f, 0);
        if (*unk168 == 0) {
            unk150 = 0;
        }
        break;
    }
}

// The node of the pillar is moved by the shake and scaled by how far the collapse is.
void grMansionPillar::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            calcWorldCallBack->m_index = 0;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
            scnMdl->m_calcWorldCallBack = calcWorldCallBack;
            scnMdl->EnableScnMdlCallbackTiming(1);
            scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
        }
        Vec3f pos = getPos();
        grNodeCallbackData* posData = calcWorldCallBack->m_nodeCallbackDatas;
        posData->m_pos.m_x = pos.m_x;
        posData->m_pos.m_y = pos.m_y;
        posData->m_pos.m_z = pos.m_z;
        grNodeCallbackData* shakeData = calcWorldCallBack->m_nodeCallbackDatas;
        shakeData->m_offsetPos.m_x = unk174;
        shakeData->m_offsetPos.m_y = unk178;
        shakeData->m_offsetPos.m_z = unk17C;
        float rate;
        if (unk184 == 1) {
            rate = 1.0f - *unk16C;
        } else {
            rate = *unk16C;
        }
        float scale = unk180;
        float clamped = mansionClamp(rate, 0.001f, 1.0f);
        grNodeCallbackData* scaleData = calcWorldCallBack->m_nodeCallbackDatas;
        scaleData->m_scale.m_x = scale;
        scaleData->m_scale.m_y = scale * clamped;
        scaleData->m_scale.m_z = scale;
        Vec3f soundPos2 = getPos();
        Vec3f soundPos(soundPos2.m_x, soundPos2.m_y, soundPos2.m_z);
        unk1A8.setPos(&soundPos);
    }
}

// The hit object of the pillar: a capsule (from 2.5 over the foot of the pillar to 32.5, 5 wide) that fighters and attacks
// hit; it has no attack of its own.
void grMansionPillar::setHit() {
    unk194 = new (Heaps::StageInstance) soCollisionHitData;
    unk198 = new (Heaps::StageInstance) soCollisionHitData::Simple;
    unk19C = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grMansionSetView);
    unk1A0 = new (Heaps::StageInstance) ykDataGroup;
    unk1A4 = new (Heaps::StageInstance) ykData;

    unk194->m_startOffsetPos.m_x = 0.0f;
    unk194->m_startOffsetPos.m_y = 2.5f;
    unk194->m_startOffsetPos.m_z = -unk164->m_z;
    unk194->m_endOffsetPos.m_x = 0.0f;
    unk194->m_endOffsetPos.m_y = 32.5f;
    unk194->m_endOffsetPos.m_z = -unk164->m_z;
    unk194->m_size = 5.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grMansionHitByte*>(unk194)->m_shape = 1;
    mansionCopyHit(unk198, unk194);
    unk198->m_height = soCollisionHitData::Height_Low;
    unk198->m_nodeIndex = m_nodeIndex;
    // MATCH-ONLY: the members of soSet are private
    grMansionSetView* set = reinterpret_cast<grMansionSetView*>(unk19C);
    set->m_elements = unk198;
    set->m_size = 1;
    unk1A0->m_hitDataSimpleSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(unk19C);
    unk1A0->m_hitGroupIndex = 0;
    unk1A4->m_dataGroups = unk1A0;
    unk1A4->m_dataGroupNum = 1;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = unk1A4;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}

// The hit points of the pillar: the stage data says how many a pillar of that kind has; in the event mode the number follows
// the level of the event.
void grMansionPillar::setHP() {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        switch (unk160) {
        case 0:
            unk15C = data[8];
            break;
        case 1:
            unk15C = data[9];
            break;
        }
        if (unk1B0 == 1 && g_GameGlobal->m_modeMelee != NULL && mansionMeleeBytes()->m_mode == MANSION_MODE_EVENT) {
            switch (mansionMeleeBytes()->m_rule) {
            case 0:
                unk15C *= data[24];
                break;
            case 1:
                unk15C *= data[25];
                break;
            case 2:
                unk15C *= data[26];
                break;
            }
        }
    }
}

void grMansionPillar::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    MANSION_SET_MOTION(unk18C, animId, loop, force, frameCount)
}

// A hit shakes the pillar and takes the damage off its hit points; the pillar breaks when they are gone.
void grMansionPillar::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    // HYPOTHESIS: the second float of the damage is what a hit takes off the pillar
    fn_27_26399C(m_yakumono);
    float power = damage->unk4;
    float random = randf();
    if (random < 0.25f) {
        unk1A8.playSE(snd_se_stage_Mansion_post_dmg_a, 0, 0, -1);
    } else if (random < 0.5f) {
        unk1A8.playSE(snd_se_stage_Mansion_post_dmg_b, 0, 0, -1);
    } else if (random < 0.75f) {
        unk1A8.playSE(snd_se_stage_Mansion_post_dmg_c, 0, 0, -1);
    } else {
        unk1A8.playSE(snd_se_stage_Mansion_post_dmg_d, 0, 0, -1);
    }
    switch (unk160) {
    case 0:
        if (unk168[-1] != 2) {
            if (unk188 == 0.0f) {
                u32 effect = g_ecMgr->setEffect(ef_ptc_stg_mansion_pillar_hit);
                g_ecMgr->setParent(effect, m_sceneModels[0], "StgManshon00pillar01", true);
            }
            break;
        }
    default:
        if (unk188 == 0.0f) {
            u32 effect = g_ecMgr->setEffect(ef_ptc_stg_mansion_pillar_damage);
            g_ecMgr->setParent(effect, m_sceneModels[0], "StgManshon00pillar01", true);
            unk188 = randf() * 5.0f + 10.0f;
        }
        unk15C -= power;
        if (unk15C < 0.0f) {
            unk15C = 0.0f;
        }
        if (unk15C == 0.0f) {
            *unk168 = 1;
        } else {
            unk170 = randf() * 2.5f + 2.5f;
        }
        break;
    }
}
