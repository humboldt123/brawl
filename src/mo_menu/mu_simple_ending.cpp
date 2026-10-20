#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_simple_ending.h>

// main.dol functions with names that the shared headers do not declare.
// Creates the camera task that draws the menu models of the name (the task keeps the scene group at +0x48)
extern "C" muEndCameraTask* fn_800AFBFC(const char* name, int heap, gfArchive* archive, int unk);
// MuObject::getNode(name) and ResNode::SetRotate(x, y, z)
extern "C" void* getNode__8MuObjectFPCc(MuObject* object, const char* name);
extern "C" void ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(void* node, float x, float y, float z);
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// MuObject::changeMaterialTex(material name, texture number, texture file)
extern "C" void changeMaterialTex__8MuObjectFPCciPQ34nw4r3g3d7ResFile(MuObject* object, const char* material, int texture,
                                                                     nw4r::g3d::ResFile* resFile);
// nw4r::g3d::ResFile::Release(), GetResAnmScn(index) and ResAnmScn::Bind(animation)
extern "C" void Release__Q34nw4r3g3d7ResFileFv(nw4r::g3d::ResFile* resFile);
extern "C" void* GetResAnmScn__Q34nw4r3g3d7ResFileCFi(const nw4r::g3d::ResFile* resFile, int index);
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(void* self, void* anm);
// gfSceneRoot::setInfoCamResAnm(animation), gfSceneRoot::removeInfoCamResAnm() and gfSceneRoot::remove(layer, model)
extern "C" void fn_8000D6DC(gfSceneRoot* root, void* resAnmScn);
extern "C" void fn_8000D7B8(gfSceneRoot* root);
extern "C" void fn_8000D1B8(gfSceneRoot* root, int layer, void* scnMdl);
// mvMoviePlayer: the constructor, the destructor (with a flag), load(path), getVideoState(), getTotalFrame(),
// drawFrame(...) (returns the frame that was drawn), play() and setScnMdl(model, unk)
extern "C" mvMoviePlayer* fn_8007E99C(void* player);
extern "C" void fn_8007EA78(mvMoviePlayer* player, int flag);
extern "C" void fn_8007EC08(mvMoviePlayer* player, const char* path, int unk0, int unk1, int unk2);
extern "C" int fn_8007EF64(mvMoviePlayer* player);
extern "C" int fn_8007EF68(mvMoviePlayer* player);
extern "C" int fn_8007EF70(mvMoviePlayer* player, int unk0, int unk1, int unk2, int unk3);
extern "C" void fn_8007F148(mvMoviePlayer* player);
extern "C" void fn_8007F2B4(mvMoviePlayer* player, void* scnMdl, int unk);

// The flag of the menu data (of the record of the game) that says that the screen is wide.
// HYPOTHESIS: the name; the flag is the high bit of the byte at 0x28.
struct muSimpleEndingMenuBits {
    u8 m_unk00[0x28];
    u8 m_isWide : 1;
};

static inline int isWideScreen() {
    return reinterpret_cast<muSimpleEndingMenuBits*>(g_GameGlobal->getGlobalRecordMenuDatap())->m_isWide;
}

muEndPicTask* muEndPicTask::create() {
    return new (Heaps::MenuInstance) muEndPicTask();
}

muEndPicTask::~muEndPicTask() { }

// The steps: the picture is shown with a sound, a button takes it away.
void muEndPicTask::processDefault() {
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    switch (m_step) {
    case Step_Create:
        m_step = Step_Appear;
        break;
    case Step_Appear: {
        // HYPOTHESIS: the top three bits of the byte at 0x2E of the record that the game global points to at 0x10 say
        // which ending it is (4: the one with the other sound)
        struct EndingByte {
            unsigned char m_kind : 3;
            unsigned char m_rest : 5;
        };
        if (reinterpret_cast<EndingByte*>(&reinterpret_cast<u8*>(*reinterpret_cast<void**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x10))[0x2E])->m_kind == 4) {
            g_sndSystem->playSE(static_cast<SndID>(0x1FD4), -1, 0, 0, -1);
        } else {
            g_sndSystem->playSE(static_cast<SndID>(0x1FD3), -1, 0, 0, -1);
        }
        nw4r::g3d::ScnGroup* root = m_cameraTask->m_scnRoot;
        root->Insert(root->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
        m_step = Step_WaitButton;
        m_frame = 0;
        break;
    }
    case Step_WaitButton: {
        // A, B, Start (or the button that is bit 20 of the status) takes the picture away
        u32 pressed = pad.m_buttonsPressedThisFrame.bits;
        bool isPressed;
        if ((pressed & 0x100) != 0) {
            isPressed = true;
        } else if ((pressed & 0x200) != 0) {
            isPressed = true;
        } else if ((pressed & 0x1000) != 0) {
            isPressed = true;
        } else {
            isPressed = (pressed >> 20) & 1;
        }
        if (isPressed == true) {
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            m_step = Step_Pressed;
        }
        break;
    }
    case Step_Pressed:
        m_step = Step_Leave;
        break;
    case Step_Leave:
        m_object->changeClrAnimN("MenEndpictures0000_TopN__0");
        m_object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        m_step = Step_LeaveWait;
        break;
    case Step_LeaveWait:
        if (m_object->isClrAnimFinished() == 1u) {
            m_step = Step_Remove;
        }
        break;
    case Step_Remove: {
        nw4r::g3d::ScnGroup* root = m_cameraTask->m_scnRoot;
        root->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
        m_step = Step_Done;
        break;
    }
    case 8:
    case 9:
        break;
    }
    m_frame++;
}

void muEndPicTask::initialize() {
    unk2C_b1 = false;
}

void muEndPicTask::release() {
    m_msg->detachMuObject(m_object);
    delete m_msg;
    m_msg = NULL;
    m_cameraTask->exit();
    m_cameraTask = NULL;
    delete m_object;
    m_object = NULL;
    Release__Q34nw4r3g3d7ResFileFv(&m_resFile);
    Release__Q34nw4r3g3d7ResFileFv(&m_resFileTex);
}

// Creates the camera, the picture, the textures and the text with the clear time.
void muEndPicTask::createData(gfArchive* archive) {
    m_cameraTask = fn_800AFBFC("EndPicTask", 0x10, archive, 0);
    m_resFile = nw4r::g3d::ResFile(archive->getData(Data_Type_Model, 3, 0xFFFE));
    m_resFileTex = nw4r::g3d::ResFile(archive->getData(Data_Type_Model, 4, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resFile);
    nw4r::g3d::ResFile::Init(&m_resFileTex);
    m_object = MuObject::create(&m_resFile, "MenEndpictures0000_TopN", 1, NULL, Heaps::MenuInstance);
    changeMaterialTex__8MuObjectFPCciPQ34nw4r3g3d7ResFile(m_object, "lambert37", 0, &m_resFileTex);
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x200, 1);
    m_msg->attachScnMdlSimple(0, m_object->m_sceneModel, 0, 0.0625f);
    m_msg->setFontColor(0, 0xFF, 0xFF, 0xFF, 0xFF);
    m_msg->setEdgeEnable(0, true);
    m_msg->setEdge(0, 0, 0, 0, 3.0f);
    m_msg->printf(0, "");
    if (isWideScreen()) {
        void* node = getNode__8MuObjectFPCc(m_object, "dammy");
        ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(&node, 1.2f, 1.2f, 1.0f);
    }
}

// Writes the clear time of a boss battle (minutes, seconds and hundredths) into the text; the longest time is 99:59 99.
void muEndPicTask::setBossBattleClearTime(u32 frames) {
    u32 framesPerSecond = *reinterpret_cast<u16*>(reinterpret_cast<u8*>(g_gfApplication) + 0xF8);
    u32 minutes;
    u32 seconds;
    int hundredths;
    if (frames >= framesPerSecond * 6000) {
        minutes = 99;
        seconds = 59;
        hundredths = 99;
    } else {
        u32 framesPerMinute = framesPerSecond * 60;
        minutes = frames / framesPerMinute;
        frames = frames - minutes * framesPerMinute;
        seconds = frames / framesPerSecond;
        hundredths = static_cast<int>(static_cast<float>(frames - seconds * framesPerSecond) * 100.0f / static_cast<float>(framesPerSecond));
    }
    m_msg->printf(0, "%02d:%02d %02d", minutes, seconds, hundredths);
}

muEndMovieTask* muEndMovieTask::create() {
    return new (Heaps::MenuInstance) muEndMovieTask();
}

muEndMovieTask::muEndMovieTask() : gfTask("EndingMovie", Category_Menu, 0xF, 8, true) {
    m_resFile = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFileModel = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    memset(m_paths, 0, sizeof(m_paths));
    int i = 0;
    sprintf(m_paths[i++], "/movie/----------.thp");
    sprintf(m_paths[i++], "/movie/End_Mario.thp");
    sprintf(m_paths[i++], "/movie/End_Donky.thp");
    sprintf(m_paths[i++], "/movie/End_Link.thp");
    sprintf(m_paths[i++], "/movie/End_Samus.thp");
    sprintf(m_paths[i++], "/movie/End_Yoshi.thp");
    sprintf(m_paths[i++], "/movie/End_Kirby.thp");
    sprintf(m_paths[i++], "/movie/End_Fox.thp");
    sprintf(m_paths[i++], "/movie/End_Pikachu.thp");
    sprintf(m_paths[i++], "/movie/End_Luigi.thp");
    sprintf(m_paths[i++], "/movie/End_Captain.thp");
    sprintf(m_paths[i++], "/movie/End_Ness.thp");
    sprintf(m_paths[i++], "/movie/End_Koopa.thp");
    sprintf(m_paths[i++], "/movie/End_Peach.thp");
    sprintf(m_paths[i++], "/movie/End_Zelda.thp");
    sprintf(m_paths[i++], "/movie/End_Zelda.thp");
    sprintf(m_paths[i++], "/movie/End_Iceclimber.thp");
    sprintf(m_paths[i++], "/movie/End_Marth.thp");
    sprintf(m_paths[i++], "/movie/End_GameWatch.thp");
    sprintf(m_paths[i++], "/movie/End_Falco.thp");
    sprintf(m_paths[i++], "/movie/End_Ganon.thp");
    sprintf(m_paths[i++], "/movie/----------.thp");
    sprintf(m_paths[i++], "/movie/End_Metaknight.thp");
    sprintf(m_paths[i++], "/movie/End_Pit.thp");
    sprintf(m_paths[i++], "/movie/End_SZerosuit.thp");
    sprintf(m_paths[i++], "/movie/End_Pikmin.thp");
    sprintf(m_paths[i++], "/movie/End_Lucas.thp");
    sprintf(m_paths[i++], "/movie/End_Diddy.thp");
    sprintf(m_paths[i++], "/movie/End_Poke.thp");
    sprintf(m_paths[i++], "/movie/End_Poke.thp");
    sprintf(m_paths[i++], "/movie/End_Poke.thp");
    sprintf(m_paths[i++], "/movie/End_Poke.thp");
    sprintf(m_paths[i++], "/movie/End_Dedede.thp");
    sprintf(m_paths[i++], "/movie/End_Lucario.thp");
    sprintf(m_paths[i++], "/movie/End_Ike.thp");
    sprintf(m_paths[i++], "/movie/End_Robot.thp");
    sprintf(m_paths[i++], "/movie/----------.thp");
    sprintf(m_paths[i++], "/movie/End_Purin.thp");
    sprintf(m_paths[i++], "/movie/End_Wario.thp");
    sprintf(m_paths[i++], "/movie/----------.thp");
    sprintf(m_paths[i++], "/movie/----------.thp");
    sprintf(m_paths[i++], "/movie/End_ToonLink.thp");
    sprintf(m_paths[i++], "/movie/----------.thp");
    sprintf(m_paths[i++], "/movie/----------.thp");
    sprintf(m_paths[i++], "/movie/End_Wolf.thp");
    sprintf(m_paths[i++], "/movie/----------.thp");
    sprintf(m_paths[i++], "/movie/End_Snake.thp");
    sprintf(m_paths[i++], "/movie/End_Sonic.thp");
    m_object = NULL;
    m_player = NULL;
    m_step = 0;
    m_frame = 0;
}

muEndMovieTask::~muEndMovieTask() { }

// The steps: put the movie on the screen, play it until the end or until a button is pressed, take it away.
void muEndMovieTask::processDefault() {
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    switch (m_step) {
    case Step_Create:
        m_step = Step_Show;
        break;
    case Step_Show:
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_object->m_sceneModel);
        m_step = Step_Play;
        break;
    case Step_Play: {
        u32 pressed = pad.m_buttonsPressedThisFrame.bits;
        u32 isPressed;
        if ((pressed & 0x100) != 0) {
            isPressed = 1;
        } else if ((pressed & 0x200) != 0) {
            isPressed = 1;
        } else if ((pressed & 0x1000) != 0) {
            isPressed = 1;
        } else {
            isPressed = (pressed >> 20) & 1;
        }
        if (isPressed == 1) {
            m_step = Step_Hide;
        }
        if (m_frameNum <= m_frameNo + 1) {
            m_step = Step_Hide;
        }
        break;
    }
    case Step_Hide:
        fn_8000D1B8(g_gfSceneRoot, 8, m_object->m_sceneModel);
        m_step = Step_Done;
        break;
    }
    m_frame++;
}

void muEndMovieTask::renderOpa() {
    switch (m_step) {
    case Step_Play:
        m_frameNo = fn_8007EF70(m_player, 0, 0, -1, -1);
        break;
    }
}

void muEndMovieTask::initialize() {
    unk2C_b1 = false;
    void* memory = operator new(1, Heaps::MenuInstance);
    mvMoviePlayer* player = static_cast<mvMoviePlayer*>(memory);
    if (memory != NULL) {
        player = fn_8007E99C(memory);
    }
    m_player = player;
    m_frameNum = 0;
    m_frameNo = 0;
}

void muEndMovieTask::release() {
    fn_8000D7B8(g_gfSceneRoot);
    Release__Q34nw4r3g3d7ResFileFv(&m_resFile);
    Release__Q34nw4r3g3d7ResFileFv(&m_resFileModel);
    delete m_object;
    m_object = NULL;
    fn_8007EA78(m_player, 1);
    m_player = NULL;
}

// Starts to load the movie of a character.
void muEndMovieTask::load(int index) {
    fn_8007EC08(m_player, m_paths[index], 0, 0, 0);
}

// Whether the movie is still loading; when it is not the number of its frames is known.
bool muEndMovieTask::isLoading() {
    int state = fn_8007EF64(m_player);
    if (state != 1) {
        return true;
    }
    m_frameNum = fn_8007EF68(m_player);
    return false;
}

// Binds the camera, creates the model for the movie (scaled for the screen) and starts the movie.
void muEndMovieTask::createData(gfArchive* archive) {
    void* anm;
    void* node;
    void* anm2;
    int wide = isWideScreen() != 0;
    m_resFile = nw4r::g3d::ResFile(archive->getData(Data_Type_Model, wide, 0xFFFE));
    m_resFileModel = nw4r::g3d::ResFile(archive->getData(Data_Type_Model, 2, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resFile);
    nw4r::g3d::ResFile::Init(&m_resFileModel);
    anm = NULL;
    anm2 = GetResAnmScn__Q34nw4r3g3d7ResFileCFi(&m_resFile, 0);
    anm = anm2;
    Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&anm, &anm2);
    fn_8000D6DC(g_gfSceneRoot, &anm);
    m_object = MuObject::create(&m_resFileModel, "MenAdvChrCmn1000_TopN", 1, NULL, Heaps::MenuInstance);
    node = getNode__8MuObjectFPCc(m_object, "dammy");
    if (wide == 0) {
        ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(&node, 0.65853655f, 0.6724138f, 1.0f);
    } else {
        ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(&node, 0.8780488f, 0.8965517f, 1.0f);
    }
    fn_8007F2B4(m_player, m_object->m_sceneModel, 0);
    fn_8007F148(m_player);
    fn_8007EF70(m_player, 0, 0, -1, -1);
}
