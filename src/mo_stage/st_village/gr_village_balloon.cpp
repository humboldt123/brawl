#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <gm/gm_global.h>
#include <gr/gr_calc_world_callback.h>
#include <it/it_manager.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <so/so_external_value_accesser.h>
#include <st/stage.h>
#include <yk/yk_no_hit_normal.h>
#include <yk/yk_normal.h>

#include <st_village/gr_village.h>

// MATCH-ONLY: the members of soSet are private
struct grVillageSetView {
    void* m_elements;
    u32 m_size;
};

// itKind::gmCheckGlobalItemSwitch (main, unnamed): whether the item may appear in the match
extern "C" bool fn_8005136C(int kind);
// HYPOTHESIS: lets the item fall with the given speed (sora_melee, unnamed); the direction is passed in f1
extern "C" void fn_27_28E48C(float lr, StageObject* item, Vec3f* speed);

grVillageBalloon::grVillageBalloon(const char* taskName) : grVillageGuestPathMove(taskName) {
    m_itemId = 0;
    m_balloonMotion = 4;
    m_posBalloon.m_x = 0.0f;
    m_posBalloon.m_y = 0.0f;
    m_posBalloon.m_z = 0.0f;
    m_wait = 0.0f;
    m_nodeItem = 0;
    m_itemMake = 0;
    m_hasHit = 0;
    unk1EA = 0;
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
}

grVillageBalloon* grVillageBalloon::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageBalloon* ground = new (Heaps::StageInstance) grVillageBalloon(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageBalloon::~grVillageBalloon() {
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

// The balloon carries an item (a bob-omb); it is made when the balloon starts to fly and follows the balloon.
void grVillageBalloon::processFixPosition() {
    if (m_itemMake == 1 && g_GameGlobal->isPrevJustGameFrame()) {
        m_itemMake = 0;
        itManager* manager = itManager::getInstance();
        if (manager != NULL) {
            Vec3f pos;
            getNodePosition(&pos, 0, m_nodeItem);
            pos.m_y -= 5.0f;
            pos.m_z += 0.5f;
            BaseItem* item = manager->createItem(static_cast<itKind>(0x40), 5000, -1, NULL, 0, 0xFFFF, 0, 0xFFFF);
            if (item != NULL) {
                item->have(-1, 0, -1);
                item->setVanishMode(false);
                item->warp(&pos);
                m_itemId = item->m_instanceId;
            }
        }
    }
}

// The sound follows the balloon, and so does the item (held a little below the balloon).
void grVillageBalloon::updateYakumono(float deltaFrame) {
    if (m_hasHit == 1) {
        Vec3f pos;
        getNodePosition(&pos, 0, m_nodeIndex);
        m_balloonSnd.setPos(&pos);
        if (m_itemId != 0) {
            itManager* manager = itManager::getInstance();
            if (manager != NULL) {
                BaseItem* item = manager->getItemFromInstanceId(m_itemId);
                if (item != NULL) {
                    getNodePosition(&pos, 0, m_nodeItem);
                    pos.m_y -= 5.0f;
                    pos.m_z += 0.5f;
                    item->warp(&pos);
                }
            }
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
        }
    }
}

// The balloon starts at the beginning of its path, waits, then flies (the motions 0 - 2 are the ways it can wave; 3 is the pop).
void grVillageBalloon::updateMove(float deltaFrame) {
    stVillageData* data = static_cast<stVillageData*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(4, false, true, NULL);
            setVisibility(0);
            disableHit(0, 0);
            if (m_first == 1) {
                m_timer = data->unk18;
                m_first = 0;
            } else {
                m_timer = data->unk1C + (data->unk20 - data->unk1C) * randf();
            }
            m_motionRatio = data->unk28;
            m_wait = data->unk2C;
            if (m_itemId != 0) {
                u32 itemId = m_itemId;
                m_itemId = 0;
                itManager* manager = itManager::getInstance();
                if (manager == NULL) {
                    return;
                }
                BaseItem* item = manager->getItemFromInstanceId(itemId);
                if (item == NULL) {
                    return;
                }
                manager->removeItem(item);
            }
            m_state = 1;
            break;
        case 1:
            if (m_timer == 0.0f) {
                m_state = 3;
            }
            break;
        case 3: {
            if (randf() < data->unk24) {
                if (m_motionRatio > 0.0f) {
                    m_motionRatio *= -1.0f;
                }
            } else if (m_motionRatio < 0.0f) {
                m_motionRatio *= -1.0f;
            }
            float r = randf();
            u8 motion;
            if (r < 0.33333334f) {
                motion = 0;
            } else if (r < 0.6666667f) {
                motion = 1;
            } else {
                motion = 2;
            }
            if (motion != m_balloonMotion) {
                setMotion(motion, true, true, &m_frameCount);
                if (m_motionRatio > 0.0f) {
                    m_frame = 0.0f;
                } else if (m_motionRatio < 0.0f) {
                    m_frame = m_frameCount;
                }
                m_balloonMotion = motion;
                enableHit(0, 0);
                if (g_GameGlobal->m_modeMelee != NULL) {
                    if (g_GameGlobal->m_modeMelee->m_meleeInitData.m_itemFrequency != 0 && fn_8005136C(0x40) == 1) {
                        m_itemMake = 1;
                    }
                    m_state = 4;
                }
            }
            break;
        }
        case 4:
            if (!m_isVisible) {
                setVisibility(1);
            }
            if (m_wait == 0.0f) {
                getNodePosition(&m_posBalloon, 0, m_nodeIndex);
                setMotion(3, false, true, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                disableHit(0, 0);
                m_balloonSnd.playSE(snd_se_stage_Village_baloon, 0, 0, -1);
                if (m_itemId != 0) {
                    u32 itemId = m_itemId;
                    m_itemId = 0;
                    itManager* manager = itManager::getInstance();
                    BaseItem* item = manager->getItemFromInstanceId(itemId);
                    if (item == NULL) {
                        return;
                    }
                    Vec3f speed(0.0f, 0.0f, 0.0f);
                    fn_27_28E48C(soExternalValueAccesser::getLr(item), item, &speed);
                    item->setVanishMode(true);
                }
                m_state = 9;
            } else if (m_motionRatio > 0.0f && getMotionFrame(0) < m_frame) {
                m_state = 0;
            } else if (m_motionRatio < 0.0f && getMotionFrame(0) > m_frame) {
                m_state = 0;
            } else {
                m_frame = getMotionFrame(0);
            }
            break;
        case 9:
            if (getMotionFrame(0) >= m_frameCount) {
                setVisibility(0);
                m_state = 0;
            }
            break;
        }
    }
}

void grVillageBalloon::updateCallBack(float deltaFrame) {
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
            switch (m_state) {
            case 9: {
                calcWorldCallBack->m_numNodeCallbackData = 1;
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = m_posBalloon.m_x;
                data->m_pos.m_y = m_posBalloon.m_y;
                data->m_pos.m_z = m_posBalloon.m_z;
                break;
            }
            default:
                calcWorldCallBack->m_numNodeCallbackData = 0;
                break;
            }
        }
    }
}

bool grVillageBalloon::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeItem, 0, "ItemPoint");
    return result;
}

// The hit object of the balloon: one hit sphere (size 7) on the node of the balloon.
void grVillageBalloon::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData)]);
    m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple)]);
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) u8[sizeof(grVillageSetView)]);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = 0.0f;
    m_hitData->m_startOffsetPos.m_y = 1.5f;
    m_hitData->m_startOffsetPos.m_z = 0.0f;
    m_hitData->m_endOffsetPos.m_x = 0.0f;
    m_hitData->m_endOffsetPos.m_y = 1.5f;
    m_hitData->m_endOffsetPos.m_z = 0.0f;
    m_hitData->m_size = 7.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grVillageHitByte*>(m_hitData)->m_shape = 1;
    u32* srcWords = reinterpret_cast<u32*>(m_hitData);
    u32* dstWords = reinterpret_cast<u32*>(m_hitSimple);
    for (int w = 0; w < 6; w += 3) {
        u32 w0 = srcWords[w];
        u32 w1 = srcWords[w + 1];
        dstWords[w] = w0;
        dstWords[w + 1] = w1;
        dstWords[w + 2] = srcWords[w + 2];
    }
    m_hitSimple->m_size = m_hitData->m_size;
    reinterpret_cast<u8*>(m_hitSimple)[0x1C] = reinterpret_cast<u8*>(m_hitData)[0x1C];
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = m_nodeIndex;
    // MATCH-ONLY: the members of soSet are private
    grVillageSetView* set = reinterpret_cast<grVillageSetView*>(m_hitSet);
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
    typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}

// A fighter hit the balloon: the time it stays is shortened by the damage.
void grVillageBalloon::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    float amount = damage->unk4;
    fn_27_26399C(m_yakumono);
    m_wait -= amount;
    if (m_wait < 0.0f) {
        m_wait = 0.0f;
    }
}

// MATCH-ONLY: two small objects with a constructor (nothing reads them): the static initializer of the original unit.
struct grVillageBalloonDummy {
    int m_a;
    int m_b;
    grVillageBalloonDummy(int a, int b) {
        m_a = a;
        m_b = b;
    }
};
static grVillageBalloonDummy sDummyA(0xFF, 0);
static grVillageBalloonDummy sDummyB(0xFF, 1);
