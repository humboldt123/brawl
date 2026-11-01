#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <mu/mu_mat_color.h>
#include <mu/mu_menu_main.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <revolution/CX/CXCompression.h>
#include <revolution/GX.h>
#include <revolution/OS/OSCache.h>
#include <snd/snd_system.h>
#include <string.h>
#include <types.h>

#include <mu/mu_proc_movie_player.h>

// RVL SDK (the VI header of OpenRVL does not declare it): the dimming of the screen after a while without input.
extern "C" BOOL VIEnableDimming(BOOL enable);
// nw4r::g3d::ResTex::GetTexObjParam (unnamed in the symbols; mu_adv_telop.cpp, st_croll.cpp and so_model_module_impl.cpp
// declare it the same way).
extern "C" bool fn_80192A44(const nw4r::g3d::ResTex* tex, void** data, u16* width, u16* height, GXTexFmt* format,
                            float* minLod, float* maxLod, u8* mipmap);

void movieDraw::renderOpa() {
    m_frame = m_player->drawFrame(0, 0, -1, -1);
}

void muProcMoviePlayer::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    g_gfApplication->unkD0.startKeepScreen();
    while (!group->Empty()) {
        group->PopBack();
    }
    if (m_movieId >= Id_AdvFirst && m_movieId <= Id_AdvLast) {
        m_isAdv = true;
    } else {
        m_isAdv = false;
    }
    initAdv();
    VIEnableDimming(false);
    m_returnCode = from;
    m_firstFrame = true;
}

int muProcMoviePlayer::proc() {
    return procAdv();
}

void muProcMoviePlayer::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}

// Writes the alpha of the caption into the color channel of its material (as muAdvTelopTask::setAlpha does).
static inline void setTelopAlpha(muProcMoviePlayer* proc) {
    nw4r::g3d::ScnMdl* scnMdl = proc->m_telop->m_scnMdl;
    nw4r::g3d::ResMdl resMdl = scnMdl->m_resMdl;
    nw4r::g3d::ResMat mat = resMdl.GetResMat("lambert42");
    nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
    nw4r::g3d::ResMatChan chan = GetResMatChanEx__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFv(&access);
    GXColor color;
    chan.GXGetChanMatColor(GX_ALPHA0, &color);
    color.a = proc->m_telopAlphaU8;
    chan.GXSetChanMatColor(GX_ALPHA0, color);
}

// Makes the model the movie is shown on (scaled for the shape of the screen and the movie) with its camera and, for the
// movies of the Adventure Mode, unpacks the parameters, makes the model of the captions and prepares the music.
void muProcMoviePlayer::setup() {
    nw4r::g3d::ResFile chr(m_handles[0].getBuffer());
    nw4r::g3d::ResFile::Init(&chr);
    m_screen = MuObject::create(&chr, "MenAdvChrCmn1000_TopN", 1, NULL, Heaps::MenuInstance);
    nw4r::g3d::ResNode node = m_screen->getNode("dammy");
    if (m_isAdv) {
        if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            ResNode_SetRotate(&node, 0.677116f, 0.6753247f, 1.0f);
        } else {
            ResNode_SetRotate(&node, 0.9028213f, 0.9004329f, 1.0f);
        }
    } else if (m_movieId == 0) {
        if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            ResNode_SetRotate(&node, 0.9756098f, 0.75f, 1.0f);
        } else {
            ResNode_SetRotate(&node, 0.9756098f, 1.0f, 1.0f);
        }
    } else {
        if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            ResNode_SetRotate(&node, 0.8780488f, 0.6724138f, 1.0f);
        } else {
            ResNode_SetRotate(&node, 0.8780488f, 0.8965517f, 1.0f);
        }
    }

    nw4r::g3d::ResFile camera(m_handles[1].getBuffer());
    nw4r::g3d::ResFile::Init(&camera);
    nw4r::g3d::ResAnmScn anmScn;
    anmScn = camera.GetResAnmScn(0);
    anmScn.Bind();
    g_gfSceneRoot->setInfoCamResAnm(&anmScn);

    if (m_isAdv) {
        void* packed = m_handles[2].getBuffer();
        m_handles[2].getSize();    // HYPOTHESIS: the packed size was read for a check that is gone; the call is in the original
        u32 size = CXGetUncompressedSize(packed);
        m_telopArcData = gfHeapManager::alloc(Heaps::MenuResource, size);
        CXUncompressLZ(packed, m_telopArcData);
        DCFlushRange(m_telopArcData, size);
        m_telopArc = new (Heaps::MenuInstance) gfArchive();
        m_telopArc->setFileImage(m_telopArcData, size, Heaps::MenuResource);
        m_telopData = static_cast<muMovieTelopData*>(m_telopArc->getData(Data_Type_Misc, 10, 0xFFFE));
        if (m_telopData->m_bgmFrame > 100000) {
            m_telopData->m_hasBgm = false;
        }
    }

    if (m_isAdv) {
        nw4r::g3d::ResFile telop;
        telop = nw4r::g3d::ResFile(m_telopArc->getData(Data_Type_Model, 0, 0xFFFE));
        nw4r::g3d::ResFile::Init(&telop);
        m_telop = MuObject::create(&telop, "Telop", 1, NULL, Heaps::MenuInstance);
        if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            Vec3f scale(0.677116f, 0.6753247f, 1.0f);
            m_telop->setObjScale(&scale);
        } else {
            Vec3f scale(0.9028213f, 0.9004329f, 1.0f);
            m_telop->setObjScale(&scale);
        }
        m_telopAlphaU8 = 0;
        setTelopAlpha(this);
    }

    if (m_telopData != NULL && m_telopData->m_hasBgm) {
        g_sndSystem->prepareBGM(m_telopData->m_bgmId, 1);
    }
}

// Starts to read the files: the model of the movie, its camera and, for the movies of the Adventure Mode, the parameters
// of the movie (movie/param/<jp or us>/<movie>.pac).
void muProcMoviePlayer::load() {
    char path[64] = "movie/param/";
    char ext[8] = ".pac";
    m_handles = new (Heaps::MenuInstance) gfFileIOHandle[3];
    m_handles[0].readRequest("menu/adventure/visual.brres", Heaps::MenuResource, 0, 0);
    if (m_isAdv) {
        if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            m_handles[1].readRequest("menu/defaultcamera/AdvMovie.brres", Heaps::MenuResource, 0, 0);
        } else {
            m_handles[1].readRequest("menu/defaultcamera/AdvMovieWide.brres", Heaps::MenuResource, 0, 0);
        }
    } else {
        m_handles[1].readRequest("menu/defaultcamera/AdvMovieWide.brres", Heaps::MenuResource, 0, 0);
    }
    if (m_isAdv) {
        char name[32];
        gmGetMovieFileNameNoPath(name, static_cast<GmMovieID>(m_movieId));
        if (g_GameGlobal->getLanguage() == 0) {
            strcat(path, "jp/");
        } else {
            strcat(path, "us/");
        }
        strcat(path, name);
        strcat(path, ext);
        m_handles[2].readRequest(path, Heaps::MenuResource, 0, 0);
    }
}

// Whether a file is still being read (the parameters are only read for the movies of the Adventure Mode).
inline bool muProcMoviePlayer::isLoading() {
    for (int i = 0; i < 3; i++) {
        if (!m_isAdv && i == 2) {
            break;
        }
        if (m_handles[i].isReady()) {
            m_handles[i].getReturnStatus();
        } else {
            return true;
        }
    }
    return false;
}

void muProcMoviePlayer::initAdv() {
    m_screen = NULL;
    m_telop = NULL;
    m_telopArcData = NULL;
    m_telopArc = NULL;
    m_telopData = NULL;
    m_player = NULL;
    m_handles = NULL;
    m_draw = NULL;
    m_telopNo = 0;
    m_screenShown = false;
    m_bgmStarted = false;
    m_state = State_Load;
    m_telopState = Telop_None;
    m_bgmId = g_sndSystem->getBGMId();
    g_sndSystem->stopBGM(0);
    load();
}

int muProcMoviePlayer::procAdv() {
    bool end = false;
    int frame = -1;
    int ret = 0;
    if (m_firstFrame) {
        ret = 4;
        m_firstFrame = false;
    }
    if (padUpdate() & Pad_End) {
        end = true;
    } else if (m_state == State_Playing) {
        if (m_draw == NULL) {
            m_draw = movieDraw::create(m_player);
        }
        frame = m_draw->m_frame;
        if (frame >= 0 && !m_screenShown) {
            g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_screen->m_sceneModel);
            m_screenShown = true;
            g_gfApplication->unkD0.endKeepScreen();
        }
    } else {
        frame = -1;
    }

    switch (m_state) {
    case State_Load:
        if (!isLoading()) {
            setup();
            m_state = State_Prepare;
        }
        break;
    case State_Prepare:
        if (m_telopData != NULL && m_telopData->m_hasBgm && !g_sndSystem->isPreparedBGM(m_telopData->m_bgmId)) {
            break;
        }
        {
            char path[32];
            gmGetMovieFileName(path, static_cast<GmMovieID>(m_movieId));
            m_player = new (Heaps::MenuInstance) mvMoviePlayer;
            m_player->setScnMdl(m_screen->m_sceneModel, 0);
            m_player->openMovie(path, 0, 0, 0);
            m_state = State_Open;
        }
        break;
    case State_Open:
        if (m_player->getVideoState() == 1) {
            m_totalFrame = m_player->getTotalFrame();
            m_state = State_Play;
        }
        break;
    case State_Play:
        if (m_telopData != NULL) {
            m_player->setVolume(m_telopData->m_movieVolume);
        }
        m_player->play();
        m_state = State_Playing;
        break;
    case State_Playing:
        if (frame >= 0 && frame >= m_totalFrame - 1) {
            m_state = State_End;
        } else if (end) {
            m_state = State_End;
            m_player->setPause();
        }
        telopProc(frame, m_state == State_End);
        if (m_telopData != NULL && m_telopData->m_hasBgm && frame >= m_telopData->m_bgmFrame && !m_bgmStarted) {
            g_sndSystem->setBGMVol(false, 0.01f * m_telopData->m_bgmVolume);
            g_sndSystem->startBGM(m_telopData->m_bgmId, m_telopData->m_bgmStart);
            m_bgmStarted = true;
        }
        break;
    case State_End:
        m_state = State_Release;
        m_telopNo = 0;
        break;
    case State_Release:
        if (m_telopNo == 0) {
            postProcAdv();
        }
        if (++m_telopNo > 3) {
            m_state = State_Exit;
        }
        break;
    }

    if (m_state == State_Exit) {
        m_state = State_Done;
        g_gfApplication->unkD0.endKeepScreen();
        m_result = m_returnCode;
        return 1;
    }
    return ret;
}

// Takes the models out of the scene, frees the movie, the files and the parameters, and puts the menu and its music back.
void muProcMoviePlayer::postProcAdv() {
    if (m_telop != NULL) {
        g_gfSceneRoot->remove(m_telop->m_sceneModel);
    }
    if (m_screen != NULL) {
        g_gfSceneRoot->remove(m_screen->m_sceneModel);
    }
    if (m_draw != NULL) {
        m_draw->exit();
        m_draw = NULL;
    }
    if (m_player != NULL) {
        delete m_player;
        m_player = NULL;
    }
    if (m_telop != NULL) {
        delete m_telop;
        m_telop = NULL;
    }
    if (m_screen != NULL) {
        delete m_screen;
        m_screen = NULL;
    }
    g_gfSceneRoot->removeInfoCamResAnm();
    if (m_handles != NULL) {
        for (int i = 0; i < 3; i++) {
            if (m_isAdv || i != 2) {
                gfHeapManager::free(m_handles[i].getBuffer());
                m_handles[i].release();
            }
        }
        delete[] m_handles;
        m_handles = NULL;
    }
    if (m_telopArc != NULL) {
        delete m_telopArc;
        m_telopArc = NULL;
    }
    if (m_telopArcData != NULL) {
        gfHeapManager::free(m_telopArcData);
        m_telopArcData = NULL;
    }
    dynamic_cast<muMenuMain*>(gfSceneManager::getInstance()->m_currentScene)->addCommonObject();
    VIEnableDimming(true);
    if (m_bgmStarted) {
        g_sndSystem->stopBGM(0);
        g_sndSystem->setBGMVol(false, 1.0f);
    }
    if (m_bgmId >= 0) {
        g_sndSystem->playBGM(m_bgmId, 0, true);
    }
}

// Shows the captions of the movie: each one is put up at its frame, fades in, waits and fades out (at once when the movie
// ends).
void muProcMoviePlayer::telopProc(int frame, bool end) {
    if (m_telopNo < 4 && m_telopData != NULL && frame >= 0) {
        muMovieTelopData::Telop* telop = &m_telopData->m_telops[m_telopNo];
        if (frame >= telop->m_start) {
            if (m_telopState == Telop_None) {
                m_telopEnd = telop->m_start + telop->m_length;
                m_telopFadeIn = telop->m_fadeIn * 2;
                m_telopFadeOut = telop->m_fadeOut * 2;
                if (m_telopFadeIn == 0) {
                    m_telopFadeIn = 1;
                }
                if (m_telopFadeOut == 0) {
                    m_telopFadeOut = 1;
                }
                m_telopState = Telop_Show;
                m_telopTexNo = m_telopNo;
                nw4r::g3d::ResFile file;
                file = nw4r::g3d::ResFile(m_telopArc->getData(Data_Type_Model, 1, 0xFFFE));
                nw4r::g3d::ResFile::Init(&file);
                m_telopTex = file.GetResTex(m_telopTexNo);
                float maxLod;
                float minLod;
                GXTexFmt format;
                void* data;
                u16 height;
                u16 width;
                u8 mipmap;
                fn_80192A44(&m_telopTex, &data, &width, &height, &format, &minLod, &maxLod, &mipmap);
                GXTexObj texObj;
                GXInitTexObj(&texObj, data, width, height, format, GX_CLAMP, GX_CLAMP, mipmap);
                m_telop->changeMaterialTex("lambert42", &texObj);
            }
            m_telopNo++;
        }
    }
    if (end && m_telopState != Telop_None) {
        m_telopState = Telop_Hide;
    }
    switch (m_telopState) {
    case Telop_Show:
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_telop->m_scnMdl);
        m_telopAlpha = 0.0f;
        m_telopStep = 255.0f / m_telopFadeIn;
        m_telopState = Telop_FadeIn;
        // fall through
    case Telop_FadeIn:
        m_telopAlpha += m_telopStep;
        if (m_telopAlpha >= 255.0f) {
            m_telopAlpha = 255.0f;
            m_telopState = Telop_Wait;
        }
        m_telopAlphaU8 = m_telopAlpha;
        setTelopAlpha(this);
        break;
    case Telop_Wait:
        if (frame >= m_telopEnd) {
            m_telopStep = -255.0f / m_telopFadeOut;
            m_telopState = Telop_FadeOut;
        }
        break;
    case Telop_FadeOut:
        m_telopAlpha += m_telopStep;
        if (m_telopAlpha <= 0.0f) {
            m_telopAlpha = 0.0f;
            m_telopState = Telop_Hide;
        }
        m_telopAlphaU8 = m_telopAlpha;
        setTelopAlpha(this);
        break;
    case Telop_Hide:
        g_gfSceneRoot->remove(m_telop->m_scnMdl);
        m_telopState = Telop_None;
        break;
    case Telop_None:
        break;
    }
}
