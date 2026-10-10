#include <cm/cm_camera_controller.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <st/st_trigger.h>
#include <string.h>
#include <types.h>

#include <st_pictchat/gr_pictchat.h>
#include <st_pictchat/st_pictchat.h>

// sndSystem::setSeqSpeed (main, unnamed): it changes how fast a sequence plays (the sound of the picture follows the speed
// of the game)
extern "C" void fn_80076E30(sndSystem* sndSystem, s32 handle, float speed);
// sndSystem::isPlay (main, unnamed)
extern "C" bool fn_80077DA0(sndSystem* sndSystem, s32 handle);

stClassInfoImpl<Stages::PictChat, stPictchat> stPictchat::bss_loc_14;

stPictchat* stPictchat::create() {
    return new (Heaps::StageInstance) stPictchat;
}

stPictchat::stPictchat() : stMelee("stPictchat", Stages::PictChat) {
    m_state = 0;
    m_timer = 0.0f;
    m_first = 1;
    m_pictID = 0;
    m_pictIDPrev = 0;
    m_pictID2 = 0;
    m_pictCount = 0;
    memset(m_pictList, 0, sizeof(m_pictList));
    m_listIndex = 0xFF;
    m_attackState[0] = 5;
    m_attackState[1] = 5;
    m_attackState[2] = 5;
    m_attackState[3] = 5;
    m_attackState[4] = 5;
    m_attackState[5] = 5;
    memset(m_tblPict, 0, sizeof(m_tblPict));
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    m_hashigoData = NULL;
    memset(m_posHashigo, 0, sizeof(m_posHashigo));
    m_hashigoFlag = 0;
    m_springData = NULL;
    memset(m_posSpring, 0, sizeof(m_posSpring));
    m_springFlag[0] = 5;
    m_springFlag[1] = 5;
    m_unk40E = 0;
    memset(m_posBomb, 0, sizeof(m_posBomb));
    m_triggerWind = NULL;
    m_windData = NULL;
    m_seHandle = -1;
}

stPictchat::~stPictchat() {
    if (m_hashigoData != NULL) {
        // MATCH-ONLY: the data of the ladders is raw storage (no cookie, no constructors)
        delete[] reinterpret_cast<u8*>(m_hashigoData);
    }
    if (m_springData != NULL) {
        delete[] m_springData;
    }
    if (m_windData != NULL) {
        delete m_windData;
    }
    for (u8 i = 0; i < 27; i++) {
        if (m_tblPict[i] != NULL) {
            delete m_tblPict[i];
        }
        m_tblPict[i] = NULL;
    }
    releaseArchive();
}

bool stPictchat::loading() {
    return true;
}

void stPictchat::createObj() {
    testStageParamInit(m_fileData, 6);
    testStageDataInit(m_fileData, 4, 0x14);
    initStageDataTbl();
    createObjWind();
    createObjBg(0);
    createObjPict(1);
    createObjPict(2);
    createObjPict(3);
    createObjPict(4);
    createObjPict(5);
    createObjPict(6);
    createObjPict(7);
    createObjPict(8);
    createObjPict(9);
    createObjPict(10);
    createObjPict(0xB);
    createObjPict(0xC);
    createObjPict(0xD);
    createObjPict(0xE);
    createObjPict(0xF);
    createObjPict(0x10);
    createObjPict(0x11);
    createObjPict(0x12);
    createObjPict(0x13);
    createObjPict(0x14);
    createObjPict(0x15);
    createObjPict(0x16);
    createObjPict(0x17);
    createObjPict(0x18);
    createObjPict(0x19);
    createObjPict(0x1A);
    createObjPict(0x1B);
    createObjPict(0x1C);
    createObjPict(0x1D);
    createCollision(m_fileData, 2, NULL);
    createObjSideBar(0x1E);
    createObjSideBarLamp(0x1F);
    createObjHashigo();
    createObjSpring();
    createObjAttack(0x27);
    createObjAttack(0x28);
    createObjAttack(0x29);
    createObjAttack(0x2A);
    createObjAttack(0x2B);
    createObjAttack(0x2C);
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 500, 0xFFFE);
    if (posData != NULL) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 8);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x1F5, "PokeTrainer00", m_pokeTrainerPos, NULL);
}

void stPictchat::createObjBg(int index) {
    grPictchatBg* ground;
    switch (index) {
    case 0:
        ground = grPictchatBg::create(1, "StgPictchat00Ground", "grPictchatMainBg");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMsgData(m_fileData->getData(Data_Type_Misc, 300, 0xFFFE));
        ground->setPictIDWork(&m_pictID);
    }
}

void grPictchatBg::setMsgData(void* msgData) {
    m_msgData = msgData;
}

void grPictchatBg::setPictIDWork(u8* pictIDWork) {
    m_pictIDWork = pictIDWork;
}


void stPictchat::createObjSideBar(int index) {
    grPictchatSideBar* ground;
    switch (index) {
    case 0x1E:
        ground = grPictchatSideBar::create(2, "StgPictchat00Sidebar", "grPictchatSideBar");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setPictCountWork(&m_pictCount);
    }
}

void grPictchatSideBar::setPosGimmickWork(Vec3f* posGimmickWork) {
    m_posGimmickWork = posGimmickWork;
}

void grPictchatSideBar::setPictCountWork(u8* pictCountWork) {
    m_pictCountWork = pictCountWork;
}


void stPictchat::createObjSideBarLamp(int index) {
    grPictchatSideBarLamp* ground;
    u8 type;
    switch (index) {
    case 0x1F:
        ground = grPictchatSideBarLamp::create(4, "StgPictchat00SidebarGlamp", "grPictchatSidebar00");
        type = 4;
        break;
    case 0x20:
        ground = grPictchatSideBarLamp::create(4, "StgPictchat00SidebarGlamp", "grPictchatSidebar01");
        type = 5;
        break;
    case 0x21:
        ground = grPictchatSideBarLamp::create(4, "StgPictchat00SidebarGlamp", "grPictchatSidebar02");
        type = 6;
        break;
    case 0x22:
        ground = grPictchatSideBarLamp::create(4, "StgPictchat00SidebarGlamp", "grPictchatSidebar03");
        type = 7;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(m_posGimmick);
        ground->setType(type);
    }
}

void grPictchatSideBarLamp::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}

void grPictchatSideBarLamp::setType(u8 type) {
    m_type = type;
}


void stPictchat::createObjPict(int index) {
    grPictchatPict* ground;
    u8 pictID;
    Vec3f* posHashigo = NULL;
    Vec3f* posSpring = NULL;
    Vec3f* posBomb = NULL;
    u8 type = 8;
    u8 count = 1;
    u8* stateWork = NULL;
    stTrigger* trigger = NULL;
    char nodeName[0x80];
    switch (index) {
    case 1:
        ground = grPictchatPict::create(10, "StgPictchat00P001StFloor", "grPictchatP001");
        pictID = 1;
        strcpy(nodeName, "P001");
        break;
    case 2:
        ground = grPictchatPict::create(0x14, "StgPictchat00P002DFloor", "grPictchatP002");
        pictID = 2;
        strcpy(nodeName, "P002");
        break;
    case 3:
        ground = grPictchatPict::create(0x1E, "StgPictchat00P003House", "grPictchatP003");
        pictID = 3;
        strcpy(nodeName, "P003");
        break;
    case 4:
        ground = grPictchatPict::create(0x28, "StgPictchat00P004Dog", "grPictchatP004");
        pictID = 4;
        strcpy(nodeName, "P004");
        break;
    case 5:
        ground = grPictchatPict::create(0x32, "StgPictchat00P005Sboth", "grPictchatP005");
        pictID = 5;
        strcpy(nodeName, "P005");
        break;
    case 6:
        ground = grPictchatPict::create(0x3C, "StgPictchat00P006Tree", "grPictchatP006");
        pictID = 6;
        strcpy(nodeName, "P006");
        break;
    case 7:
        ground = grPictchatPict007::create(0x46, "StgPictchat00P007Spike", "grPictchatP007");
        pictID = 7;
        strcpy(nodeName, "P007");
        break;
    case 8:
        ground = grPictchatPict008::create(0x50, "StgPictchat00P008L", "grPictchatP008");
        pictID = 8;
        strcpy(nodeName, "P008");
        stateWork = &m_springFlag[0];
        type = 0;
        posSpring = &m_posSpring[0];
        count = 2;
        break;
    case 9:
        ground = grPictchatPict008::create(0x51, "StgPictchat00P008R", "grPictchatP008");
        pictID = 8;
        strcpy(nodeName, "P008");
        stateWork = &m_springFlag[1];
        type = 1;
        posSpring = &m_posSpring[1];
        count = 2;
        break;
    case 0xA:
        ground = grPictchatPict009::create(0x5A, "StgPictchat00P009Clock", "grPictchatP009");
        pictID = 9;
        strcpy(nodeName, "P009");
        break;
    case 0xB:
        ground = grPictchatPict::create(100, "StgPictchat00P010Swing", "grPictchatP010");
        pictID = 10;
        strcpy(nodeName, "P010");
        break;
    case 0xC:
        ground = grPictchatPict011::create(0x6E, "StgPictchat00P011Fwheel", "grPictchatP011");
        pictID = 11;
        strcpy(nodeName, "P011");
        break;
    case 0xD:
        ground = grPictchatPict012::create(0x78, "StgPictchat00P012Coaster", "grPictchatP012");
        pictID = 12;
        strcpy(nodeName, "P012");
        break;
    case 0xE:
        ground = grPictchatPict013::create(0x82, "StgPictchat00P013Missile", "grPictchatP013A");
        pictID = 13;
        strcpy(nodeName, "P013");
        type = 2;
        posBomb = &m_posBomb[0];
        count = 2;
        break;
    case 0xF:
        ground = grPictchatPict013::create(0x82, "StgPictchat00P013Missile", "grPictchatP013B");
        pictID = 13;
        strcpy(nodeName, "P013");
        type = 3;
        posBomb = &m_posBomb[1];
        count = 2;
        break;
    case 0x10:
        ground = grPictchatPict014::create(0x8C, "StgPictchat00P014Breath", "grPictchatP014");
        pictID = 14;
        strcpy(nodeName, "P014");
        trigger = m_triggerWind;
        break;
    case 0x11:
        ground = grPictchatPict015::create(0x96, "StgPictchat00P015Flower", "grPictchatP015");
        pictID = 15;
        strcpy(nodeName, "P015");
        break;
    case 0x12:
        ground = grPictchatPict::create(0xA0, "StgPictchat00P016LotFloor", "grPictchatP016");
        pictID = 16;
        strcpy(nodeName, "P016");
        break;
    case 0x13:
        ground = grPictchatPict::create(0xAA, "StgPictchat00P017Tree2", "grPictchatP017");
        pictID = 17;
        strcpy(nodeName, "P017");
        break;
    case 0x14:
        ground = grPictchatPict::create(0xB4, "StgPictchat00P018Whale", "grPictchatP018");
        pictID = 18;
        strcpy(nodeName, "P018");
        break;
    case 0x15:
        ground = grPictchatPict::create(0xBE, "StgPictchat00P019Eye", "grPictchatP019");
        pictID = 19;
        strcpy(nodeName, "P019");
        break;
    case 0x16:
        ground = grPictchatPict::create(200, "StgPictchat00P020Box", "grPictchatP020");
        pictID = 20;
        strcpy(nodeName, "P020");
        break;
    case 0x17:
        ground = grPictchatPict::create(0xD2, "StgPictchat00P021Photo", "grPictchatP021");
        pictID = 21;
        strcpy(nodeName, "P021");
        break;
    case 0x18:
        ground = grPictchatPict022::create(0xDC, "StgPictchat00P022ladder", "grPictchatP022");
        pictID = 22;
        strcpy(nodeName, "P022");
        posHashigo = &m_posHashigo[0];
        break;
    case 0x19:
        ground = grPictchatPict::create(0xE6, "StgPictchat00P023Umbrella", "grPictchatP023");
        pictID = 23;
        strcpy(nodeName, "P023");
        break;
    case 0x1A:
        ground = grPictchatPict::create(0xF0, "StgPictchat00P024Ship", "grPictchatP024");
        pictID = 24;
        strcpy(nodeName, "P024");
        break;
    case 0x1B:
        ground = grPictchatPict025::create(0xFA, "StgPictchat00P025Fire", "grPictchatP025");
        pictID = 25;
        strcpy(nodeName, "P025");
        break;
    case 0x1C:
        ground = grPictchatPict::create(0x104, "StgPictchat00P026Human", "grPictchatP026");
        pictID = 26;
        strcpy(nodeName, "P026");
        break;
    case 0x1D:
        ground = grPictchatPict027::create(0x10E, "StgPictchat00P027Spear", "grPictchatP027");
        pictID = 27;
        strcpy(nodeName, "P027");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setPictIDWork(&m_pictID);
        ground->setPictID(pictID);
        ground->setPictCountWork(&m_pictID2);
        ground->setPictCount(count);
        ground->setType(type);
        ground->setStateWork(stateWork);
        ground->setNodeHeader(nodeName);
        ground->setTblCollCtrlAcc(m_tblPict[pictID - 1]);
        switch (index) {
        case 7:
            ground->setTblCollCtrlAcc(NULL);
            ground->setStateAttackWork(m_attackState);
            break;
        case 8:
        case 9:
            static_cast<grPictchatPict008*>(ground)->setPosSpringWork(posSpring);
            static_cast<grPictchatPict008*>(ground)->setFlgSpringWork(&m_unk40E);
            break;
        case 0xE:
        case 0xF:
            static_cast<grPictchatPict013*>(ground)->setPosBombWork(posBomb);
            break;
        case 0x10:
            static_cast<grPictchatPict014*>(ground)->setTrigger(trigger);
            break;
        case 0x18:
            static_cast<grPictchatPict022*>(ground)->setPosHashigoWork(posHashigo);
            static_cast<grPictchatPict022*>(ground)->setFlgHashigoWork(&m_hashigoFlag);
            break;
        }
    }
}

void grPictchatPict::setPosGimmickWork(Vec3f* posGimmickWork) {
    m_posGimmickWork = posGimmickWork;
}

void grPictchatPict::setPictIDWork(u8* pictIDWork) {
    m_pictIDWork = pictIDWork;
}

void grPictchatPict::setPictID(u8 pictID) {
    m_pictID = pictID;
}

void grPictchatPict::setPictCountWork(u8* pictCountWork) {
    m_pictCountWork = pictCountWork;
}

void grPictchatPict::setPictCount(u8 pictCount) {
    m_pictCount = pictCount;
}

void grPictchatPict::setType(u8 type) {
    m_type = type;
}

void grPictchatPict::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}

void grPictchatPict::setNodeHeader(const char* nodeHeader) {
    strcpy(m_nodeHeader, nodeHeader);
}

void grPictchatPict::setTblCollCtrlAcc(stDataContainer* tblCollCtrl) {
    m_collCtrlTbl = tblCollCtrl;
}

void grPictchatPict::setStateAttackWork(u8* stateAttackWork) {
    m_stateAttackWork = stateAttackWork;
}

void grPictchatPict008::setPosSpringWork(Vec3f* posSpringWork) {
    m_posSpringWork = posSpringWork;
}

void grPictchatPict008::setFlgSpringWork(u8* flgSpringWork) {
    m_flgSpringWork = flgSpringWork;
}

void grPictchatPict013::setPosBombWork(Vec3f* posBombWork) {
    m_posBombWork = posBombWork;
}

void grPictchatPict014::setTrigger(stTrigger* trigger) {
    m_trigger = trigger;
}

void grPictchatPict022::setPosHashigoWork(Vec3f* posHashigoWork) {
    m_posHashigoWork = posHashigoWork;
}

void grPictchatPict022::setFlgHashigoWork(u8* flgHashigoWork) {
    m_flgHashigoWork = flgHashigoWork;
}


void stPictchat::createObjAttack(int index) {
    grPictchatAttack007* ground;
    Vec3f* pos;
    u8* stateWork;
    u8 pictID;
    u8 attackIndex;
    switch (index) {
    case 0x27:
        ground = grPictchatAttack007::create(3, "nodeIndex", "grPictchatAttack00701");
        pos = &m_posGimmick[23];
        stateWork = &m_attackState[0];
        pictID = 7;
        attackIndex = 0;
        break;
    case 0x28:
        ground = grPictchatAttack007::create(3, "nodeIndex", "grPictchatAttack00702");
        pos = &m_posGimmick[24];
        stateWork = &m_attackState[1];
        pictID = 7;
        attackIndex = 1;
        break;
    case 0x29:
        ground = grPictchatAttack007::create(3, "nodeIndex", "grPictchatAttack00703");
        pos = &m_posGimmick[25];
        stateWork = &m_attackState[2];
        pictID = 7;
        attackIndex = 2;
        break;
    case 0x2A:
        ground = grPictchatAttack007::create(3, "nodeIndex", "grPictchatAttack00704");
        pos = &m_posGimmick[26];
        stateWork = &m_attackState[3];
        pictID = 7;
        attackIndex = 3;
        break;
    case 0x2B:
        ground = grPictchatAttack007::create(3, "nodeIndex", "grPictchatAttack00705");
        pos = &m_posGimmick[27];
        stateWork = &m_attackState[4];
        pictID = 7;
        attackIndex = 4;
        break;
    case 0x2C:
        ground = grPictchatAttack007::create(3, "nodeIndex", "grPictchatAttack00706");
        pos = &m_posGimmick[28];
        stateWork = &m_attackState[5];
        pictID = 7;
        attackIndex = 5;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
        ground->setPictIDWork(&m_pictID);
        ground->setPictID(pictID);
        ground->setStateWork(stateWork);
        if (index < 0x2D) {
            if (index >= 0x27) {
                ground->setIndex(attackIndex);
            }
        }
    }
}

void grPictchatAttack::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}

void grPictchatAttack::setPictIDWork(u8* pictIDWork) {
    m_pictIDWork = pictIDWork;
}

void grPictchatAttack::setPictID(u8 pictID) {
    m_pictID = pictID;
}

void grPictchatAttack::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}

void grPictchatAttack007::setIndex(u8 index) {
    m_index = index;
}


// The data of the two ladders (the nodes they are on are "ladderL" and "ladderR").
void stPictchat::createObjHashigo() {
    // MATCH-ONLY: the data of the ladders is raw storage (no cookie, no constructors) and its bytes are set as unsigned bytes
    u8* bytes = new (Heaps::StageResource) u8[2 * sizeof(grGimmickLadderData)];
    m_hashigoData = reinterpret_cast<grGimmickLadderData*>(bytes);
    if (bytes != NULL) {
        memset(bytes, 0, sizeof(grGimmickLadderData));
        bytes[0x30] = 0xE4;
        bytes[0x31] = 0xDC;
        bytes[0x32] = 0;
        bytes[0x33] = 1;
        strcpy(reinterpret_cast<char*>(bytes + 0x34), "ladderL");
        reinterpret_cast<u8*>(m_hashigoData)[6] = 0xFF;
        reinterpret_cast<grGimmickLadderData*>(bytes)->m_areaData.m_offsetPos.m_x = 0.0f;
        reinterpret_cast<grGimmickLadderData*>(bytes)->m_areaData.m_offsetPos.m_y = 27.0f;
        reinterpret_cast<grGimmickLadderData*>(bytes)->m_areaData.m_range.m_x = 10.0f;
        reinterpret_cast<grGimmickLadderData*>(bytes)->m_areaData.m_range.m_y = 54.0f;
        createObjHashigo1(0x23);
        bytes = reinterpret_cast<u8*>(m_hashigoData) + sizeof(grGimmickLadderData);
        memset(bytes, 0, sizeof(grGimmickLadderData));
        bytes[0x30] = 0xE5;
        bytes[0x31] = 0xDC;
        bytes[0x32] = 0;
        bytes[0x33] = 1;
        strcpy(reinterpret_cast<char*>(bytes + 0x34), "ladderR");
        reinterpret_cast<u8*>(m_hashigoData)[6] = 0xFF;
        reinterpret_cast<grGimmickLadderData*>(bytes)->m_areaData.m_offsetPos.m_x = 0.0f;
        reinterpret_cast<grGimmickLadderData*>(bytes)->m_areaData.m_offsetPos.m_y = 27.0f;
        reinterpret_cast<grGimmickLadderData*>(bytes)->m_areaData.m_range.m_x = 10.0f;
        reinterpret_cast<grGimmickLadderData*>(bytes)->m_areaData.m_range.m_y = 54.0f;
        createObjHashigo1(0x24);
    }
}

void stPictchat::createObjHashigo1(int index) {
    grPictchatLadder* ground;
    grGimmickLadderData* data;
    Vec3f* pos;
    u8 flgIndex;
    switch (index) {
    case 0x23:
        ground = grPictchatLadder::create(0xE4, "StgPictchat00P022ladderL", "grPictchatLadderL");
        data = m_hashigoData;
        pos = &m_posHashigo[0];
        flgIndex = 0x16;
        break;
    case 0x24:
        ground = grPictchatLadder::create(0xE5, "StgPictchat00P022ladderR", "grPictchatLadderR");
        pos = &m_posHashigo[1];
        flgIndex = 0x16;
        data = m_hashigoData + 1;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->setGimmickData(data);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPictIDWork(&m_pictID);
        ground->setPictID(flgIndex);
        ground->setPosWork(pos);
        ground->setFlgWork(&m_hashigoFlag);
    }
}

void grPictchatLadder::setPictIDWork(u8* pictIDWork) {
    m_pictIDWork = pictIDWork;
}

void grPictchatLadder::setPictID(u8 pictID) {
    m_pictID = pictID;
}

void grPictchatLadder::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}

void grPictchatLadder::setFlgWork(u8* flgWork) {
    m_flgWork = flgWork;
}


void stPictchat::createObjSpring() {
    m_springData = new (Heaps::StageInstance) grGimmickSpringData[2];
    if (m_springData != NULL) {
        memset(m_springData, 0, sizeof(grGimmickSpringData));
        m_springData->m_pos.m_x = 0.0f;
        m_springData->m_pos.m_y = 0.0f;
        m_springData->m_bounce = 3.5f;
        m_springData->m_mdlIndex = 2;
        grGimmickSpringData* data = m_springData;
        data->m_areaData.m_offsetPos.m_x = 0.0f;
        data->m_areaData.m_offsetPos.m_y = 0.0f;
        data->m_areaData.m_range.m_x = 30.0f;
        data->m_areaData.m_range.m_y = 10.0f;
        createObjSpring1(0x25);
        memset(m_springData, 0, sizeof(grGimmickSpringData));
        m_springData->m_pos.m_x = 0.0f;
        m_springData->m_pos.m_y = 0.0f;
        m_springData->m_bounce = 3.5f;
        m_springData->m_mdlIndex = 2;
        data = m_springData;
        data->m_areaData.m_offsetPos.m_x = 0.0f;
        data->m_areaData.m_offsetPos.m_y = 0.0f;
        data->m_areaData.m_range.m_x = 30.0f;
        data->m_areaData.m_range.m_y = 10.0f;
        createObjSpring1(0x26);
    }
}

void stPictchat::createObjSpring1(int index) {
    grPictchatSpring* ground = NULL;
    Vec3f* pos;
    u8* flgWork;
    u8 pictID;
    switch (index) {
    case 0x25:
        ground = grPictchatSpring::create(3, "nodeIndex", "grPictchatSpringL");
        pos = &m_posSpring[0];
        flgWork = &m_springFlag[0];
        pictID = 8;
        break;
    case 0x26:
        ground = grPictchatSpring::create(3, "nodeIndex", "grPictchatSpringR");
        pos = &m_posSpring[1];
        flgWork = &m_springFlag[1];
        pictID = 8;
        break;
    }
    if (ground != NULL) {
        ground->setGimmickData(m_springData);
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPictIDWork(&m_pictID);
        ground->setPictID(pictID);
        ground->setPosWork(pos);
        ground->setStateWork(flgWork);
        ground->setFlgWork(&m_unk40E);
    }
}

void grPictchatSpring::setPictIDWork(u8* pictIDWork) {
    m_pictIDWork = pictIDWork;
}

void grPictchatSpring::setPictID(u8 pictID) {
    m_pictID = pictID;
}

void grPictchatSpring::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}

void grPictchatSpring::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}

void grPictchatSpring::setFlgWork(u8* flgWork) {
    m_flgWork = flgWork;
}


// The wind of the breath (P014): a trigger with the area of the whole picture, put to sleep until the picture is drawn.
void stPictchat::createObjWind() {
    m_windData = new (Heaps::StageInstance) grGimmickWindData;
    if (m_windData != NULL) {
        memset(m_windData, 0, sizeof(grGimmickWindData));
        m_windData->m_pos.m_x = 0.0f;
        m_windData->m_pos.m_y = 36.0f;
        m_windData->m_pos.m_z = 0.0f;
        m_windData->m_speed = 0.5f;
        m_windData->m_vector = 200.0f;
        m_windData->m_areaData.m_offsetPos.m_x = 0.0f;
        m_windData->m_areaData.m_offsetPos.m_y = 0.0f;
        m_windData->m_areaData.m_range.m_x = 200.0f;
        m_windData->m_areaData.m_range.m_y = 80.0f;
        m_triggerWind = g_stTriggerMng->createTrigger(Gimmick::Area_Wind, -1);
        m_triggerWind->setWindTrigger(m_windData);
        grGimmickWindData2nd wind;
        memset(&wind, 0, sizeof(grGimmickWindData2nd));
        wind.m_pos.m_x = 0.0f;
        wind.m_pos.m_y = 36.0f;
        wind.m_pos.m_z = 0.0f;
        wind.m_speed = 0.5f;
        wind.m_vector = 195.0f;
        wind.m_60 = 20.0f;
        wind.m_64 = 1.5f;
        wind.m_72 = 80;
        wind.m_68 = 0.0f;
        wind.m_areaData.m_offsetPos.m_x = 0.0f;
        wind.m_areaData.m_offsetPos.m_y = 0.0f;
        wind.m_areaData.m_range.m_x = 200.0f;
        wind.m_areaData.m_range.m_y = 80.0f;
        m_triggerWind->setWindParam(&wind, 1);
        m_triggerWind->setAreaSleep(true);
    }
}

// The pictures of the stage: every ten ids of the stage data file hold the table of one picture (the table of the collision).
#define STPICTCHAT_LOAD_TBL(i, id)                                                                 \
    {                                                                                              \
        stDataContainerData* tbl = static_cast<stDataContainerData*>(m_fileData->getData(Data_Type_Misc, id, 0xFFFE)); \
        if (tbl != NULL) {                                                                         \
            m_tblPict[i] = stDataMultiContainer::create(tbl, Heaps::StageInstance);                \
        }                                                                                          \
    }

void stPictchat::initStageDataTbl() {
    if (m_fileData != NULL) {
    STPICTCHAT_LOAD_TBL(0, 0xA)
    STPICTCHAT_LOAD_TBL(1, 0x14)
    STPICTCHAT_LOAD_TBL(2, 0x1E)
    STPICTCHAT_LOAD_TBL(3, 0x28)
    STPICTCHAT_LOAD_TBL(4, 0x32)
    STPICTCHAT_LOAD_TBL(5, 0x3C)
    STPICTCHAT_LOAD_TBL(6, 0x46)
    STPICTCHAT_LOAD_TBL(7, 0x50)
    STPICTCHAT_LOAD_TBL(8, 0x5A)
    STPICTCHAT_LOAD_TBL(9, 0x64)
    STPICTCHAT_LOAD_TBL(10, 0x6E)
    STPICTCHAT_LOAD_TBL(11, 0x78)
    STPICTCHAT_LOAD_TBL(12, 0x82)
    STPICTCHAT_LOAD_TBL(13, 0x8C)
    STPICTCHAT_LOAD_TBL(14, 0x96)
    STPICTCHAT_LOAD_TBL(15, 0xA0)
    STPICTCHAT_LOAD_TBL(16, 0xAA)
    STPICTCHAT_LOAD_TBL(17, 0xB4)
    STPICTCHAT_LOAD_TBL(18, 0xBE)
    STPICTCHAT_LOAD_TBL(19, 0xC8)
    STPICTCHAT_LOAD_TBL(20, 0xD2)
    STPICTCHAT_LOAD_TBL(21, 0xDC)
    STPICTCHAT_LOAD_TBL(22, 0xE6)
    STPICTCHAT_LOAD_TBL(23, 0xF0)
    STPICTCHAT_LOAD_TBL(24, 0xFA)
    STPICTCHAT_LOAD_TBL(25, 0x104)
    STPICTCHAT_LOAD_TBL(26, 0x10E)
    }
}

void stPictchat::update(float deltaFrame) {
    updatePict(deltaFrame);
}

// The state of the picture: 0 starts the wait, 1 waits and chooses the next picture, 2 plays its sound (it is the sound of the
// picture and it follows the speed of the game) until the time is over, 3 waits for the erase.
void stPictchat::updatePict(float deltaFrame) {
    stPictchatData* data = static_cast<stPictchatData*>(m_stageData);
    if (data != NULL) {
        float timer = m_timer - deltaFrame;
        m_timer = timer;
        if (timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            if (m_first == 1) {
                m_timer = data->unk00;
                g_sndSystem->playSE(static_cast<SndID>(0x1D1D), 0, 0, 0, -1);
                m_first = 0;
            } else {
                m_timer = data->unk04 + (data->unk08 - data->unk04) * randf();
            }
            m_state = 1;
            break;
        case 1:
            if (m_timer == 0.0f) {
                if (m_listIndex >= 0x1B) {
                    initPictIDList();
                    m_listIndex = 0;
                }
                m_pictID = m_pictList[m_listIndex];
                m_listIndex++;
                float wait = data->unk0C + (data->unk10 - data->unk0C) * randf();
                m_pictIDPrev = m_pictID;
                m_pictCount++;
                m_timer = wait;
                if (m_pictCount > 0x17) {
                    m_pictCount = 0x17;
                }
                switch (m_pictID) {
            case 1:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2057), 0, 0, 0, -1);
                break;
            case 2:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2058), 0, 0, 0, -1);
                break;
            case 3:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2059), 0, 0, 0, -1);
                break;
            case 4:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x205A), 0, 0, 0, -1);
                break;
            case 5:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x205B), 0, 0, 0, -1);
                break;
            case 6:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x205C), 0, 0, 0, -1);
                break;
            case 7:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x205D), 0, 0, 0, -1);
                break;
            case 8:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x205E), 0, 0, 0, -1);
                break;
            case 9:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x205F), 0, 0, 0, -1);
                break;
            case 10:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2060), 0, 0, 0, -1);
                break;
            case 11:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2061), 0, 0, 0, -1);
                break;
            case 12:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2062), 0, 0, 0, -1);
                break;
            case 13:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2063), 0, 0, 0, -1);
                break;
            case 14:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2064), 0, 0, 0, -1);
                break;
            case 15:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2065), 0, 0, 0, -1);
                break;
            case 16:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2066), 0, 0, 0, -1);
                break;
            case 17:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2067), 0, 0, 0, -1);
                break;
            case 18:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2068), 0, 0, 0, -1);
                break;
            case 19:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2069), 0, 0, 0, -1);
                break;
            case 20:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x206A), 0, 0, 0, -1);
                break;
            case 21:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x206B), 0, 0, 0, -1);
                break;
            case 22:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x206C), 0, 0, 0, -1);
                break;
            case 23:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x206D), 0, 0, 0, -1);
                break;
            case 24:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x206E), 0, 0, 0, -1);
                break;
            case 25:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x206F), 0, 0, 0, -1);
                break;
            case 26:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2070), 0, 0, 0, -1);
                break;
            case 27:
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2071), 0, 0, 0, -1);
                break;
                }
                m_state = 2;
            }
            break;
        case 2:
            if (m_seHandle != -1) {
                if (fn_80077DA0(g_sndSystem, m_seHandle) == 1) {
                    float rate = static_cast<float>(g_gfApplication->m_frameRate) / 60.0f;
                    float speed = deltaFrame;
                    if (rate > 1.0f) {
                        speed = deltaFrame * rate;
                    }
                    fn_80076E30(g_sndSystem, m_seHandle, speed);
                } else {
                    m_seHandle = -1;
                }
            }
            if (m_timer == 0.0f || m_pictID == 0x1D) {
                m_pictID = 0x1D;
                m_state = 3;
            }
            break;
        case 3:
            if (m_pictID == 0x1E) {
                m_state = 0;
            }
            break;
        }
    }
}

// The list of the pictures is the pictures 1 to 27, shuffled.
void stPictchat::initPictIDList() {
    m_pictList[0] = 1;
    m_pictList[1] = 2;
    m_pictList[2] = 3;
    m_pictList[3] = 4;
    m_pictList[4] = 5;
    m_pictList[5] = 6;
    m_pictList[6] = 7;
    m_pictList[7] = 8;
    m_pictList[8] = 9;
    m_pictList[9] = 10;
    m_pictList[10] = 11;
    m_pictList[11] = 12;
    m_pictList[12] = 13;
    m_pictList[13] = 14;
    m_pictList[14] = 15;
    m_pictList[15] = 16;
    m_pictList[16] = 17;
    m_pictList[17] = 18;
    m_pictList[18] = 19;
    m_pictList[19] = 20;
    m_pictList[20] = 21;
    m_pictList[21] = 22;
    m_pictList[22] = 23;
    m_pictList[23] = 24;
    m_pictList[24] = 25;
    m_pictList[25] = 26;
    m_pictList[26] = 27;
    u32 count = 27;
    for (u32 i = 0; i < 27; i++) {
        float range = randf();
        u8 other = static_cast<u8>(static_cast<int>(static_cast<float>(count) * range));
        u8 temp = m_pictList[i & 0xFF];
        m_pictList[i & 0xFF] = m_pictList[other];
        m_pictList[other] = temp;
    }
}
