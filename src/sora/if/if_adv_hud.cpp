#include <gf/gf_heap_manager.h>
#include <if/if_adv_gauge.h>
#include <if/if_player.h>
#include <mu/menu.h>
#include <mt/mt_vector.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <gm/gm_global.h>
#include <gf/gf_3d_scene.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <math.h>
#include <stdlib.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
MuObject* lbl_805A0338 = NULL;
nw4r::g3d::ScnMdlExpand* lbl_805A033C = NULL;
MuObject* lbl_805A0340 = NULL;
nw4r::g3d::ScnMdlExpand* lbl_805A0344 = NULL;

// HYPOTHESIS: table layouts are shared with the adventure enemy HUD.
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


static const IfAdvZakoModelInfo lbl_804093B0[7] = {
    { "InfAdvPos_TopN", 0, 0, 1 },
    { "InfAdvScore0000_TopN", 1, 1, 2 },
    { "InfAdvScore0001_TopN", 2, 11, 3 },
    { "InfAdvCrowd0000_TopN", 12, 12, 4 },
    { "InfAdvCrowd0001_TopN", 13, 63, 5 },
    { "InfAdvPsta0000_TopN", 64, 64, 6 },
    { "InfAdvStock0000_TopN", 65, 65, 7 },
};

static const IfAdvZakoScnMdlInfo lbl_804093E8[3] = {
    { 0, 0, 0, 8 },
    { 1, 1, 1, 9 },
    { 2, 2, 12, 50 },
};

IfAdvGauge* IfAdvGauge::create() {
    return new (Heaps::InfoInstance) IfAdvGauge();
}

IfAdvGauge::IfAdvGauge() {
    for (int index = 0; index < 0x55; index++) {
        m_objects[index] = NULL;
    }
    for (int index = 0; index < 7; index++) {
        m_models[index] = NULL;
    }
    m_pressStartFrame = 0;
    for (int index = 0; index < 50; index++) {
        m_crowdDelFrame[index] = 0;
        m_crowdDelActive[index] = 0;
    }
    m_faceOffsetX = 0.0f;
    m_faceOffsetY = 0.0f;
    m_faceAngle = 0.0f;
    unk358 = 0.0f;
    m_faceSway = 0.0f;
    m_crowdDelIndex = 13;
    m_crowdCount = 0;
    m_isScoreVisible = 1;
    m_isTimeVisible = 1;
}

IfAdvGauge::~IfAdvGauge() {
    for (int index = 0; index < 7; index++) {
        if (m_models[index] != NULL) {
            m_models[index]->Destroy();
        }
    }
    for (int index = 0; index < 0x55; index++) {
        delete m_objects[index];
    }
    g_gfSceneRoot->removeInfoCamResAnm();
}

void IfAdvGauge::createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile) {
    int count;
    int index;
    int instance;
    const IfAdvZakoModelInfo* info = static_cast<const IfAdvZakoModelInfo*>(nodeInfo);
    MuObject* object;
    for (index = 0; index < nodeCount; index++, info++) {
        count = 1;
        if (info->m_start < info->m_end) {
            count = info->m_end - info->m_start + 1;
        }
        for (instance = 0; instance < count; instance++) {
            object = MuObject::create(resFile, info->m_name, info->m_priority, NULL, Heaps::InfoInstance);
            m_objects[info->m_start + instance] = object;
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

void IfAdvGauge::createExpMdl(const void* nodeInfo, s32 nodeCount) {
    int count;
    int index;
    int instance;
    const IfAdvZakoScnMdlInfo* info = static_cast<const IfAdvZakoScnMdlInfo*>(nodeInfo);
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::InfoInstance);
    for (index = 0; index < nodeCount; index++, info++) {
        count = 1;
        if (info->m_start < info->m_end) {
            count = info->m_end - info->m_start + 1;
        }
        for (instance = 0; instance < count; instance++) {
            m_models[info->m_start + instance] = nw4r::g3d::ScnMdlExpand::Construct(
                allocator, NULL, info->m_capacity, m_objects[info->m_objectNo + instance]->m_sceneModel);
        }
    }
}

void IfAdvGauge::createModel(nw4r::g3d::ResFile* resFile) {
    MuObject* object;
    createObjResFile(lbl_804093B0, 7, resFile);
    createExpMdl(lbl_804093E8, 3);
    int objectNos[5] = { 0, 1, 12, 64, 65 };
    int* nos = objectNos;
    for (int index = 0; index < 5; index++, nos++) {
        object = m_objects[*nos];
        object->enableSkipMatrix(true);
        object->enableSkipMaterial(true);
    }
}

static const IfAdvZakoModelInfo lbl_80409408[3] = {
    { "InfPos_TopN", 0x54, 0x54, 9 },
    { "InfTime_TopN", 0x4b, 0x4b, 8 },
    { "InfTimenum_TopN", 0x4c, 0x52, 8 },
};

static const IfAdvZakoScnMdlInfo lbl_805A2878[2] = {
    { 6, 6, 0x54, 4 },
    { 5, 5, 0x4b, 8 },
};

static const IfAdvZakoModelInfo lbl_805A2880[1] = {
    { "InfAdvStock0001_TopN", 0x43, 0x4a, 7 },
};

static const IfAdvZakoScnMdlInfo lbl_805A2888[2] = {
    { 4, 4, 0x4a, 1 },
    { 3, 3, 0x41, 10 },
};

void IfAdvGauge::createModel2(nw4r::g3d::ResFile* resFile) {
    createObjResFile(lbl_80409408, 3, resFile);
    createExpMdl(lbl_805A2878, 2);
}

void IfAdvGauge::createModelStRes(nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* stResFile) {
    createObjResFile(lbl_805A2880, 1, resFile);
    m_stockResFile = stResFile;
    createExpMdl(lbl_805A2888, 2);
}

// HYPOTHESIS: map name IfAdvGauge::main; IfAdvMngr still calls it by its address name.
extern "C" void fn_8010BDCC(IfAdvGauge* gauge) {
    gauge->crowdDelAnimeUpdate();
}

void IfAdvGauge::createLocator(nw4r::g3d::ResFile* resFile) {
    char animName[0x40];
    const char* name = "InfAdvPos_TopN";
    lbl_805A0338 = MuObject::create(resFile, name, 1, NULL, Heaps::InfoInstance);
    strcpy(animName, name);
    strcat(animName, "__0");
    lbl_805A0338->changeAnimN(animName);
    lbl_805A0338->m_modelAnim->setUpdateRate(0.0f);
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        lbl_805A0338->setFrameNode(0.0f);
    } else {
        lbl_805A0338->setFrameNode(1.0f);
    }
    lbl_805A033C = nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::InfoInstance), NULL, 0x20, lbl_805A0338->m_sceneModel);
}

void IfAdvGauge::destroyLocator() {
    if (lbl_805A033C != NULL) {
        lbl_805A033C->Destroy();
        lbl_805A033C = NULL;
    }
    if (lbl_805A0338 != NULL) {
        delete lbl_805A0338;
        lbl_805A0338 = NULL;
    }
    if (lbl_805A0344 != NULL) {
        lbl_805A0344->Destroy();
        lbl_805A0344 = NULL;
    }
    if (lbl_805A0340 != NULL) {
        delete lbl_805A0340;
        lbl_805A0340 = NULL;
    }
}

void IfAdvGauge::createLocator2(nw4r::g3d::ResFile* resFile) {
    char animName[0x40];
    const char* name = "InfPos_TopN";
    lbl_805A0340 = MuObject::create(resFile, name, 9, NULL, Heaps::InfoInstance);
    strcpy(animName, name);
    strcat(animName, "__0");
    lbl_805A0340->changeAnimN(animName);
    lbl_805A0340->m_modelAnim->setUpdateRate(0.0f);
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        lbl_805A0340->setFrameNode(0.0f);
    } else {
        lbl_805A0340->setFrameNode(1.0f);
    }
    lbl_805A0344 = nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::InfoInstance), NULL, 0x20, lbl_805A0340->m_sceneModel);
}

// HYPOTHESIS: map name IfAdvGauge::startScene (the map lists it at a wrong address); IfAdvMngr calls it by its address name.
extern "C" void fn_8010C004(nw4r::g3d::ScnGroup* group) {
    group->Insert(group->sceneItemsCount, lbl_805A033C);
    group->Insert(group->sceneItemsCount, lbl_805A0344);
}

void IfAdvGauge::draw2PEnt() {
    MuObject* object = m_objects[0x40];
    if (++m_pressStartFrame == 30) {
        m_pressStartFrame = 0;
    }
    object->setFrameMatCol(m_pressStartFrame);
}

void IfAdvGauge::addPressStart(nw4r::g3d::ScnMdlExpand* model) {
    model->PushBack(m_objects[0x40]->m_sceneModel, "pos02");
}

void IfAdvGauge::removePressStart(nw4r::g3d::ScnMdlExpand* model) {
    model->Remove(m_objects[0x40]->m_sceneModel);
}

// HYPOTHESIS: map name IfAdvGauge::drawScore; IfAdvMngr calls it by its address name.
// Shows the score as up to nine digits, least significant digit in the first slot.
extern "C" void fn_8010C0FC(IfAdvGauge* gauge, u32 score) {
    if (gauge->m_isScoreVisible) {
        char nodeName[6] = {};
        char text[10] = {};
        char reversed[10] = {};
        char digit[2] = {};
        MuObject* objects[9];
        nw4r::g3d::ScnMdlExpand* model = gauge->m_models[1];
        MuObject* anchor = gauge->getObject(1);
        lbl_805A033C->Remove(model);
        lbl_805A033C->PushBack(model, "pos00");
        u32 length = sprintf(text, "%d", score);
        if (length == 0) {
            length = 1;
        }
        if (length > 9) {
            length = 9;
        }
        for (int index = 0; index < length; index++) {
            reversed[index] = text[length - 1 - index];
        }
        anchor->setFrameNode(length);
        anchor->setFrameVisible(length);
        // MATCH-ONLY: the pointer into the digit slots reproduces the original's loop induction register.
        MuObject** digitObjects = &gauge->m_objects[2];
        for (u32 index = 0; index < length; index++) {
            objects[index] = digitObjects[index];
            sprintf(nodeName, "pos%02d", index);
            model->Remove(objects[index]->m_sceneModel);
            model->PushBack(objects[index]->m_sceneModel, nodeName);
            digit[0] = reversed[index];
            objects[index]->setFrameTex(atoi(digit));
        }
    }
}

void IfAdvGauge::removeScore() {
    lbl_805A033C->Remove(m_models[1]);
    m_isScoreVisible = 0;
}

void IfAdvGauge::drawCrowd() {
    int index;
    char nodeName[6] = {};
    nw4r::g3d::ScnMdlExpand* model = m_models[2];
    lbl_805A033C->Remove(model);
    lbl_805A033C->PushBack(model, "pos03");
    for (index = 0; index < 50; index++) {
        model->Remove(m_objects[13 + index]->m_sceneModel);
    }
    for (index = 0; index < m_crowdCount; index++) {
        sprintf(nodeName, "pos%02d", index);
        model->PushBack(m_objects[13 + index]->m_sceneModel, nodeName);
    }
    m_objects[12]->setFrameNode(0.0f);
}

void IfAdvGauge::setCrowd(int crowd) {
    m_crowdCount = crowd;
    m_objects[13 + crowd]->setFrameNode(0.0f);
    m_objects[13 + m_crowdCount]->setFrameMatCol(0.0f);
}

void IfAdvGauge::setCrowdDelAnimeNum(s32 num) {
    m_crowdDelIndex = num;
    m_crowdDelActive[num] = 1;
}

void IfAdvGauge::crowdDelAnimeUpdate() {
    for (s32 index = 0; index < 50; index++) {
        if (m_crowdDelActive[index]) {
            m_crowdDelFrame[index]++;
            int length = m_objects[13 + index]->getNodeAnimLength();
            if (length == m_crowdDelFrame[index]) {
                m_crowdDelFrame[index] = 0;
                m_crowdDelActive[index] = 0;
                m_crowdCount = index;
                m_objects[13 + m_crowdCount]->setFrameNode(0.0f);
                m_objects[13 + m_crowdCount]->setFrameMatCol(0.0f);
                char nodeName[6] = {};
                nw4r::g3d::ScnMdlExpand* model = m_models[2];
                lbl_805A033C->Remove(model);
                lbl_805A033C->PushBack(model, "pos03");
                sprintf(nodeName, "pos%02d", index);
                model->Remove(m_objects[13 + index]->m_sceneModel);
                m_crowdCount = index;
                m_objects[12]->setFrameNode(0.0f);
            }
            m_objects[13 + index]->setFrameNode(m_crowdDelFrame[index]);
            m_objects[13 + index]->setFrameMatCol(m_crowdDelFrame[index]);
        }
    }
}

// HYPOTHESIS: map name IfAdvGauge::drawCountTime (the map puts it at the address of crowdDelAnimeUpdate; IfAdvMngr
// calls this function once per frame with a frame counter). Shows minutes, seconds and hundredths from a 60 fps counter.
// TODO(nonmatching): 99.92, register/schedule class: only the ones digit of the hundredths is in r30 where the original
// has r27. An inline helper that sets one tens/ones pair (as for the material lookup below) gives all three digits r27
// but moves the slot loop's counter and pointers (r27/r31/r30 become r31/r30/r27); declaration-order sweeps, index
// reuse, accessors and two permuter runs (from each form) did not close it.
extern "C" void fn_8010C6C8(IfAdvGauge* gauge, u32 frames) {
    if (gauge->m_isTimeVisible) {
        int index;
        int digit;
        char nodeName[6] = {};
        MuObject* objects[6];
        MuObject* clock = gauge->m_objects[0x4b];
        if (frames < 60) {
            clock->setFrameVisible(1.0f);
        } else if (frames < 3600) {
            clock->setFrameVisible(2.0f);
        } else {
            clock->setFrameVisible(3.0f);
        }
        nw4r::g3d::ScnMdlExpand* model = gauge->m_models[5];
        lbl_805A033C->Remove(model);
        lbl_805A033C->PushBack(model, "pos04");
        // MATCH-ONLY: as in fn_8010C0FC, the pointer into the digit slots reproduces the loop induction register.
        MuObject** digitObjects = &gauge->m_objects[0x4c];
        for (index = 0; index < 6; index++) {
            objects[index] = digitObjects[index];
            sprintf(nodeName, "pos%02d", index);
            model->Remove(objects[index]->m_sceneModel);
            model->PushBack(objects[index]->m_sceneModel, nodeName);
        }
        u32 rest = frames - frames / 216000 * 216000;
        int minutes = rest / 3600;
        rest -= minutes * 3600;
        int seconds = rest / 60;
        rest -= seconds * 60;
        // MATCH-ONLY: the original converts the 60 at run time instead of folding it into the float constant.
        u32 framesPerSecond = 60;
        int hundredths = (int)(100.0f * (float)rest / (float)framesPerSecond);
        digit = minutes % 10;
        objects[5]->setFrameTex((minutes - digit) / 10);
        objects[4]->setFrameTex(digit);
        digit = seconds % 10;
        objects[3]->setFrameTex((seconds - digit) / 10);
        objects[2]->setFrameTex(digit);
        digit = hundredths % 10;
        objects[1]->setFrameTex((hundredths - digit) / 10);
        objects[0]->setFrameTex(digit);
        gauge->m_objects[0x4b]->m_modelAnim->setUpdateRate(0.0f);
        gauge->m_objects[0x4b]->setFrame(2.0f);
    }
}

void IfAdvGauge::removeCountTime() {
    lbl_805A033C->Remove(m_models[5]);
    m_isTimeVisible = 0;
}

void IfAdvGauge::drawStockBase(int show) {
    nw4r::g3d::ScnMdlExpand* model = m_models[3];
    if (show != 0) {
        lbl_805A033C->Remove(model);
        lbl_805A033C->PushBack(model, "pos07");
    } else {
        lbl_805A033C->Remove(model);
    }
}

// HYPOTHESIS: an inline helper (name invented, possibly a MuObject::changeMaterialTex overload taking the material
// by name) shared by updateStockSlot and drawStockFaceMove. Its by-value arguments are read before the material
// lookup, in the original's order; the same code written out at both call sites colors the registers differently.
static inline void changeMaterialTexByName(MuObject* object, const char* matName, const char* textureName,
                                           nw4r::g3d::ResFile* resFile) {
    object->changeMaterialTex(object->m_resMdl.GetResMat(matName)->m_id, textureName, resFile);
}

// HYPOTHESIS: map name IfAdvGauge::updateStockSlot; IfAdvMngr calls it by its address name.
// Puts the first num stock icons (up to 6) in the stock slot and takes the others out of it.
extern "C" void fn_8010CA94(IfAdvGauge* gauge, s32 num, u8* stock) {
    char nodeName[6] = {};
    // BYTES: the buffer and array sizes reproduce the original's stack frame.
    char textureName[24];
    MuObject* objects[10];
    int index;
    int slot = 0x43;
    nw4r::g3d::ScnMdlExpand* model = gauge->m_models[3];
    if (num > 6) {
        num = 6;
    }
    if (num < 0) {
        num = 0;
    }
    for (index = 0; index < num; slot++, index++) {
        objects[index] = gauge->m_objects[slot];
        sprintf(nodeName, "pos%02d", index);
        model->Remove(objects[index]->m_sceneModel);
        model->PushBack(objects[index]->m_sceneModel, nodeName);
        float icon = muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeGmCharacterKind2MuStockchkind(stock[1 + index])) * 10 + 1;
        int iconId = icon;
        if (iconId == 621) {
            changeMaterialTexByName(objects[index], "stock_lambert87", "InfStc.981", gauge->m_stockResFile);
        } else {
            sprintf(textureName, "InfStc.%03d", iconId);
            changeMaterialTexByName(objects[index], "stock_lambert87", textureName, gauge->m_stockResFile);
        }
    }
    // BUG: slot is not advanced here, so only the first unused icon (slot 0x43 + num) is taken out of the stock slot
    // and the icons after it stay in; the version that advances slot does not match.
    for (; index < 6; index++) {
        objects[index] = gauge->m_objects[slot];
        model->Remove(objects[index]->m_sceneModel);
    }
}

void IfAdvGauge::deleteStockSlotFirstIcon() {
    m_models[3]->Remove(m_objects[0x43]->m_sceneModel);
}

void IfAdvGauge::drawStockSlotAnime(int count) {
    m_objects[0x41]->setFrameNode(count);
}

// BYTES: the original subtracts the two positions and takes the absolute x/y distance from the same paired-single
// registers; MWCC emits ps_abs only from asm, so the vector subtraction and the absolute value are one block here.
// The absolute value goes to its own register variable (the allocator then gives it the register of the difference):
// an in-place ps_abs ties the difference store to it, and the scheduler then emits that store one slot earlier.
static inline void subtractWithAbsXY(register Vec3f* difference, register Vec3f* distance, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 fr3, fr2, fr1, frAbs, fr0;
    // clang-format off
    asm {
        psq_l  fr2, Vec3f.m_x(lhs), 0, 0
        psq_l  fr1, Vec3f.m_x(rhs), 0, 0
        ps_sub fr1, fr2, fr1
        psq_l  fr3, Vec3f.m_z(lhs), 1, 0
        psq_l  fr2, Vec3f.m_z(rhs), 1, 0
        ps_sub fr0, fr3, fr2
        psq_st fr1, Vec3f.m_x(difference), 0, 0
        ps_abs frAbs, fr1
        psq_st fr0, Vec3f.m_z(difference), 1, 0
        psq_st frAbs, Vec3f.m_x(distance), 0, 0
    }
    // clang-format on
}

// HYPOTHESIS: map name IfAdvGauge::drawStockFaceMove; IfAdvMngr calls it by its address name. The last parameter is
// unread. Moves the stock icon that was lost from the stock slot toward the player over 16 frames.
extern "C" void fn_8010CCE0(IfAdvGauge* gauge, float iconId, s32 count, IfPlayer* player, u8 unused) {
    MuObject* slot = gauge->m_objects[0x41];
    MuObject* face = gauge->m_objects[0x4a];
    // HYPOTHESIS: gfSceneRoot::remove/add take a ScnMdl (the mangled name), so the original passed the group through a cast.
    nw4r::g3d::ScnMdl* faceModel = reinterpret_cast<nw4r::g3d::ScnMdl*>(gauge->m_models[4]);
    if (gauge->m_isStockFaceMoving) {
        char textureName[20];
        Vec3f slotPos;
        Vec3f playerPos;
        Vec3f difference;
        Vec3f distance;
        slotPos = slot->getGlobalPosition("pos00");
        playerPos = player->getPlayerLocatorGlobalPos();
        g_gfSceneRoot->remove(faceModel);
        g_gfSceneRoot->add(gfSceneRoot::LayerType(8), faceModel);
        subtractWithAbsXY(&difference, &distance, &playerPos, &slotPos);
        if (iconId == 621.0f) {
            changeMaterialTexByName(face, "stock_lambert87", "InfStc.981", gauge->m_stockResFile);
        } else {
            sprintf(textureName, "InfStc.%03d", (int)iconId);
            changeMaterialTexByName(face, "stock_lambert87", textureName, gauge->m_stockResFile);
        }
        gauge->m_faceOffsetX += distance.m_x / 15.0f;
        gauge->m_faceOffsetY += distance.m_y / 15.0f;
        gauge->m_faceAngle = count * 12;
        if (count == 0) {
            gauge->m_faceOffsetX = 0.0f;
            gauge->m_faceOffsetY = 0.0f;
        }
        float sway = distance.m_y * (float)sin(0.017453292f * gauge->m_faceAngle);
        gauge->m_faceSway = sway;
        Vec3f trans(slotPos.m_x + gauge->m_faceOffsetX, sway + (slotPos.m_y - gauge->m_faceOffsetY), 0.0f);
        face->setTrans(&trans);
    } else {
        g_gfSceneRoot->remove(faceModel);
    }
    if (count == 15) {
        gauge->m_isStockFaceMoving = false;
        gauge->m_faceOffsetX = 0.0f;
        gauge->m_faceOffsetY = 0.0f;
    }
}

