#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mu/mu_mat_color.h>
#include <types.h>
#include <ut/ut_relocate.h>

#include <mu/mu_word_of_song.h>

// gfSceneRoot: main.dol has the functions, the shared header does not.
extern "C" void fn_8000D6DC(gfSceneRoot* root, nw4r::g3d::ResAnmScn* anmScn);
extern "C" void fn_8000D7B8(gfSceneRoot* root);
// nw4r::g3d::ResAnmScn
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(nw4r::g3d::ResAnmScn* self, nw4r::g3d::ResAnmScn anmScn);
extern "C" void Release__Q34nw4r3g3d9ResAnmScnFv(nw4r::g3d::ResAnmScn* self);

inline muWordOfSongTask::muWordOfSongTask(const char* name)
    : gfTask(name, Category_Menu, 0xF, 8, true), m_handleScene(), m_handleFade(), m_handleWord(), m_handleParam(), m_resScene(),
      m_resFade(), m_resWord(), m_anmScn() {
    m_state = 0;
}

muWordOfSongTask* muWordOfSongTask::create() {
    return new (Heaps::MenuInstance) muWordOfSongTask("Song of word");
}

muWordOfSongTask::~muWordOfSongTask() { }

// Whether a frame is in a range (the ends belong to it).
static inline bool isBetween(int first, int last, int frame) {
    if (first <= frame && frame <= last) {
        return true;
    }
    return false;
}

// Keeps an alpha between 0 and 1.
static inline float clampAlpha(float alpha) {
    if (alpha < 0.0f) {
        alpha = 0.0f;
    }
    if (1.0f < alpha) {
        alpha = 1.0f;
    }
    return alpha;
}

void muWordOfSongTask::processDefault() {
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    switch (m_state) {
    case State_Idle:
        break;
    case State_Start:
        m_fade->m_state = 1;
        m_state = 2;
        m_frame = 0;
        break;
    case State_Play:
        if (m_param->m_endFrame <= m_frame) {
            m_state = 3;
            m_fade->setFade(CFadeColor(0.0f, 0.0f, 0.0f, 1.0f), CFadeColor(0.0f, 0.0f, 0.0f, 0.0f), 240.0f);
        }
        break;
    case State_FadeOut:
        break;
    case State_End:
        break;
    }
    if (m_state == 2 || m_state == 3) {
        for (int i = 0; i < m_count; i++) {
            int start = m_entries[i].m_start;
            if (isBetween(start, m_entries[i].m_end, m_frame) == true) {
                if (isBetween(start, start + m_entries[i].m_fadeIn, m_frame) == true) {
                    setAlpha(i, clampAlpha(m_entries[i].m_fadeInStep + m_entries[i].m_alpha));
                }
                if (isBetween(m_entries[i].m_end - m_entries[i].m_fadeOut, m_entries[i].m_end, m_frame) == true) {
                    setAlpha(i, clampAlpha(m_entries[i].m_fadeOutStep + m_entries[i].m_alpha));
                }
                if (m_entries[i].m_visible == 0) {
                    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_words[i]->m_scnMdl);
                    m_entries[i].m_visible = 1;
                }
            } else if (m_entries[i].m_visible == 1) {
                g_gfSceneRoot->remove(m_words[i]->m_scnMdl);
                m_entries[i].m_visible = 0;
            }
        }
    }
    m_frame++;
}

void muWordOfSongTask::initialize() {
    unk2C_b1 = false;
    m_words = NULL;
    m_blackOut = NULL;
    m_param = NULL;
    m_models = NULL;
    m_count = 2;
    m_entries = NULL;
    m_added = 0;
    m_frame = 0;
}

// Frees the files and the models.
void muWordOfSongTask::release() {
    gfHeapManager::free(m_handleScene.getBuffer());
    m_handleScene.release();
    gfHeapManager::free(m_handleFade.getBuffer());
    m_handleFade.release();
    gfHeapManager::free(m_handleWord.getBuffer());
    m_handleWord.release();
    gfHeapManager::free(m_handleParam.getBuffer());
    m_handleParam.release();
    m_resScene.Release();
    m_resFade.Release();
    m_resWord.Release();
    for (int i = 0; i < m_count; i++) {
        delete m_words[i];
        m_words[i] = NULL;
    }
    delete[] m_words;
    m_words = NULL;
    delete m_blackOut;
    m_blackOut = NULL;
    delete m_entries;
    m_entries = NULL;
}

// Starts to read the files: the scene (it depends on the shape of the screen), the fade, the words and their timing.
void muWordOfSongTask::createDataRequest() {
    if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        m_handleScene.readRequest("/menu/defaultcamera/DefaultScene.brres", Heaps::MenuResource, 0, 0);
    } else {
        m_handleScene.readRequest("/menu/defaultcamera/DefaultSceneWide.brres", Heaps::MenuResource, 0, 0);
    }
    m_handleFade.readRequest("/menu/adventure/fade.brres", Heaps::MenuResource, 0, 0);
    m_handleWord.readRequest("/menu/adventure/word_of_song.brres", Heaps::MenuResource, 0, 0);
    m_handleParam.readRequest("/menu/adventure/mu_word_of_song.dat", Heaps::MenuResource, 0, 0);
}

bool muWordOfSongTask::isLoading() {
    bool loading;
    if (!m_handleScene.isReady()) {
        loading = true;
    } else if (!m_handleFade.isReady()) {
        loading = true;
    } else if (!m_handleWord.isReady()) {
        loading = true;
    } else {
        loading = !m_handleParam.isReady();
    }
    return loading;
}

// Makes the models of the words, and the entries with their timing, when the files are read.
void muWordOfSongTask::createData() {
    m_resScene = nw4r::g3d::ResFile(m_handleScene.getBuffer());
    m_resFade = nw4r::g3d::ResFile(m_handleFade.getBuffer());
    m_resWord = nw4r::g3d::ResFile(m_handleWord.getBuffer());
    nw4r::g3d::ResFile::Init(&m_resScene);
    nw4r::g3d::ResFile::Init(&m_resFade);
    nw4r::g3d::ResFile::Init(&m_resWord);
    u8* buffer = static_cast<u8*>(m_handleParam.getBuffer());
    u32 size = m_handleParam.getSize();
    utRelocate relocate(buffer, size);
    m_param = static_cast<const muWordOfSongParam*>(relocate.getPublicAddress("muWordOfSongParam"));
    m_models = static_cast<const muWordModelParam*>(relocate.getPublicAddress("muWordModelParam"));
    m_count = m_param->m_count;
    m_entries = new (Heaps::MenuInstance) muWordOfSongEntry[m_count];
    for (int i = 0; i < m_count; i++) {
        m_entries[i].m_visible = 0;
        m_entries[i].m_start = m_param->m_startFrame;
        m_entries[i].m_start += m_models[i].m_start;
        m_entries[i].m_end = m_param->m_startFrame;
        m_entries[i].m_end += m_models[i].m_start;
        m_entries[i].m_end += m_models[i].m_length;
        m_entries[i].m_fadeIn = m_models[i].m_fadeIn;
        m_entries[i].m_fadeOut = m_models[i].m_fadeOut;
        m_entries[i].m_fadeInStep = 1.0f / m_models[i].m_fadeIn;
        m_entries[i].m_fadeOutStep = -1.0f / m_models[i].m_fadeOut;
        m_entries[i].m_alpha = 0.0f;
    }
    m_words = new (Heaps::MenuInstance) MuObject*[m_count];
    for (int i = 0; i < m_count; i++) {
        m_words[i] = MuObject::create(&m_resWord, m_models[i].m_name, 1, NULL, Heaps::MenuInstance);
    }
    m_blackOut = MuObject::create(&m_resFade, "black_out", 1, NULL, Heaps::MenuInstance);
}

// Puts the animation of the scene (camera and light) in the scene.
void muWordOfSongTask::addModel() {
    if (m_added != 1) {
        m_anmScn = m_resScene.GetResAnmScn(0);
        Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&m_anmScn, m_anmScn);
        fn_8000D6DC(g_gfSceneRoot, &m_anmScn);
        m_added = 1;
    }
}

void muWordOfSongTask::removeModel() {
    if (m_added) {
        fn_8000D7B8(g_gfSceneRoot);
        Release__Q34nw4r3g3d9ResAnmScnFv(&m_anmScn);
        for (int i = 0; i < m_count; i++) {
            g_gfSceneRoot->remove(m_words[i]->m_scnMdl);
        }
        m_added = 0;
    }
}

void muWordOfSongTask::end() {
    m_state = 4;
}

// Stops the song at once.
void muWordOfSongTask::skip() {
    removeModel();
    end();
}

// Sets the alpha of a word (0 - 1) in every material of its model.
void muWordOfSongTask::setAlpha(int index, float alpha) {
    m_entries[index].m_alpha = alpha;
    nw4r::g3d::ResMdl* mdl = &m_words[index]->m_resMdl;
    u32 count = GetResMatNumEntries__Q34nw4r3g3d6ResMdlCFv(mdl);
    float scaled = 255.0f * alpha;
    for (u32 i = 0; i < count; i++) {
        nw4r::g3d::ScnMdl* scnMdl = m_words[index]->m_scnMdl;
        nw4r::g3d::ResMat mat = GetResMat__Q34nw4r3g3d6ResMdlCFi(mdl, i);
        nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
        nw4r::g3d::ResMatChan chan = GetResMatChanEx__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFv(&access);
        GXColor color;
        chan.GXGetChanMatColor(GX_ALPHA0, &color);
        color.a = static_cast<int>(scaled);
        chan.GXSetChanMatColor(GX_ALPHA0, color);
    }
}
