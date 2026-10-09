#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL
#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the stage RELs that also hold the damage module
#define SO_COLLISION_GROUP_ALIGNED // soArrayVector<soCollisionGroup, 30> places its elements 4-aligned
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies it member by member

#include <cm/cm_quake.h>
#include <ec/ec_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <snd/snd_system.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>
#include <yk/yk_normal.h>

#include <st_dxgreens/gr_dxgreens.h>

// Yakumono::deactivate, Yakumono::clearAttack and Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_263464(Yakumono* yakumono);
extern "C" void fn_27_263750(Yakumono* yakumono);
extern "C" void fn_27_26399C(Yakumono* yakumono);

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct dxgreensHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// MATCH-ONLY: layout of soSet<T> (its members are private)
struct dxgreensSetView {
    void* m_elements;
    u32 m_size;
};

// MATCH-ONLY: a flag of the Yakumono the gimmick switches on after creating it
struct dxgreensYakumonoFlags {
    u8 _pad[0xA4];
    unsigned char m_flag : 1;
    unsigned char m_rest : 7;
};

grDxGreensBlockPos* grDxGreensBlockPos::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxGreensBlockPos* ground = new (Heaps::StageInstance) grDxGreensBlockPos(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxGreensBlockPos::grDxGreensBlockPos(const char* taskName) : grDxGreens(taskName) {
    m_blockData = NULL;
    m_posBlock = NULL;
    m_posMade = 0;
    memset(m_attackOn, 0, sizeof(m_attackOn));
    memset(m_posX, 0, sizeof(m_posX));
    m_yakumonoMade = 0;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_simpleSets = NULL;
    m_dataGroups = NULL;
    m_data = NULL;
    m_workAttack = NULL;
    m_yakumonoAttack = NULL;
    memset(m_node, 0, sizeof(m_node));
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grDxGreensBlockPos::~grDxGreensBlockPos() {
    if (m_hitData != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitData);
    }
    m_hitData = NULL;
    if (m_hitSimple != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitSimple);
    }
    m_hitSimple = NULL;
    if (m_simpleSets != NULL) {
        delete[] reinterpret_cast<u8*>(m_simpleSets);
    }
    m_simpleSets = NULL;
    if (m_dataGroups != NULL) {
        delete[] m_dataGroups;
    }
    m_dataGroups = NULL;
    if (m_data != NULL) {
        delete m_data;
    }
    m_data = NULL;
    if (m_workAttack != NULL) {
        delete m_workAttack;
    }
    m_workAttack = NULL;
}

void grDxGreensBlockPos::preExit() {
    if (m_yakumonoAttack != NULL) {
        fn_27_263464(m_yakumonoAttack);
    }
    grYakumono::preExit();
}

void grDxGreensBlockPos::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updatePos(deltaFrame);
    }
}

// Creates the hit objects the first time (and switches the hit spheres off) and afterwards follows the state of the blocks:
// a block that was hit (3) breaks (4) and, when it is a hard one, attacks; a block at rest (2) can be hit; a block that
// is gone (7) takes its attack away.
void grDxGreensBlockPos::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade == 1) {
        for (u8 column = 0; column < 6; column++) {
            for (u8 row = 0; row < 5; row++) {
                stDxGreensBlockData* block = &m_blockData[row + column * 5];
                switch (block->m_state) {
                case 2:
                    enableHit(block->m_index + column * 5, 0);
                    break;
                case 3:
                    block->m_state = 4;
                    if (block->m_hard == 1) {
                        setAttack(column, row);
                        m_attackOn[column * 5 + row] = 1;
                        Vec3f offset(0.0f, 0.0f, 0.0f);
                        cmReqQuake(cmQuake::Amplitude_L, &offset);
                    } else {
                        Vec3f offset(0.0f, 0.0f, 0.0f);
                        cmReqQuake(cmQuake::Amplitude_S, &offset);
                        g_sndSystem->playSE(SndID(0x1DBA), 0, 0, 0, -1);
                    }
                    break;
                case 7:
                    if (m_attackOn[column * 5 + row] == 1) {
                        fn_27_263750(m_yakumonoAttack);
                        m_attackOn[column * 5 + row] = 0;
                    }
                    break;
                }
            }
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
            for (u8 column = 0; column < 6; column++) {
                for (u8 row = 0; row < 5; row++) {
                    disableHit(row + column * 5, 0);
                }
            }
        }
    }
}

// Publishes the positions of the blocks (the node plus a few units) to the stage the first time.
void grDxGreensBlockPos::updatePos(float deltaFrame) {
    if (m_posMade != 1) {
        for (u8 column = 0; column < 6; column++) {
            for (u8 row = 0; row < 5; row++) {
                Vec3f pos;
                getNodePosition(&pos, 0, m_node[column * 5 + row]);
                pos.m_y += 3.75f;
                Vec3f* dst = &m_posBlock[row + column * 5];
                dst->m_x = pos.m_x;
                dst->m_y = pos.m_y;
                dst->m_z = pos.m_z;
                m_posX[column * 5 + row] = pos.m_x;
            }
        }
        m_posMade = 1;
    }
}

bool grDxGreensBlockPos::setNode() {
    bool result = grGimmick::setNode();
    if (result) {
        m_node[0] = getNodeIndex(0, "BlockN3");
        m_node[1] = getNodeIndex(0, "BlockN4");
        m_node[2] = getNodeIndex(0, "BlockN9");
        m_node[3] = getNodeIndex(0, "BlockN10");
        m_node[4] = getNodeIndex(0, "BlockN15");
        m_node[5] = getNodeIndex(0, "BlockN2");
        m_node[6] = getNodeIndex(0, "BlockN5");
        m_node[7] = getNodeIndex(0, "BlockN8");
        m_node[8] = getNodeIndex(0, "BlockN11");
        m_node[9] = getNodeIndex(0, "BlockN14");
        m_node[10] = getNodeIndex(0, "BlockN1");
        m_node[11] = getNodeIndex(0, "BlockN6");
        m_node[12] = getNodeIndex(0, "BlockN7");
        m_node[13] = getNodeIndex(0, "BlockN12");
        m_node[14] = getNodeIndex(0, "BlockN13");
        m_node[15] = getNodeIndex(0, "BlockN28");
        m_node[16] = getNodeIndex(0, "BlockN27");
        m_node[17] = getNodeIndex(0, "BlockN22");
        m_node[18] = getNodeIndex(0, "BlockN21");
        m_node[19] = getNodeIndex(0, "BlockN16");
        m_node[20] = getNodeIndex(0, "BlockN29");
        m_node[21] = getNodeIndex(0, "BlockN26");
        m_node[22] = getNodeIndex(0, "BlockN23");
        m_node[23] = getNodeIndex(0, "BlockN20");
        m_node[24] = getNodeIndex(0, "BlockN17");
        m_node[25] = getNodeIndex(0, "BlockN30");
        m_node[26] = getNodeIndex(0, "BlockN25");
        m_node[27] = getNodeIndex(0, "BlockN24");
        m_node[28] = getNodeIndex(0, "BlockN19");
        m_node[29] = getNodeIndex(0, "BlockN18");
    }
    return result;
}

// Builds the hit objects: the first one has a hit sphere on every block node (and the damage module that tells which one
// was hit), the second one holds the attacks of the blocks and follows the first one.
void grDxGreensBlockPos::setHit() {
    // MATCH-ONLY: the original allocates the arrays of the spheres as raw storage (no element construction and no array cookie)
    m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData) * 30]);
    m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple) * 30]);
    m_simpleSets = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(new (Heaps::StageInstance) u8[sizeof(dxgreensSetView) * 30]);
    m_dataGroups = new (Heaps::StageInstance) ykDataGroup[30];
    m_data = new (Heaps::StageInstance) ykData;
    Vec3f pos(0.0f, 0.0f, 0.0f);
    u8 index = 0;
    for (u8 column = 0; column < 6; column++) {
        for (u8 row = 0; row < 5; row++) {
            u8 idx = index;
            index++;
            soCollisionHitData* hit = &m_hitData[idx];
            hit->m_startOffsetPos.m_x = 0.0f;
            hit->m_startOffsetPos.m_y = 3.75f;
            hit->m_startOffsetPos.m_z = 0.0f;
            hit->m_endOffsetPos.m_x = 0.0f;
            hit->m_endOffsetPos.m_y = 3.75f;
            hit->m_endOffsetPos.m_z = 0.0f;
            hit->m_size = 6.0f;
            // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
            reinterpret_cast<dxgreensHitByte*>(hit)->m_shape = 1;
            soCollisionHitData::Simple* simple = &m_hitSimple[idx];
            // MATCH-ONLY: the positions are copied word by word
            u32* srcWords = reinterpret_cast<u32*>(hit);
            u32* dstWords = reinterpret_cast<u32*>(simple);
            for (int w = 0; w < 6; w += 3) {
                u32 w0 = srcWords[w];
                u32 w1 = srcWords[w + 1];
                dstWords[w] = w0;
                dstWords[w + 1] = w1;
                dstWords[w + 2] = srcWords[w + 2];
            }
            simple->m_size = hit->m_size;
            reinterpret_cast<u8*>(simple)[0x1c] = reinterpret_cast<u8*>(hit)[0x1c];
            simple->m_height = soCollisionHitData::Height_Low;
            simple->m_nodeIndex = m_node[column * 5 + row];
            // MATCH-ONLY: the members of soSet are private
            dxgreensSetView* set = reinterpret_cast<dxgreensSetView*>(m_simpleSets) + idx;
            set->m_elements = simple;
            set->m_size = 1;
            m_dataGroups[idx].m_hitDataSimpleSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(set);
            m_dataGroups[idx].m_hitGroupIndex = idx;
        }
    }
    m_data->m_dataGroups = m_dataGroups;
    m_data->m_dataGroupNum = 30;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    info.m_pos = &pos;
    info.m_work = m_data;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 30, 30, soCollisionHitModuleImpl, 0x3FF, true> >
        HitConfig;
    HitConfig* yakumono = new (Heaps::StageInstance) HitConfig(&info);
    setYakumono(yakumono);

    m_workAttack = new (Heaps::StageInstance) grDxGreensWork;
    m_workAttack->unk0 = 0;
    m_workAttack->unk4 = 0;
    ykInitInfo infoAttack = { 0, 0, 0x11, 0, 0 };
    infoAttack.m_ground = this;
    infoAttack.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    infoAttack.m_pos = &pos;
    infoAttack.m_work = m_workAttack;
    typedef ykNoHitNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 30, 0, soCollisionAttackModuleImpl, 1, false, true> >
        AttackConfig;
    AttackConfig* attack = new (Heaps::StageInstance) AttackConfig(&infoAttack);
    m_yakumonoAttack = attack;
    reinterpret_cast<dxgreensYakumonoFlags*>(attack)->m_flag = 1;
    attack->initAttackPosXWork(reinterpret_cast<int>(m_posX), 30);
    attack->m_connectedTask = this;
    if (m_attachedTask == NULL) {
        m_attachedTask = attack;
    } else {
        gfTask* last = m_attachedTask;
        while (last->m_nextTask != NULL) {
            last = last->m_nextTask;
        }
        last->m_nextTask = attack;
    }
}

// The block breaks: it makes the stage shake and burns an attack of its own when it is a hard one.
void grDxGreensBlockPos::setAttack(u8 column, u8 row) {
    u32 idx = column * 5 + row;
    if (m_attackOn[idx] != 1) {
        soCollisionAttackData attack(1.0f);
        stDxGreensBlockData* block = &m_blockData[idx];
        if (block != NULL) {
            Vec3f offset;
            offset.m_x = block->m_pos.m_x;
            offset.m_y = block->m_pos.m_y;
            offset.m_z = 0.0f;
            setAttackGimmickDetails(&attack, 15.0f, 1.0f, 1.0f, 1.0f,
                20, &offset, 361, 50, 0, 78, 0,
                0x3FF, 7, false, 15,
                soCollisionAttackData::Attribute_Fire, soCollisionAttackData::Sound_Level_Large,
                soCollisionAttackData::Sound_Attribute_Fire,
                false, false, false, true, false, false, 0, 60,
                false, false, false, soCollisionAttackData::Lr_Check_Pos,
                false, false, false, false, false, soCollisionAttackData::Region_None, false);
            m_yakumonoAttack->setAttack(idx, 0, &attack);
            m_attackOn[idx] = 1;
        }
    }
}

// A fighter hit one of the blocks (the index of the hit sphere tells which one): it breaks, the stage shakes and the hit
// sphere is switched off.
void grDxGreensBlockPos::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    fn_27_26399C(m_yakumono);
    disableHit(index, 0);
    int column;
    u8 row;
    if (index < 5) {
        column = 0;
        row = index;
    } else if (index < 10) {
        column = 1;
        row = index - 5;
    } else if (index < 15) {
        column = 2;
        row = index - 10;
    } else if (index < 20) {
        column = 3;
        row = index - 15;
    } else if (index < 25) {
        column = 4;
        row = index - 20;
    } else {
        column = 5;
        row = index - 25;
    }
    for (u8 i = 0; i < 5; i++) {
        stDxGreensBlockData* block = &m_blockData[i + column * 5];
        if (block->m_index == row && block->m_state == 2) {
            block->m_state = 4;
            if (block->m_hard == 1) {
                setAttack(column, row);
                Vec3f offset(0.0f, 0.0f, 0.0f);
                cmReqQuake(cmQuake::Amplitude_L, &offset);
                g_ecMgr->setEffect(static_cast<EfID>(0x5E0003), &block->m_posTgt);
                g_sndSystem->playSE(SndID(0x50), 0, 0, 0, -1);
            } else {
                Vec3f offset(0.0f, 0.0f, 0.0f);
                cmReqQuake(cmQuake::Amplitude_S, &offset);
                g_ecMgr->setEffect(static_cast<EfID>(0x5E0004), &block->m_posTgt);
                g_sndSystem->playSE(SndID(0x1DBA), 0, 0, 0, -1);
            }
        }
    }
}
