#include <gf/gf_3d_scene.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mu/mu_mat_color.h>
#include <nw4r/g3d/g3d_rescommon.h>
#include <snd/snd_system.h>
#include <types.h>

#include <mu/mu_title.h>

// The vertex colors of a model: main.dol has them, the shared g3d headers do not. A ResVtxClr is a pointer to its data.
namespace nw4r {
namespace g3d {
struct ResVtxClrData {
    char m_unk0[8];
    s32 m_dataOffset;               // 0x08 from the start of the data
    char m_unkC[0x12];
    u16 m_numColors;                // 0x1E
};
}  // namespace g3d
}  // namespace nw4r
extern "C" u32 GetResVtxClrNumEntries__Q34nw4r3g3d6ResMdlCFv(const nw4r::g3d::ResMdl* mdl);
extern "C" nw4r::g3d::ResVtxClrData* GetResVtxClr__Q34nw4r3g3d6ResMdlCFi(const nw4r::g3d::ResMdl* mdl, int index);
// ResVtxClr::DCStore(sync): self is the address of the pointer to the data
extern "C" void DCStore__Q34nw4r3g3d9ResVtxClrFb(nw4r::g3d::ResVtxClrData** self, bool sync);

// gfSceneRoot: main.dol has the functions, the shared header does not.
extern "C" void fn_8000D6DC(gfSceneRoot* root, nw4r::g3d::ResAnmScn* anmScn);
extern "C" void fn_8000D7B8(gfSceneRoot* root);
// nw4r::g3d::ResAnmScn
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(nw4r::g3d::ResAnmScn* self, nw4r::g3d::ResAnmScn anmScn);
extern "C" void Release__Q34nw4r3g3d9ResAnmScnFv(nw4r::g3d::ResAnmScn* self);
// main.dol: the file of the archive that has the number
extern "C" void* fn_800B03F0(gfArchive* archive, int index);

// HYPOTHESIS: the constructor takes the name of the task (it keeps the string first in the data of the unit).
inline muTitleTask::muTitleTask(const char* name)
    : gfTask(name, Category_Menu, 0xF, 8, true), m_handleA(), m_handleB(), m_resScene(), m_resTitle(), m_anmScn() { }

muTitleTask* muTitleTask::create() {
    return new (Heaps::MenuInstance) muTitleTask("Title");
}

muTitleTask::~muTitleTask() { }

void muTitleTask::processDefault() {
    switch (m_state) {
    case State_Idle:
        break;
    case State_Begin:
        m_counterA = 0;
        m_alpha = 0.0f;
        m_backgroundAlpha = 0.0f;
        if (m_short == 1) {
            m_state = State_Short;
        } else {
            m_state = State_StartLogo;
        }
        break;
    case State_StartLogo:
        m_logo->setFrame(0.0f);
        m_logo->m_modelAnim->setUpdateRate(1.0f);
        m_state = State_WaitLogo;
        m_counterB = 0;
        break;
    case State_WaitLogo:
        if (m_counterB >= 0x3A) {
            m_state = State_StartFade;
        }
        break;
    case State_StartFade:
        m_alpha = 0.0f;
        m_state = State_FadeIn;
        m_backgroundAlpha = 0.0f;
        m_counterB = 0;
        break;
    case State_FadeIn:
        m_alpha = static_cast<float>(m_counterB) / 70.0f;
        m_backgroundAlpha = static_cast<float>(m_counterB) / 70.0f;
        if (m_counterB >= 0x46) {
            m_state = State_FadeInEnd;
        }
        break;
    case State_FadeInEnd:
        m_alpha = 1.0f;
        m_state = State_WaitAnim;
        m_backgroundAlpha = 1.0f;
        break;
    case State_WaitAnim:
        if (250.0f <= m_logo->m_modelAnim->getFrame()) {
            m_logo->m_modelAnim->setUpdateRate(0.0f);
            m_state = State_Hold;
        }
        break;
    case State_Hold:
        m_state = State_HoldEnd;
        break;
    case State_HoldEnd:
        m_state = State_WaitTimeOut;
        m_counterB = 0;
        break;
    case State_WaitTimeOut:
        if (m_counterA >= 0x3C0) {
            m_timeOut = 1;
            m_state = State_TimeOut;
        }
        break;
    case State_TimeOut:
        m_state = State_Replay;
        break;
    case State_Replay:
        m_logo->setFrame(261.0f);
        m_logo->m_modelAnim->setUpdateRate(1.0f);
        m_state = State_ReplayWait;
        break;
    case State_ReplayWait:
        if (280.0f <= m_logo->m_modelAnim->getFrame()) {
            m_state = State_ReplayEnd;
        }
        break;
    case State_ReplayEnd:
        m_logo->m_modelAnim->setUpdateRate(0.0f);
        m_state = State_End;
        break;
    case State_Short:
        m_logo->setFrame(0.0f);
        m_logo->m_modelAnim->setUpdateRate(1.0f);
        m_state = State_ShortWait;
        m_counterB = 0;
        break;
    case State_ShortWait:
        break;
    case State_ShortEnd:
        m_state = State_End;
        break;
    case 18:
        break;
    case 19:
        break;
    }
    m_counterB++;
    m_counterA++;
}

// Gives the alpha to the materials of the background, and to its vertex colors.
void muTitleTask::renderPre() {
    if (m_state != State_Idle && m_state != State_End && m_state != 19) {
        float alpha = m_alpha;
        nw4r::g3d::ResMdl* mdl = &m_background->m_resMdl;
        u32 count = GetResMatNumEntries__Q34nw4r3g3d6ResMdlCFv(mdl);
        for (u32 i = 0; i < count; i++) {
            nw4r::g3d::ScnMdl* scnMdl = m_background->m_scnMdl;
            nw4r::g3d::ResMat mat = GetResMat__Q34nw4r3g3d6ResMdlCFi(mdl, i);
            nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
            nw4r::g3d::ResMatChan chan = GetResMatChanEx__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFv(&access);
            GXColor color;
            chan.GXGetChanMatColor(GX_ALPHA0, &color);
            color.a = static_cast<int>(static_cast<float>(color.a) * alpha);
            chan.GXSetChanMatColor(GX_ALPHA0, color);
        }
        setObjectBGVertexAlpha(m_backgroundAlpha);
    }
}

void muTitleTask::initialize() {
    unk2C_b1 = false;
    m_background = NULL;
    m_logo = NULL;
    m_logoShort = NULL;
    m_alpha = 0.0f;
    m_backgroundAlpha = 0.0f;
    m_vertexColors = NULL;
    m_vertexColorCount = 0;
    m_normalAdded = 0;
    m_shortAdded = 0;
    m_timeOut = 0;
    m_short = 0;
    m_counterA = 0;
    m_state = 0;
    m_counterB = 0;
}

void muTitleTask::release() {
    for (u32 i = 0; i < m_vertexColorCount; i++) {
        delete[] m_vertexColors[i];
        m_vertexColors[i] = NULL;
    }
    delete[] m_vertexColors;
    m_vertexColors = NULL;
    m_vertexColorCount = 0;
    delete m_background;
    m_background = NULL;
    delete m_logo;
    m_logo = NULL;
    delete m_logoShort;
    m_logoShort = NULL;
    // HYPOTHESIS: the game releases the same file twice (the second one is m_resTitle)
    m_resScene.Release();
    m_resScene.Release();
}

// MATCH-ONLY: the game has this function in the unit (it reads the files of the title), nothing calls it and the linker takes
// it out (the names of the files stay in the data of the unit).
void muTitleTask::createDataRequest() {
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        m_handleA.readRequest("/menu/defaultcamera/DefaultScene.brres", Heaps::MenuResource, 0, 0);
    } else {
        m_handleA.readRequest("/menu/defaultcamera/DefaultSceneWide.brres", Heaps::MenuResource, 0, 0);
    }
    m_handleB.readRequest("/menu/titleloop/title.brres", Heaps::MenuResource, 0, 0);
}

// Makes the models of the title from the file of the archive (the scene depends on the shape of the screen).
void muTitleTask::createData(gfArchive* archive) {
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        m_resScene = nw4r::g3d::ResFile(fn_800B03F0(archive, 0xA));
    } else {
        m_resScene = nw4r::g3d::ResFile(fn_800B03F0(archive, 0xB));
    }
    m_resTitle = nw4r::g3d::ResFile(fn_800B03F0(archive, 0x3C));
    nw4r::g3d::ResFile::Init(&m_resScene);
    nw4r::g3d::ResFile::Init(&m_resTitle);
    m_background = MuObject::create(&m_resTitle, "TitleBg", 1, NULL, Heaps::MenuInstance);
    m_logo = MuObject::create(&m_resTitle, "TitleFront", 1, NULL, Heaps::MenuInstance);
    m_logoShort = MuObject::create(&m_resTitle, "TitleFront2s", 1, NULL, Heaps::MenuInstance);
    initVertexColorParam();
}

// Puts the animation of the scene and the models of the title in the scene.
void muTitleTask::addModelNormal() {
    if (m_normalAdded != 1) {
        m_anmScn = m_resScene.GetResAnmScn(0);
        Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&m_anmScn, m_anmScn);
        fn_8000D6DC(g_gfSceneRoot, &m_anmScn);
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_background->m_scnMdl);
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_logo->m_scnMdl);
        m_normalAdded = 1;
    }
}

void muTitleTask::removeModelNormal() {
    if (m_normalAdded) {
        fn_8000D7B8(g_gfSceneRoot);
        Release__Q34nw4r3g3d9ResAnmScnFv(&m_anmScn);
        g_gfSceneRoot->remove(m_background->m_scnMdl);
        g_gfSceneRoot->remove(m_logo->m_scnMdl);
        m_normalAdded = 0;
    }
}

void muTitleTask::addModelShort() {
    if (m_shortAdded != 1) {
        m_anmScn = m_resScene.GetResAnmScn(0);
        Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&m_anmScn, m_anmScn);
        fn_8000D6DC(g_gfSceneRoot, &m_anmScn);
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_logoShort->m_scnMdl);
        m_shortAdded = 1;
    }
}

void muTitleTask::removeModelShort() {
    if (m_shortAdded) {
        fn_8000D7B8(g_gfSceneRoot);
        Release__Q34nw4r3g3d9ResAnmScnFv(&m_anmScn);
        g_gfSceneRoot->remove(m_logoShort->m_scnMdl);
        m_shortAdded = 0;
    }
}

// Takes all the models out of the scene.
void muTitleTask::skip() {
    removeModelNormal();
    removeModelShort();
}

// Asks the title to go on with the demo: plays the sound and goes to the state of the time out, unless the title is already
// on the way (the second part of the animation, or the end).
void muTitleTask::requestTimeOut() {
    switch (m_state) {
    case State_Replay:
    case State_ReplayWait:
    case State_ReplayEnd:
    case State_End:
    case 19:
        break;
    default:
        g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
        m_timeOut = 0;
        m_state = State_Replay;
        break;
    }
}

// Keeps a copy of the vertex colors of the background (their alpha changes when the background fades).
void muTitleTask::initVertexColorParam() {
    nw4r::g3d::ResMdl* mdl = &m_background->m_resMdl;
    m_vertexColorCount = GetResVtxClrNumEntries__Q34nw4r3g3d6ResMdlCFv(mdl);
    m_vertexColors = new (Heaps::MenuInstance) u8*[m_vertexColorCount];
    for (u32 i = 0; i < m_vertexColorCount; i++) {
        nw4r::g3d::ResVtxClrData* vtxClr = GetResVtxClr__Q34nw4r3g3d6ResMdlCFi(mdl, i);
        u16 count = vtxClr->m_numColors;
        int size = count * 4;
        m_vertexColors[i] = new (Heaps::MenuInstance) u8[size];
        memset(m_vertexColors[i], 0, count * 4);
        const u8* src = vtxClr->m_dataOffset != 0 ? reinterpret_cast<u8*>(vtxClr) + vtxClr->m_dataOffset : NULL;
        for (int j = 0; j < size; j++) {
            m_vertexColors[i][j] = src[j];
        }
    }
}

// Sets the alpha of the vertex colors of the background from the ones that were kept.
// MATCH-ONLY: the game does not inline this function into renderPre.
#pragma dont_inline on
void muTitleTask::setObjectBGVertexAlpha(float alpha) {
    for (u32 i = 0; i < m_vertexColorCount; i++) {
        nw4r::g3d::ResVtxClrData* vtxClr = GetResVtxClr__Q34nw4r3g3d6ResMdlCFi(&m_background->m_resMdl, i);
        u8* data = vtxClr->m_dataOffset != 0 ? reinterpret_cast<u8*>(vtxClr) + vtxClr->m_dataOffset : NULL;
        u16 count = vtxClr->m_numColors;
        for (int j = 0; j < count; j++) {
            data[j * 4 + 3] = static_cast<int>(alpha * static_cast<float>(m_vertexColors[i][j * 4 + 3]));
        }
        DCStore__Q34nw4r3g3d9ResVtxClrFb(&vtxClr, false);
    }
}

#pragma dont_inline reset

void muTitleTask::resetAnimetionParam() {
    m_logo->setFrame(0.0f);
}
