#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/menu.h>
#include <mu/mu_adv_comer.h>

// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// MuMsg::printData(message, data, length)
extern "C" void printData__5MuMsgFUlPCvi(MuMsg* msg, u32 msgIndex, const void* data, int length);

// What the game keeps in the data of the adventure mode about the pad of the player: the second word is the number of the
// pad (-1: there is none).
// HYPOTHESIS: the names; the data is at 0x4928 of the data of the adventure mode.
struct muAdvComerPadInfo {
    int m_unk0;
    int m_padNo;
};

muAdvComerTask* muAdvComerTask::create() {
    return new (Heaps::MenuInstance) muAdvComerTask();
}

#define FACE_PATH(index, number) sprintf(m_facePaths[index], "/menu/adventure/comer_tex/tex_face" number ".brres")

// Whether a file of the window is still loading.
static inline bool isLoading(muAdvComerTask* task) {
    return !task->m_fileComer.isReady() ? true : (!task->m_fileFace.isReady() ? true : !task->m_fileMsg.isReady());
}

// Whether the animations of the model are over.
static inline bool isAnimEnd(muAdvComerTask* task) {
    return !task->m_object->isNodeAnimFinished() ? false : task->m_object->isClrAnimFinished();
}

muAdvComerTask::muAdvComerTask() : gfTask("AdvComer", Category_Menu, 0xF, 8, true) {
    m_resComer = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFace = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_menuRoot = NULL;
    m_object = NULL;
    m_msg = NULL;
    memset(m_facePaths, 0, sizeof(m_facePaths));
    FACE_PATH(0, "---");
    FACE_PATH(1, "001");
    FACE_PATH(2, "002");
    FACE_PATH(3, "003");
    FACE_PATH(4, "004");
    FACE_PATH(5, "005");
    FACE_PATH(6, "006");
    FACE_PATH(7, "007");
    FACE_PATH(8, "008");
    FACE_PATH(9, "009");
    FACE_PATH(10, "010");
    FACE_PATH(11, "011");
    FACE_PATH(12, "012");
    FACE_PATH(13, "013");
    FACE_PATH(14, "014");
    FACE_PATH(15, "015");
    FACE_PATH(16, "016");
    FACE_PATH(17, "017");
    FACE_PATH(18, "018");
    FACE_PATH(19, "019");
    FACE_PATH(20, "020");
    FACE_PATH(21, "021");
    FACE_PATH(22, "022");
    FACE_PATH(23, "023");
    FACE_PATH(24, "024");
    FACE_PATH(25, "025");
    FACE_PATH(26, "026");
    FACE_PATH(27, "027");
    FACE_PATH(28, "028");
    FACE_PATH(29, "029");
    FACE_PATH(30, "030");
    FACE_PATH(31, "031");
    FACE_PATH(32, "032");
    FACE_PATH(33, "033");
    FACE_PATH(34, "034");
    FACE_PATH(35, "035");
    FACE_PATH(36, "036");
    FACE_PATH(37, "037");
    FACE_PATH(38, "038");
    FACE_PATH(39, "039");
    FACE_PATH(40, "040");
    FACE_PATH(41, "041");
    FACE_PATH(42, "042");
    FACE_PATH(43, "043");
    FACE_PATH(44, "044");
    FACE_PATH(45, "045");
    FACE_PATH(46, "046");
    FACE_PATH(47, "047");
    m_step = 0;
    m_frame = 0;
}

#undef FACE_PATH
muAdvComerTask::~muAdvComerTask() { }

void muAdvComerTask::processDefault() {
    muAdvComerPadInfo info;
    u8* advData = reinterpret_cast<u8*>(g_GameGlobal->m_advData);
    if (advData != NULL) {
        info = *reinterpret_cast<muAdvComerPadInfo*>(advData + 0x4928);
    } else {
        info.m_unk0 = 0;
        info.m_padNo = -1;
    }
    muAdvComerPadInfo pad = info;
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(pad.m_padNo, &status);
    switch (m_step) {
    case Step_Request:
        m_fileComer.readRequest("/menu/adventure/comer.brres", Heaps::MenuResource, 0, 0);
        m_fileFace.readRequest(m_facePaths[m_faceNo], Heaps::MenuResource, 0, 0);
        m_fileMsg.readRequest("/menu/msg/adv_comer_en.msbin", Heaps::MenuResource, 0, 0);
        m_step = Step_Load;
        break;
    case Step_Load: {
        if (!isLoading(this)) {
            m_step = Step_Create;
        }
        break;
    }
    case Step_Create:
        createWindow();
        m_step = Step_Appear;
        break;
    case Step_Appear:
        g_sndSystem->playSE(static_cast<SndID>(0xD7), -1, 0, 0, -1);
        m_object->changeNodeAnimNIf("MenAdvComer0000_TopN__0");
        m_object->changeClrAnimNIf("MenAdvComer0000_TopN__0");
        m_step = Step_AppearWait;
        break;
    case Step_AppearWait: {
        if (isAnimEnd(this) == true) {
            m_step = Step_Show;
        }
        break;
    }
    case Step_Show:
        m_object->changeNodeAnimNIf("MenAdvComer0000_TopN__1");
        m_object->changeClrAnimNIf("MenAdvComer0000_TopN__1");
        m_step = Step_WaitButton;
        break;
    case Step_WaitButton:
        if ((status.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_A) != 0) {
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            m_step = Step_Leave;
        }
        break;
    case Step_Leave:
        m_object->changeNodeAnimNIf("MenAdvComer0000_TopN__2");
        m_object->changeClrAnimNIf("MenAdvComer0000_TopN__2");
        m_step = Step_LeaveWait;
        break;
    case Step_LeaveWait: {
        if (isAnimEnd(this) == true) {
            m_step = Step_End;
        }
        break;
    }
    case Step_End:
        m_msg->detachMuObject(m_object);
        delete m_msg;
        m_msg = NULL;
        m_menuRoot->exit();
        m_menuRoot = NULL;
        delete m_object;
        m_object = NULL;
        m_fileComer.release();
        m_fileFace.release();
        gfHeapManager::free(m_fileMsg.getBuffer());
        m_fileMsg.release();
        operator delete(m_resComer.ptr());
        operator delete(m_resFace.ptr());
        exit();
        m_step++;
        break;
    }
    m_frame++;
}

// Remembers the fighter that joins and picks the face for it.
void muAdvComerTask::setFighter(int kind) {
    m_kind = kind;
    unk2C_b1 = false;
    m_faceNo = muMenu::exchangeGmCharacterKind2Something(kind) + 1;
}

// Makes the window: the scene, the model with the face of the fighter and the text with the name.
void muAdvComerTask::createWindow() {
    m_menuRoot = MenuRoot::create("AdvComer", 0x10, "/menu/defaultcamera/CharacterSelect.brres");
    m_resComer = nw4r::g3d::ResFile(m_fileComer.getBuffer());
    m_resFace = nw4r::g3d::ResFile(m_fileFace.getBuffer());
    nw4r::g3d::ResFile::Init(&m_resComer);
    nw4r::g3d::ResFile::Init(&m_resFace);
    m_object = MuObject::create(&m_resComer, "MenAdvComer0000_TopN", 1, NULL, Heaps::MenuInstance);
    m_object->changeMaterialTex("Character_Face_Mat", 0, &m_resFace);
    m_menuRoot->scene->Insert(reinterpret_cast<u32*>(m_menuRoot->scene)[0xE4 / 4], m_object->m_scnMdl);
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x200, 1);
    m_msg->setMsgData(m_fileMsg.getBuffer());
    m_msg->attachScnMdlSimple(0, m_object->m_sceneModel, 0, 0.0625f);
    m_msg->setFontColor(0, 0, 0, 0, 0xFF);
    m_msg->getWScale(0, 50.0f);
    char name[0x100];
    char text[0x100];
    int length;
    memset(name, 0, sizeof(name));
    memset(text, 0, sizeof(text));
    const char* data = m_msg->getMsgData(0, &length);
    memcpy(text, data, length);
    strcat(name, muMenu::exchangeMuStockchkind2MuCharName(m_kind));
    strcat(name, text);
    printData__5MuMsgFUlPCvi(m_msg, 0, name, strlen(name));
}
