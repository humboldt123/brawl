#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL
#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the stage RELs that also hold the damage module
#define SO_COLLISION_GROUP_ALIGNED // soArrayVector<soCollisionGroup, N> places its elements 4-aligned
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies it member by member

#include <ec/ec_mgr.h>
#include <gf/gf_camera.h>
#include <gf/gf_draw.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <gm/gm_global_mode_melee.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_calc_world_callback.h>
#include <it/it_manager.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <revolution/GX.h>
#include <string.h>
#include <yk/yk_normal.h>

#include <st_mariopast/gr_mariopast.h>

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);
// itManager::lotCreateItem (sora_melee, unnamed)
extern "C" void fn_27_2AA7AC(itManager* manager, ItemKind kind, int lotId, Vec3f* pos, int a, int b, int c);
// the stage that is played (sora_melee .bss)
extern "C" Stage* lbl_27_bss_5668;
// Matrix::setTrans (main, unnamed)
extern "C" void fn_8003F03C(Matrix* mtx, float x, float y, float z);
// HYPOTHESIS: makes a texture object of a texture resource, and sets the wrap mode of it (main, unnamed)
extern "C" void fn_8001AA88(GXTexObj* texObj, nw4r::g3d::ResTex* tex);
extern "C" void Destroy__Q34nw4r3g3d6G3dObjFv(void* obj);
extern "C" void fn_801F2C58(GXTexObj* texObj, int wrapS, int wrapT);

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct mariopastHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// MATCH-ONLY: layout of soSet<T> (its members are private)
struct mariopastSetView {
    void* m_elements;
    u32 m_size;
};

// MATCH-ONLY: the flags of a joint (bytes 0x54 and 0x55 are bit fields in the header, shown here as plain bytes)
struct mariopastJointView {
    u8 _pad[0x54];
    u8 m_flags54;
    u8 m_flags55;
    u8 m_flags56;
};

// MATCH-ONLY: the first bits of the byte at 0xF of a collision line
struct mariopastLineView {
    u8 _pad[0xF];
    u8 m_flags;
};

// MATCH-ONLY: the zone byte and the layout of the stage's melee settings
static inline u8* mpItemFrequencyByte() {
    return reinterpret_cast<u8*>(&g_GameGlobal->m_modeMelee->m_meleeInitData) + 0xE;
}

grMarioPastBlockPosLite::grMarioPastBlockPosLite(const char* taskName) : grYakumono(taskName) {
    m_scnProc = NULL;
    m_stage = 0;
    m_group = 0;
    m_num = 0;
    m_blocks = NULL;
    m_joints = NULL;
    m_posGimmick = NULL;
    m_limit = NULL;
    m_mtx.setIdentity();
    m_hitSimple[0] = NULL;
    m_hitSimple[1] = NULL;
    m_simpleSets[0] = NULL;
    m_simpleSets[1] = NULL;
    m_dataGroups[0] = NULL;
    m_dataGroups[1] = NULL;
    m_data[0] = NULL;
    m_data[1] = NULL;
    Vec3f zero(0.0f, 0.0f, 0.0f);
    m_snd.setPos(&zero);
    m_rowNum[0] = 0;
    m_rowNum[1] = 0;
    memset(m_rowHit[0], -1, sizeof(m_rowHit[0]));
    memset(m_rowHit[1], -1, sizeof(m_rowHit[1]));
}

grMarioPastBlockPosLite* grMarioPastBlockPosLite::create(int mdlIndex, const char* taskName) {
    grMarioPastBlockPosLite* ground = new (Heaps::StageInstance) grMarioPastBlockPosLite(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->m_heapType = Heaps::StageInstance;
        ground->makeCalcuCallback(1, Heaps::StageInstance);
        ground->setCalcuCallbackRoot(1);
        ground->setupMelee();
    }
    return ground;
}

grMarioPastBlockPosLite::~grMarioPastBlockPosLite() {
    if (m_scnProc != NULL) {
        Destroy__Q34nw4r3g3d6G3dObjFv(m_scnProc);
    }
    m_scnProc = NULL;
    if (m_hitSimple[0] != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitSimple[0]);
    }
    m_hitSimple[0] = NULL;
    if (m_hitSimple[1] != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitSimple[1]);
    }
    m_hitSimple[1] = NULL;
    if (m_simpleSets[0] != NULL) {
        delete[] reinterpret_cast<u8*>(m_simpleSets[0]);
    }
    m_simpleSets[0] = NULL;
    if (m_simpleSets[1] != NULL) {
        delete[] reinterpret_cast<u8*>(m_simpleSets[1]);
    }
    m_simpleSets[1] = NULL;
    if (m_dataGroups[0] != NULL) {
        delete[] m_dataGroups[0];
    }
    m_dataGroups[0] = NULL;
    if (m_dataGroups[1] != NULL) {
        delete[] m_dataGroups[1];
    }
    m_dataGroups[1] = NULL;
    if (m_data[0] != NULL) {
        delete m_data[0];
    }
    m_data[0] = NULL;
    if (m_data[1] != NULL) {
        delete m_data[1];
    }
    m_data[1] = NULL;
}

// The blocks are drawn by the ground itself: a scene object whose draw function (drawProc) draws all the blocks.
void grMarioPastBlockPosLite::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    m_resFile = nw4r::g3d::ResFile(data->getData(Data_Type_Tex, 0, 0xFFFE));
    u32 size;
    m_scnProc = nw4r::g3d::ScnProc::Construct(gfHeapManager::getMEMAllocator(Heaps::StageInstance), &size, drawProc, false, true);
    if (m_scnProc != NULL) {
        m_scnProc->SetUserData(this);
        g_gfSceneRoot->add(static_cast<gfSceneRoot::LayerType>(0), reinterpret_cast<nw4r::g3d::ScnMdl*>(m_scnProc));
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            scnMdl->m_calcWorldCallBack = &m_calcWorldCallBack;
            scnMdl->EnableScnMdlCallbackTiming(1);
            scnMdl->m_nodeIndex = m_calcWorldCallBack.m_nodeCallbackDatas[0].m_nodeIndex;
        }
    }
}

void grMarioPastBlockPosLite::drawProc(nw4r::g3d::ScnProc* proc, bool opa) {
    if (proc != NULL && opa != true) {
        gfCameraManager::getManager()->m_cameras[0].setGX();
        static_cast<grMarioPastBlockPosLite*>(proc->GetUserData())->drawProcBlocks();
    }
}

void grMarioPastBlockPosLite::processAnim() {
    if (m_posGimmick != NULL) {
        setPos(m_posGimmick);
        fn_8003F03C(&m_mtx, m_posGimmick->m_x, m_posGimmick->m_y, m_posGimmick->m_z);
    }
    Ground::processAnim();
}

void grMarioPastBlockPosLite::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    updateBlocks(deltaFrame);
}

// Switches the collision of the blocks on (and makes the joints follow the matrix of the ground).
void grMarioPastBlockPosLite::initCollisionJoint() {
    u32 num = m_num;
    stMarioPastBlockDt* block = m_blocks;
    mariopastJointView* joint = reinterpret_cast<mariopastJointView*>(m_joints);
    if (num != 0 && block != NULL && joint != NULL) {
        for (u32 i = 0; i < num; i++) {
            joint->m_flags56 |= 0x80;
            *reinterpret_cast<Matrix**>(reinterpret_cast<u8*>(joint) + 0x5C) = &m_mtx;
            joint->m_flags54 = (joint->m_flags54 & 0x2F) | 8;
            *reinterpret_cast<Ground**>(reinterpret_cast<u8*>(joint) + 0x58) = this;
            if (block->m_kind == 3) {
                setHatenaHideBlockCollision(i, 1);
            }
            joint = reinterpret_cast<mariopastJointView*>(reinterpret_cast<u8*>(joint) + 0x60);
            block++;
        }
    }
}

// Groups the blocks into rows of the same height (a row is a hit group of the Yakumono), gives every block a hit sphere
// and creates the Yakumono; its size depends on the number of blocks of the model.
void grMarioPastBlockPosLite::initYaku() {
    u32 num = m_num;
    stMarioPastBlockDt* blocks = m_blocks;
    if (num > 0 && blocks != NULL) {
        int rowCount = 0;
        float rowY[10];
        memset(rowY, 0, sizeof(rowY));
        stMarioPastBlockDt* block = blocks;
        for (u32 i = 0; i != num; i++) {
            float* p = rowY;
            int row = 0;
            for (int k = rowCount; k != 0; k--) {
                if (*p - 0.1f < block->m_pos.m_y && block->m_pos.m_y < *p + 0.1f) {
                    break;
                }
                p++;
                row++;
            }
            if (row == rowCount) {
                rowCount++;
                rowY[row] = block->m_pos.m_y;
            }
            block->m_row = row;
            block++;
        }
        m_rowNum[m_group] = rowCount;
        // MATCH-ONLY: the original allocates the spheres and the sets as raw storage
        m_hitSimple[m_group] = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[num * sizeof(soCollisionHitData::Simple)]);
        soCollisionHitData::Simple* simple = m_hitSimple[m_group];
        mariopastSetView* sets = reinterpret_cast<mariopastSetView*>(new (Heaps::StageInstance) u8[rowCount * sizeof(mariopastSetView)]);
        m_simpleSets[m_group] = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(sets);
        ykDataGroup* dataGroups = new (Heaps::StageInstance) ykDataGroup[rowCount];
        m_dataGroups[m_group] = dataGroups;
        ykData* data = new (Heaps::StageInstance) ykData;
        m_data[m_group] = data;
        int row;
        int offset = 0;
        for (row = 0; row < rowCount; row++) {
            stMarioPastBlockDt* b = m_blocks;
            int count = 0;
            sets->m_elements = NULL;
            for (u32 i = 0; i < num; i++) {
                if (b->m_row == row) {
                    if (sets->m_elements == NULL) {
                        sets->m_elements = simple;
                    }
                    simple->m_endOffsetPos = b->m_posOrg;
                    simple->m_startOffsetPos = simple->m_endOffsetPos;
                    simple->m_size = 5.0f;
                    reinterpret_cast<mariopastHitByte*>(simple)->m_shape = 1;
                    simple->m_height = soCollisionHitData::Height_Low;
                    simple->m_nodeIndex = 0;
                    simple++;
                    b->m_index = count;
                    count++;
                }
                b++;
            }
            if (count == 0) {
                m_rowHit[m_group][row] = 1;
            } else {
                m_rowHit[m_group][row] = count;
            }
            sets->m_size = count;
            dataGroups->m_hitDataSimpleSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(sets);
            sets++;
            dataGroups->m_hitGroupIndex = row;
            dataGroups++;
        }
        m_data[m_group]->m_dataGroupNum = rowCount;
        m_data[m_group]->m_dataGroups = m_dataGroups[m_group];

        ykInitInfo info = { 0, 0, 0x10, 0, 0 };
        info.m_ground = this;
        info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
        Vec3f pos = getPos();
        info.m_pos = &pos;
        info.m_work = m_data[m_group];
        Yakumono* yakumono;
        if (m_stage == 0) {
            if (m_group == 0) {
                typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                                 soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 19, 19, soCollisionHitModuleImpl, 0x3FF, true> >
                    Config;
                yakumono = new (Heaps::StageInstance) Config(&info);
            } else {
                typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                                 soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 25, 25, soCollisionHitModuleImpl, 0x3FF, true> >
                    Config;
                yakumono = new (Heaps::StageInstance) Config(&info);
            }
        } else if (m_group == 0) {
            typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                             soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 58, 58, soCollisionHitModuleImpl, 0x3FF, true> >
                Config;
            yakumono = new (Heaps::StageInstance) Config(&info);
        } else {
            typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                             soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 205, 205, soCollisionHitModuleImpl, 0x3FF, true> >
                Config;
            yakumono = new (Heaps::StageInstance) Config(&info);
        }
        yakumono->setCollisionHitSelfCatagory(9);
        setYakumono(yakumono);
        int rows = m_rowNum[m_group];
        int offsetRow = 0;
        for (int r = 0; r != rows; r++) {
            int hits = m_rowHit[m_group][r];
            for (int j = 0; j != hits; j++) {
                disableHit(j, r);
            }
        }
    }
}

// Moves the blocks with the scroll, keeps their zone relative to the camera area, plays the bump of a block that was hit
// (the block jumps up and a brick shakes) and switches the hit spheres on when a block comes into the camera area and off when
// it leaves it.
void grMarioPastBlockPosLite::updateBlocks(float deltaFrame) {
    u32 num = m_num;
    stMarioPastBlockDt* block = m_blocks;
    mariopastJointView* joint = reinterpret_cast<mariopastJointView*>(m_joints);
    if (num != 0 && block != NULL && joint != NULL) {
        Vec3f base(0.0f, 0.0f, 0.0f);
        if (m_posGimmick != NULL) {
            base.m_x = m_posGimmick->m_x;
            base.m_y = m_posGimmick->m_y;
            base.m_z = m_posGimmick->m_z;
        }
        float left = 0.0f;
        float right = 0.0f;
        if (m_limit != NULL) {
            left = m_limit[0].m_x - 5.0f;
            right = m_limit[1].m_x + 5.0f;
        }
        float* data = static_cast<float*>(getStageData());
        float bump = 0.0f;
        if (data != NULL) {
            bump = data[3];
        }
        for (u32 i = 0; i < num; i++) {
            Vec3f pos;
            pos.m_x = base.m_x + block->m_posOrg.m_x;
            pos.m_y = base.m_y + block->m_posOrg.m_y;
            pos.m_z = base.m_z + block->m_posOrg.m_z;
            if (block->m_pos.m_x < pos.m_x) {
                block->m_state = 1;
                u8 count = 1;
                if (block->m_item == 6) {
                    count = 3;
                }
                block->m_count = count;
                bool hasBump = true;
                block->m_timer = 0.0f;
                if (block->m_kind != 0 && block->m_kind != 1) {
                    hasBump = false;
                }
                block->m_bump = hasBump ? bump : 0.0f;
                block->m_bumpTime = 0.0f;
                if (block->m_kind == 3) {
                    setHatenaHideBlockCollision(i, 1);
                }
            }
            bool inside = false;
            block->m_pos.m_x = pos.m_x;
            block->m_pos.m_y = pos.m_y;
            block->m_pos.m_z = pos.m_z;
            u8 oldZone = block->m_zone & 3;
            u8 zone;
            if (left <= pos.m_x) {
                if (pos.m_x <= right) {
                    zone = 1;
                    inside = true;
                } else {
                    zone = 0;
                }
            } else {
                zone = 2;
            }
            if (zone != oldZone) {
                if (zone == 1) {
                    zone = 5;
                } else if (oldZone == 1) {
                    zone = zone | 8;
                }
            }
            block->m_zone = zone;
            if (inside) {
                u8 state = block->m_state;
                if (state == 4) {
                    float time = block->m_bumpTime - deltaFrame;
                    if (time <= 0.0f) {
                        block->m_bumpTime = 0.0f;
                        if (block->m_kind == 0) {
                            block->m_state = 1;
                            block->m_zone = block->m_zone | 0x10;
                        } else if (block->m_count == 0) {
                            block->m_state = 5;
                            block->m_zone = block->m_zone | 0x40;
                        } else {
                            block->m_state = 1;
                            block->m_zone = block->m_zone | 0x10;
                        }
                    } else {
                        block->m_bumpTime = block->m_bumpTime - deltaFrame;
                        block->m_speedX = 0.0f;
                        float r = 1.0f - 0.083333336f * time;
                        if (r - 0.0f < 0.0f) {
                            r = 0.0f;
                        }
                        float clamped = 1.0f;
                        if (r - 1.0f < 0.0f) {
                            clamped = r;
                        }
                        float idx = 0.00390625f * (float)(s16)(int)(32768.0f * clamped);
                        block->m_speedY = 7.5f * nw4r::math::SinFIdx(idx);
                    }
                } else if (state < 4 && state > 1) {
                    float time = block->m_bumpTime - deltaFrame;
                    if (time <= 0.0f) {
                        block->m_bumpTime = 0.0f;
                        if (block->m_kind == 0) {
                            block->m_state = 1;
                            block->m_zone = block->m_zone | 0x10;
                        } else if (block->m_count == 0) {
                            block->m_state = 5;
                            block->m_zone = block->m_zone | 0x40;
                        } else {
                            block->m_state = 1;
                            block->m_zone = block->m_zone | 0x10;
                        }
                    } else {
                        block->m_bumpTime = block->m_bumpTime - deltaFrame;
                        if ((u32)time == ((u32)time / 3) * 3 && (u32)(time + deltaFrame) != ((u32)(time + deltaFrame) / 3) * 3) {
                            float sinA;
                            float cosA;
                            mtSinCosf(6.2831855f * randf(), &sinA, &cosA);
                            float length = 0.75f + 0.5f * randf();
                            block->m_speedX = length * cosA;
                            block->m_speedY = length * sinA;
                        }
                    }
                }
                float timer = block->m_timer;
                block->m_timer = timer - deltaFrame;
                if (timer - deltaFrame < 0.0f) {
                    block->m_timer = 0.0f;
                }
            }
            u8 flags = block->m_zone;
            if ((flags & 0x5C) != 0) {
                if ((flags & 0x10) != 0) {
                    enableHit(block->m_index, block->m_row);
                }
                if ((flags & 4) != 0) {
                    enableHit(block->m_index, block->m_row);
                    joint->m_flags54 = (joint->m_flags54 & 0xBF) | 0x50;
                    if (block->m_state < 4 && block->m_state != 0) {
                        joint->m_flags55 |= 0x10;
                    } else {
                        joint->m_flags55 &= 0xEF;
                    }
                }
                if ((flags & 8) != 0) {
                    disableHit(block->m_index, block->m_row);
                    joint->m_flags54 &= 0x2F;
                }
                if ((flags & 0x40) != 0) {
                    joint->m_flags55 &= 0xEF;
                }
            }
            block++;
            joint = reinterpret_cast<mariopastJointView*>(reinterpret_cast<u8*>(joint) + 0x60);
        }
    }
}

// A block that was broken or bumped drops an item (if the match has items on): the kind of item depends on what the block
// holds (HYPOTHESIS: the numbers are the groups of the item lot of the stage).
void grMarioPastBlockPosLite::makeItem(stMarioPastBlockDt* block) {
    if (g_GameGlobal->m_modeMelee == NULL) {
        return;
    }
    if (static_cast<s8>(*mpItemFrequencyByte()) == 0) {
        return;
    }
    Vec3f pos;
    pos.m_x = block->m_pos.m_x;
    pos.m_z = block->m_pos.m_z;
    pos.m_y = block->m_pos.m_y + 10.0f;
    int lotId;
    switch (block->m_item) {
    case 4:
        lotId = 1;
        break;
    case 5:
        lotId = 2;
        break;
    case 6:
        lotId = 3;
        break;
    case 7:
        lotId = 5;
        break;
    case 8:
        lotId = 4;
        break;
    default:
        lotId = 5;
        break;
    }
    ItemKind kind;
    kind.m_kind = static_cast<itKind>(*reinterpret_cast<int*>(reinterpret_cast<u8*>(lbl_27_bss_5668) + 0x44));
    kind.m_variation = 0;
    fn_27_2AA7AC(itManager::getInstance(), kind, lotId, &pos, -1, -1, -1);
}

// A ? block that is hidden has no collision on the lines until it was hit; this switches the lines of the block on or off.
void grMarioPastBlockPosLite::setHatenaHideBlockCollision(u32 index, int on) {
    if (m_joints != NULL && index < m_num) {
        grCollisionJoint* joint = reinterpret_cast<grCollisionJoint*>(reinterpret_cast<u8*>(m_joints) + index * 0x60);
        // HYPOTHESIS: the line count of a joint is the halfword at +2 (not named in the header).
        u16 lineNum = *reinterpret_cast<u16*>(reinterpret_cast<u8*>(joint) + 2);
        for (u32 i = 0; i < lineNum; i++) {
            mariopastLineView* line = reinterpret_cast<mariopastLineView*>(joint->getLine(i));
            if (line != NULL) {
                if (on == 0) {
                    line->m_flags |= 0x10;
                } else if (((line->m_flags >> 1) & 1) == 0) {
                    line->m_flags &= 0xEF;
                }
            }
        }
    }
}

// A fighter hits a block from below (the damage record tells which hit sphere of the row it was).
void grMarioPastBlockPosLite::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    u32 num = m_num;
    if (static_cast<u32>(index) < num && m_blocks != NULL && m_joints != NULL) {
        stMarioPastBlockDt* block = NULL;
        grCollisionJoint* joint = NULL;
        int i = 0;
        stMarioPastBlockDt* b = m_blocks;
        for (; num != 0; num--) {
            if (static_cast<u32>(index) == b->m_row && reinterpret_cast<u8*>(damage)[0x35] == b->m_index) {
                block = &m_blocks[i];
                joint = reinterpret_cast<grCollisionJoint*>(reinterpret_cast<u8*>(m_joints) + i * 0x60);
                break;
            }
            b++;
            i++;
        }
        if (block != NULL && joint != NULL) {
            mariopastJointView* jointView = reinterpret_cast<mariopastJointView*>(joint);
            if (block->m_kind == 0) {
                float hp = block->m_bump - reinterpret_cast<float*>(attackerInfo)[1];
                if (hp <= 0.0f) {
                    block->m_bump = 0.0f;
                    if (block->m_state != 6) {
                        block->m_state = 6;
                        block->m_count = 0;
                        disableHit(block->m_index, block->m_row);
                        jointView->m_flags54 &= 0x2F;
                        if (m_stage == 0) {
                            g_ecMgr->setEffect(static_cast<EfID>(0x360003), &block->m_pos);
                        } else {
                            g_ecMgr->setEffect(static_cast<EfID>(0x360004), &block->m_pos);
                        }
                        float r = randf();
                        if (r >= 0.33333334f) {
                            if (r >= 0.6666667f) {
                                m_snd.playSE(SndID(0x1B74), 0, 0, -1);
                            } else {
                                m_snd.playSE(SndID(0x1B73), 0, 0, -1);
                            }
                        } else {
                            m_snd.playSE(SndID(0x1B72), 0, 0, -1);
                        }
                    }
                } else {
                    block->m_bump = hp;
                    block->m_state = 2;
                    block->m_bumpTime = randf() * 5.0f + 7.0f;
                    block->m_speedX = 0.0f;
                    block->m_speedY = 0.0f;
                    m_snd.playSE(SndID(0x1B75), 0, 0, -1);
                    disableHit(block->m_index, block->m_row);
                }
            } else if (block->m_kind < 4) {
                if (block->m_state == 1) {
                    block->m_state = 4;
                    block->m_bumpTime = 12.0f;
                    block->m_speedX = 0.0f;
                    block->m_speedY = 0.0f;
                    m_snd.playSE(SndID(0x1B70), 0, 0, -1);
                    disableHit(block->m_index, block->m_row);
                    if (block->m_count != 0) {
                        block->m_count--;
                    }
                    makeItem(block);
                    if (block->m_count == 0 && block->m_kind == 3) {
                        setHatenaHideBlockCollision(i, 0);
                    }
                } else {
                    block->m_state = 2;
                    block->m_bumpTime = randf() * 5.0f + 7.0f;
                    block->m_speedX = 0.0f;
                    block->m_speedY = 0.0f;
                    block->m_state = 3;
                    m_snd.playSE(SndID(0x1B71), 0, 0, -1);
                }
            }
        }
    }
}

// MATCH-ONLY: the original reads the second byte of the collision status' first word as a signed bit field.
struct mariopastCollView {
    char _0[8];
    int m_first : 8;
    int m_hitSide : 8;
    int m_rest : 16;
};

// A fighter hits a block from below with his head: the same as above, only found by the collision joint.
void grMarioPastBlockPosLite::receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* joint, bool isStartCollision) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask(collStatus, reinterpret_cast<CollCategoryFlag*>(&sOwnerFlags)) &&
        reinterpret_cast<mariopastCollView*>(collStatus)->m_hitSide == 2 && m_num != 0 && m_blocks != NULL && m_joints != NULL) {
        u32 index = (reinterpret_cast<u8*>(joint) - reinterpret_cast<u8*>(m_joints)) / 0x60;
        if (index < m_num) {
            stMarioPastBlockDt* block = &m_blocks[index];
            if (block->m_timer <= 0.0f) {
                if (block->m_state > 4 || block->m_state < 2) {
                    block->m_timer = 5.0f;
                    if (block->m_kind == 0) {
                        if (block->m_state != 6) {
                            if (block->m_bump - 10.0f <= 0.0f) {
                                block->m_state = 6;
                                block->m_count = 0;
                                disableHit(block->m_index, block->m_row);
                                reinterpret_cast<mariopastJointView*>(joint)->m_flags54 &= 0x2F;
                                if (m_stage == 0) {
                                    g_ecMgr->setEffect(static_cast<EfID>(0x360003), &block->m_pos);
                                } else {
                                    g_ecMgr->setEffect(static_cast<EfID>(0x360004), &block->m_pos);
                                }
                                float r = randf();
                                if (r >= 0.33333334f) {
                                    if (r >= 0.6666667f) {
                                        m_snd.playSE(SndID(0x1B74), 0, 0, -1);
                                    } else {
                                        m_snd.playSE(SndID(0x1B73), 0, 0, -1);
                                    }
                                } else {
                                    m_snd.playSE(SndID(0x1B72), 0, 0, -1);
                                }
                            } else {
                                block->m_bump = block->m_bump - 10.0f;
                                block->m_state = 2;
                                block->m_bumpTime = randf() * 5.0f + 7.0f;
                                block->m_speedX = 0.0f;
                                block->m_speedY = 0.0f;
                                m_snd.playSE(SndID(0x1B75), 0, 0, -1);
                                disableHit(block->m_index, block->m_row);
                            }
                        }
                    } else if (block->m_kind < 4) {
                        if (block->m_state == 5) {
                            m_snd.playSE(SndID(0x1B71), 0, 0, -1);
                        } else {
                            block->m_state = 4;
                            block->m_bumpTime = 12.0f;
                            block->m_speedX = 0.0f;
                            block->m_speedY = 0.0f;
                            m_snd.playSE(SndID(0x1B70), 0, 0, -1);
                            disableHit(block->m_index, block->m_row);
                            if (block->m_count != 0) {
                                block->m_count--;
                            }
                            makeItem(block);
                            if (block->m_count == 0 && block->m_kind == 3) {
                                setHatenaHideBlockCollision(index, 0);
                            }
                        }
                    }
                }
            } else {
                block->m_timer = 5.0f;
            }
        }
    }
}

// ---- drawing ----

// The faces of a block (a cube of 8.5 units) are drawn as quads with a texture of two halves; `dark` blocks (the ? blocks that
// are used up) use the lower half of the texture.
void stMarioPastBlockDt::drawBlocks(int count, stMarioPastBlockDt* blocks, u16* order, int dark) {
    const float d = 4.25f;
    for (int i = 0; i < count; i++) {
        stMarioPastBlockDt* block = &blocks[*order];
        float x = block->m_pos.m_x;
        u8 state = block->m_state;
        float y = block->m_pos.m_y;
        if ((u8)(state - 2) < 3) {
            x = x + block->m_speedX;
            y = y + block->m_speedY;
        }
        bool used = false;
        if (dark != 0) {
            if (state < 6 && state > 1 && block->m_count == 0) {
                used = true;
            } else {
                used = false;
            }
        }
        float z = block->m_pos.m_z;
        float v0 = used ? 0.0f : 0.5f;
        float v1 = used ? 0.5f : 1.0f;
        GXBegin(GX_QUADS, GX_VTXFMT1, 4);
        float top = y + d;
        GXPosition3f32(x - d, top, z - d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x + d, top, z - d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(x - d, top, z + d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(0.0f, 0.5f);
        GXPosition3f32(x + d, top, z + d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(1.0f, 0.5f);
        GXBegin(GX_QUADS, GX_VTXFMT1, 4);
        float bottom = y - d;
        GXPosition3f32(x - d, bottom, z + d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x + d, bottom, z + d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(x - d, bottom, z - d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(0.0f, 0.5f);
        GXPosition3f32(x + d, bottom, z - d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(1.0f, 0.5f);
        GXBegin(GX_QUADS, GX_VTXFMT1, 4);
        float front = z + d;
        GXPosition3f32(x - d, y + d, front);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(0.0f, v0);
        GXPosition3f32(x + d, y + d, front);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(1.0f, v0);
        GXPosition3f32(x - d, y - d, front);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(0.0f, v1);
        GXPosition3f32(x + d, y - d, front);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(1.0f, v1);
        GXBegin(GX_QUADS, GX_VTXFMT1, 4);
        float left = x - d;
        GXPosition3f32(left, y - d, z + d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(1.0f, v1);
        GXPosition3f32(left, y - d, z - d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(0.0f, v1);
        GXPosition3f32(left, y + d, z + d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(1.0f, v0);
        GXPosition3f32(left, y + d, z - d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(0.0f, v0);
        GXBegin(GX_QUADS, GX_VTXFMT1, 4);
        float right = x + d;
        order++;
        GXPosition3f32(right, y + d, z + d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(0.0f, v0);
        GXPosition3f32(right, y + d, z - d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(1.0f, v0);
        GXPosition3f32(right, y - d, z + d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(0.0f, v1);
        GXPosition3f32(right, y - d, z - d);
        GXColor1u32(0xFFFFFFFF);
        GXTexCoord2f32(1.0f, v1);
    }
}

// Sorts the blocks that are inside the camera area (and not gone) into the ones drawn with the normal half of the texture and
// the ones drawn with the other half; the order arrays get their indices.
void stMarioPastBlockDt::countDrawNum(int count, stMarioPastBlockDt* blocks, int* normal, int* dark, u16* normalOrder,
                                      u16* darkOrder) {
    int normalNum = 0;
    int darkNum = 0;
    s16 index = 0;
    for (; count > 0; count--, blocks++) {
        if ((blocks->m_zone & 3) == 1) {
            u8 state = blocks->m_state;
            u8 kind = blocks->m_kind;
            bool draw;
            if (state == 6) {
                draw = false;
            } else if (kind == 3) {
                if (state < 4) {
                    if (state < 2 || blocks->m_count != 0) {
                        draw = false;
                    } else {
                        draw = true;
                    }
                } else if (state > 5) {
                    draw = false;
                } else {
                    draw = true;
                }
            } else {
                draw = true;
            }
            if (draw) {
                bool isNormal;
                if (kind == 1) {
                    if (state < 6 && state > 1 && blocks->m_count == 0) {
                        isNormal = false;
                    } else {
                        isNormal = true;
                    }
                } else if (kind == 0) {
                    isNormal = true;
                } else if (kind < 4) {
                    isNormal = false;
                } else {
                    isNormal = true;
                }
                if (isNormal) {
                    *normalOrder++ = index;
                    normalNum++;
                } else {
                    *darkOrder++ = index;
                    darkNum++;
                }
            }
        }
        index++;
    }
    *normal = normalNum;
    *dark = darkNum;
}

// Draws the blocks of the model in two passes (the normal blocks and the other ones, each with its half of the texture).
void grMarioPastBlockPosLite::drawProcBlocks() {
    static const char* const sTexNames[2][2] = { { "MH_block_01_A", "MH_hatena_01_A" }, { "M2H_block_01_A", "M2H_hatena_01_A" } };
    u32 stage = m_stage;
    stMarioPastBlockDt* blocks = m_blocks;
    if (m_num != 0 && blocks != NULL) {
        int normalNum;
        int darkNum;
        u16 normalOrder[263];
        u16 darkOrder[263];
        stMarioPastBlockDt::countDrawNum(m_num, blocks, &normalNum, &darkNum, normalOrder, darkOrder);
        for (int pass = 0; pass < 2; pass++) {
            int num = pass == 0 ? normalNum : darkNum;
            if (num > 0) {
                const char* name = NULL;
                if (stage < 2) {
                    name = sTexNames[stage][pass];
                }
                bool hasTex = false;
                if (m_resFile.ptr() != NULL && m_resFile.HasResTex()) {
                    hasTex = true;
                }
                if (hasTex) {
                    gfDrawSetVtxPosColorTexPrimEnvironment();
                    nw4r::g3d::ResTex tex = m_resFile.GetResTex(name);
                    GXTexObj texObj;
                    fn_8001AA88(&texObj, &tex);
                    fn_801F2C58(&texObj, 0, 0);
                    GXLoadTexObj(&texObj, GX_TEXMAP0);
                } else {
                    gfDrawSetVtxPosColorPrimEnvironment();
                }
                GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
                GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
                GXSetZMode(1, GX_LEQUAL, 1);
                stMarioPastBlockDt::drawBlocks(num, blocks, pass == 0 ? normalOrder : darkOrder, pass);
            }
        }
    }
}
