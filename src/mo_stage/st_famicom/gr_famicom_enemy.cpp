#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gr/gr_calc_world_callback.h>
#include <it/it_manager.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <so/so_external_value_accesser.h>
#include <snd/snd_system.h>
#include <st/st_utility.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_famicom/gr_famicom.h>

// HYPOTHESIS: BaseItem members of sora_melee that have no names yet
extern "C" int fn_27_28CCF0(BaseItem* item, u32 param);    // BaseItem::getParamInt
extern "C" BaseItem* fn_27_2A8A00(itManager* manager, int index); // itManager::getSafeItemFromIndex
extern "C" bool fn_27_28DC8C(BaseItem* item);               // BaseItem::isPickUp
extern "C" Stage* lbl_27_bss_5668;

// MATCH-ONLY: the members of soSet are private
struct grFamicomSetView {
    void* m_elements;
    u32 m_size;
};

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grFamicomHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// MATCH-ONLY: the copy of a hit sphere to its simple form (word by word, as the original does it)
static inline void famicomCopyHit(soCollisionHitData::Simple* dst, soCollisionHitData* src) {
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

// MATCH-ONLY: views of parts of the item and of the item manager that the enemy reads (nothing names them: the holder
// of the item (at 0x1a4 / 0x14) tells how the item is carried, 2 means it is held by a fighter; the list of the manager
// (at 0x6d8) tells how many items there are).
class famicomItemHolder {
public:
    virtual void unk8();
    virtual void unkC();
    virtual void unk10();
    virtual int getCarryKind();
};
struct famicomItemHolderBox {
    u8 _pad[0x14];
    famicomItemHolder* m_holder;
};
struct famicomItemView {
    u8 _pad[0x1A4];
    famicomItemHolderBox* m_box;
};
class famicomItemList {
public:
    virtual void unk8();
    virtual void unkC();
    virtual void unk10();
    virtual int getCount();
};
struct famicomItemManagerView {
    u8 _pad[0x6D8];
    famicomItemList* m_list;
};

// HYPOTHESIS: the clamp helper that the other stages use as well.
static inline float famicomClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

// HYPOTHESIS: two static objects of the original (the constants {0xFF, 0} and {0xFF, 1}; nothing refers to them in this
// file, they only make the static initializer of the file).
struct grFamicomEnemyMarker {
    int m_a;
    int m_b;
    grFamicomEnemyMarker(int a, int b) : m_a(a), m_b(b) { }
};
static grFamicomEnemyMarker sMarkerA(0xFF, 0);
static grFamicomEnemyMarker sMarkerB(0xFF, 1);

grFamicomEnemy::grFamicomEnemy(const char* taskName) : grFamicom(taskName), m_snd() {
    m_motion = 0;
    m_actionTimer = 0.0f;
    m_attackTimer = 0.0f;
    m_speed = 0.0f;
    m_fallSpeed = 0.0f;
    m_firstStep = 0;
    m_moving = 1;
    m_turnState = 0;
    m_hitDir = 0.0f;
    m_level = 1;
    m_index = 0;
    m_landCount = 0;
    m_enemyData = NULL;
    m_posCtrlWork = NULL;
    m_posLimitWork = NULL;
    m_tossData = NULL;
    m_hasHit = 0;
    m_attackSet = 0;
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
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
    m_itemId = 0;
    m_unk1ac = 0;
    m_fixPosition = 0;
    m_dangerZone = -1;
    m_scale.m_x = 0.0f;
    m_scale.m_y = 0.0f;
    m_scale.m_z = 0.0f;
}

grFamicomEnemy::~grFamicomEnemy() {
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

// The position is set by the stage after the collision (when the enemy was made an item on the floor).
void grFamicomEnemy::processFixPosition() {
    if (m_fixPosition == 1) {
        toItem();
        m_fixPosition = 0;
    }
}

void grFamicomEnemy::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateMove(deltaFrame);
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
        m_snd.setPos(&m_enemyData[m_index].m_pos);
    }
}

// The hit object is made the first time this is called; after that the ground follows the record of the enemy.
void grFamicomEnemy::updateYakumono(float deltaFrame) {
    if (m_hasHit == 1) {
        setPos(&m_enemyData[m_index].m_pos);
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
            nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
            if (scnMdl != NULL) {
                nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
                if (model.IsValid()) {
                    nw4r::g3d::ResNode node = model.GetResNode(m_nodeIndex);
                    if (node.IsValid()) {
                        m_scale.m_x = node.ref().m_scale.m_x;
                        m_scale.m_y = node.ref().m_scale.m_y;
                        m_scale.m_z = node.ref().m_scale.m_z;
                    }
                }
            }
        }
    }
}

// The state machine of the enemy. The state of the record is the one the stage and the floors see (1 = waits, 3 = off the
// stage, 4 = comes in, 5 = comes out of the pipe, 7 = blown away, 8 = about to go into the pipe, 0 = walks, 0xD = free).
void grFamicomEnemy::updateMove(float deltaFrame) {
    stFamicomData* data = static_cast<stFamicomData*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_timer = m_timer - deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    m_enemyData[m_index].m_timer = m_enemyData[m_index].m_timer - deltaFrame;
    if (m_enemyData[m_index].m_timer < 0.0f) {
        m_enemyData[m_index].m_timer = 0.0f;
    }
    if (m_dangerZone != -1) {
        g_aiMgr->delDangerZone(m_dangerZone);
        m_dangerZone = -1;
    }
    switch (m_level) {
    case 2:
        m_speed = data->unk40;
        break;
    case 0:
    case 1:
        m_speed = 1.0f;
        break;
    case 3:
        m_speed = data->unk44;
        break;
    default:
        m_speed = 1.0f;
        break;
    }
    m_enemyData[m_index].m_grounded = 0;
    int landed = 0;
    Vec3f hitPos;
    switch (m_state) {
    case 6:
    case 10:
    case 11:
    case 12:
    case 14:
        if (0.0f < m_fallSpeed) {
            Vec3f start;
            start.m_x = m_enemyData[m_index].m_pos.m_x;
            start.m_y = m_enemyData[m_index].m_pos.m_y + 10.5f;
            start.m_z = m_enemyData[m_index].m_pos.m_z;
            Vec3f dir;
            dir.m_x = 0.0f;
            dir.m_y = -11.0f;
            dir.m_z = 0.0f;
            hitPos.m_x = 0.0f;
            hitPos.m_y = 0.0f;
            hitPos.m_z = 0.0f;
            Vec3f hitNormal;
            hitNormal.m_x = 0.0f;
            hitNormal.m_y = 0.0f;
            hitNormal.m_z = 0.0f;
            bool ray = stRayCheck(&start, &dir, &hitPos, &hitNormal, false, NULL, false, 1);
            if (ray != 1 || hitNormal.m_y <= 0.0f || hitPos.m_y <= m_enemyData[m_index].m_pos.m_y - m_fallSpeed) {
                landed = 0;
                m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y - m_fallSpeed * deltaFrame;
                m_landCount = 0;
            } else {
                if (m_landCount > 2) {
                    m_enemyData[m_index].m_grounded = 1;
                }
                m_landCount = m_landCount + 1;
                if (m_landCount > 5) {
                    m_landCount = 5;
                }
                u8 i = 0;
                for (int n = 7; n != 0; n--, i++) {
                    stFamicomYukaToss* toss = &m_tossData[i];
                    if (toss->m_state == 0 && toss->m_x - 10.0f < hitPos.m_x && hitPos.m_x < toss->m_x + 10.0f &&
                        toss->m_y - 10.0f < hitPos.m_y && hitPos.m_y < toss->m_y + 10.0f) {
                        float rate = famicomClamp((m_enemyData[m_index].m_pos.m_x - (hitPos.m_x - 10.0f)) / 20.0f, 0.0f, 1.0f);
                        m_enemyData[m_index].m_hitDir = 1.0f - rate;
                        break;
                    }
                }
                landed = 1;
            }
        }
        break;
    }
    switch (m_state) {
    case 0:
        setVisibility(0);
        disableHit(0, 0);
        requestMove();
        m_state = 1;
        break;
    case 1:
        if (m_timer == 0.0f && m_enemyData[m_index].m_state != 0xD && isAppear() != 0) {
            m_enemyData[m_index].m_state = 5;
            playSEAppear();
            m_state = 4;
        }
        break;
    case 4:
        if (m_isVisible) {
            setVisibility(1);
        }
        if (m_firstStep == 1) {
            m_firstStep = 0;
            m_timer = 60.0f;
        } else {
            m_timer = 0.0f;
        }
        m_state = 5;
        break;
    case 5:
        if (m_timer == 0.0f) {
            enableHit(0, 0);
            m_enemyData[m_index].m_pos.m_z = 0.0f;
            m_enemyData[m_index].m_state = 0;
            m_state = 6;
        } else {
            float rate = famicomClamp(1.0f - m_timer / 60.0f, 0.0f, 1.0f);
            if (m_enemyData[m_index].m_dir == 1) {
                m_enemyData[m_index].m_pos.m_x = m_posCtrlWork[0].m_x + rate * 20.0f;
            } else {
                m_enemyData[m_index].m_pos.m_x = m_posCtrlWork[1].m_x - rate * 20.0f;
            }
            if (m_timer <= 30.0f) {
                float rate2 = famicomClamp(1.0f - m_timer / 30.0f, 0.0f, 1.0f);
                float angle = nw4r::math::CosFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::S16ToF32((s16)(int)(rate2 * 16384.0f))));
                if (m_enemyData[m_index].m_dir == 1) {
                    m_enemyData[m_index].m_pos.m_z = m_posCtrlWork[0].m_z - angle * 15.0f;
                } else {
                    m_enemyData[m_index].m_pos.m_z = m_posCtrlWork[1].m_z - angle * 15.0f;
                }
            }
        }
        break;
    case 6:
        m_attackTimer = m_attackTimer - deltaFrame;
        if (m_attackTimer < 0.0f) {
            m_attackTimer = 0.0f;
        }
        if (m_attackTimer == 0.0f && m_attackSet == 0) {
            setAttack();
        }
        if (m_enemyData[m_index].m_dir == 1) {
            if (m_moving == 1) {
                m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x + m_speed * deltaFrame;
            }
            float x = m_enemyData[m_index].m_pos.m_x;
            if (x <= m_posLimitWork[1].m_x) {
                if (m_posCtrlWork[3].m_x - 21.0f <= x && x <= m_posCtrlWork[3].m_x + 21.0f && isBottomLine() == 1) {
                    disableHit(0, 0);
                    disableAttack(0);
                    m_attackSet = 0;
                    m_enemyData[m_index].m_state = 4;
                    m_timer = 60.0f;
                    m_state = 8;
                }
            } else {
                setVisibility(0);
                disableHit(0, 0);
                disableAttack(0);
                m_attackSet = 0;
                m_enemyData[m_index].m_state = 3;
                m_state = 7;
            }
        } else {
            if (m_moving == 1) {
                m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x - m_speed * deltaFrame;
            }
            float x = m_enemyData[m_index].m_pos.m_x;
            if (m_posLimitWork[0].m_x <= x) {
                if (x <= m_posCtrlWork[2].m_x + 21.0f && m_posCtrlWork[2].m_x - 21.0f <= x && isBottomLine() == 1) {
                    disableHit(0, 0);
                    disableAttack(0);
                    m_attackSet = 0;
                    m_enemyData[m_index].m_state = 4;
                    m_timer = 60.0f;
                    m_state = 8;
                }
            } else {
                setVisibility(0);
                disableHit(0, 0);
                disableAttack(0);
                m_attackSet = 0;
                m_enemyData[m_index].m_state = 3;
                m_state = 7;
            }
        }
        if (m_fallSpeed <= 0.0f) {
            m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y - m_fallSpeed * deltaFrame;
            m_fallSpeed = m_fallSpeed + (data->unk34 * 0.75f) * deltaFrame;
        } else {
            if (landed == 1) {
                m_enemyData[m_index].m_pos.m_y = hitPos.m_y;
                m_fallSpeed = data->unk34;
                if (m_enemyData[m_index].m_grounded == 1) {
                    if (m_enemyData[m_index].m_hitDir == -1.0f) {
                        if (m_enemyData[m_index].m_state == 8) {
                            m_state = 0xD;
                        }
                    } else {
                        m_state = 0xD;
                    }
                }
                m_moving = 1;
            }
            m_fallSpeed = m_fallSpeed + data->unk34 * deltaFrame;
        }
        if (m_landCount > 2 && isTurn() == 1) {
            requestTurn();
        }
        {
            Vec2f zoneA;
            Vec2f zoneB;
            zoneA.m_x = m_enemyData[m_index].m_pos.m_x - 25.0f;
            zoneA.m_y = m_enemyData[m_index].m_pos.m_y + 30.0f;
            zoneB.m_x = m_enemyData[m_index].m_pos.m_x + 25.0f;
            zoneB.m_y = m_enemyData[m_index].m_pos.m_y - 20.0f;
            m_dangerZone = g_aiMgr->setDangerZone(&zoneA, &zoneB, m_dangerZone, false, false);
        }
        break;
    case 7:
        if (m_enemyData[m_index].m_dir == 1) {
            m_enemyData[m_index].m_pos.m_x = m_posLimitWork[0].m_x;
        } else {
            m_enemyData[m_index].m_pos.m_x = m_posLimitWork[1].m_x;
        }
        m_state = 4;
        m_timer = 0.0f;
        break;
    case 8:
        if (m_timer == 0.0f) {
            setVisibility(0);
            m_state = 9;
        } else {
            float rate = famicomClamp(m_timer / 60.0f, 0.0f, 1.0f);
            if (m_enemyData[m_index].m_dir == 1) {
                m_enemyData[m_index].m_pos.m_x = m_posCtrlWork[3].m_x - rate * 21.0f;
            } else {
                m_enemyData[m_index].m_pos.m_x = m_posCtrlWork[2].m_x + rate * 21.0f;
            }
            float rate2 = famicomClamp(1.0f - (m_timer - 30.0f) / 30.0f, 0.0f, 1.0f);
            if (m_enemyData[m_index].m_dir == 1) {
                m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y + rate2 * ((m_posCtrlWork[3].m_y - m_enemyData[m_index].m_pos.m_y) - 6.9f);
            } else {
                m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y + rate2 * ((m_posCtrlWork[2].m_y - m_enemyData[m_index].m_pos.m_y) - 6.9f);
            }
            float rate3 = famicomClamp(1.0f - (m_timer - 30.0f) / 30.0f, 0.0f, 1.0f);
            float angle = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::S16ToF32((s16)(int)(rate3 * 16384.0f))));
            if (m_enemyData[m_index].m_dir == 1) {
                m_enemyData[m_index].m_pos.m_z = m_posCtrlWork[3].m_z - angle * 15.0f;
            } else {
                m_enemyData[m_index].m_pos.m_z = m_posCtrlWork[2].m_z - angle * 15.0f;
            }
        }
        break;
    case 9: {
        stFamicomEnemyData* enemy = &m_enemyData[m_index];
        if (enemy->m_dir == 1) {
            enemy->m_pos.m_x = m_posCtrlWork[1].m_x;
            enemy->m_pos.m_y = m_posCtrlWork[1].m_y;
            enemy->m_pos.m_z = m_posCtrlWork[1].m_z;
            m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y - 6.9f;
            m_enemyData[m_index].m_pos.m_z = m_enemyData[m_index].m_pos.m_z - 15.0f;
            m_enemyData[m_index].m_dir = 0;
            m_enemyData[m_index].m_speed.m_x = 0.0f;
            m_enemyData[m_index].m_speed.m_y = 180.0f;
            m_enemyData[m_index].m_speed.m_z = 0.0f;
        } else {
            enemy->m_pos.m_x = m_posCtrlWork[0].m_x;
            enemy->m_pos.m_y = m_posCtrlWork[0].m_y;
            enemy->m_pos.m_z = m_posCtrlWork[0].m_z;
            m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y - 6.9f;
            m_enemyData[m_index].m_pos.m_z = m_enemyData[m_index].m_pos.m_z - 15.0f;
            m_enemyData[m_index].m_dir = 1;
            m_enemyData[m_index].m_speed.m_x = 0.0f;
            m_enemyData[m_index].m_speed.m_y = 0.0f;
            m_enemyData[m_index].m_speed.m_z = 0.0f;
        }
        setPos(&m_enemyData[m_index].m_pos);
        m_firstStep = 1;
        m_timer = data->unk48;
        m_state = 1;
        break;
    }
    case 10:
        m_speed = 1.0f;
        if (landed == 1 && m_enemyData[m_index].m_grounded == 1) {
            if (m_enemyData[m_index].m_hitDir == -1.0f) {
                if (m_enemyData[m_index].m_state == 8) {
                    m_state = 0xD;
                }
            } else {
                m_state = 0xD;
            }
        }
        break;
    case 11:
    case 12:
        m_speed = 1.0f;
        if (m_itemId == 0) {
            switch (m_turnState) {
            case 1:
                if (m_enemyData[m_index].m_dir == 1) {
                    m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x + (m_speed * deltaFrame) * 0.75f;
                } else {
                    m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x - (m_speed * deltaFrame) * 0.75f;
                }
                break;
            case 0:
                break;
            case 2:
                if (m_enemyData[m_index].m_dir == 1) {
                    m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x - (m_speed * deltaFrame) * 0.75f;
                } else {
                    m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x + (m_speed * deltaFrame) * 0.75f;
                }
                break;
            }
            if (m_enemyData[m_index].m_pos.m_x < m_posLimitWork[0].m_x) {
                m_enemyData[m_index].m_pos.m_x = m_posLimitWork[1].m_x;
            }
            if (m_posLimitWork[1].m_x < m_enemyData[m_index].m_pos.m_x) {
                m_enemyData[m_index].m_pos.m_x = m_posLimitWork[0].m_x;
            }
        }
        {
            float fall = m_fallSpeed;
            if (fall <= 0.0f) {
                if (0.0f <= fall) {
                    if (isExistItem() == 1 && isRemoveItem() == 1) {
                        m_enemyData[m_index].m_grounded = 1;
                        if (m_enemyData[m_index].m_hitDir == -1.0f) {
                            if (m_enemyData[m_index].m_state == 8) {
                                requestReMoveAttack();
                            }
                        } else {
                            requestReMoveAttack();
                            m_enemyData[m_index].m_hitDir = -1.0f;
                        }
                    }
                } else {
                    m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y - fall * deltaFrame;
                    m_landCount = 0;
                }
            } else if (landed == 1) {
                if (m_itemId == 0) {
                    m_enemyData[m_index].m_pos.m_y = hitPos.m_y;
                }
                m_turnState = 0;
                m_fallSpeed = 0.0f;
                if (m_state == 0xB) {
                    requestReMove();
                }
                if (m_enemyData[m_index].m_grounded == 1) {
                    if (m_enemyData[m_index].m_hitDir == -1.0f) {
                        if (m_enemyData[m_index].m_state == 8) {
                            requestReMoveAttack();
                        }
                    } else {
                        requestReMoveAttack();
                        m_enemyData[m_index].m_hitDir = -1.0f;
                    }
                }
            }
        }
        if (m_itemId == 0) {
            m_fallSpeed = m_fallSpeed + (data->unk34 * 0.75f) * deltaFrame;
        }
        break;
    case 13:
        if (g_GameGlobal->isPrevJustGameFrame() == true) {
            requestDown();
            m_enemyData[m_index].m_hitDir = -1.0f;
        }
        break;
    case 14:
        m_speed = 1.0f;
        if (isExistItem() == 1 && isRemoveItem() == 1) {
            m_enemyData[m_index].m_grounded = 1;
            if (m_enemyData[m_index].m_hitDir == -1.0f) {
                if (m_enemyData[m_index].m_state == 8) {
                    requestReMoveAttack();
                }
            } else {
                requestReMoveAttack();
                m_enemyData[m_index].m_hitDir = -1.0f;
            }
        }
        break;
    case 15:
        if (m_turnState == 1) {
            if (m_enemyData[m_index].m_dir == 1) {
                m_fallSpeed = m_speed * deltaFrame;
            } else {
                m_fallSpeed = -(m_speed * deltaFrame);
            }
        } else if (m_turnState == 0 || m_turnState >= 3) {
        } else {
            if (m_enemyData[m_index].m_dir == 1) {
                m_fallSpeed = -(m_speed * deltaFrame);
            } else {
                m_fallSpeed = m_speed * deltaFrame;
            }
        }
        m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x + m_fallSpeed;
        m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y + (m_speed * deltaFrame) * 1.5f;
        {
            Vec3f* limit = m_posLimitWork;
            Vec3f* pos = &m_enemyData[m_index].m_pos;
            if (pos->m_x < limit[0].m_x - 10.0f || limit[1].m_x + 10.0f < pos->m_x || limit[0].m_y + 10.0f < pos->m_y ||
                pos->m_y < limit[1].m_y - 10.0f) {
                m_state = 17;
            }
        }
        break;
    case 16: {
        switch (m_turnState) {
        case 1:
            if (m_enemyData[m_index].m_dir == 1) {
                m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x + (m_speed * deltaFrame) * 0.75f;
            } else {
                m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x - (m_speed * deltaFrame) * 0.75f;
            }
            break;
        case 0:
            break;
        case 2:
            if (m_enemyData[m_index].m_dir == 1) {
                m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x - (m_speed * deltaFrame) * 0.75f;
            } else {
                m_enemyData[m_index].m_pos.m_x = m_enemyData[m_index].m_pos.m_x + (m_speed * deltaFrame) * 0.75f;
            }
            break;
        }
        m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y - m_fallSpeed;
        m_fallSpeed = m_fallSpeed + data->unk34 * 0.75f;
        Vec3f* limit = m_posLimitWork;
        Vec3f* pos = &m_enemyData[m_index].m_pos;
        if ((pos->m_x < limit[0].m_x - 10.0f || limit[1].m_x + 10.0f < pos->m_x || limit[0].m_y + 100.0f < pos->m_y ||
             pos->m_y < limit[1].m_y - 10.0f) &&
            m_fixPosition != 1 && (isExistItem() != 1 || isRemoveItem() != 0)) {
            m_state = 17;
        }
        break;
    }
    case 17:
        m_speed = 1.0f;
        setVisibility(0);
        preExit();
        exit();
        lbl_27_bss_5668->removeGround(this);
        m_enemyData[m_index].m_state = 0xD;
        m_state = 18;
        break;
    }
}

void grFamicomEnemy::updateMotion(float deltaFrame) {
    if (isExistItem() != 1 || isRemoveItem() != 0) {
        m_actionTimer = m_actionTimer - deltaFrame;
        if (m_actionTimer < 0.0f) {
            m_actionTimer = 0.0f;
        }
    }
}

void grFamicomEnemy::updateCallBack(float deltaFrame) {
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
            if (m_enemyData != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                Vec3f* pos = &m_enemyData[m_index].m_pos;
                Vec3f offset;
                offset.m_x = pos->m_x;
                offset.m_y = pos->m_y + 6.9f;
                offset.m_z = pos->m_z;
                data->m_pos.m_x = offset.m_x;
                data->m_pos.m_y = offset.m_y;
                data->m_pos.m_z = offset.m_z;
                data = calcWorldCallBack->m_nodeCallbackDatas;
                Vec3f* speed = &m_enemyData[m_index].m_speed;
                data->m_rot.m_x = speed->m_x;
                data->m_rot.m_y = speed->m_y;
                data->m_rot.m_z = speed->m_z;
                Vec3f scale;
                scale.m_x = m_scale.m_x * 1.15f;
                scale.m_y = m_scale.m_y * 1.15f;
                data = calcWorldCallBack->m_nodeCallbackDatas;
                scale.m_z = m_scale.m_z * 1.15f;
                data->m_scale.m_x = scale.m_x;
                data->m_scale.m_y = scale.m_y;
                data->m_scale.m_z = scale.m_z;
                if (m_itemId != 0) {
                    itManager* items = itManager::getInstance();
                    if (items == NULL) {
                        return;
                    }
                    BaseItem* item = items->getItemFromInstanceId(m_itemId);
                    if (item == NULL) {
                        return;
                    }
                    if (item->isHaved() == true) {
                        m_state = 0xE;
                    }
                    Vec3f itemPos = soExternalValueAccesser::getPos(item);
                    Vec3f* enemyPos = &m_enemyData[m_index].m_pos;
                    enemyPos->m_x = itemPos.m_x;
                    enemyPos->m_y = itemPos.m_y;
                    enemyPos->m_z = itemPos.m_z;
                    m_enemyData[m_index].m_pos.m_y = m_enemyData[m_index].m_pos.m_y - 6.9f;
                }
                if (m_enemyData[m_index].m_dir == 1) {
                    m_yakumono->setLr(1.0f);
                } else {
                    m_yakumono->setLr(-1.0f);
                }
            }
        }
    }
}

// The hit object of the enemy: an attack module (the enemy hurts fighters) and a hit module with a damage module (fighters
// hurt the enemy), both with one part. The hit sphere (size 9.2) is around the record of the enemy.
void grFamicomEnemy::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData)]);
    m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple)]);
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grFamicomSetView);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = 0.0f;
    m_hitData->m_startOffsetPos.m_y = 0.0f;
    m_hitData->m_startOffsetPos.m_z = 0.0f;
    m_hitData->m_endOffsetPos.m_x = 0.0f;
    m_hitData->m_endOffsetPos.m_y = 0.0f;
    m_hitData->m_endOffsetPos.m_z = 0.0f;
    m_hitData->m_size = 9.2f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grFamicomHitByte*>(m_hitData)->m_shape = 1;
    famicomCopyHit(m_hitSimple, m_hitData);
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = 0;
    // MATCH-ONLY: the members of soSet are private
    grFamicomSetView* set = reinterpret_cast<grFamicomSetView*>(m_hitSet);
    set->m_elements = m_hitSimple;
    set->m_size = 1;
    m_dataGroup->m_hitDataSimpleSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(m_hitSet);
    m_dataGroup->m_hitGroupIndex = 0;
    m_data->m_dataGroups = m_dataGroup;
    m_data->m_dataGroupNum = 1;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    info.m_pos = &m_enemyData[m_index].m_pos;
    info.m_work = m_data;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(info.m_pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}

// A fighter hit the enemy from the side it is looking at or from behind: the side the hit came from is remembered, and a
// walking enemy turns around (the rest of the states do nothing).
void grFamicomEnemy::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    m_hitDir = damage->unk7c;
    switch (m_state) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 13:
    case 14:
        return;
    case 6:
        m_state = 0xD;
        break;
    case 0xC:
        if (m_itemId == 0) {
            requestFall();
        }
        break;
    default:
        return;
    }
    if (damage->unk7c == -1.0f) {
        if (m_enemyData[m_index].m_dir == 1) {
            m_turnState = 1;
        } else {
            m_turnState = 2;
        }
    } else if (m_enemyData[m_index].m_dir == 1) {
        m_turnState = 2;
    } else {
        m_turnState = 1;
    }
}

void grFamicomEnemy::onInflict(soCollisionLog* collisionLog, u32 unk2, float power) {
    if (m_enemyData[m_index].m_timer != 0.0f) {
        return;
    }
    requestTurn();
}

void grFamicomEnemy::requestBlowOff() {
    if (getStageData() != NULL) {
        m_actionTimer = 1000.0f;
        disableHit(0, 0);
        disableAttack(0);
        m_attackSet = 0;
        m_turnState = 0;
        m_fallSpeed = m_speed;
        m_enemyData[m_index].m_state = 7;
        m_snd.playSE(static_cast<SndID>(0x1CE8), 0, 0, -1);
        m_state = 0xF;
    }
}

void grFamicomEnemy::requestFall() {
    if (getStageData() != NULL) {
        m_actionTimer = 1000.0f;
        disableHit(0, 0);
        disableAttack(0);
        m_attackSet = 0;
        m_turnState = 0;
        m_fallSpeed = -1.75f;
        m_enemyData[m_index].m_state = 7;
        m_snd.playSE(static_cast<SndID>(0x1CE8), 0, 0, -1);
        m_state = 0x10;
    }
}

// Another enemy that is coming in on the same side blocks the way in.
bool grFamicomEnemy::isAppear() {
    stFamicomData* data = static_cast<stFamicomData*>(getStageData());
    bool result;
    if (data == NULL) {
        result = false;
    } else {
        for (u8 i = 0; i != data->m_enemyMax; i++) {
            if (i != m_index) {
                stFamicomEnemyData* other = &m_enemyData[i];
                if (other->m_state == 5) {
                    if (m_enemyData[m_index].m_dir == 1) {
                        if (other->m_dir == 1) {
                            return false;
                        }
                    } else if (other->m_dir == 0) {
                        return false;
                    }
                }
            }
        }
        result = true;
    }
    return result;
}

bool grFamicomEnemy::isBottomLine() {
    float y = m_enemyData[m_index].m_pos.m_y;
    if (y > -1.0f && y < 1.0f) {
        return true;
    }
    return false;
}

bool grFamicomEnemy::isExistItem() {
    if (m_itemId == 0) {
        return false;
    }
    itManager* items = itManager::getInstance();
    if (items == NULL) {
        return false;
    }
    return items->getItemFromInstanceId(m_itemId) != NULL;
}

bool grFamicomEnemy::isRemoveItem() {
    if (m_itemId == 0) {
        return false;
    }
    itManager* items = itManager::getInstance();
    if (items == NULL) {
        return false;
    }
    BaseItem* item = items->getItemFromInstanceId(m_itemId);
    if (item == NULL) {
        return false;
    }
    if (!fn_27_28DC8C(item)) {
        return false;
    }
    if (reinterpret_cast<famicomItemView*>(item)->m_box->m_holder->getCarryKind() == 2) {
        return false;
    }
    return item->getParamFloat(0x3EE) == 0.0f;
}

bool grFamicomEnemy::isTurnGround() {
    if (m_itemId == 0) {
        return false;
    }
    itManager* items = itManager::getInstance();
    if (items == NULL) {
        return false;
    }
    BaseItem* item = items->getItemFromInstanceId(m_itemId);
    if (item == NULL) {
        return false;
    }
    return fn_27_28CCF0(item, 0x10000009) <= 1;
}

// Another enemy (or an item on the floor) right in front of the enemy makes it turn around.
bool grFamicomEnemy::isTurn() {
    if (m_enemyData[m_index].m_timer == 0.0f) {
        stFamicomData* data = static_cast<stFamicomData*>(getStageData());
        if (data == NULL) {
            return false;
        }
        for (u8 i = 0; i != data->m_enemyMax; i++) {
            if (i != m_index) {
                stFamicomEnemyData* other = &m_enemyData[i];
                stFamicomEnemyData* self = &m_enemyData[m_index];
                if (other->m_pos.m_y == self->m_pos.m_y) {
                    float diff = other->m_pos.m_x - self->m_pos.m_x;
                    if (self->m_dir == 1) {
                        if (0.0f <= diff && diff <= 10.0f) {
                            return true;
                        }
                    } else if (diff <= 0.0f && -10.0f <= diff) {
                        return true;
                    }
                }
            }
        }
        itManager* items = itManager::getInstance();
        if (items == NULL) {
            return false;
        }
        int count = reinterpret_cast<famicomItemManagerView*>(items)->m_list->getCount();
        for (int i = 0; i != count; i++) {
            BaseItem* item = fn_27_2A8A00(items, i);
            if (item != NULL) {
                int held = reinterpret_cast<famicomItemView*>(item)->m_box->m_holder->getCarryKind();
                u32 flags = fn_27_28CCF0(item, 0x10000008);
                if ((flags & 0x4000) == 0 && held != 2) {
                    Vec3f itemPos = soExternalValueAccesser::getPos(item);
                    float top = (itemPos.m_y - item->getParamFloat(0xD5C)) + 10.0f;
                    float bottom = (itemPos.m_y - item->getParamFloat(0xD5C)) - 10.0f;
                    float left = itemPos.m_x - item->getParamFloat(0xD5D);
                    float right = itemPos.m_x + item->getParamFloat(0xD5D);
                    if (__fabs(left - itemPos.m_x) < 10.0f) {
                        left = itemPos.m_x - 10.0f;
                    }
                    if (__fabs(right - itemPos.m_x) < 10.0f) {
                        right = itemPos.m_x + 10.0f;
                    }
                    stFamicomEnemyData* self = &m_enemyData[m_index];
                    float x = self->m_pos.m_x;
                    if (left < x && x < right && self->m_pos.m_y < top && bottom < self->m_pos.m_y) {
                        float diff = itemPos.m_x - x;
                        if (self->m_dir == 1) {
                            if (0.0f <= diff && diff <= 10.0f) {
                                return true;
                            }
                        } else if (diff <= 0.0f && -10.0f <= diff) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
    return false;
}
