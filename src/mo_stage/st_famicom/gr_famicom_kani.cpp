#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <gf/gf_model.h>
#include <it/it_manager.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_famicom/gr_famicom.h>
#include <st_famicom/gr_famicom_anim.h>

// BaseItem::fall (sora_melee, unnamed)
extern "C" void fn_27_28E48C(BaseItem* item, Vec3f* speed, float lr);

// HYPOTHESIS: two static objects of the original (the constants {0xFF, 0} and {0xFF, 1}; nothing refers to them in this
// file, they only make the static initializer of the file).
struct grFamicomKaniMarker {
    int m_a;
    int m_b;
    grFamicomKaniMarker(int a, int b) : m_a(a), m_b(b) { }
};
static grFamicomKaniMarker sMarkerA(0xFF, 0);
static grFamicomKaniMarker sMarkerB(0xFF, 1);

// The animation of the enemy depends on how angry it is (its level, 1 - 3): the three animations of a kind are 4 apart.
#define FAMICOM_LEVEL_MOTION(a, b, c, loop)                                \
    switch (m_level) {                                                     \
    case 1:                                                                \
        setMotion(a, loop, false, &m_actionTimer);                         \
        break;                                                             \
    case 2:                                                                \
        setMotion(b, loop, false, &m_actionTimer);                         \
        break;                                                             \
    case 3:                                                                \
        setMotion(c, loop, false, &m_actionTimer);                         \
        break;                                                             \
    }

grFamicomKani* grFamicomKani::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grFamicomKani* ground = new (Heaps::StageInstance) grFamicomKani(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grFamicomKani::~grFamicomKani() {
}

void grFamicomKani::updateMotion(float deltaFrame) {
    grFamicomEnemy::updateMotion(deltaFrame);
    if (getStageData() != NULL) {
        m_motionRatio = 1.0f;
        switch (m_motion) {
        case 1:
        case 5:
        case 9:
            requestDown2();
            break;
        case 3:
        case 7:
        case 11:
            if (m_actionTimer == 0.0f) {
                stFamicomEnemyData* enemy = &m_enemyData[m_index];
                if (enemy->m_dir == 1) {
                    enemy->m_dir = 0;
                    enemy = &m_enemyData[m_index];
                    enemy->m_speed.m_x = 0.0f;
                    enemy->m_speed.m_y = 180.0f;
                    enemy->m_speed.m_z = 0.0f;
                } else {
                    enemy->m_dir = 1;
                    enemy = &m_enemyData[m_index];
                    enemy->m_speed.m_x = 0.0f;
                    enemy->m_speed.m_y = 0.0f;
                    enemy->m_speed.m_z = 0.0f;
                }
                requestReMove();
                m_attackTimer = 30.0f;
            }
            break;
        case 13:
        case 14:
        case 15:
            if (m_actionTimer == 0.0f) {
                m_level = m_level + 1;
                if (m_level > 3) {
                    m_level = 3;
                }
                requestReMove();
            }
            break;
        }
    }
}

// The sidestepper bites with a cutting attack (power 13 / 15 / 17 for its levels).
void grFamicomKani::setAttack() {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = 0.0f;
        offset.m_z = 0.0f;
        u32 power;
        switch (m_level) {
        case 2:
            power = 15;
            break;
        case 1:
            power = 13;
            break;
        case 3:
            power = 17;
            break;
        default:
            power = 0;
            break;
        }
        setAttackGimmickDetails(&attack, 4.6f, 1.0f, 1.0f, 1.0f,
            power, &offset, 0, 100, 0, 30, 0,
            0x2F7, 7, false, 15,
            soCollisionAttackData::Attribute_Cutup, soCollisionAttackData::Sound_Level_Large, soCollisionAttackData::Sound_Attribute_Cutup,
            false, false, false, true, false, false, 10, 120,
            false, false, false, soCollisionAttackData::Lr_Check_Speed,
            false, false, false, false, false, soCollisionAttackData::Region_Body, false);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackSet = 1;
    }
}

// The enemy that was flipped over becomes an item (a sidestepper shell) that the fighters can pick up and throw; it gets the
// speed of the hit that made it.
void grFamicomKani::toItem() {
    if (m_itemId == 0) {
        itManager* items = itManager::getInstance();
        if (items != NULL) {
            BaseItem* item = items->createItem(static_cast<itKind>(0x4B), m_level - 1, -1, NULL, 0, 0xFFFF, 0, 0xFFFF);
            if (item != NULL) {
                Vec3f* enemyPos = &m_enemyData[m_index].m_pos;
                Vec3f pos;
                pos.m_x = enemyPos->m_x;
                pos.m_y = enemyPos->m_y + 15.0f;
                pos.m_z = enemyPos->m_z;
                item->warp(&pos);
                Vec3f speed;
                speed.m_x = 0.0f;
                speed.m_y = 1.5f;
                speed.m_z = 0.0f;
                float lr;
                if (m_hitDir == -1.0f) {
                    speed.m_x = m_speed;
                    lr = 1.0f;
                } else {
                    lr = 1.0f;
                    if (m_hitDir == 1.0f) {
                        speed.m_x = -m_speed;
                        lr = -1.0f;
                    } else if (m_enemyData[m_index].m_speed.m_y == 180.0f) {
                        lr = -1.0f;
                    }
                }
                fn_27_28E48C(item, &speed, lr);
                m_itemId = item->m_instanceId;
                setVisibility(0);
            }
        }
    }
}

void grFamicomKani::toGround() {
    if (m_itemId != 0) {
        itManager* items = itManager::getInstance();
        if (items != NULL) {
            BaseItem* item = items->getItemFromInstanceId(m_itemId);
            if (item != NULL) {
                items->removeItem(item);
                m_itemId = 0;
                setVisibility(1);
                m_state = 0xC;
            }
        }
    }
}

// The sidestepper turns angry when it is hit once from below: it gets faster (the angry animations 0 / 4 / 8).
void grFamicomKani::requestAngry() {
    stFamicomData* data = static_cast<stFamicomData*>(getStageData());
    if (data != NULL) {
        FAMICOM_LEVEL_MOTION(0, 4, 8, false)
        m_angry = 1;
        disableHit(0, 0);
        disableAttack(0);
        m_attackSet = 0;
        m_fallSpeed = -1.75f;
        m_moving = 2;
        stFamicomEnemyData* enemy = &m_enemyData[m_index];
        float side = enemy->m_hitDir;
        if (0.0f <= side && side <= 1.0f) {
            if (enemy->m_dir != 1 || 0.5f <= side) {
                if (enemy->m_dir == 0 && 0.6f < enemy->m_hitDir) {
                    m_moving = 1;
                }
            } else {
                m_moving = 1;
            }
        }
        playSEDown();
        m_state = 0xB;
    }
}

void grFamicomKani::requestDown() {
    if (m_angry == 0) {
        requestAngry();
    } else {
        stFamicomData* data = static_cast<stFamicomData*>(getStageData());
        if (data != NULL) {
            FAMICOM_LEVEL_MOTION(1, 5, 9, false)
            m_actionTimer = data->unk38;
            m_timer = data->unk38;
            disableHit(0, 0);
            disableAttack(0);
            m_attackSet = 0;
            m_moving = 0;
            float side = m_enemyData[m_index].m_hitDir;
            if (0.0f <= side && side <= 1.0f) {
                if (0.5f <= side) {
                    if (side <= 0.6f) {
                        m_hitDir = 500.0f;
                    } else {
                        m_hitDir = 1.0f;
                    }
                } else {
                    m_hitDir = -1.0f;
                }
            }
            playSEDown();
            m_fixPosition = 1;
            m_enemyData[m_index].m_state = 1;
            m_state = 0xC;
        }
    }
}

// The flipped enemy gets up again (it is removed if it was left on the floor with an item).
void grFamicomKani::requestDown2() {
    if (m_fixPosition != 1) {
        if (isExistItem() == 0) {
            m_state = 0x11;
        } else if (isRemoveItem() != 0 && isTurnGround() != 0) {
            m_angry = 0;
            FAMICOM_LEVEL_MOTION(14, 15, 15, false)
            m_actionTimer = 60.0f;
            toGround();
            m_state = 0xC;
        }
    }
}

void grFamicomKani::requestMove() {
    stFamicomData* data = static_cast<stFamicomData*>(getStageData());
    if (data == NULL) {
        return;
    }
    switch (m_level) {
    case 2:
        m_speed = data->unk30 * data->unk40;
        break;
    case 1:
        m_speed = data->unk30;
        break;
    case 3:
        m_speed = data->unk30 * data->unk44;
        break;
    default:
        m_speed = 0.0f;
        break;
    }
    m_fallSpeed = data->unk34;
    if (randf() > 0.5f) {
        Vec3f* src = &m_posCtrlWork[0];
        Vec3f* dst = &m_enemyData[m_index].m_pos;
        dst->m_x = src->m_x;
        dst->m_y = src->m_y;
        dst->m_z = src->m_z;
        m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y - 6.9f;
        m_enemyData[m_index].m_pos.m_z = m_enemyData[m_index].m_pos.m_z - 15.0f;
        m_enemyData[m_index].m_dir = 1;
        m_enemyData[m_index].m_speed.m_x = 0.0f;
        m_enemyData[m_index].m_speed.m_y = 0.0f;
        m_enemyData[m_index].m_speed.m_z = 0.0f;
    } else {
        Vec3f* src = &m_posCtrlWork[1];
        Vec3f* dst = &m_enemyData[m_index].m_pos;
        dst->m_x = src->m_x;
        dst->m_y = src->m_y;
        dst->m_z = src->m_z;
        m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y - 6.9f;
        m_enemyData[m_index].m_pos.m_z = m_enemyData[m_index].m_pos.m_z - 15.0f;
        m_enemyData[m_index].m_dir = 0;
        m_enemyData[m_index].m_speed.m_x = 0.0f;
        m_enemyData[m_index].m_speed.m_y = 180.0f;
        m_enemyData[m_index].m_speed.m_z = 0.0f;
 
    }
    FAMICOM_LEVEL_MOTION(2, 6, 10, true)
    m_firstStep = 1;
    m_state = 4;
}

void grFamicomKani::requestReMove() {
    stFamicomData* data = static_cast<stFamicomData*>(getStageData());
    if (data == NULL) {
        return;
    }
    switch (m_level) {
    case 2:
        m_speed = data->unk30 * data->unk40;
        break;
    case 1:
        m_speed = data->unk30;
        break;
    case 3:
        m_speed = data->unk30 * data->unk44;
        break;
    default:
        m_speed = 0.0f;
        break;
    }
    m_fallSpeed = data->unk34;
    if (m_angry == 0) {
        FAMICOM_LEVEL_MOTION(2, 6, 10, true)
    } else {
        FAMICOM_LEVEL_MOTION(0, 4, 8, true)
    }
    m_state = 4;
    m_attackTimer = 0.0f;
}

void grFamicomKani::requestReMoveAttack() {
    if (m_fixPosition != 1 && getStageData() != NULL) {
        if (isExistItem() == 0) {
            m_state = 0x11;
        } else {
            FAMICOM_LEVEL_MOTION(2, 6, 10, true)
            m_angry = 0;
            disableHit(0, 0);
            disableAttack(0);
            m_attackSet = 0;
            toGround();
            m_fallSpeed = -1.75f;
            m_firstStep = 0;
            stFamicomEnemyData* enemy = &m_enemyData[m_index];
            float side = enemy->m_hitDir;
            if (0.0f <= side && side <= 1.0f) {
                if (0.5f <= side) {
                    if (0.6f < side) {
                        enemy->m_dir = 0;
                        enemy = &m_enemyData[m_index];
                        enemy->m_speed.m_x = 0.0f;
                        enemy->m_speed.m_y = 180.0f;
                        enemy->m_speed.m_z = 0.0f;
                        m_firstStep = 1;
                    }
                } else {
                    enemy->m_dir = 1;
                    enemy = &m_enemyData[m_index];
                    enemy->m_speed.m_x = 0.0f;
                    enemy->m_speed.m_y = 0.0f;
                    enemy->m_speed.m_z = 0.0f;
                    m_firstStep = 1;
                }
            }
            m_state = 4;
        }
    }
}

void grFamicomKani::requestTurn() {
    disableHit(0, 0);
    disableAttack(0);
    m_attackSet = 0;
    FAMICOM_LEVEL_MOTION(3, 7, 11, true)
    m_enemyData[m_index].m_state = 2;
    m_enemyData[m_index].m_timer = 60.0f;
    m_state = 10;
}

void grFamicomKani::playSEAppear() {
    m_snd.playSE(static_cast<SndID>(0x1CE6), 0, 0, -1);
}

void grFamicomKani::playSEDown() {
    m_snd.playSE(static_cast<SndID>(0x1CE7), 0, 0, -1);
}

// The animations 13 - 15 (the sidestepper getting up) use the character and visibility animations 1, 5 and 9 (the first
// one of the file for each of the three levels); every other kind uses its own index.
void grFamicomKani::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

    u32 index = animId;
    if (index == 14) {
        index = 5;
    } else if (index < 14) {
        if (index > 12) {
            index = 1;
        }
    } else if (index < 16) {
        index = 9;
    }

    if (index < 0xC) {
        bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > index);
        if (result) {
            grFamicomSetChrAnim2(index, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > index);
        if (result) {
            grFamicomSetVisibilityAnim2(index, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > index);
        if (result) {
            grFamicomSetTexPatAnim2(index, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > index);
        if (result) {
            grFamicomSetTexSrtAnim2(index, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > index);
        if (result) {
            grFamicomSetColorAnim2(index, model, modelAnim, Heaps::StageInstance);
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
