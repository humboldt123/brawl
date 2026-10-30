#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <if/if_adv_boss.h>
#include <math.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <string.h>
#include <types.h>

namespace nw4r {
namespace g3d {
// HYPOTHESIS: declaration prefix of the attachment group; its storage after ScnGroup is not modeled.
class ScnMdlExpand : public ScnGroup {
public:
    static ScnMdlExpand* Construct(MEMAllocator* allocator, u32* size, int capacity, ScnMdl* scnMdl);
    bool PushBack(ScnObj* object, const char* nodeName);
};
} // namespace g3d
} // namespace nw4r

// HYPOTHESIS: map names MuObject::enableSkipMatrix, enableSkipMaterial
extern "C" void fn_800B20B0(MuObject* object, bool enableSkip);
extern "C" void fn_800B21C4(MuObject* object, bool enableSkip);
// HYPOTHESIS: MuMsg::create with a third argument that is not a HeapType in the symbol
// HYPOTHESIS: Message::setBufField34 on the private message of MuMsg
extern "C" void setBufField34__7MessageFii(void* message, int index, int value);

// HYPOTHESIS: the private message pointer of MuMsg.
struct IfAdvBossMsgView {
    u8 unk0[0x8];
    void* m_message;
};

// HYPOTHESIS: entry layouts of the model and attachment tables.
struct IfAdvBossModelInfo {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

struct IfAdvBossScnMdlInfo {
    u8 m_start;
    u8 m_end;
    u8 m_objectNo;
    u8 m_capacity;
};

// HYPOTHESIS: the by-value argument of dispBoss: the slot, the two gauge values, the message line and the texture source.
struct IfAdvBossDispInfo {
    int m_index;
    float m_value;
    float m_max;
    u32 m_line;
    nw4r::g3d::ResFile m_resFile;
};

static const IfAdvBossModelInfo lbl_80409310[4] = {
    { "InfAdvPos_TopN", 0, 0, 0x10 },
    { "InfAdvBoss0000_TopN", 1, 2, 0x11 },
    { "InfAdvBoss0001_TopN", 3, 3, 0x12 },
    { "InfAdvBoss0002_TopN", 4, 4, 0x12 },
};

static const IfAdvBossScnMdlInfo lbl_80409330[3] = {
    { 0, 0, 0, 1 },
    { 1, 1, 1, 1 },
    { 2, 2, 2, 1 },
};

MuObject* lbl_805A0330;
nw4r::g3d::ScnMdlExpand* lbl_805A0334;

IfAdvBoss* IfAdvBoss::create(void* msgData) {
    return new (Heaps::InfoInstance) IfAdvBoss(msgData);
}

IfAdvBoss::IfAdvBoss(void* msgData) :
    unk24(0), m_msgData(msgData), unk2C(0), m_moveFrame(0), m_isMoveDisp(false), m_damageIndex(0), m_deathIndex(0) {
    m_msg = MuMsg::create(8, Heaps::InfoInstance, Heaps::MenuInstance);
    m_msg->allocMsgBuf(0x100, 2);
    for (int i = 0; i < 5; i++) {
        m_objects[i] = NULL;
    }
    for (int i = 0; i < 3; i++) {
        m_scnMdls[i] = NULL;
    }
    for (int i = 0; i < 2; i++) {
        m_isDamageDisp[i] = false;
        m_isDeathDisp[i] = false;
        m_damageFrame[i] = 0;
        m_deathFrame[i] = 0;
    }
}

IfAdvBoss::~IfAdvBoss() {
    for (int i = 0; i < 3; i++) {
        if (m_scnMdls[i] != NULL) {
            m_scnMdls[i]->Destroy();
        }
    }
    for (int i = 0; i < 5; i++) {
        delete m_objects[i];
    }
    if (m_msg != NULL) {
        delete m_msg;
        m_msg = NULL;
    }
}

void IfAdvBoss::createModel(nw4r::g3d::ResFile* resFile) {
    createObjResFile(lbl_80409310, 4, resFile);
    createExpMdl(lbl_80409330, 3);
    MuObject* object;
    for (int i = 0; i < 2; i++) {
        object = m_objects[i];
        fn_800B20B0(object, true);
        fn_800B21C4(object, true);
    }
}

void IfAdvBoss::createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile) {
    int num;
    int i;
    int j;
    const IfAdvBossModelInfo* info = static_cast<const IfAdvBossModelInfo*>(nodeInfo);
    MuObject* object;
    for (i = 0; i < nodeCount; i++, info++) {
        num = 1;
        if (info->m_start < info->m_end) {
            num = info->m_end - info->m_start + 1;
        }
        for (j = 0; j < num; j++) {
            object = MuObject::create(resFile, info->m_name, info->m_priority, NULL, Heaps::InfoInstance);
            m_objects[info->m_start + j] = object;
            char animName[0x40];
            strcpy(animName, info->m_name);
            strcat(animName, "__0");
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

void IfAdvBoss::createExpMdl(const void* nodeInfo, s32 nodeCount) {
    int num;
    int i;
    int j;
    const IfAdvBossScnMdlInfo* info = static_cast<const IfAdvBossScnMdlInfo*>(nodeInfo);
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::InfoInstance);
    for (i = 0; i < nodeCount; i++, info++) {
        num = 1;
        if (info->m_start < info->m_end) {
            num = info->m_end - info->m_start + 1;
        }
        for (j = 0; j < num; j++) {
            m_scnMdls[info->m_start + j] = nw4r::g3d::ScnMdlExpand::Construct(
                allocator, NULL, info->m_capacity, m_objects[info->m_objectNo + j]->m_sceneModel);
        }
    }
}

void IfAdvBoss::createLocator(nw4r::g3d::ResFile* resFile) {
    const char* name = "InfAdvPos_TopN";
    lbl_805A0330 = MuObject::create(resFile, name, 0x10, NULL, Heaps::InfoInstance);
    char animName[0x40];
    strcpy(animName, name);
    strcat(animName, "__0");
    lbl_805A0330->changeAnimN(animName);
    lbl_805A0330->m_modelAnim->setUpdateRate(0.0f);
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        lbl_805A0330->setFrameNode(0.0f);
    } else {
        lbl_805A0330->setFrameNode(1.0f);
    }
    lbl_805A0334 = nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::InfoInstance), NULL, 0x20,
                                                      lbl_805A0330->m_sceneModel);
}

void IfAdvBoss::destroyLocator() {
    if (lbl_805A0334 != NULL) {
        lbl_805A0334->Destroy();
        lbl_805A0334 = NULL;
    }
    if (lbl_805A0330 != NULL) {
        delete lbl_805A0330;
        lbl_805A0330 = NULL;
    }
}

void IfAdvBoss::startScene(nw4r::g3d::ScnGroup* group) {
    group->Insert(group->sceneItemsCount, lbl_805A0334);
}

// HYPOTHESIS: map name IfAdvBoss::main; IfMngr still calls it by its address name.
extern "C" void fn_80108E5C(IfAdvBoss* boss) {
    MuObject* object;
    if (boss->m_isDamageDisp[0]) {
        if (++boss->m_damageFrame[0] == 1) {
            object = boss->m_objects[1];
            object->changeAnimN("InfAdvBoss0000_TopN__0");
            object->setFrameNode(0.0f);
            object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            object->setFrameMatCol(0.0f);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        }
        if (boss->m_damageFrame[0] == 14) {
            boss->m_damageFrame[0] = 0;
            boss->m_isDamageDisp[0] = false;
        }
    }
    if (boss->m_isDeathDisp[0]) {
        if (++boss->m_deathFrame[0] == 1) {
            object = boss->m_objects[1];
            object->changeAnimN("InfAdvBoss0000_TopN__1");
            object->setFrameNode(0.0f);
            object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            object->setFrameMatCol(0.0f);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        }
        if (boss->m_deathFrame[0] == 60) {
            boss->m_deathFrame[0] = 0;
            boss->m_isDeathDisp[0] = false;
            lbl_805A0334->Remove(boss->m_scnMdls[1]);
            object = boss->m_objects[1];
            object->setFrameNode(0.0f);
            object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
            object->setFrameMatCol(0.0f);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        }
    }
    if (boss->m_isDamageDisp[1]) {
        if (++boss->m_damageFrame[1] == 1) {
            object = boss->m_objects[2];
            object->changeAnimN("InfAdvBoss0000_TopN__0");
            object->setFrameNode(0.0f);
            object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            object->setFrameMatCol(0.0f);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        }
        if (boss->m_damageFrame[1] == 14) {
            boss->m_damageFrame[1] = 0;
            boss->m_isDamageDisp[1] = false;
        }
    }
    if (boss->m_isDeathDisp[1]) {
        if (++boss->m_deathFrame[1] == 1) {
            object = boss->m_objects[2];
            object->changeAnimN("InfAdvBoss0000_TopN__1");
            object->setFrameNode(0.0f);
            object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            object->setFrameMatCol(0.0f);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        }
        if (boss->m_deathFrame[1] == 60) {
            boss->m_deathFrame[1] = 0;
            boss->m_isDeathDisp[1] = false;
            lbl_805A0334->Remove(boss->m_scnMdls[2]);
            object = boss->m_objects[2];
            object->setFrameNode(0.0f);
            object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
            object->setFrameMatCol(0.0f);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        }
    }
    if (!boss->m_isMoveDisp) {
        boss->m_moveFrame = 0;
        int frame = boss->m_moveFrame;
        if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            lbl_805A0330->setFrameNode(2.0f + frame);
        } else {
            lbl_805A0330->setFrameNode(10.0f + frame);
        }
    } else {
        if (boss->m_moveFrame < 8) {
            int frame = boss->m_moveFrame;
            if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
                lbl_805A0330->setFrameNode(2.0f + frame);
            } else {
                lbl_805A0330->setFrameNode(10.0f + frame);
            }
            boss->m_moveFrame++;
        }
    }
}

// HYPOTHESIS: map name IfAdvBoss::dispBoss; the type of its by-value argument is unknown, so the address name stays.
extern "C" void fn_801092E0(IfAdvBoss* boss, IfAdvBossDispInfo info) {
    u32 index;
    u32 line;
    char name[6] = { 0 };
    nw4r::g3d::ScnMdlExpand* group;
    MuObject* object;
    MuObject* gauge;
    if (info.m_index == 0) {
        group = boss->m_scnMdls[1];
        object = boss->m_objects[1];
        gauge = boss->m_objects[3];
        memcpy(name, "pos06", 5);
    } else {
        info.m_index = 1;
        group = boss->m_scnMdls[2];
        object = boss->m_objects[2];
        gauge = boss->m_objects[4];
        memcpy(name, "pos08", 5);
    }
    object->changeMaterialTex("lambert40", 0, &info.m_resFile);
    lbl_805A0334->Remove(group);
    lbl_805A0334->PushBack(group, name);
    line = info.m_line;
    index = info.m_index;
    boss->m_msg->detachMuObject(object);
    boss->m_msg->attachScnMdlSimple(index, object->m_sceneModel, 0, 0.0625f);
    boss->m_msg->setEdgeEnable(index, true);
    boss->m_msg->setEdge(index, 0xdd, 0xdd, 0xdd, 3.0f);
    boss->m_msg->setAlignMode(index, MuMsg::Align_Left);
    boss->m_msg->setFontColor(index, 0, 0, 0, 0xff);
    boss->m_msg->setFontType(index, 4);
    setBufField34__7MessageFii(reinterpret_cast<IfAdvBossMsgView*>(boss->m_msg)->m_message, index, 7);
    boss->m_msg->printIndex(index, line, boss->m_msgData);
    group->Remove(gauge->m_sceneModel);
    if (info.m_value > 0.0f) {
        group->Remove(gauge->m_sceneModel);
        group->PushBack(gauge->m_sceneModel, "pos00");
        int angle = 45.0f * (info.m_value / info.m_max);
        float x = 50.0f * info.m_value / info.m_max;
        tan(0.017453292f * angle);
        Vec3f pos;
        pos.m_x = x;
        pos.m_y = 0.0f;
        pos.m_z = 0.0f;
        gauge->setPos(&pos, "rightN");
    }
}

// HYPOTHESIS: map name IfAdvBoss::eraseBoss; IfMngr still calls it by its address name.
extern "C" void fn_80109540(IfAdvBoss* boss, int index) {
    lbl_805A0334->Remove(index == 0 ? boss->m_scnMdls[1] : boss->m_scnMdls[2]);
    MuObject* object = index == 0 ? boss->m_objects[1] : boss->m_objects[2];
    object->setFrameNode(0.0f);
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    object->setFrameMatCol(0.0f);
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
}
