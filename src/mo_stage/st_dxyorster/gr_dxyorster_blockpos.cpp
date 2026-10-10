#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL
#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the stage RELs that also hold the damage module (the stage RELs of the hit objects with a hit module)
#define SO_COLLISION_GROUP_ALIGNED // soArrayVector<soCollisionGroup, 9> places its elements 4-aligned
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies it member by member

#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <string.h>

#include <st_dxyorster/gr_dxyorster.h>
#include <yk/yk_normal.h>

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct dxyorsterHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// MATCH-ONLY: a position copied as three words
struct dxyorsterWords3 {
    u32 w0, w1, w2;
};

// MATCH-ONLY: layout of soSet<T> (its members are private)
struct dxyorsterSetView {
    void* m_elements;
    u32 m_size;
};

grDxYorsterBlockPos* grDxYorsterBlockPos::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxYorsterBlockPos* ground = new (Heaps::StageInstance) grDxYorsterBlockPos(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxYorsterBlockPos::grDxYorsterBlockPos(const char* taskName) : grDxYorster(taskName) {
    m_yakumonoMade = 0;
    m_posGimmickWork = NULL;
    m_dmgWork = NULL;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_simpleSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
    memset(m_node, 0, sizeof(m_node));
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grDxYorsterBlockPos::~grDxYorsterBlockPos() {
    if (m_hitData != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitData);
    }
    m_hitData = NULL;
    if (m_hitSimple != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitSimple);
    }
    m_hitSimple = NULL;
    if (m_simpleSet != NULL) {
        delete m_simpleSet;
    }
    m_simpleSet = NULL;
    if (m_dataGroup != NULL) {
        delete m_dataGroup;
    }
    m_dataGroup = NULL;
    if (m_data != NULL) {
        delete m_data;
    }
    m_data = NULL;
}

// Publishes the positions of the nine block nodes to the stage.
void grDxYorsterBlockPos::processAnim() {
    Ground::processAnim();
    if (m_posGimmickWork != NULL) {
        getNodePosition(&m_posGimmickWork[0], 0, m_node[0]);
        getNodePosition(&m_posGimmickWork[1], 0, m_node[1]);
        getNodePosition(&m_posGimmickWork[2], 0, m_node[2]);
        getNodePosition(&m_posGimmickWork[3], 0, m_node[3]);
        getNodePosition(&m_posGimmickWork[4], 0, m_node[4]);
        getNodePosition(&m_posGimmickWork[5], 0, m_node[5]);
        getNodePosition(&m_posGimmickWork[6], 0, m_node[6]);
        getNodePosition(&m_posGimmickWork[7], 0, m_node[7]);
        getNodePosition(&m_posGimmickWork[8], 0, m_node[8]);
    }
}

void grDxYorsterBlockPos::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
    }
}

// Creates the hit object (nine hit spheres on the block nodes) the first time and afterwards turns the hit spheres of the
// blocks on and off from the damage bytes of the stage (1 = a block was hit, 100 = it may be hit again).
void grDxYorsterBlockPos::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade == 1) {
        for (u32 i = 0; i < 9; i++) {
            switch (m_dmgWork[(u8)i]) {
            case 1:
                disableHit((u8)i, 0);
                m_dmgWork[(u8)i] = 2;
                break;
            case 2:
                break;
            case 100:
                enableHit((u8)i, 0);
                m_dmgWork[(u8)i] = 0;
                break;
            }
        }
    } else {
        // MATCH-ONLY: the original allocates both arrays as raw storage (no element construction and no array cookie)
        m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData) * 9]);
        m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple) * 9]);
        m_simpleSet = new (Heaps::StageInstance) soSet<soCollisionHitData::Simple>;
        m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
        m_data = new (Heaps::StageInstance) ykData;
        Vec3f pos = Vec3f(0.0f, 0.0f, 0.0f);
        for (u32 i = 0; i < 9; i++) {
            u8 idx = i;
            m_hitData[idx].m_startOffsetPos.m_x = 0.0f;
            m_hitData[idx].m_startOffsetPos.m_y = 0.0f;
            m_hitData[idx].m_startOffsetPos.m_z = 0.0f;
            m_hitData[idx].m_endOffsetPos.m_x = 0.0f;
            m_hitData[idx].m_endOffsetPos.m_y = 0.0f;
            m_hitData[idx].m_endOffsetPos.m_z = 0.0f;
            m_hitData[idx].m_size = 5.0f;
            // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
            reinterpret_cast<dxyorsterHitByte*>(&m_hitData[idx])->m_shape = 0;
            soCollisionHitData* hit = &m_hitData[idx];
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
            m_hitSimple[idx].m_height = soCollisionHitData::Height_Low;
            m_hitSimple[idx].m_nodeIndex = m_node[idx];
        }
        // MATCH-ONLY: the members of soSet are private
        reinterpret_cast<dxyorsterSetView*>(m_simpleSet)->m_elements = m_hitSimple;
        reinterpret_cast<dxyorsterSetView*>(m_simpleSet)->m_size = 9;
        m_dataGroup->m_hitDataSimpleSet = m_simpleSet;
        m_dataGroup->m_hitGroupIndex = 0;
        m_data->m_dataGroupNum = 1;
        m_data->m_dataGroups = m_dataGroup;

        ykInitInfo info = { 0, 0, 0x10, 0, 0 };
        info.m_ground = this;
        info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
        info.m_pos = &pos;
        info.m_work = m_data;
        typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                         soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 9, 9, soCollisionHitModuleImpl, 0x3FF, true> >
            Config;
        Config* yakumono = new (Heaps::StageInstance) Config(&info);
        yakumono->setCollisionHitSelfCatagory(9);
        setYakumono(yakumono);
    }
    m_yakumonoMade = 1;
}

bool grDxYorsterBlockPos::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "KurukuruBlockA");
    getNodeIndex(&m_node[1], 0, "KurukuruBlockB");
    getNodeIndex(&m_node[2], 0, "KurukuruBlockC");
    getNodeIndex(&m_node[3], 0, "KurukuruBlockD");
    getNodeIndex(&m_node[4], 0, "KurukuruBlockE");
    getNodeIndex(&m_node[5], 0, "KurukuruBlockF");
    getNodeIndex(&m_node[6], 0, "KurukuruBlockG");
    getNodeIndex(&m_node[7], 0, "KurukuruBlockH");
    getNodeIndex(&m_node[8], 0, "KurukuruBlockI");
    return result;
}

// A fighter hit one of the blocks: the stage learns which one from the damage record.
void grDxYorsterBlockPos::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    fn_27_26399C(m_yakumono);
    if (m_dmgWork != NULL) {
        m_dmgWork[reinterpret_cast<u8*>(damage)[0x35]] = 1;
    }
}
