#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>
#include <yk/yk_normal.h>

#include <st_plankton/gr_plankton.h>

grPlanktonAshibaLeft* grPlanktonAshibaLeft::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPlanktonAshibaLeft* ground = new (Heaps::StageInstance) grPlanktonAshibaLeft(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPlanktonAshibaLeft::grPlanktonAshibaLeft(const char* taskName) : grPlankton(taskName) {
    setRot(0.0f, 0.0f, 0.0f);
    strcpy(m_tgtNodeTop, "");
    unk1D8 = 0;
    unk1D9 = 100;
    m_rotTimer = 0.0f;
    m_dir = 1;
    m_hasYakumono = 0;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(0x20);
}

grPlanktonAshibaLeft::~grPlanktonAshibaLeft() {
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

void grPlanktonAshibaLeft::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        updateYakumono(deltaFrame);
        updateRot(deltaFrame);
        updateCallback(0);
    }
}

// Makes the hit object of the leaf: one capsule from the node of the base to the node of the top, once.
void grPlanktonAshibaLeft::updateYakumono(float deltaFrame) {
    if (m_hasYakumono != 1) {
        // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
        m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData)]);
        m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple)]);
        m_hitSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(new (Heaps::StageInstance) u8[sizeof(grPlanktonSetView)]);
        m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
        m_data = new (Heaps::StageInstance) ykData;
        Vec3f topPos;
        Vec3f basePos;
        getNodePosition(&topPos, 0, m_nodeTop);
        getNodePosition(&basePos, 0, m_nodeIndex);
        Vec3f diff = topPos - basePos;
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

// The leaf goes back to its rest angle step by step, once it was not hit for a while; it stays inside its limits.
void grPlanktonAshibaLeft::updateRot(float deltaFrame) {
    stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
    if (data != NULL) {
        m_rotTimer = m_rotTimer - deltaFrame;
        if (m_rotTimer < 0.0f) {
            m_rotTimer = 0.0f;
        }
        Vec3f next;
        Vec3f rot = getRot();
        next = rot;
        float z = rot.m_z;
        if (m_rotTimer == 0.0f) {
            float target = data->unk2C;
            if (z != target) {
                next.m_z = target;
                if (z > target) {
                    z = z - data->unk48;
                    if (z >= target) {
                        m_rotTimer = data->unk44;
                        next.m_z = z;
                    }
                } else {
                    z = z + data->unk48;
                    if (z <= target) {
                        m_rotTimer = data->unk44;
                        next.m_z = z;
                    }
                }
            }
        } else {
            if (z < data->unk40) {
                next.m_z = data->unk40;
            }
            if (data->unk3C < next.m_z) {
                next.m_z = data->unk3C;
            }
        }
        setRot(rot.m_x, rot.m_y, next.m_z);
    }
}

void grPlanktonAshibaLeft::updateCallback(u32 index) {
    if ((int)m_calcWorldCallBack.m_numNodeCallbackData > 0) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[index];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                m_calcWorldCallBack.m_index = 0;
                scnMdl->m_calcWorldCallBack = &m_calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = m_calcWorldCallBack.m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f rot;
            rot = getRot();
            m_calcWorldCallBack.m_nodeCallbackDatas[0].m_offsetRot.m_x = rot.m_x;
            m_calcWorldCallBack.m_nodeCallbackDatas[0].m_offsetRot.m_y = rot.m_y;
            m_calcWorldCallBack.m_nodeCallbackDatas[0].m_offsetRot.m_z = rot.m_z;
        }
    }
}

bool grPlanktonAshibaLeft::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeTop, 0, getTgtNodeTop());
    return result;
}

// A hit turns the leaf (a hard hit twice as far); it changes the direction it turns at its limits.
void grPlanktonAshibaLeft::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
    if (data != NULL) {
        float amount = damage->unk4;
        fn_27_26399C(m_yakumono);
        if (amount != 0.0f) {
            Vec3f next;
            Vec3f rot = getRot();
            next = rot;
            float push = 0.0f;
            if (amount < data->unk30) {
                if (m_dir == 1) {
                    push += data->unk38;
                } else {
                    push -= data->unk38;
                }
            } else if (amount >= data->unk34) {
                push += data->unk38 * 2.0f;
            }
            next.m_z = rot.m_z + push;
            setRot(rot.m_x, rot.m_y, next.m_z);
            if (next.m_z < data->unk40) {
                m_dir = 1;
            }
            if (next.m_z > data->unk3C) {
                m_dir = 0;
            }
            m_rotTimer = data->unk44;
        }
    }
}

void grPlanktonAshibaLeft::setTgtNodeTop(const char* name) {
    if (name != NULL) {
        strcpy(m_tgtNodeTop, "");
        strncpy(m_tgtNodeTop, name, 0x7F);
    }
}

void grPlanktonAshibaLeft::initAshibaData() {
    stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
    if (data != NULL) {
        Vec3f next;
        Vec3f rot = getRot();
        next.m_x = rot.m_x;
        next.m_y = rot.m_y;
        next.m_z = data->unk2C;
        setRot(rot.m_x, rot.m_y, next.m_z);
    }
}
