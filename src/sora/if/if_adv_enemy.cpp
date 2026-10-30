#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <if/if_adv_enemy.h>
#include <math.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <stdio.h>
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
struct IfAdvZakoMsgView {
    u8 unk0[0x8];
    void* m_message;
};

// HYPOTHESIS: entry layouts of the model and attachment tables.
struct IfAdvZakoModelInfo {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

struct IfAdvZakoScnMdlInfo {
    u8 m_start;
    u8 m_end;
    u8 m_objectNo;
    u8 m_capacity;
};

// HYPOTHESIS: the by-value argument of dispZako: the slot, the two gauge values, the message line and the texture source.
struct IfAdvZakoDispInfo {
    int m_index;
    float m_value;
    float m_max;
    u32 m_line;
    nw4r::g3d::ResFile m_resFile;
};

static const IfAdvZakoModelInfo lbl_804092E0[4] = {
    { "InfAdvPos_TopN", 0, 0, 0x20 },
    { "InfAdvEnemy0000_TopN", 1, 2, 0x21 },
    { "InfAdvEnemy0001_TopN", 3, 3, 0x22 },
    { "InfAdvEnemy0002_TopN", 4, 4, 0x22 },
};

static const IfAdvZakoScnMdlInfo lbl_80409300[3] = {
    { 0, 0, 0, 1 },
    { 1, 1, 1, 1 },
    { 2, 2, 2, 1 },
};

MuObject* lbl_805A0328;
nw4r::g3d::ScnMdlExpand* lbl_805A032C;

IfAdvZako* IfAdvZako::create(int arg, void* msgData) {
    return new (Heaps::InfoInstance) IfAdvZako(arg, msgData);
}

IfAdvZako::IfAdvZako(int arg, void* msgData) : unk24(arg), unk30(0), m_msgData(msgData) {
    m_msg = MuMsg::create(8, Heaps::InfoInstance, Heaps::MenuInstance);
    m_msg->allocMsgBuf(0x100, 2);
    for (int i = 0; i < 5; i++) {
        m_objects[i] = NULL;
    }
    for (int i = 0; i < 3; i++) {
        m_scnMdls[i] = NULL;
    }
}

IfAdvZako::~IfAdvZako() {
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

void IfAdvZako::createModel(nw4r::g3d::ResFile* resFile) {
    createObjResFile(lbl_804092E0, 4, resFile);
    createExpMdl(lbl_80409300, 3);
    MuObject* object;
    for (int i = 0; i < 2; i++) {
        object = m_objects[i];
        fn_800B20B0(object, true);
        fn_800B21C4(object, true);
    }
}

void IfAdvZako::createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile) {
    int num;
    int i;
    int j;
    const IfAdvZakoModelInfo* info = static_cast<const IfAdvZakoModelInfo*>(nodeInfo);
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

void IfAdvZako::createExpMdl(const void* nodeInfo, s32 nodeCount) {
    int num;
    int i;
    int j;
    const IfAdvZakoScnMdlInfo* info = static_cast<const IfAdvZakoScnMdlInfo*>(nodeInfo);
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

void IfAdvZako::createLocator(nw4r::g3d::ResFile* resFile) {
    const char* name = "InfAdvPos_TopN";
    lbl_805A0328 = MuObject::create(resFile, name, 0x20, NULL, Heaps::InfoInstance);
    char animName[0x40];
    strcpy(animName, name);
    strcat(animName, "__0");
    lbl_805A0328->changeAnimN(animName);
    lbl_805A0328->m_modelAnim->setUpdateRate(0.0f);
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        lbl_805A0328->setFrameNode(0.0f);
    } else {
        lbl_805A0328->setFrameNode(1.0f);
    }
    lbl_805A032C = nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::InfoInstance), NULL, 0x20,
                                                      lbl_805A0328->m_sceneModel);
}

void IfAdvZako::destroyLocator() {
    if (lbl_805A032C != NULL) {
        lbl_805A032C->Destroy();
        lbl_805A032C = NULL;
    }
    if (lbl_805A0328 != NULL) {
        delete lbl_805A0328;
        lbl_805A0328 = NULL;
    }
}

void IfAdvZako::startScene(nw4r::g3d::ScnGroup* group) {
    group->Insert(group->sceneItemsCount, lbl_805A032C);
}

void IfAdvZako::main() { }

// HYPOTHESIS: map name IfAdvZako::dispZako; the type of its by-value argument is unknown, so the address name stays.
extern "C" void fn_80108548(IfAdvZako* zako, IfAdvZakoDispInfo info) {
    u32 index;
    u32 line;
    char name[6] = { 0 };
    nw4r::g3d::ScnMdlExpand* group;
    MuObject* object;
    MuObject* gauge;
    if (info.m_index == 0) {
        group = zako->m_scnMdls[1];
        object = zako->m_objects[1];
        gauge = zako->m_objects[3];
    } else {
        info.m_index = 1;
        group = zako->m_scnMdls[2];
        object = zako->m_objects[2];
        gauge = zako->m_objects[4];
    }
    sprintf(name, "pos%02d", info.m_index + 1);
    lbl_805A032C->Remove(group);
    lbl_805A032C->PushBack(group, name);
    object->changeMaterialTex("enemyMat", 0, &info.m_resFile);
    line = info.m_line;
    index = info.m_index;
    zako->m_msg->detachMuObject(object);
    zako->m_msg->attachScnMdlSimple(index, object->m_sceneModel, 0, 0.0625f);
    zako->m_msg->setFontType(index, 4);
    zako->m_msg->setEdgeEnable(index, true);
    zako->m_msg->setEdge(index, 0xdd, 0xdd, 0xdd, 3.0f);
    zako->m_msg->setAlignMode(index, MuMsg::Align_Center);
    zako->m_msg->setFontColor(index, 0, 0, 0, 0xff);
    setBufField34__7MessageFii(reinterpret_cast<IfAdvZakoMsgView*>(zako->m_msg)->m_message, index, 7);
    zako->m_msg->printIndex(index, line, zako->m_msgData);
    group->Remove(gauge->m_sceneModel);
    if (info.m_value > 0.0f) {
        group->Remove(gauge->m_sceneModel);
        group->PushBack(gauge->m_sceneModel, "pos00");
        int angle = 45.0f * (info.m_value / info.m_max);
        Vec3f pos;
        pos.m_x = info.m_value * (float)tan(0.017453292f * angle) / 1.5f;
        pos.m_y = 0.0f;
        pos.m_z = 0.0f;
        gauge->setPos(&pos, "rightN");
    }
}

void IfAdvZako::erasesZako(int index) {
    lbl_805A032C->Remove(index == 0 ? m_scnMdls[1] : m_scnMdls[2]);
    MuObject* object = index == 0 ? m_objects[1] : m_objects[2];
    object->setFrameNode(0.0f);
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    object->setFrameMatCol(0.0f);
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
}

void IfAdvZako::zakoDamageOrDeath(int kind, int index) {
    MuObject* object = index == 0 ? m_objects[1] : m_objects[2];
    if (kind == 0) {
        object->changeAnimN("InfAdvEnemy0000_TopN__0");
    } else {
        object->changeAnimN("InfAdvEnemy0000_TopN__1");
    }
    object->setFrameNode(0.0f);
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    object->setFrameMatCol(0.0f);
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
}
