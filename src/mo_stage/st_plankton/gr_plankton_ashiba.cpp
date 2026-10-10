#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ec/ec_mgr.h>
#include <gm/gm_global.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>
#include <yk/yk_normal.h>

#include <st_plankton/gr_plankton.h>

grPlanktonAshiba* grPlanktonAshiba::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPlanktonAshiba* ground = new (Heaps::StageInstance) grPlanktonAshiba(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPlanktonAshiba::grPlanktonAshiba(const char* taskName) : grPlankton(taskName) {
    m_leaf = NULL;
    m_nodeLeaf = 0;
    m_nodeLine = 0;
    setPos(0.0f, 0.0f, 0.0f);
    setRot(0.0f, 0.0f, 0.0f);
    m_colorLevel = 100;
    m_colorTimer = 0.0f;
    m_rotTimer = 0.0f;
    m_lineTimer = 0.0f;
    m_landTimer = 0.0f;
    m_dir = 1;
    m_hasYakumono = 0;
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
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    callback->m_nodeCallbackDatas[0].m_flags |= 0x20;
    m_event = 0;
    if (g_GameGlobal->m_modeMelee == NULL) {
        return;
    }
    if (planktonMeleeBytes()->m_mode != 7) {
        return;
    }
    if (planktonMeleeBytes()->m_event == 0x24) {
        m_event = 1;
    }
}

grPlanktonAshiba::~grPlanktonAshiba() {
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

void grPlanktonAshiba::processAnim() {
    Ground::processAnim();
    if (m_leaf != NULL) {
        getNodePosition(&m_leaf->m_posA, 0, m_nodeLeaf);
        getNodePosition(&m_leaf->m_posB, 0, m_nodeLine);
    }
}

void grPlanktonAshiba::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        updateYakumono(deltaFrame);
        updateRot(deltaFrame);
        updateColor(deltaFrame);
        updateLine(deltaFrame);
        updateLanding(deltaFrame);
        updateHanenbou(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Makes the hit object of the leaf: one capsule along the leaf (from the node of the leaf to the node of the line), once.
void grPlanktonAshiba::updateYakumono(float deltaFrame) {
    if (m_hasYakumono != 1) {
        // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
        m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData)]);
        m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple)]);
        m_hitSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(new (Heaps::StageInstance) u8[sizeof(grPlanktonSetView)]);
        m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
        m_data = new (Heaps::StageInstance) ykData;
        Vec3f leafPos;
        Vec3f linePos;
        getNodePosition(&leafPos, 0, m_nodeLeaf);
        getNodePosition(&linePos, 0, m_nodeLine);
        Vec3f diff = leafPos - linePos;
        m_hitData->m_startOffsetPos.m_x = 0.0f;
        m_hitData->m_startOffsetPos.m_y = 0.0f;
        m_hitData->m_startOffsetPos.m_z = 0.0f;
        m_hitData->m_endOffsetPos.m_x = diff.m_x;
        m_hitData->m_endOffsetPos.m_y = diff.m_y;
        m_hitData->m_endOffsetPos.m_z = 0.0f;
        m_hitData->m_size = 1.5f;
        // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
        reinterpret_cast<grPlanktonHitByte*>(m_hitData)->m_shape = 1;
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
        m_hitSimple->m_nodeIndex = 0;
        // MATCH-ONLY: the members of soSet are private
        grPlanktonSetView* set = reinterpret_cast<grPlanktonSetView*>(m_hitSet);
        set->m_elements = m_hitSimple;
        set->m_size = 1;
        m_dataGroup->m_hitDataSimpleSet = m_hitSet;
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
        setYakumono(yakumono);
        yakumono->setReactionFrame(0);
        yakumono->setCollisionHitOpponentCategory(0x108, 0);
        yakumono->setCollisionHitSelfCatagory(9);
        m_hasYakumono = 1;
    }
}

// The leaf turns back to its rest angle step by step, once it was not hit for a while; it stays inside its limits.
void grPlanktonAshiba::updateRot(float deltaFrame) {
    stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
    if (data != NULL) {
        m_rotTimer = m_rotTimer - deltaFrame;
        if (m_rotTimer < 0.0f) {
            m_rotTimer = 0.0f;
        }
        float angle = m_leaf->m_angle;
        if (m_rotTimer == 0.0f) {
            float target = data->unk08;
            if (angle != target) {
                if (angle > target) {
                    float next = angle - data->unk24;
                    angle = target;
                    if (next >= target) {
                        m_rotTimer = data->unk20;
                        angle = next;
                    }
                } else {
                    float next = angle + data->unk24;
                    angle = target;
                    if (next <= target) {
                        m_rotTimer = data->unk20;
                        angle = next;
                    }
                }
            }
        } else {
            if (angle < data->unk1C) {
                angle = data->unk1C;
            }
            if (data->unk18 < angle) {
                angle = data->unk18;
            }
        }
        m_leaf->m_angle = angle;
    }
}

// The color of the leaf follows how often it was hit (the colors are in the stage data).
void grPlanktonAshiba::updateColor(float deltaFrame) {
    stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
    if (data != NULL) {
        m_colorTimer = m_colorTimer - deltaFrame;
        if (m_colorTimer < 0.0f) {
            m_colorTimer = 0.0f;
        }
        u8 level = m_leaf->m_hitCount;
        if (level == m_colorLevel) {
            if (m_colorTimer == 0.0f && level != 0) {
                m_leaf->m_hitCount = level - 1;
            }
        } else {
            nw4r::g3d::ResMatTevColor tevColor;
            nw4r::g3d::ResMat mat;
            nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
            if (scnMdl != NULL) {
                nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
                if (model.IsValid()) {
                    mat = model.GetResMat("leafA");
                    if (mat.IsValid()) {
                        nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
                        tevColor = access.GetResMatTevColor(0);
                        if (tevColor.IsValid()) {
                            u8 index = m_leaf->m_hitCount;
                            if (index > 14) {
                                index = 14;
                            }
                            tevColor.GXSetTevColor(GX_TEVREG1, data->unk7C[index]);
                            tevColor.GXSetTevColor(GX_TEVREG2, data->unkB8[index]);
                            tevColor.DCStore(false);
                            mat.DCStore(false);
                            m_colorLevel = m_leaf->m_hitCount;
                            float time = data->unk04;
                            m_colorTimer = time;
                            if (m_event == 1 && g_GameGlobal->m_modeMelee != NULL) {
                                switch (planktonMeleeBytes()->m_rule) {
                                case 0:
                                    m_colorTimer = time * data->unk70;
                                    break;
                                case 1:
                                    m_colorTimer = time * data->unk74;
                                    break;
                                case 2:
                                    m_colorTimer = time * data->unk78;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// The line of the leaf (a material) fades from the color of a hit.
void grPlanktonAshiba::updateLine(float deltaFrame) {
    stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
    if (data != NULL) {
        m_lineTimer = m_lineTimer - deltaFrame;
        if (m_lineTimer < 0.0f) {
            m_lineTimer = 0.0f;
        }
        if (m_lineTimer != 0.0f) {
            nw4r::g3d::ResMatTevColor tevColor;
            nw4r::g3d::ResMat mat;
            nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
            if (scnMdl != NULL) {
                nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
                if (model.IsValid()) {
                    mat = model.GetResMat("leafB");
                    if (mat.IsValid()) {
                        nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
                        tevColor = access.GetResMatTevColor(0);
                        if (tevColor.IsValid()) {
                            float rate = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)((1.0f - m_lineTimer / 10.0f) * 16384.0f))));
                            rate = nw4r::math::FSelect(rate - 0.0f, rate, 0.0f);
                            rate = nw4r::math::FSelect(rate - 1.0f, 1.0f, rate);
                            GXColor color;
                            color.r = 255.0f - rate * 83.0f;
                            color.g = color.r;
                            color.b = 255.0f - rate * 43.0f;
                            color.a = color.r;
                            tevColor.GXSetTevColor(GX_TEVREG1, color);
                            u8 value = 255.0f - rate * 255.0f;
                            color.r = value;
                            color.g = value;
                            color.b = value;
                            color.a = value;
                            tevColor.GXSetTevColor(GX_TEVREG2, color);
                            tevColor.DCStore(false);
                            mat.DCStore(false);
                        }
                    }
                }
            }
        }
    }
}

void grPlanktonAshiba::updateLanding(float deltaFrame) {
    m_landTimer = m_landTimer - deltaFrame;
    if (0.0f <= m_landTimer) {
        return;
    }
    m_landTimer = 0.0f;
}

// A flower of the leaf: the hit count goes up when the stage drops a leaf onto the leaf (hanenbou) and the timer starts.
void grPlanktonAshiba::updateHanenbou(float deltaFrame) {
    if (m_leaf != NULL && m_leaf->m_wait == 0) {
        stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
        if (data != NULL) {
            m_lineTimer = 10.0f;
            m_leaf->m_hitCount = m_leaf->m_hitCount + 1;
            if (m_leaf->m_hitCount >= 15) {
                m_leaf->m_hitCount = 14;
            }
            float time = data->unk04;
            m_colorTimer = time;
            if (m_event == 1) {
                if (g_GameGlobal->m_modeMelee == NULL) {
                    return;
                }
                switch (planktonMeleeBytes()->m_rule) {
                case 0:
                    m_colorTimer = time * data->unk70;
                    break;
                case 1:
                    m_colorTimer = time * data->unk74;
                    break;
                case 2:
                    m_colorTimer = time * data->unk78;
                    break;
                }
            }
            m_leaf->m_wait = 3;
        }
    }
}

void grPlanktonAshiba::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            float angle = m_leaf->m_angle;
            if (m_leaf->m_flip == 0) {
                angle = -angle;
            }
            calcWorldCallBack->m_nodeCallbackDatas[0].m_offsetPos.m_z = m_leaf->unk00;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_offsetRot.m_z = angle;
        }
    }
}

// Finds the two nodes of the leaf (the place of the leaf and the end of its line) by the number of the leaf.
bool grPlanktonAshiba::setNode() {
    bool result = grGimmick::setNode();
    switch (m_leaf->m_index) {
    case 0:
        getNodeIndex(&m_nodeLeaf, 0, "AshibaA_Loc");
        getNodeIndex(&m_nodeLine, 0, "StgPlanktonAshibaA");
        break;
    case 1:
        getNodeIndex(&m_nodeLeaf, 0, "AshibaB_Loc");
        getNodeIndex(&m_nodeLine, 0, "StgPlanktonAshibaB");
        break;
    case 2:
        getNodeIndex(&m_nodeLeaf, 0, "AshibaC_Loc");
        getNodeIndex(&m_nodeLine, 0, "StgPlanktonAshibaC");
        break;
    case 3:
        getNodeIndex(&m_nodeLeaf, 0, "AshibaD_Loc");
        getNodeIndex(&m_nodeLine, 0, "StgPlanktonAshibaD");
        break;
    case 4:
        getNodeIndex(&m_nodeLeaf, 0, "AshibaE_Loc");
        getNodeIndex(&m_nodeLine, 0, "StgPlanktonAshibaE");
        break;
    case 5:
        getNodeIndex(&m_nodeLeaf, 0, "AshibaF_Loc");
        getNodeIndex(&m_nodeLine, 0, "StgPlanktonAshibaF");
        break;
    case 6:
        getNodeIndex(&m_nodeLeaf, 0, "AshibaG_Loc");
        getNodeIndex(&m_nodeLine, 0, "StgPlanktonAshibaG");
        break;
    case 7:
        getNodeIndex(&m_nodeLeaf, 0, "AshibaH_Loc");
        getNodeIndex(&m_nodeLine, 0, "StgPlanktonAshibaH");
        break;
    case 8:
        getNodeIndex(&m_nodeLeaf, 0, "AshibaI_Loc");
        getNodeIndex(&m_nodeLine, 0, "StgPlanktonAshibaI");
        break;
    case 9:
        getNodeIndex(&m_nodeLeaf, 0, "AshibaJ_Loc");
        getNodeIndex(&m_nodeLine, 0, "StgPlanktonAshibaJ");
        break;
    }
    return result;
}

// A hit pushes the leaf: a hard hit twice as far; the leaf changes the way it turns at its limits. Hits also count for the
// color of the leaf.
void grPlanktonAshiba::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
    if (data != NULL) {
        float amount = damage->unk4;
        fn_27_26399C(m_yakumono);
        if (amount != 0.0f) {
            float angle = m_leaf->m_angle;
            float push = 0.0f;
            if (amount < data->unk0C) {
                if (m_dir == 1) {
                    push += data->unk14;
                } else {
                    push -= data->unk14;
                }
            } else if (amount >= data->unk10) {
                if (m_dir == 1) {
                    push += data->unk14 * 2.0f;
                } else {
                    push -= data->unk14 * 2.0f;
                }
            }
            m_leaf->m_angle = angle + push;
            if (m_dir == 1 && m_leaf->m_angle > data->unk18) {
                m_dir = 0;
            }
            if (m_dir == 0 && m_leaf->m_angle < data->unk1C) {
                m_dir = 1;
            }
            m_rotTimer = data->unk20;
            m_leaf->m_hitCount = m_leaf->m_hitCount + 1;
            if (m_leaf->m_hitCount >= 15) {
                m_leaf->m_hitCount = 14;
            }
            float time = data->unk04;
            m_colorTimer = time;
            if (m_event == 1 && g_GameGlobal->m_modeMelee != NULL) {
                switch (planktonMeleeBytes()->m_rule) {
                case 0:
                    m_colorTimer = time * data->unk70;
                    break;
                case 1:
                    m_colorTimer = time * data->unk74;
                    break;
                case 2:
                    m_colorTimer = time * data->unk78;
                    break;
                }
            }
        }
    }
}

// The length of a vector (the same code as the original: it is written out, and zero for an almost empty vector).
static inline float planktonLength(Vec3f& v) {
    float lengthSq = v.m_z * v.m_z + v.m_x * v.m_x + v.m_y * v.m_y;
    float length = 0.0f;
    if (!((float)fabs(lengthSq) <= 1.17549435e-38f)) {
        length = lengthSq * rsqrtf(lengthSq);
    }
    return length;
}

// A fighter lands on the leaf: a ripple is made where he lands, and the sound depends on how far along the leaf it is.
void grPlanktonAshiba::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, reinterpret_cast<int*>(&sOwnerFlags))) {
        m_landTimer = 5.0f;
        if (isFirstContact == 1) {
            stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
            if (data != NULL && lbl_27_bss_5668 != NULL) {
                stPlankton* stage = static_cast<stPlankton*>(lbl_27_bss_5668);
                Vec2f contact;
                collStatus->getPos(&contact);
                Vec3f hit(contact.m_x, contact.m_y, 0.0f);
                stage->makeHamon(&hit, m_leaf->m_hitCount);
                Vec3f along;
                along = m_leaf->m_posA - m_leaf->m_posB;
                float lengthAlong = planktonLength(along);
                Vec3f to;
                to = hit - m_leaf->m_posB;
                float lengthTo = planktonLength(to);
                float rate = lengthTo / lengthAlong;
                u32 id;
                if (rate >= 0.125f) {
                    if (rate >= 0.25f) {
                        if (rate >= 0.375f) {
                            if (rate >= 0.5f) {
                                if (rate >= 0.625f) {
                                    if (rate >= 0.75f) {
                                        if (rate >= 0.875f) {
                                            id = 0x1D91;
                                        } else {
                                            id = 0x1D90;
                                        }
                                    } else {
                                        id = 0x1D8F;
                                    }
                                } else {
                                    id = 0x1D8E;
                                }
                            } else {
                                id = 0x1D8D;
                            }
                        } else {
                            id = 0x1D8C;
                        }
                    } else {
                        id = 0x1D8B;
                    }
                } else {
                    id = 0x1D8A;
                }
                m_snd.playSE(static_cast<SndID>(id), 0, 0, -1);
                m_snd.setPos(&hit);
            }
        }
    }
}
