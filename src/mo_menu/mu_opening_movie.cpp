#include <gf/gf_3d_scene.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <types.h>

#include <mu/mu_opening_movie.h>

// gfSceneRoot: main.dol has the functions, the shared header does not.
extern "C" void fn_8000D6DC(gfSceneRoot* root, nw4r::g3d::ResAnmScn* anmScn);
extern "C" void fn_8000D7B8(gfSceneRoot* root);
extern "C" void fn_8000D1B8(gfSceneRoot* root, int layer, nw4r::g3d::ScnMdl* model);
// nw4r::g3d::ResAnmScn, ResNode
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(nw4r::g3d::ResAnmScn* self, nw4r::g3d::ResAnmScn anmScn);
extern "C" void ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(void* node, float x, float y, float z);
// main.dol: the file of the archive that has the number
extern "C" void* fn_800B03F0(gfArchive* archive, int index);
// MuObject::getNode(name)
extern "C" void* getNode__8MuObjectFPCc(MuObject* object, const char* name);

// HYPOTHESIS: the constructor takes the name of the task (it keeps the string first in the data of the unit).
inline muOpeningMovieTask::muOpeningMovieTask(const char* name)
    : gfTask(name, Category_Menu, 0xF, 8, true), m_handleA(), m_handleB(), m_resScene(), m_resChr(), m_object(NULL),
      m_movie(NULL), m_state(0), m_counter(0) { }

muOpeningMovieTask* muOpeningMovieTask::create() {
    return new (Heaps::MenuInstance) muOpeningMovieTask("OpeningMovie");
}

muOpeningMovieTask::~muOpeningMovieTask() { }

void muOpeningMovieTask::processDefault() {
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
void muOpeningMovieTask::renderOpa() {
    switch (m_state) {
    case 2:
        m_frame = fn_8007EF70(m_movie, 0, 0, -1, -1);
        break;
    }
}

void muOpeningMovieTask::initialize() {
    unk2C_b1 = false;
    muMovie* movie = reinterpret_cast<muMovie*>(new (Heaps::MenuInstance) u8);
    if (movie != NULL) {
        movie = fn_8007E99C(movie);
    }
    m_movie = movie;
    m_frameCount = 0;
    m_frame = 0;
}

void muOpeningMovieTask::release() {
    fn_8000D7B8(g_gfSceneRoot);
    m_resScene.Release();
    m_resChr.Release();
    delete m_object;
    m_object = NULL;
    fn_8007EA78(m_movie, 1);
    m_movie = NULL;
}

void muOpeningMovieTask::openMovie() {
    fn_8007EC08(m_movie, "/movie/OPMovie.thp", 0, 0, 0);
}

// Whether the movie is still being prepared; when it is ready, takes how many frames it has.
bool muOpeningMovieTask::isLoadingMovie() {
    if (fn_8007EF64(m_movie) != 1) {
        return true;
    }
    m_frameCount = fn_8007EF68(m_movie);
    return false;
}

// Makes the model that shows the movie, with the camera and the light of the scene.
void muOpeningMovieTask::createData(gfArchive* archive) {
    m_resScene = nw4r::g3d::ResFile(fn_800B03F0(archive, 0x15));
    m_resChr = nw4r::g3d::ResFile(fn_800B03F0(archive, 0x1E));
    nw4r::g3d::ResFile::Init(&m_resScene);
    nw4r::g3d::ResFile::Init(&m_resChr);
    nw4r::g3d::ResAnmScn anmScn;
    anmScn = m_resScene.GetResAnmScn(0);
    Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&anmScn, anmScn);
    fn_8000D6DC(g_gfSceneRoot, &anmScn);
    m_object = MuObject::create(&m_resChr, "MenAdvChrCmn1000_TopN", 1, NULL, Heaps::MenuInstance);
}

// Sets the model that shows the movie, and its size for the shape of the screen.
void muOpeningMovieTask::setupMovie() {
    fn_8007F2B4(m_movie, m_object->m_sceneModel, 0);
    float x = 1.0f;
    float y = 1.0344827f;
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        void* node = getNode__8MuObjectFPCc(m_object, "dammy");
        ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(&node, 0.8f * x, 0.8f * y, x);
    } else {
        void* node = getNode__8MuObjectFPCc(m_object, "dammy");
        ResNode_SetRotate__FPQ34nw4r3g3d7ResNodefff(&node, x, y, x);
    }
}

void muOpeningMovieTask::startMovie() {
    fn_8007F148(m_movie);
    fn_8007EF70(m_movie, 0, 0, -1, -1);
}

void muOpeningMovieTask::addMovieModel() {
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_object->m_sceneModel);
}

void muOpeningMovieTask::removeMovieModel() {
    fn_8000D1B8(g_gfSceneRoot, 8, m_object->m_sceneModel);
}

// Takes the model out of the scene and ends the movie.
void muOpeningMovieTask::skip() {
    removeMovieModel();
    m_state = 3;
}
