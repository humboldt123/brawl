#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <it/item.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_earth/gr_earth.h>
#include <st_earth/gr_earth_anim.h>
#include <st_earth/gr_earth_hit.h>

// BaseItem::requestLost (sora_melee, unnamed)
extern "C" void fn_27_28ECF4(BaseItem* item);

// HYPOTHESIS: two small constant objects (an 0xFF marker with an index) that the unit's headers instantiate.
struct grEarthOniyonMarker {
    int m_a;
    int m_b;
    grEarthOniyonMarker(int a, int b) : m_a(a), m_b(b) { }
};
static grEarthOniyonMarker sMarkerA(0xFF, 0);
static grEarthOniyonMarker sMarkerB(0xFF, 1);

grEarthOniyon::grEarthOniyon(const char* taskName) : grEarth(taskName), m_snd() {
    m_color = 3;
    m_itemStep = 0;
    m_itemTimer = 0.0f;
    m_itemKind = 0;
    m_landingLeaf = 0;
    m_fallSpeed = 0.0f;
    m_posOffset.m_x = 0.0f;
    m_posOffset.m_y = 0.0f;
    m_posOffset.m_z = 0.0f;
    m_posLeaf.m_x = 0.0f;
    m_posLeaf.m_y = 0.0f;
    m_posLeaf.m_z = 0.0f;
    m_posBase.m_x = 0.0f;
    m_posBase.m_y = 0.0f;
    m_posBase.m_z = 0.0f;
    m_rot.m_x = 0.0f;
    m_rot.m_y = 90.0f;
    m_rot.m_z = 0.0f;
    m_hp = 0.0f;
    m_motion = 8;
    m_frameCount = 0.0f;
    m_started = 0;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
    m_seHandle = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grEarthOniyon* grEarthOniyon::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthOniyon* ground = new (Heaps::StageInstance) grEarthOniyon(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthOniyon::~grEarthOniyon() {
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

void grEarthOniyon::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made once (when the ground has its model).
void grEarthOniyon::updateYakumono(float deltaFrame) {
    if (m_started != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_started = 1;
        }
    }
}

// The steps of the oniyon: 0 arrives, 1 waits for the stage, 0x10 - 0x12 land (the animation, then it falls onto the leaf),
// 0x13 stays on the leaf, 0x14 / 0x15 take off, 0x16 / 0x17 make the items.
void grEarthOniyon::updateMove(float deltaFrame) {
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    if (data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        if (m_yakumono != NULL) {
            fn_27_2637E8(m_yakumono, 0, 2, 0);
        }
        switch (m_state) {
        case 0:
            m_posOffset.m_x = 0.0f;
            m_posOffset.m_y = 500.0f;
            m_posOffset.m_z = 0.0f;
            m_state = 1;
            setVisibility(0);
            break;
        case 1:
            m_seHandle = -1;
            break;
        case 0x10:
            if (m_motion != 7) {
                setMotion(7, false, &m_frameCount);
            }
            m_posOffset.m_x = 0.0f;
            m_posOffset.m_y = 0.0f;
            m_posOffset.m_z = 0.0f;
            m_posBase.m_x = m_posLeaf.m_x;
            m_posBase.m_y = m_posLeaf.m_y;
            m_posBase.m_z = m_posLeaf.m_z;
            m_state = 0x11;
            setVisibility(1);
            break;
        case 0x11:
            if (getMotionFrame(0) < m_frameCount) {
                if (m_frameCount - 250.0f <= getMotionFrame(0) && m_motion == 7 && m_seHandle < 0) {
                    Vec3f pos;
                    getNodePosition(&pos, m_unk1, 3);
                    m_snd.setPos(&pos);
                    m_seHandle = m_snd.playSE(static_cast<SndID>(0x1CCA), 0, 0, -1);
                }
            } else if (m_motion == 0) {
                m_state = 0x12;
                m_fallSpeed = 0.0f;
            } else {
                setMotion(0, false, &m_frameCount);
                Vec3f pos;
                getNodePosition(&pos, m_unk1, 3);
                m_snd.setPos(&pos);
                m_seHandle = m_snd.playSE(static_cast<SndID>(0x1CCB), 0, 0, -1);
            }
            break;
        case 0x12: {
            float speed = m_fallSpeed - deltaFrame * 0.05f;
            float fall = m_posOffset.m_y + speed;
            m_fallSpeed = speed;
            m_posOffset.m_y = fall;
            if (m_posBase.m_y + fall <= m_posLeaf.m_y) {
                Vec3f pos;
                getNodePosition(&pos, m_unk1, 3);
                m_snd.setPos(&pos);
                m_seHandle = m_snd.playSE(static_cast<SndID>(0x1CCC), 0, 0, -1);
                m_posOffset.m_y = 0.0f;
                m_hp = data->unk64;
                m_state = 0x13;
            }
            break;
        }
        case 0x13:
            if (m_hp == 0.0f) {
                requestTakeOff();
            } else {
                if (m_yakumono != NULL) {
                    fn_27_2637E8(m_yakumono, 0, 0, 0);
                }
                if (m_frameCount <= getMotionFrame(0)) {
                    if (m_motion == 0) {
                        setMotion(1, false, &m_frameCount);
                    } else if (m_motion != 2) {
                        setMotion(2, true, &m_frameCount);
                    }
                }
            }
            break;
        case 0x14: {
            if (m_hp == 0.0f) {
                setMotion(6, false, &m_frameCount);
            } else {
                setMotion(5, false, &m_frameCount);
            }
            Vec3f pos;
            getNodePosition(&pos, m_unk1, 3);
            m_snd.setPos(&pos);
            m_seHandle = m_snd.playSE(static_cast<SndID>(0x1CCD), 0, 0, -1);
            m_posBase.m_x = m_posLeaf.m_x + m_posOffset.m_x;
            m_posBase.m_y = m_posLeaf.m_y + m_posOffset.m_y;
            m_posBase.m_z = m_posLeaf.m_z + m_posOffset.m_z;
            m_posOffset.m_x = 0.0f;
            m_posOffset.m_y = 0.0f;
            m_posOffset.m_z = 0.0f;
            m_state = 0x15;
            break;
        }
        case 0x15:
            if (m_frameCount <= getMotionFrame(0)) {
                m_state = 0;
            }
            break;
        case 0x16:
            setMotion(4, false, &m_frameCount);
            m_state = 0x17;
            // fall through
        case 0x17:
            if (m_hp == 0.0f) {
                requestTakeOff();
            } else {
                m_itemTimer = m_itemTimer - deltaFrame;
                if (m_itemTimer < 0.0f) {
                    m_itemTimer = 0.0f;
                }
                switch (m_itemStep) {
                case 2:
                    if (m_itemTimer <= 0.0f) {
                        makeItem();
                        m_itemTimer = randf() * 2.0f + 3.0f;
                        if (m_itemKind == 2) {
                            m_itemStep = 0xFF;
                        } else {
                            m_itemStep = m_itemStep + 1;
                        }
                    }
                    break;
                case 0:
                    m_itemStep = m_itemStep + 1;
                    m_itemTimer = randf() * 2.0f + 3.0f;
                    break;
                case 1:
                    if (m_itemTimer <= 0.0f) {
                        makeItem();
                        m_itemTimer = randf() * 2.0f + 3.0f;
                        if (m_itemKind == 1) {
                            m_itemStep = 0xFF;
                        } else {
                            m_itemStep = m_itemStep + 1;
                        }
                    }
                    break;
                case 4:
                    if (m_itemTimer <= 0.0f) {
                        makeItem();
                        m_itemTimer = randf() * 2.0f + 3.0f;
                        if (m_itemKind == 4) {
                            m_itemStep = 0xFF;
                        } else {
                            m_itemStep = m_itemStep + 1;
                        }
                    }
                    break;
                case 3:
                    if (m_itemTimer <= 0.0f) {
                        makeItem();
                        m_itemTimer = randf() * 2.0f + 3.0f;
                        if (m_itemKind == 3) {
                            m_itemStep = 0xFF;
                        } else {
                            m_itemStep = m_itemStep + 1;
                        }
                    }
                    break;
                }
                if (m_frameCount <= getMotionFrame(0)) {
                    setMotion(2, true, &m_frameCount);
                    m_state = 0x13;
                }
            }
            break;
        }
        Vec3f pos;
        getNodePosition(&pos, m_unk1, 3);
        m_snd.setPos(&pos);
    }
}

void grEarthOniyon::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f pos;
            if (m_state == 0x17 || m_state == 0x13) {
                pos.m_x = m_posLeaf.m_x + m_posOffset.m_x;
                pos.m_y = m_posLeaf.m_y + m_posOffset.m_y;
                pos.m_z = m_posLeaf.m_z + m_posOffset.m_z;
            } else {
                pos.m_x = m_posBase.m_x + m_posOffset.m_x;
                pos.m_y = m_posBase.m_y + m_posOffset.m_y;
                pos.m_z = m_posBase.m_z + m_posOffset.m_z;
            }
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = pos.m_x;
            data->m_pos.m_y = pos.m_y;
            data->m_pos.m_z = pos.m_z;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_rot.m_x = m_rot.m_x;
            data->m_rot.m_y = m_rot.m_y;
            data->m_rot.m_z = m_rot.m_z;
            getNodePosition(&pos, m_unk1, 3);
            m_snd.setPos(&pos);
        }
    }
}

// The hit object of the oniyon: one hit sphere (size 6) and the damage module (it reports its damage to onDamage).
void grEarthOniyon::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = new (Heaps::StageInstance) soCollisionHitData;
    m_hitSimple = new (Heaps::StageInstance) soCollisionHitData::Simple;
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grEarthSetView);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    float sine;
    float cosine;
    mtSinCosf(-2.0943952f, &sine, &cosine);
    m_hitData->m_startOffsetPos.m_x = 10.0f * sine;
    m_hitData->m_startOffsetPos.m_y = -4.0f;
    m_hitData->m_startOffsetPos.m_z = 10.0f * cosine;
    m_hitData->m_endOffsetPos.m_x = 0.0f;
    m_hitData->m_endOffsetPos.m_y = 0.0f;
    m_hitData->m_endOffsetPos.m_z = 0.0f;
    m_hitData->m_size = 6.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grEarthHitByte*>(m_hitData)->m_shape = 0;
    grEarthCopyHit(m_hitSimple, m_hitData);
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = 3;

    // MATCH-ONLY: the members of soSet are private
    grEarthSetView* set = reinterpret_cast<grEarthSetView*>(m_hitSet);
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

// The oniyon makes the item over the leaf: the item of the kind of the stage, from a lot (the item kind of the oniyon says
// which lot).
void grEarthOniyon::makeItem() {
    grEarthMeleeView* melee = reinterpret_cast<grEarthMeleeView*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 8);
    if (g_GameGlobal->m_modeMelee != NULL && melee->m_itemsOn != 0) {
        itManager* items = itManager::getInstance();
        if (items != NULL) {
            Vec3f pos;
            getNodePosition(&pos, m_unk1, 3);
            int lot;
            switch (m_itemKind) {
            case 3:
                lot = 5;
                break;
            case 1:
                lot = 3;
                break;
            case 0:
                return;
            case 2:
                lot = 4;
                break;
            default:
                if (4 < m_itemKind) {
                    return;
                }
                lot = 6;
                break;
            }
            ItemKind kind;
            kind.m_kind = static_cast<itKind>(*reinterpret_cast<int*>(reinterpret_cast<u8*>(lbl_27_bss_5668) + 0x44));
            kind.m_variation = 0;
            fn_27_2AA7AC(items, kind, lot, &pos, -1, -1, -1);
            m_snd.playSE(static_cast<SndID>(0x1CCE), 0, 0, -1);
        }
    }
}

// The animations of the oniyon are bound with all their parts: the one of the index is looked for in the file, in every kind
// of animation.
void grEarthOniyon::setMotion(u32 animId, bool loop, float* frameCount) {
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
                if (animId < 8) {
                    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
                    if (result) {
                        grEarthSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
                    }
                    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
                    if (result) {
                        grEarthSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
                    }
                    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
                    if (result) {
                        grEarthSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
                    }
                    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
                    if (result) {
                        grEarthSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
                    }
                    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
                    if (result) {
                        grEarthSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
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
        }
    }
}

// The stage says that the oniyon lands on the leaf (0, 1 or 2) when it waits.
void grEarthOniyon::requestLanding(u8 leaf) {
    if (m_state != 1) {
        return;
    }
    m_landingLeaf = leaf;
    m_state = 0x10;
}

void grEarthOniyon::requestTakeOff() {
    if (m_state != 0x13) {
        return;
    }
    m_state = 0x14;
}

// The kind of the items that the oniyon makes depends on its color and on the kind of the pellet that hit it (0 - 8).
void grEarthOniyon::requestItem(int kind) {
    if (m_state != 0x13) {
        return;
    }
    u8 color = m_color;
    m_itemStep = 0;
    if (color == 1) {
        switch (kind) {
        case 0:
            m_itemKind = 1;
            break;
        case 1:
            m_itemKind = 2;
            break;
        case 2:
            m_itemKind = 1;
            break;
        case 3:
            m_itemKind = 2;
            break;
        case 4:
            m_itemKind = 3;
            break;
        case 5:
            m_itemKind = 2;
            break;
        case 6:
            m_itemKind = 3;
            break;
        case 7:
            m_itemKind = 4;
            break;
        case 8:
            m_itemKind = 3;
            break;
        }
    } else if (color == 0) {
        switch (kind) {
        case 0:
            m_itemKind = 2;
            break;
        case 1:
            m_itemKind = 1;
            break;
        case 2:
            m_itemKind = 1;
            break;
        case 3:
            m_itemKind = 3;
            break;
        case 4:
            m_itemKind = 2;
            break;
        case 5:
            m_itemKind = 2;
            break;
        case 6:
            m_itemKind = 4;
            break;
        case 7:
            m_itemKind = 3;
            break;
        case 8:
            m_itemKind = 3;
            break;
        }
    } else if (color < 3) {
        switch (kind) {
        case 0:
            m_itemKind = 1;
            break;
        case 1:
            m_itemKind = 1;
            break;
        case 2:
            m_itemKind = 2;
            break;
        case 3:
            m_itemKind = 2;
            break;
        case 4:
            m_itemKind = 2;
            break;
        case 5:
            m_itemKind = 3;
            break;
        case 6:
            m_itemKind = 3;
            break;
        case 7:
            m_itemKind = 3;
            break;
        case 8:
            m_itemKind = 4;
            break;
        }
    }
    if (m_itemKind == 0) {
        return;
    }
    if (4 < m_itemKind) {
        return;
    }
    m_state = 0x16;
}

bool grEarthOniyon::isPreLanding() {
    return m_state == 0x11 || m_state == 0x12;
}

bool grEarthOniyon::isLanding() {
    return m_state == 0x13;
}

bool grEarthOniyon::isTakeOff() {
    return m_state == 1;
}

// A pellet that hits the oniyon takes some of its life: the oniyon leaves when none is left, and an item (a pellet of the
// stage) that hits it makes the items.
void grEarthOniyon::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    float amount = damage->unk4;
    fn_27_26399C(m_yakumono);
    m_hp = m_hp - amount;
    if (m_hp < 0.0f) {
        m_hp = 0.0f;
    }
    if (m_hp == 0.0f) {
        m_seHandle = m_snd.playSE(static_cast<SndID>(0x1CCF), 0, 0, -1);
        return;
    }
    if (attackerInfo->m_directSoKind == StageObject_Item) {
        BaseItem* item = static_cast<BaseItem*>(gfTask::getTask(attackerInfo->m_directTaskId));
        if (item == NULL) {
            return;
        }
        if (item->m_kind == Item_Stage_Pellet) {
            requestItem(item->m_variation & 0xFF);
            fn_27_28ECF4(item);
            return;
        }
    }
    setMotion(3, false, &m_frameCount);
    m_seHandle = m_snd.playSE(static_cast<SndID>(0x1CCF), 0, 0, -1);
}

// The color of the oniyon is chosen by the weights of the stage (the one that was shown last is less likely) and the colors
// of the model are changed to it.
u8 grEarthOniyon::selectColor(int lastColor) {
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    u8 color;
    if (data == NULL) {
        color = 0;
    } else {
        float weight0 = data->unk54;
        float weight1 = data->unk58;
        float weight2 = data->unk5C;
        switch (lastColor) {
        case 1:
            weight1 = weight1 * data->unk60;
            break;
        case 0:
            weight0 = weight0 * data->unk60;
            break;
        case 2:
            weight2 = weight2 * data->unk60;
            break;
        }
        float value = (weight2 + (weight0 + weight1)) * randf();
        if (weight0 <= value) {
            if (weight0 + weight1 <= value) {
                color = 2;
            } else {
                color = 1;
            }
        } else {
            color = 0;
        }
        nw4r::g3d::ResMat mat;
        nw4r::g3d::ResMatTevColor tev;
        GXColor tevColor;
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
            if (model.IsValid()) {
                mat = model.GetResMat("body");
                if (mat.IsValid()) {
                    nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
                    tev = access.GetResMatTevColor(false);
                    if (tev.IsValid()) {
                        if (color == 1) {
                            tevColor.r = 0x00;
                            tevColor.g = 0x00;
                            tevColor.b = 0x80;
                            tevColor.a = 0xFF;
                        } else if (color == 0) {
                            tevColor.r = 0x80;
                            tevColor.g = 0x00;
                            tevColor.b = 0x00;
                            tevColor.a = 0xFF;
                        } else {
                            if (2 < color) {
                                return color;
                            }
                            tevColor.r = 0x80;
                            tevColor.g = 0x64;
                            tevColor.b = 0x00;
                            tevColor.a = 0xFF;
                        }
                        tev.GXSetTevColor(GX_TEVREG1, tevColor);
                        tev.GXSetTevColor(GX_TEVREG2, tevColor);
                        tev.DCStore(false);
                        mat.DCStore(false);
                        m_color = color;
                    }
                }
            }
        }
    }
    return color;
}
