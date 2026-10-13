#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <types.h>

#include <mu/mu_how_to_play.h>

// gfSceneRoot: main.dol has the functions, the shared header does not.
extern "C" void fn_8000D6DC(gfSceneRoot* root, nw4r::g3d::ResAnmScn* anmScn);
extern "C" void fn_8000D7B8(gfSceneRoot* root);
extern "C" void fn_8000D1B8(gfSceneRoot* root, int layer, nw4r::g3d::ScnMdl* model);
// nw4r::g3d::ResAnmScn, ResNode
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(nw4r::g3d::ResAnmScn* self, nw4r::g3d::ResAnmScn anmScn);
extern "C" void ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(void* node, float x, float y, float z);
// MuObject::getNode(name)
extern "C" void* getNode__8MuObjectFPCc(MuObject* object, const char* name);

// HYPOTHESIS: the constructor takes the name of the task (it keeps the string first in the data of the unit).
inline muHowToPlayTask::muHowToPlayTask(const char* name)
    : gfTask(name, Category_Menu, 0xF, 8, true), m_handleScene(), m_handleChr(), m_resScene(), m_resChr(), m_object(NULL),
      m_movie(NULL), m_state(0), m_counter(0) { }

muHowToPlayTask* muHowToPlayTask::create() {
    return new (Heaps::MenuInstance) muHowToPlayTask("HowToPlay");
}

muHowToPlayTask::~muHowToPlayTask() { }

void muHowToPlayTask::processDefault() {
    switch (m_state) {
    case 0:
        break;
    case 1:
        m_state = 2;
        break;
    case 2:
        if (m_frameCount <= m_frame + 1) {
            m_state = 3;
        }
        break;
    case 3:
        break;
    case 4:
        break;
    }
    m_counter++;
}

// Plays a frame of the movie while the movie runs.
void muHowToPlayTask::renderOpa() {
    switch (m_state) {
    case 2:
        m_frame = fn_8007EF70(m_movie, 0, 0, -1, -1);
        break;
    }
}

void muHowToPlayTask::initialize() {
    unk2C_b1 = false;
    muMovie* movie = reinterpret_cast<muMovie*>(new (Heaps::MenuInstance) u8);
    if (movie != NULL) {
        movie = fn_8007E99C(movie);
    }
    m_movie = movie;
    m_frameCount = 0;
    m_frame = 0;
}

void muHowToPlayTask::release() {
    fn_8000D7B8(g_gfSceneRoot);
    gfHeapManager::free(m_handleScene.getBuffer());
    m_handleScene.release();
    gfHeapManager::free(m_handleChr.getBuffer());
    m_handleChr.release();
    m_resScene.Release();
    m_resChr.Release();
    delete m_object;
    m_object = NULL;
    fn_8007EA78(m_movie, 1);
    m_movie = NULL;
}

// Starts to read the files of the scene and of the model, and opens the movie.
void muHowToPlayTask::openMovie() {
    m_handleScene.readRequest("/menu/defaultcamera/AdvMovieWide.brres", Heaps::MenuResource, 0, 0);
    m_handleChr.readRequest("/menu/adventure/visual.brres", Heaps::MenuResource, 0, 0);
    fn_8007EC08(m_movie, "/movie/HowtoPlay.thp", 0, 0, 0);
}

// Whether the files or the movie are still being read; when all is ready, takes how many frames the movie has.
bool muHowToPlayTask::isLoadingMovie() {
    if (!m_handleScene.isReady()) {
        return true;
    }
    if (!m_handleChr.isReady()) {
        return true;
    }
    if (fn_8007EF64(m_movie) != 1) {
        return true;
    }
    m_frameCount = fn_8007EF68(m_movie);
    return false;
}

// Makes the model that shows the movie, with the camera and the light of the scene, and its size for the shape of the screen.
void muHowToPlayTask::createData() {
    m_resScene = nw4r::g3d::ResFile(m_handleScene.getBuffer());
    m_resChr = nw4r::g3d::ResFile(m_handleChr.getBuffer());
    nw4r::g3d::ResFile::Init(&m_resScene);
    nw4r::g3d::ResFile::Init(&m_resChr);
    nw4r::g3d::ResAnmScn anmScn;
    anmScn = m_resScene.GetResAnmScn(0);
    Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&anmScn, anmScn);
    fn_8000D6DC(g_gfSceneRoot, &anmScn);
    m_object = MuObject::create(&m_resChr, "MenAdvChrCmn1000_TopN", 1, NULL, Heaps::MenuInstance);
    void* node = getNode__8MuObjectFPCc(m_object, "dammy");
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(&node, 0.9756098f, 0.75f, 1.0f);
    } else {
        ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(&node, 0.9756098f, 1.0f, 1.0f);
    }
    fn_8007F2B4(m_movie, m_object->m_sceneModel, 0);
}

void muHowToPlayTask::startMovie() {
    fn_8007F148(m_movie);
    fn_8007EF70(m_movie, 0, 0, -1, -1);
}

void muHowToPlayTask::addMovieModel() {
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_object->m_sceneModel);
}

void muHowToPlayTask::removeMovieModel() {
    fn_8000D1B8(g_gfSceneRoot, 8, m_object->m_sceneModel);
}

// Takes the model out of the scene and ends the movie.
void muHowToPlayTask::skip() {
    removeMovieModel();
    m_state = 3;
}
