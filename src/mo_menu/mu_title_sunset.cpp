#include <cm/cm_camera_controller.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <types.h>

#include <mu/mu_title_sunset.h>

// gfSceneRoot: main.dol has the functions, the shared header does not.
extern "C" void fn_8000D07C(gfSceneRoot* root, bool unk);
extern "C" void fn_8000D2E0(gfSceneRoot* root, nw4r::g3d::ResAnmScn* anmScn, int unk);
extern "C" void fn_8000E708(gfSceneRoot* root);
// nw4r::g3d::ResAnmScn
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(nw4r::g3d::ResAnmScn* self, nw4r::g3d::ResAnmScn anmScn);
extern "C" void Release__Q34nw4r3g3d9ResAnmScnFv(nw4r::g3d::ResAnmScn* self);
// muMenu: the kinds of the characters of the menu
extern "C" bool isCharUsable__6muMenuFiii(int kind);
extern "C" int exchangeMuCharKindToGmCharacterKind__6muMenuFiii(int kind);
extern "C" int exchangeMuCharKindToMuSelchkind__6muMenuFiii(int kind);
extern "C" int isCharLargeSize__6muMenuFiii(int kind);

// Whether the character of the menu is a big one (it takes the room of two characters in the sunset).
static inline bool isLargeChar(int kind) {
    return isCharLargeSize__6muMenuFiii(exchangeMuCharKindToMuSelchkind__6muMenuFiii(kind)) == 1;
}

// MSL: fprintf and abort, and the files (stderr is the third one)
extern "C" void fn_803F86E4(void* file, const char* format, ...);
extern "C" void fn_803FC984();
extern "C" u8 __files[];

// HYPOTHESIS: std::bitset<51> (the game's copy of the library: two words of bits) with test(), which checks the position and
// prints a message when it is out of range.
class SeenSet {
public:
    u32 m_bits[2];

    bool test(u32 pos) const {
        if (pos >= 51) {
            fn_803F86E4(__files + 0xA0, "index out of range of bitset::test");
            fn_803FC984();
        }
        u32 mask = 1 << (pos & 31);
        return m_bits[pos >> 5] & mask;
    }
};

// The adventure mode data up to the set of the characters that were seen.
struct AdvDataSeen {
    char m_unk0[0x4898];
    SeenSet m_seen;
};

extern "C" void OSReport(const char* format, ...);

// The color that the scene has behind the models when the task is gone.
static const GXColor sBackColor = {0xFF, 0xFF, 0xFF, 0xFF};

muTitleSunsetTask* muTitleSunsetTask::create() {
    return new (Heaps::MenuInstance) muTitleSunsetTask();
}

// The names of the models of the scene: the places of the characters and the characters.
muTitleSunsetTask::muTitleSunsetTask()
    : gfTask("TitleSunset", Category_Menu, 0xF, 8, true), m_handle(), m_resFile(), m_anmScn() {
    memset(m_nagashiName, 0, 0x460);
    sprintf(m_nagashiName[0], "nagashi_posi1");
    sprintf(m_nagashiName[1], "nagashi_posi2");
    sprintf(m_nagashiName[2], "nagashi_posi3");
    sprintf(m_nagashiName[3], "nagashi_posi4");
    sprintf(m_nagashiName[4], "nagashi_posi5");
    sprintf(m_nagashiName[5], "nagashi_posi6");
    sprintf(m_nagashiName[6], "nagashi_posi7");
    sprintf(m_nagashiName[7], "nagashi_posi8");
    sprintf(m_nagashiName[8], "nagashi_posi9");
    sprintf(m_nagashiName[9], "nagashi_posi10");
    sprintf(m_nagashiName[10], "nagashi_posi11");
    sprintf(m_nagashiName[11], "nagashi_posi12");
    sprintf(m_nagashiName[12], "nagashi_posi13");
    sprintf(m_nagashiName[13], "nagashi_posi14");
    sprintf(m_nagashiName[14], "nagashi_posi15");
    sprintf(m_nagashiName[15], "nagashi_posi16");
    sprintf(m_nagashiName[16], "nagashi_posi17");
    sprintf(m_nagashiName[17], "nagashi_posi18");
    sprintf(m_nagashiName[18], "nagashi_posi19");
    sprintf(m_nagashiName[19], "nagashi_posi20");
    sprintf(m_nagashiName[20], "nagashi_posi21");
    sprintf(m_nagashiName[21], "nagashi_posi22");
    sprintf(m_nagashiName[22], "nagashi_posi23");
    sprintf(m_nagashiName[23], "nagashi_posi24");
    sprintf(m_nagashiName[24], "nagashi_posi25");
    sprintf(m_nagashiName[25], "nagashi_posi26");
    sprintf(m_nagashiName[26], "nagashi_posi27");
    sprintf(m_nagashiName[27], "nagashi_posi28");
    sprintf(m_nagashiName[28], "nagashi_posi29");
    sprintf(m_nagashiName[29], "nagashi_posi30");
    sprintf(m_nagashiName[30], "nagashi_posi31");
    sprintf(m_nagashiName[31], "nagashi_posi32");
    sprintf(m_nagashiName[32], "nagashi_posi33");
    sprintf(m_nagashiName[33], "nagashi_posi34");
    sprintf(m_nagashiName[34], "nagashi_posi35");
    memset(m_backName, 0, 0x460);
    sprintf(m_backName[0], "back_posi1");
    sprintf(m_backName[1], "back_posi2");
    sprintf(m_backName[2], "back_posi3");
    sprintf(m_backName[3], "back_posi4");
    sprintf(m_backName[4], "back_posi5");
    sprintf(m_backName[5], "back_posi6");
    sprintf(m_backName[6], "back_posi7");
    sprintf(m_backName[7], "back_posi8");
    sprintf(m_backName[8], "back_posi9");
    sprintf(m_backName[9], "back_posi10");
    sprintf(m_backName[10], "back_posi11");
    sprintf(m_backName[11], "back_posi12");
    sprintf(m_backName[12], "back_posi13");
    sprintf(m_backName[13], "back_posi14");
    sprintf(m_backName[14], "back_posi15");
    sprintf(m_backName[15], "back_posi16");
    sprintf(m_backName[16], "back_posi17");
    sprintf(m_backName[17], "back_posi18");
    sprintf(m_backName[18], "back_posi19");
    sprintf(m_backName[19], "back_posi20");
    sprintf(m_backName[20], "back_posi21");
    sprintf(m_backName[21], "back_posi22");
    sprintf(m_backName[22], "back_posi23");
    sprintf(m_backName[23], "back_posi24");
    sprintf(m_backName[24], "back_posi25");
    sprintf(m_backName[25], "back_posi26");
    sprintf(m_backName[26], "back_posi27");
    sprintf(m_backName[27], "back_posi28");
    sprintf(m_backName[28], "back_posi29");
    sprintf(m_backName[29], "back_posi30");
    sprintf(m_backName[30], "back_posi31");
    sprintf(m_backName[31], "back_posi32");
    sprintf(m_backName[32], "back_posi33");
    sprintf(m_backName[33], "back_posi34");
    sprintf(m_backName[34], "back_posi35");
    memset(m_charNagashiName, 0, 0x460);
    sprintf(m_charNagashiName[0], "bil_mario");
    sprintf(m_charNagashiName[1], "bil_donkey");
    sprintf(m_charNagashiName[2], "bil_link");
    sprintf(m_charNagashiName[3], "bil_samus");
    sprintf(m_charNagashiName[4], "bil_yoshi");
    sprintf(m_charNagashiName[5], "bil_kirby");
    sprintf(m_charNagashiName[6], "bil_fox");
    sprintf(m_charNagashiName[7], "bil_pikachu");
    sprintf(m_charNagashiName[8], "bil_luigi");
    sprintf(m_charNagashiName[9], "bil_captain");
    sprintf(m_charNagashiName[10], "bil_ness");
    sprintf(m_charNagashiName[11], "bil_koopa");
    sprintf(m_charNagashiName[12], "bil_peach");
    sprintf(m_charNagashiName[13], "bil_zelda");
    sprintf(m_charNagashiName[14], "bil_iceclimber");
    sprintf(m_charNagashiName[15], "bil_falco");
    sprintf(m_charNagashiName[16], "bil_ganon");
    sprintf(m_charNagashiName[17], "bil_wario");
    sprintf(m_charNagashiName[18], "bil_metaknight");
    sprintf(m_charNagashiName[19], "bil_pit");
    sprintf(m_charNagashiName[20], "bil_pikmin");
    sprintf(m_charNagashiName[21], "bil_lucas");
    sprintf(m_charNagashiName[22], "bil_diddy");
    sprintf(m_charNagashiName[23], "bil_dedede");
    sprintf(m_charNagashiName[24], "bil_ike");
    sprintf(m_charNagashiName[25], "bil_robot");
    sprintf(m_charNagashiName[26], "bil_snake");
    sprintf(m_charNagashiName[27], "bil_pokemon");
    sprintf(m_charNagashiName[28], "bil_lucario");
    sprintf(m_charNagashiName[29], "bil_marth");
    sprintf(m_charNagashiName[30], "bil_gamewatch");
    sprintf(m_charNagashiName[31], "bil_purin");
    sprintf(m_charNagashiName[32], "bil_toonlink");
    sprintf(m_charNagashiName[33], "bil_wolf");
    sprintf(m_charNagashiName[34], "bil_sonic");
    memset(m_charBackName, 0, 0x460);
    sprintf(m_charBackName[0], "bk_mario");
    sprintf(m_charBackName[1], "bk_donkey");
    sprintf(m_charBackName[2], "bk_link");
    sprintf(m_charBackName[3], "bk_samus");
    sprintf(m_charBackName[4], "bk_yoshi");
    sprintf(m_charBackName[5], "bk_kirby");
    sprintf(m_charBackName[6], "bk_fox");
    sprintf(m_charBackName[7], "bk_pikachu");
    sprintf(m_charBackName[8], "bk_luigi");
    sprintf(m_charBackName[9], "bk_captain");
    sprintf(m_charBackName[10], "bk_ness");
    sprintf(m_charBackName[11], "bk_koopa");
    sprintf(m_charBackName[12], "bk_peach");
    sprintf(m_charBackName[13], "bk_zelda");
    sprintf(m_charBackName[14], "bk_iceclimber");
    sprintf(m_charBackName[15], "bk_falco");
    sprintf(m_charBackName[16], "bk_ganon");
    sprintf(m_charBackName[17], "bk_wario");
    sprintf(m_charBackName[18], "bk_metaknight");
    sprintf(m_charBackName[19], "bk_pit");
    sprintf(m_charBackName[20], "bk_pikmin");
    sprintf(m_charBackName[21], "bk_lucas");
    sprintf(m_charBackName[22], "bk_diddy");
    sprintf(m_charBackName[23], "bk_dedede");
    sprintf(m_charBackName[24], "bk_ike");
    sprintf(m_charBackName[25], "bk_robot");
    sprintf(m_charBackName[26], "bk_snake");
    sprintf(m_charBackName[27], "bk_pokemon");
    sprintf(m_charBackName[28], "bk_lucario");
    sprintf(m_charBackName[29], "bk_marth");
    sprintf(m_charBackName[30], "bk_gamewatch");
    sprintf(m_charBackName[31], "bk_purin");
    sprintf(m_charBackName[32], "bk_toonlink");
    sprintf(m_charBackName[33], "bk_wolf");
    sprintf(m_charBackName[34], "bk_sonic");
}

muTitleSunsetTask::~muTitleSunsetTask() {
    GXColor color = sBackColor;
    // HYPOTHESIS: the scene has four clear colors, and the title screen changed them
    u8* scene = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_gfSceneRoot) + 0x40);
    fn_8000E708(g_gfSceneRoot);
    GXColor* colors = *reinterpret_cast<GXColor**>(scene + 0x2880);
    colors[0] = color;
    colors[1] = color;
    colors[2] = color;
    colors[3] = color;
}

// Whether a character is in the list (of the river, or of the sunset).
static inline bool isInList(const int* list, int count, int kind) {
    for (int i = 0; i < count; i++) {
        if (list[i] == kind) {
            return true;
        }
    }
    return false;
}

void muTitleSunsetTask::processDefault() {
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    switch (m_state) {
    case 0:
        break;
    case State_Start:
        if (!m_skipFirst) {
            m_fade->setFade(CFadeColor(1.0f, 1.0f, 1.0f, 1.0f), CFadeColor(1.0f, 1.0f, 1.0f, 0.0f), 10.0f);
            m_state = State_ShowNagashi;
            m_startFrame = 0.0f;
        } else {
            m_startFrame = 10.0f;
            m_state = State_ShowSunsetB;
        }
        break;
    case State_ShowNagashi:
        removeSunsetModel();
        if (m_nagashiLoaded != 1) {
            g_gfSceneRoot->add(gfSceneRoot::Layer_Ground, m_nagashiScene->m_scnMdl);
            g_gfSceneRoot->add(gfSceneRoot::Layer_Ground, m_nagashi->m_scnMdl);
            g_gfSceneRoot->add(gfSceneRoot::Layer_Ground, m_nagashiPos->m_scnMdl);
            for (int i = 0; i < CharCount; i++) {
                if (isInList(m_charList, m_charCount, i) == true) {
                    g_gfSceneRoot->add(gfSceneRoot::Layer_All, m_charNagashi[i]->m_scnMdl);
                }
            }
            for (int i = 0; i < m_charCount; i++) {
                Vec3f pos;
                m_nagashiPos->getPos(&pos, m_nagashiName[i]);
                m_charNagashi[m_charList[i]]->setTrans(&pos);
            }
            if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
                m_anmScn = m_resFile.GetResAnmScn(3);
            } else {
                m_anmScn = m_resFile.GetResAnmScn(2);
            }
            Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&m_anmScn, m_anmScn);
            fn_8000D2E0(g_gfSceneRoot, &m_anmScn, 0x2B);
            m_nagashiLoaded = 1;
        }
        m_frameA = 0.0f;
        g_gfSceneRoot->setCurrentFrame(m_startFrame);
        m_state = State_PlayNagashi;
        break;
    case State_PlayNagashi:
        m_scrollA = m_scrollA + m_speedA;
        g_gfSceneRoot->setCurrentFrame(0.85f * m_scrollA + m_startFrame);
        m_frameA = m_frameA + 1.0f;
        if (static_cast<float>(m_param->m_lengthA) - m_fadeParams[0].m_time <= m_frameA) {
            setFadeFromParamID(0);
            m_fade->setEnableFade(false);
        }
        if (static_cast<float>(m_param->m_lengthA) <= m_frameA) {
            m_state = State_EndNagashi;
        }
        break;
    case State_EndNagashi:
        m_fade->setEnableFade(true);
        setFadeFromParamID(1);
        m_state = State_ShowSunsetC;
        break;
    case State_ShowSunsetB:
        removeNagashiModel();
        addSunsetModel();
        m_frameB = 0.0f;
        g_gfSceneRoot->setCurrentFrame(0.0f);
        m_state = State_PlayB;
        break;
    case State_PlayB:
        m_scrollB = m_scrollB + m_speedB;
        g_gfSceneRoot->setCurrentFrame(m_scrollB);
        m_frameB = m_frameB + 1.0f;
        if (static_cast<float>(m_param->m_lengthB) - m_fadeParams[2].m_time <= m_frameB) {
            setFadeFromParamID(2);
            m_fade->setEnableFade(false);
        }
        if (static_cast<float>(m_param->m_lengthB) <= m_frameB) {
            m_state = State_EndB;
        }
        break;
    case State_EndB:
        m_fade->setEnableFade(true);
        setFadeFromParamID(3);
        if (!m_skipFirst) {
            m_state = State_ShowSunsetC;
        } else {
            m_state = State_ShowNagashi;
        }
        break;
    case State_ShowSunsetC:
        removeNagashiModel();
        addSunsetModel();
        if (!m_skipFirst) {
            m_speedC = 0.0f;
        }
        m_frameC = 0.0f;
        g_gfSceneRoot->setCurrentFrame(360.0f);
        m_state = State_PlayC;
        break;
    case State_PlayC:
        m_scrollC = m_scrollC + m_speedC;
        g_gfSceneRoot->setCurrentFrame(360.0f + 0.25f * m_scrollC);
        m_frameC = m_frameC + 1.0f;
        if (static_cast<float>(m_param->m_lengthC) - m_fadeParams[4].m_time <= m_frameC) {
            setFadeFromParamID(4);
            m_fade->setEnableFade(false);
        }
        if (static_cast<float>(m_param->m_lengthC) <= m_frameC) {
            m_state = State_EndC;
        }
        break;
    case State_EndC:
        m_fade->setEnableFade(true);
        setFadeFromParamID(5);
        m_state = State_Hold;
        break;
    case State_Hold:
        g_gfSceneRoot->setCurrentFrame(360.0f + m_scrollC);
        break;
    case State_End:
        break;
    }
}

void muTitleSunsetTask::initialize() {
    unk2C_b1 = false;
    m_fade = NULL;
    m_nagashiScene = NULL;
    m_nagashi = NULL;
    m_nagashiPos = NULL;
    for (int i = 0; i < CharCount; i++) {
        m_charNagashi[i] = NULL;
    }
    m_sunsetScene = NULL;
    m_gake = NULL;
    m_x1 = NULL;
    for (int i = 0; i < CharCount; i++) {
        m_charBack[i] = NULL;
    }
    m_param = NULL;
    m_fadeParams = NULL;
    m_charCount = 0;
    memset(m_charList, 0, sizeof(m_charList));
    memset(m_charOrder, 0, sizeof(m_charOrder));
    m_scrollA = 0.0f;
    m_speedA = 1.0f;
    m_frameA = 0.0f;
    m_startFrame = 0.0f;
    m_scrollB = 0.0f;
    m_speedB = 1.0f;
    m_frameB = 0.0f;
    m_scrollC = 0.0f;
    m_speedC = 0.0f;
    m_frameC = 0.0f;
    m_skipFirst = 0;
    m_nagashiLoaded = 0;
    m_sunsetLoaded = 0;
    m_state = 0;
    m_unk1458 = 0;
    fn_8000D07C(g_gfSceneRoot, true);
    CameraController::getInstance()->changeInput(CameraController::AI);
}

// Deletes the models of the scene.
void muTitleSunsetTask::release() {
    fn_8000D07C(g_gfSceneRoot, false);
    m_resFile.Release();
    delete m_nagashiScene;
    m_nagashiScene = NULL;
    delete m_nagashi;
    m_nagashi = NULL;
    delete m_nagashiPos;
    m_nagashiPos = NULL;
    for (int i = 0; i < CharCount; i++) {
        delete m_charNagashi[i];
        m_charNagashi[i] = NULL;
    }
    delete m_sunsetScene;
    m_sunsetScene = NULL;
    delete m_gake;
    m_gake = NULL;
    delete m_x1;
    m_x1 = NULL;
    delete m_backPosi;
    m_backPosi = NULL;
    for (int i = 0; i < CharCount; i++) {
        delete m_charBack[i];
        m_charBack[i] = NULL;
    }
}

// Makes the models of the scene from the file that the title scene read.
void muTitleSunsetTask::createData(nw4r::g3d::ResFile* resFile) {
    m_resFile = *resFile;
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_nagashiScene = MuObject::create(&m_resFile, "StgOpeningSunset_nagashi", 1, NULL, Heaps::MenuInstance);
    m_nagashiScene->changeTexSrtAnimN("StgOpeningSunset");
    m_nagashi = MuObject::create(&m_resFile, "gr_nagashi", 1, NULL, Heaps::MenuInstance);
    m_nagashiPos = MuObject::create(&m_resFile, "nagashi_posi", 1, NULL, Heaps::MenuInstance);
    for (int i = 0; i < CharCount; i++) {
        m_charNagashi[i] = MuObject::create(&m_resFile, m_charNagashiName[i], 1, NULL, Heaps::MenuInstance);
    }
    m_sunsetScene = MuObject::create(&m_resFile, "StgOpeningSunset", 1, NULL, Heaps::MenuInstance);
    m_sunsetScene->changeNodeAnimN("StgOpeningSunset");
    m_sunsetScene->changeTexSrtAnimN("StgOpeningSunset");
    m_gake = MuObject::create(&m_resFile, "gake", 1, NULL, Heaps::MenuInstance);
    m_x1 = MuObject::create(&m_resFile, "x1", 1, NULL, Heaps::MenuInstance);
    m_x1->changeNodeAnimN("x1");
    m_x1->changeClrAnimN("x1");
    m_x1->changeTexSrtAnimN("x1");
    m_backPosi = MuObject::create(&m_resFile, "back_posi", 1, NULL, Heaps::MenuInstance);
    for (int i = 0; i < CharCount; i++) {
        m_charBack[i] = MuObject::create(&m_resFile, m_charBackName[i], 1, NULL, Heaps::MenuInstance);
    }
}

// Picks the characters that are shown, in a random order. The big ones go to the end of the list of the sunset.
void muTitleSunsetTask::createCharaList() {
    createEnableUseCharaList(m_charList);
    for (int i = 0; i < m_charCount; i++) {
        int other = rand() % m_charCount;
        int kind = m_charList[i];
        m_charList[i] = m_charList[other];
        m_charList[other] = kind;
    }
    memcpy(m_charOrder, m_charList, sizeof(m_charOrder));
    for (int i = 0; i < m_charCount; i++) {
        for (int j = i; j < m_charCount; j++) {
            if (isLargeChar(m_charOrder[i]) == true) {
                if (!isLargeChar(m_charOrder[j])) {
                    int kind = m_charOrder[j];
                    m_charOrder[j] = m_charOrder[i];
                    m_charOrder[i] = kind;
                }
            }
        }
    }
}

// Sets how fast the scene moves in each part, from the length of the part.
void muTitleSunsetTask::setSunsetParam(const muTitleSunsetParam* param) {
    m_param = param;
    m_speedA = (960.0f / static_cast<float>(m_param->m_lengthA)) * (static_cast<float>(m_charCount) / 35.0f);
    if (m_param->m_lengthB != 0) {
        m_speedB = 360.0f / static_cast<float>(m_param->m_lengthB);
    }
    if (m_param->m_lengthC != 0) {
        m_speedC = 360.0f / static_cast<float>(m_param->m_lengthC);
    }
}

void muTitleSunsetTask::setFadeParam(const muTitleSunsetFade* fadeParams) {
    m_fadeParams = fadeParams;
}

// Takes both groups of models out of the scene at once and stops on the last frame.
void muTitleSunsetTask::skip() {
    removeNagashiModel();
    removeSunsetModel();
    m_state = State_Hold;
}

void muTitleSunsetTask::removeNagashiModel() {
    if (m_nagashiLoaded) {
        g_gfSceneRoot->removeResAnmScn();
        Release__Q34nw4r3g3d9ResAnmScnFv(&m_anmScn);
        for (int i = 0; i < CharCount; i++) {
            g_gfSceneRoot->remove(m_charNagashi[i]->m_scnMdl);
        }
        g_gfSceneRoot->remove(m_nagashiScene->m_scnMdl);
        g_gfSceneRoot->remove(m_nagashi->m_scnMdl);
        g_gfSceneRoot->remove(m_nagashiPos->m_scnMdl);
        m_nagashiLoaded = 0;
    }
}

// Puts the scene of the sunset and the characters in the scene, and the animation of the scene.
void muTitleSunsetTask::addSunsetModel() {
    if (m_sunsetLoaded != 1) {
        g_gfSceneRoot->add(gfSceneRoot::Layer_Ground, m_sunsetScene->m_scnMdl);
        g_gfSceneRoot->add(gfSceneRoot::Layer_Ground, m_gake->m_scnMdl);
        g_gfSceneRoot->add(gfSceneRoot::Layer_Ground, m_x1->m_scnMdl);
        g_gfSceneRoot->add(gfSceneRoot::Layer_Ground, m_backPosi->m_scnMdl);
        for (int i = 0; i < CharCount; i++) {
            if (isInList(m_charOrder, m_charCount, i) == true) {
                g_gfSceneRoot->add(gfSceneRoot::Layer_All, m_charBack[i]->m_scnMdl);
            }
        }
        setCharaModelPosFromListSunset();
        if (!g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            m_anmScn = m_resFile.GetResAnmScn(1);
        } else {
            m_anmScn = m_resFile.GetResAnmScn(0);
        }
        Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&m_anmScn, m_anmScn);
        fn_8000D2E0(g_gfSceneRoot, &m_anmScn, 0x2B);
        m_sunsetLoaded = 1;
    }
}

void muTitleSunsetTask::removeSunsetModel() {
    if (m_sunsetLoaded) {
        g_gfSceneRoot->removeResAnmScn();
        Release__Q34nw4r3g3d9ResAnmScnFv(&m_anmScn);
        for (int i = 0; i < CharCount; i++) {
            g_gfSceneRoot->remove(m_charBack[i]->m_scnMdl);
        }
        g_gfSceneRoot->remove(m_sunsetScene->m_scnMdl);
        g_gfSceneRoot->remove(m_gake->m_scnMdl);
        g_gfSceneRoot->remove(m_x1->m_scnMdl);
        g_gfSceneRoot->remove(m_backPosi->m_scnMdl);
        m_sunsetLoaded = 0;
    }
}

// Moves the characters of the sunset to the places that "back_posi" has, scaled to the number of the characters.
void muTitleSunsetTask::setCharaModelPosFromListSunset() {
    float scale = static_cast<float>(m_charCount) / 35.0f;
    Vec3f pos;
    Vec3f origin;
    Vec3f factor;
    m_backPosi->getPos(&origin, "back_posi_sc");
    m_backPosi->getAnimScale(&factor, "back_posi_sc");
    scale = 1.0f / scale;
    for (int i = 0; i < m_charCount; i++) {
        int kind = m_charOrder[i];
        m_backPosi->getPos(&pos, m_backName[i]);
        pos.m_x = pos.m_x * factor.m_x + origin.m_x;
        pos.m_y = pos.m_y * factor.m_y + origin.m_y;
        pos.m_z = pos.m_z * factor.m_z * scale + origin.m_z;
        m_charBack[kind]->setTrans(&pos);
    }
}

// Starts the fade of a part of the sunset.
void muTitleSunsetTask::setFadeFromParamID(int id) {
    if (m_fade != NULL) {
        if (m_fadeParams[id].m_enabled != 0) {
            float time = m_fadeParams[id].m_time;
            CFadeColor from;
            from.m_r = static_cast<float>(m_fadeParams[id].m_from[0]) / 255.0f;
            from.m_g = static_cast<float>(m_fadeParams[id].m_from[1]) / 255.0f;
            from.m_b = static_cast<float>(m_fadeParams[id].m_from[2]) / 255.0f;
            from.m_a = static_cast<float>(m_fadeParams[id].m_from[3]) / 255.0f;
            CFadeColor to;
            to.m_r = static_cast<float>(m_fadeParams[id].m_to[0]) / 255.0f;
            to.m_g = static_cast<float>(m_fadeParams[id].m_to[1]) / 255.0f;
            to.m_b = static_cast<float>(m_fadeParams[id].m_to[2]) / 255.0f;
            to.m_a = static_cast<float>(m_fadeParams[id].m_to[3]) / 255.0f;
            m_fade->setFade(from, to, time);
        }
    }
}

// Makes the list of the characters that the scene may show: all the characters that the menu allows, or the ones that
// the adventure mode has unlocked.
void muTitleSunsetTask::createEnableUseCharaList(int* list) {
    m_charCount = 0;
    for (int i = 0; i < CharCount; i++) {
        if (!m_skipFirst) {
            if (isCharUsable__6muMenuFiii(i) == 1) {
                list[m_charCount] = i;
                m_charCount++;
            }
        } else {
            int kind = exchangeMuCharKindToGmCharacterKind__6muMenuFiii(i);
            // HYPOTHESIS: the adventure data has a set of the characters that were seen
            if (reinterpret_cast<AdvDataSeen*>(g_GameGlobal->m_advData)->m_seen.test(kind) == true) {
                list[m_charCount] = i;
                m_charCount++;
            }
        }
    }
    if (false) {
        printEnableUseCharaList();
    }
}

// MATCH-ONLY: the game has this function in the unit, nothing calls it and the linker takes it out (the messages stay in
// the data of the unit).
void muTitleSunsetTask::printEnableUseCharaList() {
    OSReport("===================================\n");
    OSReport("EnbaleUseCharaList\n");
    for (int i = 0; i < m_charCount; i++) {
        OSReport("chara list %2d:%2d\n", i, m_charList[i]);
    }
}
