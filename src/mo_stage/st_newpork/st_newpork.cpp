#include <ai/ai_mgr.h>
#include <cm/cm_subject.h>
#include <ec/ec_mgr.h>
#include <ef/ef_id.h>
#include <ef/ef_screen.h>
#include <gf/gf_archive.h>
#include <gf/gf_model.h>
#include <gm/gm_lib.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_madein.h>
#include <gr/gr_norfair_player_area_check.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <revolution/OS/OSError.h>
#include <snd/snd_id.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <types.h>

#include <st_newpork/st_newpork.h>

// Unnamed helpers of other units that the stage calls directly.
// HYPOTHESIS: grMadein::setMatrix(Matrix*, bool): lets the ground (and optionally its attack) follow the matrix.
extern "C" void fn_27_279FC8(grMadein* ground, Matrix* matrix, bool followAttack);
// grMadein::switchToMatrix
extern "C" void fn_27_279FA4(grMadein* ground);
// grSeqYakumono::createSeSeqWork(int bankCount)
extern "C" void fn_27_27C150(grSeqYakumono* ground, int bankCount);
// grSeqYakumono::registSeSeq(bank, table, entryCount)
extern "C" void fn_27_27C1E8(grSeqYakumono* ground, u32 bank, const void* table, int entryCount);
// grSeqYakumono::playSeSeq(frame, bank)
extern "C" void fn_27_27C208(grSeqYakumono* ground, float frame, int bank);
// grSeqYakumono::playSeSeq1(frame, frame, bank)
extern "C" void fn_27_27C470(grSeqYakumono* ground, float frame, float frameEnd, int bank);
// Matrix::normalize / setRotateX / setRotateY / rotZ
extern "C" void fn_8003E5B4(Matrix* matrix);
extern "C" void fn_8003E87C(Matrix* matrix, float angle);
extern "C" void fn_8003E918(Matrix* matrix, float angle);
extern "C" void fn_8003EBF4(Matrix* matrix, float angle);
// cmSubject::clear() / setPos(Vec3f*)
extern "C" void fn_8009F1FC(cmSubject* subject);
extern "C" void fn_8009EF8C(cmSubject* subject, Vec3f* pos);
// efScreen::cancelFill(frames, handle)
extern "C" void fn_8005E3C8(efScreen* screen, float frames, u8* handle);
// efScreen::requestFlash(frames, int, u8, u32, GXColor*)
extern "C" int requestFlash__8efScreenFfiUcUlP8_GXColor(efScreen* screen, float frames, int a, u8 b, u32 c, GXColor* color);

// The stage's constants: the "no tag" value the coroutine states start with, followed by the tuning values of the three
// hazards (in frames unless noted). HYPOTHESIS: only the use of each value is known.
struct stNewporkStatics {
    int m_noTag;
    float m_param[19];
};
// m_param[0]  plank strength            m_param[1]  plank damage scale       m_param[2]  (unused)
// m_param[5]  plank gone for            m_param[6]  plank blinks for         m_param[7]  limousine first wait
// m_param[8]/[9] limousine wait range   m_param[10] limousine speed          m_param[12] Chimera first wait
// m_param[13]/[14] Chimera wait range   m_param[15] Chimera chance to show   m_param[17] Chimera stays
// m_param[18] Chimera wait after it left
static stNewporkStatics sStatics = {
    0,
    {6.0f, 0.1f, 2.0f, 60.0f, 0.5f, 1200.0f, 120.0f, 900.0f, 300.0f, 1200.0f, 0.7f, 0.35f, 1200.0f, 1800.0f, 2700.0f,
     0.5f, 0.3f, 900.0f, 3600.0f},
};

inline stNewporkSeq::stNewporkSeq() : m_tag(sStatics.m_noTag), m_line(0) { }

// HYPOTHESIS: the stage's three coroutines are hand-written state machines in the binary: every resume point saves a
// marker and the number of its source line in the sequence, and the dispatch is a switch over those numbers. The
// numbers below are the original line numbers (they only have to be unique), kept so that the cases can be compared
// with the binary. Each resume point has a static marker of its own whose value is always 1.
#define NEWPORK_SAVE(seq, lineNumber)                       \
    do {                                                    \
        static int sTag = 1;                                \
        (seq) = stNewporkSeq(sTag, (lineNumber));           \
    } while (0)

// HYPOTHESIS: the 3 bits at the top of the word at cmSubject+8 are a mode that the stage sets to 1 while the subject
// follows the Chimera and clears (together with the next 3 bits) when the Chimera shows up.
struct newporkSubjectBits {
    u32 mode : 3;
    u32 rest : 29;
};
static inline void newporkEnableSubject(cmSubject* subject) {
    reinterpret_cast<newporkSubjectBits*>(reinterpret_cast<u8*>(subject) + 8)->mode = 1;
}
static inline void newporkDisableSubject(cmSubject* subject) {
    u32* word = reinterpret_cast<u32*>(reinterpret_cast<u8*>(subject) + 8);
    *word = *word & 0x03FFFFFF;
}

// MATCH-ONLY: passing the offset by value reproduces the original's temporary copies (same idiom as grGreenhillBreak).
static inline void newporkAssignVec2f(Vec2f& dst, Vec2f src) {
    dst = src;
}

// Registers sound effect number `slot` of a ground: played at once, at the ground's origin, without a repeat.
#define NEWPORK_SOUND(slot, soundId)                                      \
    m_soundEffects[slot].m_id = (soundId);                                \
    m_soundEffects[slot].m_repeatFrame = 0;                               \
    m_soundEffects[slot].m_nodeIndex = 0;                                 \
    m_soundEffects[slot].m_endFrame = 0;                                  \
    newporkAssignVec2f(m_soundEffects[slot].m_offsetPos, Vec2f(0.0f, 0.0f))

// MATCH-ONLY: the original builds this color in a temporary and copies it.
static inline GXColor newporkColor(u8 r, u8 g, u8 b, u8 a) {
    GXColor color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

// True when a fighter stands in the area.
static inline bool newporkIsAnyPlayerIn(grPlayerAreaCheck* area) {
    bool found = false;
    for (int i = 0; i < 4; i++) {
        found = found | area->m_playerChecks[i];
    }
    return found;
}

stClassInfoImpl<Stages::NewPork, stNewpork> stNewpork::bss_loc_14;

stNewpork* stNewpork::create() {
    return new (Heaps::StageInstance) stNewpork;
}

stNewpork::stNewpork() : stMelee("stNewpork", Stages::NewPork), m_chimeraMtx(true), m_screenFill(0xFF) {
    m_chikei = NULL;
    m_chikeiBG = NULL;
    m_viking = NULL;
    m_kowareIta = NULL;
    m_limousine = NULL;
    m_limousineBG = NULL;
    m_limousineActive = NULL;
    m_limousineMove = NULL;
    m_chimera = NULL;
    m_chimeraAttack = NULL;
    m_chimeraArea = NULL;
    m_chimeraArea2 = NULL;
    m_param = NULL;
    m_nodeVikingMove = 0;
    m_nodeKowareItaPos = 0;
    m_nodeKowareItaModel = 0;
    m_nodeLimousineAttach = 0;
    m_nodeChimeraCtrl = 0;
    m_nodeChimeraHead = 0;
    m_plankTimer = 0.0f;
    m_plankHitCooldown = 0.0f;
    m_plankBrokenTime = 0.0f;
    m_plankBroken = false;
    m_limousineTimer = 0.0f;
    m_limousineLast = 0;
    m_limousineLast2 = 0;
    m_chimeraReady = false;
    m_chimeraMotion = 0;
    m_chimeraFrame = 0.0f;
    m_chimeraStay = 0.0f;
    m_chimeraWait = 0.0f;
    m_chimeraMtx.setIdentity();
    m_chimeraHit = false;
    m_chimeraPos.m_x = 0.0f;
    m_chimeraPos.m_y = 0.0f;
    m_chimeraPos.m_z = 0.0f;
    m_chimeraHitStop = 0.0f;
    m_chimeraSpeed.m_x = 0.0f;
    m_chimeraSpeed.m_y = 0.0f;
    m_chimeraSpeed.m_z = 0.0f;
    m_areaCooldown = 0.0f;
    m_areaCooldown2 = 0.0f;
    m_plankJoint = NULL;
    fn_8009F1FC(&m_subject);
}

stNewpork::~stNewpork() {
    releaseArchive();
    fn_8009EE60(&m_subject, -1);
}

bool stNewpork::loading() {
    return true;
}

void stNewpork::createObj() {
    testStageParamInit(m_fileData, 10);
    m_param = sStatics.m_param;

    m_chikei = grNewpork::create(grNewpork::Part_Chikei, "", "grNewporkChikei");
    if (m_chikei != NULL) {
        addGround(m_chikei);
        m_chikei->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_chikei->setStageData(m_stageData);
        m_chikei->startEntityAutoLoop();
        m_nodeVikingMove = m_chikei->getNodeIndex(0, "Pos_VIKING_moveParts");
        m_nodeKowareItaPos = m_chikei->getNodeIndex(0, "Pos_kowareIta");
    }

    m_chikeiBG = grNewpork::create(grNewpork::Part_ChikeiBG, "", "grNewporkChikeiBG");
    if (m_chikeiBG != NULL) {
        addGround(m_chikeiBG);
        m_chikeiBG->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_chikeiBG->setStageData(m_stageData);
        m_chikeiBG->startEntityAutoLoop();
    }

    m_viking = grNewpork::create(grNewpork::Part_Viking, "", "grNewporkViking");
    if (m_viking != NULL) {
        addGround(m_viking);
        m_viking->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_viking->setStageData(m_stageData);
        m_viking->startEntityAutoLoop();
        fn_27_279FA4(m_viking);
    }

    m_kowareIta = grNewpork::create(grNewpork::Part_KowareIta, "", "grNewporkKowareIta");
    if (m_kowareIta != NULL) {
        addGround(m_kowareIta);
        m_kowareIta->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_kowareIta->setStageData(m_stageData);
        m_nodeKowareItaModel = m_chikei->getNodeIndex(0, "StgNewpork00_kowareIta");
        m_kowareIta->thisIsKowareita();
        Vec3f start(-20.67f, 0.0f, 0.0f);
        Vec3f end(20.67f, 0.0f, 0.0f);
        m_kowareIta->setHitPoint(2.0f, &start, &end, true, 0);
        m_kowareIta->setHitCategory(grMadein::Hit_Category_Wall);
        m_kowareIta->initializeEntity();
        m_kowareIta->startEntity();
        m_kowareIta->setMotionRatio(0.0f);
        fn_27_279FA4(m_kowareIta);
    }

    m_limousine = grNewpork::create(grNewpork::Part_Limousine, "", "grNewporkLimousine");
    if (m_limousine != NULL) {
        addGround(m_limousine);
        m_limousine->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_limousine->setStageData(m_stageData);
        m_limousine->thisIsLimousine();
        m_limousine->initializeEntity();
        m_limousine->setEnableCollisionStatus(false);
        fn_27_279FA4(m_limousine);
    }

    m_limousineBG = grNewpork::create(grNewpork::Part_LimousineBG, "", "grNewporkLimousineBG");
    if (m_limousineBG != NULL) {
        addGround(m_limousineBG);
        m_limousineBG->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_limousineBG->setStageData(m_stageData);
        m_limousineBG->thisIsLimousine();
        m_limousineBG->initializeEntity();
        m_limousineBG->setEnableCollisionStatus(false);
        fn_27_279FA4(m_limousineBG);
    }

    m_limousineMove = grNewpork::create(grNewpork::Part_LimousineMove, "", "grNewporkLimousineMove");
    if (m_limousineMove != NULL) {
        addGround(m_limousineMove);
        m_limousineMove->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_limousineMove->setStageData(m_stageData);
        m_limousineMove->initializeEntity();
        m_nodeLimousineAttach = m_limousineMove->getNodeIndex(0, "Limousine_attach");
    }

    m_chimera = grNewpork::create(grNewpork::Part_UltimateChimera, "", "grNewporkUltimateChimeraModel");
    if (m_chimera != NULL) {
        addGround(m_chimera);
        m_chimera->startup(m_fileData, 0, gfSceneRoot::Layer_Ground_And_Chara);
        m_chimera->setStageData(m_stageData);
        m_nodeChimeraCtrl = m_chimera->getNodeIndex(0, "StgNewporkUltimateChimera_TR_Ctrl");
        m_nodeChimeraHead = m_chimera->getNodeIndex(0, "StgNewporkUltimateChimera_HeadN");
        Vec3f headOffset(0.0f, 2.0f, 13.0f);
        m_chimera->thisIsUltimateChimera(8.5f, &headOffset, m_nodeChimeraHead, true);
        m_chimera->initializeEntity();
        fn_27_279FA4(m_chimera);
        Matrix identity;
        fn_27_279FC8(m_chimera, &identity, true);

        m_chimeraAttack = grNewpork::create(grNewpork::Part_UltimateChimeraArea, "", "");
        if (m_chimeraAttack != NULL) {
            addGround(m_chimeraAttack);
            m_chimeraAttack->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            m_chimeraAttack->setStageData(m_stageData);
            Vec3f bodyOffset(2.5f, 8.5f, 0.0f);
            m_chimeraAttack->thisIsUltimateChimera(4.75f, &bodyOffset, 0, false);
            m_chimeraAttack->initializeEntity();
            fn_27_279FA4(m_chimeraAttack);
            fn_27_279FC8(m_chimeraAttack, &identity, true);
        }
    }

    m_chimeraArea = grPlayerAreaCheck::create(4, "grNewporkUltimateChimeraArea");
    if (m_chimeraArea != NULL) {
        addGround(m_chimeraArea);
        m_chimeraArea->setArea(0, 0, 20, 22);
        m_chimeraArea->setGimmickData(&m_chimeraArea->_playerAreaCheckData);
        m_chimeraArea->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_chimeraArea->setStageData(m_stageData);
    }

    m_chimeraArea2 = grPlayerAreaCheck::create(4, "grNewporkUltimateChimeraArea2");
    if (m_chimeraArea2 != NULL) {
        addGround(m_chimeraArea2);
        m_chimeraArea2->setArea(0, 0, 20, 24);
        m_chimeraArea2->setGimmickData(&m_chimeraArea2->_playerAreaCheckData);
        m_chimeraArea2->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_chimeraArea2->setStageData(m_stageData);
    }

    createCollision(m_fileData, 2, NULL);

    // Find the collision joint of the plank so that its ledge can be switched off while it is there.
    if (m_chikei != NULL) {
        u32 plankNode;
        m_chikei->getNodeIndex(&plankNode, 0, "Pos_kowareIta");
        grCollision* collision = m_chikei->m_collision;
        if (collision != NULL) {
            u32 jointNum = collision->m_jointLen;
            for (u32 i = 0; i != jointNum; i++) {
                grCollisionJoint* joint = collision->getJoint(i);
                if (joint != NULL && joint->m_ground == m_chikei && joint->m_nodeIndex == plankNode) {
                    m_plankJoint = joint;
                    break;
                }
            }
        }
    }

    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 30);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 101, "PokeTrainer00", m_pokeTrainerPos, NULL);

    m_seqPlank.m_tag = sStatics.m_noTag;
    m_seqPlank.m_line = 0;
    m_seqLimousine.m_tag = sStatics.m_noTag;
    m_seqLimousine.m_line = 0;
    m_seqChimera.m_tag = sStatics.m_noTag;
    m_seqChimera.m_line = 0;
}

void stNewpork::update(float deltaFrame) {
    g_aiMgr->delDangerZone(0);

    if (m_chikei != NULL && m_viking != NULL) {
        Matrix vikingMtx(true);
        m_chikei->getNodeMatrix(&vikingMtx, 0, m_nodeVikingMove);
        fn_27_279FC8(m_viking, &vikingMtx, true);
    }

    // The breaking plank of the Viking boat. The plank starts with m_param[0] of strength; every hit takes some of it
    // away (the harder the hit the more) and when it is used up the plank breaks and disappears for a while.
    if (m_chikei != NULL && m_kowareIta != NULL) {
        Matrix plankMtx(true);
        m_chikei->getNodeMatrix(&plankMtx, 0, m_nodeKowareItaPos);
        fn_27_279FC8(m_kowareIta, &plankMtx, true);
        switch (m_seqPlank.m_line) {
            case 0:
                NEWPORK_SAVE(m_seqPlank, 0x1d5);
                // FALL-THROUGH
            case 0x1d5:
            case 0x249:
                m_plankTimer = m_param[0];
                m_kowareIta->startEntity();
                m_kowareIta->setMotionRatio(0.0f);
                m_kowareIta->setEnableCollisionStatus(true);
                m_kowareIta->enableHit(0, 0);
                m_plankBroken = false;
                m_plankHitCooldown = 0.0f;
                if (m_plankJoint != NULL) {
                    m_plankJoint->m_0x52 = 0x4000;
                }
            plankHold:
                NEWPORK_SAVE(m_seqPlank, 0x1e5);
                goto plankEnd;
            case 0x1e5: {
                m_plankHitCooldown -= deltaFrame;
                if (m_plankHitCooldown <= 0.0f) {
                    m_plankHitCooldown = 0.0f;
                }
                if (!m_kowareIta->wasHit()) {
                    goto plankHold;
                }
                m_plankTimer -= m_param[1] * m_kowareIta->getLastDamage();
                m_kowareIta->clearHit();
                Matrix modelMtx(true);
                m_kowareIta->getNodeMatrix(&modelMtx, 0, m_nodeKowareItaModel);
                Vec3f hitPos;
                modelMtx.getPosition(&hitPos);
                if (m_plankTimer <= 0.0f) {
                    m_kowareIta->seBreakKowareita();
                    g_ecMgr->setEffect(ef_ptc_stg_newpork_kowareyuka_hakai, &hitPos);
                    if (m_plankJoint != NULL) {
                        m_plankJoint->m_0x52 = 0;
                    }
                } else {
                    m_kowareIta->seHitKowareita();
                    if (m_plankHitCooldown <= 0.0f) {
                        m_kowareIta->seSandKowareita();
                        g_ecMgr->setEffect(ef_ptc_stg_newpork_kowareyuka_damage, &hitPos);
                        m_plankHitCooldown = 80.0f + 40.0f * randf();
                    }
                    goto plankHold;
                }
                m_kowareIta->setEnableCollisionStatus(false);
                m_kowareIta->disableHit(0, 0);
                m_kowareIta->setMotionRatio(1.0f);
                m_plankBroken = true;
                m_plankTimer = 220.0f;
                m_plankBrokenTime = 0.0f;
                goto plankBrokenWait;
            }
            plankBrokenYield:
                NEWPORK_SAVE(m_seqPlank, 0x22b);
                goto plankEnd;
            case 0x22b:
                m_plankTimer -= deltaFrame;
                m_plankBrokenTime += deltaFrame;
            plankBrokenWait:
                if (0.0f <= m_plankTimer) {
                    goto plankBrokenYield;
                }
                m_kowareIta->endEntity();
                NEWPORK_SAVE(m_seqPlank, 0x230);
                goto plankEnd;
            case 0x230:
                m_plankTimer = m_param[5] - 220.0f;
                goto plankGoneWait;
            plankGoneResume:
                NEWPORK_SAVE(m_seqPlank, 0x236);
                goto plankEnd;
            case 0x236:
                m_plankTimer -= deltaFrame;
            plankGoneWait:
                if (0.0f <= m_plankTimer) {
                    goto plankGoneResume;
                }
                m_plankTimer = m_param[6];
                goto plankBlinkWait;
            plankBlink:
                m_kowareIta->startEntity();
                m_kowareIta->setMotionRatio(0.0f);
                NEWPORK_SAVE(m_seqPlank, 0x23f);
                goto plankEnd;
            case 0x23f:
                m_plankTimer -= deltaFrame;
                NEWPORK_SAVE(m_seqPlank, 0x241);
                goto plankEnd;
            case 0x241:
                m_plankTimer -= deltaFrame;
                m_kowareIta->endEntity();
                NEWPORK_SAVE(m_seqPlank, 0x244);
                goto plankEnd;
            case 0x244:
                m_plankTimer -= deltaFrame;
            plankBlinkWait:
                if (0.0f <= m_plankTimer) {
                    goto plankBlink;
                }
                NEWPORK_SAVE(m_seqPlank, 0x249);
                break;
        }
    }
plankEnd:

    // The limousine drives through every now and then: a random one of five paths that differs from the last two.
    if (m_limousine != NULL && m_limousineBG != NULL && m_limousineMove != NULL) {
        switch (m_seqLimousine.m_line) {
            case 0:
                NEWPORK_SAVE(m_seqLimousine, 0x252);
                // FALL-THROUGH
            case 0x252:
                m_limousineTimer = m_param[7];
                m_limousineLast = -1;
                m_limousineLast2 = -1;
                goto limousinePick;
            limousineWait:
                m_limousineTimer -= deltaFrame;
                NEWPORK_SAVE(m_seqLimousine, 0x25c);
                goto limousineEnd;
            case 0x25c:
            limousinePick: {
                if (0.0f < m_limousineTimer) {
                    goto limousineWait;
                }
                int candidates[5];
                int count = 0;
                int* next = candidates;
                int last = m_limousineLast;
                for (int i = 0; i < 5; i++) {
                    if (last != i && m_limousineLast2 != i) {
                        *next++ = i;
                        count++;
                    }
                }
                u32 rnd = randi(count);
                if (rnd >= (u32)(count - 1)) {
                    rnd = count - 1;
                }
                int motion = candidates[rnd];
                m_limousineMove->setMotion((u8)motion);
                m_limousineMove->startEntity();
                m_limousineMove->setMotionRatio(m_param[10]);
                m_limousineLast2 = m_limousineLast;
                m_limousineLast = motion;
                NEWPORK_SAVE(m_seqLimousine, 0x279);
                goto limousineEnd;
            }
            case 0x279:
                NEWPORK_SAVE(m_seqLimousine, 0x27a);
                goto limousineEnd;
            case 0x27a:
                if (m_limousineLast == 0) {
                    m_limousineActive = m_limousine;
                    m_limousineActive->startEntityAutoLoop();
                    m_limousineActive->seMoveLimousine();
                    m_limousineActive->setEnableCollisionStatus(true);
                } else {
                    m_limousineActive = m_limousineBG;
                    m_limousineActive->startEntityAutoLoop();
                    m_limousineActive->seMoveLimousine();
                    m_limousineActive->setEnableCollisionStatus(false);
                }
                goto limousineMoving;
            limousineFollow: {
                Matrix attachMtx(true);
                m_limousineMove->getNodeMatrix(&attachMtx, 0, m_nodeLimousineAttach);
                fn_27_279FC8(m_limousineActive, &attachMtx, true);
                NEWPORK_SAVE(m_seqLimousine, 0x298);
                goto limousineEnd;
            }
            case 0x298:
            limousineMoving:
                if (!m_limousineMove->isEndEntity()) {
                    goto limousineFollow;
                }
                m_limousineActive->endEntity();
                m_limousineActive->setEnableCollisionStatus(false);
                m_limousineActive->seMoveLimousineStop();
                m_limousineActive = NULL;
                {
                    float range = m_param[9] - m_param[8];
                    m_limousineTimer = m_param[8] + range * randf();
                }
                goto limousinePick;
        }
    }
limousineEnd:

    // The Ultimate Chimera shows up on the street after a while, walks around for a time (an attack follows when a
    // fighter stands in front of it or behind it) and leaves again. It leaves in a hurry when the plank broke and
    // the Chimera would walk through the hole.
    if (m_chimera != NULL) {
        bool following = false;
        if (m_chimeraReady) {
            m_chimera->getNodeMatrix(&m_chimeraMtx, 0, m_nodeChimeraCtrl);
            m_chimeraPos.m_y = m_chimeraMtx(1, 3);
            m_chimeraPos.m_x = m_chimeraMtx(0, 3);
            m_chimeraPos.m_z = m_chimeraMtx(2, 3);
            Matrix headMtx(true);
            m_chimera->getNodeMatrix(&headMtx, 0, m_nodeChimeraHead);
            fn_8003E5B4(&headMtx);
            Vec3f frontPos(0.0f, 3.0f, 12.0f);
            headMtx.mulPos(&frontPos, &frontPos);
            m_chimeraArea->setPos(&frontPos);
            Vec3f backPos(0.0f, 3.0f, -8.0f);
            headMtx.mulPos(&backPos, &backPos);
            m_chimeraArea2->setPos(&backPos);
            fn_8009EF8C(&m_subject, &m_chimeraPos);
            following = true;
        }
        if (m_chimera->m_isVisible) {
            Vec2f zoneMin(m_chimeraPos.m_x, m_chimeraPos.m_y);
            zoneMin -= Vec2f(55.0f, 20.0f);
            Vec2f zoneMax = Vec2f(m_chimeraPos.m_x, m_chimeraPos.m_y) + Vec2f(55.0f, 40.0f);
            g_aiMgr->setDangerZone(&zoneMin, &zoneMax, -1, false, false);
        }
        switch (m_seqChimera.m_line) {
            case 0:
                NEWPORK_SAVE(m_seqChimera, 0x2cb);
                // FALL-THROUGH
            case 0x2cb:
                m_chimeraMotion = 0;
                m_areaCooldown = 0.0f;
                m_areaCooldown2 = 0.0f;
                newporkEnableSubject(&m_subject);
                m_chimeraWait = m_param[12];
                goto chimeraWaitReport;
            chimeraFixedWait: {
                float fixedWait = m_param[18];
                m_chimeraWait = fixedWait;
                OSReport("UltimateChimeraWaitTime: fixed %d\n", (int)fixedWait);
                goto chimeraWaitCheck;
            }
            chimeraWaitTick:
                m_chimeraWait -= deltaFrame;
                NEWPORK_SAVE(m_seqChimera, 0x2f3);
                goto chimeraEnd;
            case 0x2f3:
            chimeraWaitCheck:
                if (0.0f < m_chimeraWait) {
                    goto chimeraWaitTick;
                }
            chimeraRandomWait: {
                float range = m_param[14] - m_param[13];
                m_chimeraWait = m_param[13] + range * randf();
            }
            chimeraWaitReport:
                OSReport("UltimateChimeraWaitTime: random %d\n", (int)m_chimeraWait);
                newporkEnableSubject(&m_subject);
                goto chimeraChance;
            chimeraChanceTick:
                m_chimeraWait -= deltaFrame;
                NEWPORK_SAVE(m_seqChimera, 0x302);
                goto chimeraEnd;
            case 0x302:
            chimeraChance:
                if (0.0f < m_chimeraWait) {
                    goto chimeraChanceTick;
                }
                if (m_param[15] >= 1.0f * randf()) {
                    OSReport("UltimateChimeraWaitTime: Put\n");
                    goto chimeraAppear;
                }
                OSReport("UltimateChimeraWaitTime: Wait Again\n");
                goto chimeraRandomWait;
            chimeraAppear: {
                u32 motion = randi(3);
                if (motion >= 2) {
                    motion = 2;
                }
                m_chimeraMotion = motion;
                Matrix idMtx;
                fn_27_279FC8(m_chimera, &idMtx, true);
                fn_27_279FC8(m_chimeraAttack, &idMtx, true);
                m_chimera->setMotion((u8)m_chimeraMotion);
                m_chimera->startEntityAutoLoop();
                m_chimera->setSleepAttack(true);
                m_chimeraAttack->startEntityAutoLoop();
                m_chimeraAttack->setSleepAttack(true);
                gfModelAnimation* anim = m_chimera->m_modelAnims[0];
                if (anim != NULL) {
                    float lastFrame = (float)anim->getFrameCount() - 1.0f;
                    anim->setFrame((float)floor(lastFrame * randf()));
                }
                m_chimera->setVisibility(0);
                m_chimeraReady = true;
                NEWPORK_SAVE(m_seqChimera, 0x332);
                goto chimeraEnd;
            }
            case 0x332:
                NEWPORK_SAVE(m_seqChimera, 0x333);
                goto chimeraEnd;
            case 0x333: {
                m_chimera->seEntryUltimateChimera();
                g_ecMgr->setEffect(ef_ptc_stg_newpork_syutugen, &m_chimeraPos);
                gfModelAnimation* anim = m_chimera->m_modelAnims[0];
                if (anim != NULL) {
                    m_chimeraFrame = anim->getFrame();
                }
                m_chimera->setMotion(5);
                m_chimera->startEntity();
                m_chimera->setSleepAttack(true);
                fn_27_279FC8(m_chimera, &m_chimeraMtx, true);
                m_chimeraAttack->startEntity();
                m_chimeraAttack->setSleepAttack(true);
                fn_27_279FC8(m_chimeraAttack, &m_chimeraMtx, true);
                GXColor fill = newporkColor(0, 0, 0, 0x70);
                m_chimeraReady = false;
                newporkDisableSubject(&m_subject);
                m_screenFill = (u32)g_efScreen->requestFill(60.0f, 0, 0xFF, &fill) >> 24;
                NEWPORK_SAVE(m_seqChimera, 0x353);
                goto chimeraEnd;
            }
            case 0x353:
                NEWPORK_SAVE(m_seqChimera, 0x354);
                goto chimeraEnd;
            case 0x354:
            case 0x35b: {
                gfModelAnimation* waitAnim = m_chimera->m_modelAnims[0];
                if (waitAnim != NULL && !(waitAnim->getFrame() >= (float)waitAnim->getFrameCount())) {
                    NEWPORK_SAVE(m_seqChimera, 0x35b);
                    goto chimeraEnd;
                }
                {
                    m_chimera->setMotion((u8)m_chimeraMotion);
                    m_chimera->startEntityAutoLoop();
                    m_chimera->setSleepAttack(true);
                    m_chimeraAttack->startEntityAutoLoop();
                    m_chimeraAttack->setSleepAttack(true);
                    gfModelAnimation* anim = m_chimera->m_modelAnims[0];
                    if (anim != NULL) {
                        anim->setFrame(m_chimeraFrame);
                    }
                    fn_27_27C470(m_chimera, m_chimeraFrame, m_chimeraFrame, m_chimeraMotion);
                    Matrix idMtx;
                    fn_27_279FC8(m_chimera, &idMtx, true);
                    fn_27_279FC8(m_chimeraAttack, &idMtx, true);
                    m_chimeraReady = true;
                    m_chimeraStay = m_param[17];
                }
                goto chimeraStayCheck;
            }
            chimeraTick:
                if (isChimeraBlockedByPlank()) {
                    goto chimeraLeave;
                }
                if (m_chimeraReady) {
                    gfModelAnimation* anim = m_chimera->m_modelAnims[0];
                    if (anim != NULL) {
                        fn_27_27C208(m_chimera, anim->getFrame(), m_chimeraMotion);
                        if (m_chimeraMotion == 1 && m_plankBroken) {
                            float frame = anim->getFrame();
                            m_chimeraFrame = frame;
                            if (320.0f == (float)floor(frame)) {
                                m_chimeraMotion = 3;
                                m_chimera->setMotion(3);
                                m_chimera->startEntity();
                                m_chimera->setSleepAttack(true);
                                m_chimeraAttack->startEntity();
                                m_chimeraAttack->setSleepAttack(true);
                                float newFrame = m_chimeraFrame - 320.0f;
                                anim->setFrame(newFrame);
                                fn_27_27C470(m_chimera, newFrame, newFrame, m_chimeraMotion);
                            }
                        } else if (m_chimeraMotion == 3) {
                            float over = anim->getFrame() - (float)anim->getFrameCount();
                            if (0.0f <= over) {
                                m_chimeraMotion = 1;
                                m_chimera->setMotion(1);
                                m_chimera->startEntityAutoLoop();
                                m_chimera->setSleepAttack(true);
                                m_chimeraAttack->startEntityAutoLoop();
                                m_chimeraAttack->setSleepAttack(true);
                                float newFrame = 630.0f + over;
                                anim->setFrame(newFrame);
                                fn_27_27C470(m_chimera, newFrame, newFrame, m_chimeraMotion);
                            }
                        }
                    }
                }
                if (following && m_chimeraStay <= m_param[17] - 120.0f) {
                    bool inFront = false;
                    bool inBack = false;
                    if (m_areaCooldown <= 0.0f) {
                        inFront = newporkIsAnyPlayerIn(m_chimeraArea);
                    } else {
                        m_areaCooldown -= deltaFrame;
                    }
                    if (!inFront) {
                        if (m_areaCooldown2 <= 0.0f) {
                            inFront = newporkIsAnyPlayerIn(m_chimeraArea2);
                            inBack = inFront;
                        } else {
                            m_areaCooldown2 -= deltaFrame;
                        }
                    }
                    if (inFront) {
                        if (inBack) {
                            m_areaCooldown = 0.0f;
                            m_areaCooldown2 = 60.0f;
                            Matrix turn;
                            fn_8003E918(&turn, 3.1415927f);
                            m_chimeraMtx.mul(&turn, &m_chimeraMtx);
                        } else {
                            m_areaCooldown = 60.0f;
                            m_areaCooldown2 = 0.0f;
                        }
                        gfModelAnimation* anim = m_chimera->m_modelAnims[0];
                        if (anim != NULL) {
                            m_chimeraFrame = anim->getFrame();
                        }
                        m_chimera->setMotion(4);
                        m_chimera->startEntity();
                        fn_27_279FC8(m_chimera, &m_chimeraMtx, true);
                        m_chimera->setSleepAttack(true);
                        m_chimeraReady = false;
                        m_chimera->seAttackUltimateChimera();
                        m_chimeraAttack->startEntity();
                        m_chimeraAttack->setSleepAttack(true);
                        fn_27_279FC8(m_chimeraAttack, &m_chimeraMtx, true);
                        NEWPORK_SAVE(m_seqChimera, 0x3fc);
                        goto chimeraEnd;
                    }
                }
                goto chimeraStayTick;
            case 0x3fc:
                if (isChimeraBlockedByPlank()) {
                    goto chimeraLeave;
                }
                NEWPORK_SAVE(m_seqChimera, 0x3ff);
                goto chimeraEnd;
            case 0x3ff:
                if (isChimeraBlockedByPlank()) {
                    goto chimeraLeave;
                }
                m_chimeraHit = false;
            chimeraAttackLoop: {
                gfModelAnimation* anim = m_chimera->m_modelAnims[0];
                if (anim == NULL) {
                    goto chimeraAttackDone;
                }
                {
                    float frameCount = (float)anim->getFrameCount();
                    if (anim->getFrame() >= frameCount) {
                        goto chimeraAttackDone;
                    }
                    float frame = anim->getFrame();
                    float windowStart = 1.0f + 0.5f * (float)anim->getFrameCount();
                    bool sleep = false;
                    float windowEnd = 0.9f * (float)anim->getFrameCount();
                    if (frame < windowStart || windowEnd < frame) {
                        sleep = true;
                    }
                    m_chimera->setSleepAttack(sleep);
                    m_chimeraAttack->setSleepAttack(sleep);
                }
                if (m_chimeraHit) {
                    goto chimeraAfterAttack;
                }
                for (int slot = 0; slot < 4; slot++) {
                    if (m_chimera->isAttackSlotHit(slot)) {
                        m_chimeraHit = true;
                        OSReport("UltimateChimeraReadFlush!!!\n");
                        GXColor flash = {0xFF, 0, 0, 0};
                        requestFlash__8efScreenFfiUcUlP8_GXColor(g_efScreen, 25.0f, 0, 0x80, 3, &flash);
                        break;
                    }
                }
                if (!m_chimeraHit) {
                    goto chimeraAfterAttack;
                }
                anim->setUpdateRate(0.0f);
                m_chimeraHitStop = 25.0f;
                goto chimeraHitStopLoop;
            }
            chimeraHitStopTick:
                m_chimeraHitStop -= deltaFrame;
                NEWPORK_SAVE(m_seqChimera, 0x427);
                goto chimeraEnd;
            case 0x427:
                if (isChimeraBlockedByPlank()) {
                    m_chimera->m_modelAnims[0]->setUpdateRate(1.0f);
                    goto chimeraLeave;
                }
            chimeraHitStopLoop:
                if (0.0f <= m_chimeraHitStop) {
                    goto chimeraHitStopTick;
                }
                m_chimera->m_modelAnims[0]->setUpdateRate(1.0f);
            chimeraAfterAttack:
                NEWPORK_SAVE(m_seqChimera, 0x436);
                goto chimeraEnd;
            case 0x436:
                if (isChimeraBlockedByPlank()) {
                    goto chimeraLeave;
                }
                goto chimeraAttackLoop;
            chimeraAttackDone: {
                m_chimera->setMotion((u8)m_chimeraMotion);
                if (m_chimeraMotion == 3) {
                    m_chimera->startEntity();
                } else {
                    m_chimera->startEntityAutoLoop();
                }
                m_chimera->setSleepAttack(true);
                m_chimeraAttack->startEntity();
                m_chimeraAttack->setSleepAttack(true);
                gfModelAnimation* anim = m_chimera->m_modelAnims[0];
                if (anim != NULL) {
                    anim->setFrame(m_chimeraFrame);
                }
                Matrix idMtx;
                fn_27_279FC8(m_chimera, &idMtx, true);
                fn_27_279FC8(m_chimeraAttack, &idMtx, true);
                m_chimeraReady = true;
            }
            chimeraStayTick:
                m_chimeraStay -= deltaFrame;
                NEWPORK_SAVE(m_seqChimera, 0x46b);
                goto chimeraEnd;
            case 0x46b:
            chimeraStayCheck:
                if (0.0f <= m_chimeraStay) {
                    goto chimeraTick;
                }
            chimeraVanish:
                m_chimera->seVanishUltimateChimera();
                g_ecMgr->setEffect(ef_ptc_stg_newpork_syositu, &m_chimeraPos);
                NEWPORK_SAVE(m_seqChimera, 0x474);
                goto chimeraEnd;
            case 0x474:
                NEWPORK_SAVE(m_seqChimera, 0x475);
                goto chimeraEnd;
            case 0x475:
                NEWPORK_SAVE(m_seqChimera, 0x476);
                goto chimeraEnd;
            case 0x476:
                NEWPORK_SAVE(m_seqChimera, 0x477);
                goto chimeraEnd;
            case 0x477: {
                m_chimera->endEntity();
                m_chimeraAttack->endEntity();
                newporkEnableSubject(&m_subject);
                u8 fillHandle = m_screenFill;
                fn_8005E3C8(g_efScreen, 60.0f, &fillHandle);
                goto chimeraFixedWait;
            }
            chimeraLeave:
                m_chimera->setMotion(6);
                m_chimera->startEntityAutoLoop();
                m_chimera->setSleepAttack(true);
                m_chimeraAttack->startEntity();
                m_chimeraAttack->setSleepAttack(true);
                if (m_chimeraReady) {
                    fn_27_279FC8(m_chimera, &m_chimeraMtx, true);
                    fn_27_279FC8(m_chimeraAttack, &m_chimeraMtx, true);
                    m_chimeraReady = false;
                }
                m_chimera->seAttackStopUltimateChimera();
                m_chimera->seScreemUltimateChimera();
                m_chimeraSpeed.m_x = 0.0f;
                m_chimeraSpeed.m_y = 0.0f;
                m_chimeraSpeed.m_z = 0.0f;
                m_chimeraStay = 0.0f;
                goto chimeraFlyCheck;
            chimeraFly: {
                m_chimeraStay += deltaFrame;
                Matrix spin;
                fn_8003E87C(&spin, 0.025132744f * deltaFrame);
                fn_8003EBF4(&spin, 0.003141593f * deltaFrame);
                m_chimeraMtx.mul(&spin, &m_chimeraMtx);
                m_chimeraSpeed.m_y -= 0.0392f * deltaFrame;
                m_chimeraMtx.getPosition(&m_chimeraPos);
                m_chimeraPos += m_chimeraSpeed;
                m_chimeraMtx(0, 3) = m_chimeraPos.m_x;
                m_chimeraMtx(1, 3) = m_chimeraPos.m_y;
                m_chimeraMtx(2, 3) = m_chimeraPos.m_z;
                fn_27_279FC8(m_chimera, &m_chimeraMtx, true);
                fn_27_279FC8(m_chimeraAttack, &m_chimeraMtx, true);
                NEWPORK_SAVE(m_seqChimera, 0x4a8);
                goto chimeraEnd;
            }
            case 0x4a8:
            chimeraFlyCheck:
                if (m_chimeraStay <= 320.0f) {
                    goto chimeraFly;
                }
                goto chimeraVanish;
        }
    }
chimeraEnd:;
}

// Footsteps of the Ultimate Chimera: sound slot 2 / 3 (the two step sounds) at the given frame of its walk animation.
struct stNewporkStep {
    int m_slot;
    float m_frame;
    int m_unk;
};
static stNewporkStep sStepsA[26] = {
    {2, 29.0f, 0},  {3, 62.0f, 0},  {2, 73.0f, 0},  {3, 106.0f, 0}, {2, 152.0f, 0}, {3, 186.0f, 0}, {2, 211.0f, 0},
    {3, 242.0f, 0}, {2, 287.0f, 0}, {3, 341.0f, 0}, {2, 382.0f, 0}, {3, 414.0f, 0}, {2, 455.0f, 0}, {3, 483.0f, 0},
    {2, 520.0f, 0}, {3, 553.0f, 0}, {2, 557.0f, 0}, {3, 606.0f, 0}, {2, 630.0f, 0}, {3, 662.0f, 0}, {2, 697.0f, 0},
    {3, 721.0f, 0}, {2, 743.0f, 0}, {3, 788.0f, 0}, {2, 812.0f, 0}, {3, 837.0f, 0},
};
static stNewporkStep sStepsB[34] = {
    {2, 31.0f, 0},  {3, 61.0f, 0},  {2, 93.0f, 0},   {3, 119.0f, 0},  {2, 161.0f, 0},  {3, 183.0f, 0},
    {2, 210.0f, 0}, {3, 250.0f, 0}, {2, 279.0f, 0},  {3, 303.0f, 0},  {2, 331.0f, 0},  {3, 362.0f, 0},
    {2, 391.0f, 0}, {3, 420.0f, 0}, {2, 470.0f, 0},  {3, 483.0f, 0},  {2, 511.0f, 0},  {3, 540.0f, 0},
    {2, 572.0f, 0}, {3, 600.0f, 0}, {2, 633.0f, 0},  {3, 660.0f, 0},  {2, 692.0f, 0},  {3, 722.0f, 0},
    {2, 750.0f, 0}, {3, 783.0f, 0}, {2, 813.0f, 0},  {3, 841.0f, 0},  {2, 895.0f, 0},  {3, 932.0f, 0},
    {2, 962.0f, 0}, {3, 992.0f, 0}, {2, 1024.0f, 0}, {3, 1051.0f, 0},
};
static stNewporkStep sStepsC[2] = {
    {2, 7.0f, 0},
    {3, 40.0f, 0},
};
static stNewporkStep sStepsD[36] = {
    {2, 31.0f, 0},  {3, 61.0f, 0},  {2, 93.0f, 0},   {3, 121.0f, 0},  {2, 160.0f, 0},  {3, 192.0f, 0},
    {2, 214.0f, 0}, {3, 245.0f, 0}, {2, 271.0f, 0},  {3, 302.0f, 0},  {2, 332.0f, 0},  {3, 361.0f, 0},
    {2, 393.0f, 0}, {3, 423.0f, 0}, {2, 452.0f, 0},  {3, 483.0f, 0},  {2, 512.0f, 0},  {3, 550.0f, 0},
    {2, 571.0f, 0}, {3, 602.0f, 0}, {2, 633.0f, 0},  {3, 661.0f, 0},  {2, 691.0f, 0},  {3, 722.0f, 0},
    {2, 752.0f, 0}, {3, 784.0f, 0}, {2, 812.0f, 0},  {3, 848.0f, 0},  {2, 873.0f, 0},  {3, 905.0f, 0},
    {2, 930.0f, 0}, {3, 966.0f, 0}, {2, 992.0f, 0},  {3, 1023.0f, 0}, {2, 1053.0f, 0}, {3, 1078.0f, 0},
};

grNewpork* grNewpork::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grNewpork* ground = new (Heaps::StageInstance) grNewpork(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->m_heapType = Heaps::StageInstance;
        ground->makeCalcuCallback(1, Heaps::StageInstance);
        ground->setCalcuCallbackRoot(7);
    }
    return ground;
}

grNewpork::~grNewpork() { }

void grNewpork::thisIsUltimateChimera(float size, Vec3f* offset, u32 nodeIndex, bool withSounds) {
    setAttack(size, offset);
    m_attackInfo->m_preset = Attack_Overwrite;
    createAttackPointNormal(getOverwriteAttackData());
    getOverwriteAttackData()->m_power = 100;
    getOverwriteAttackData()->m_reactionEffect = 50;
    getOverwriteAttackData()->m_reactionFix = 0;
    getOverwriteAttackData()->m_reactionAdd = 70;
    getOverwriteAttackData()->m_hitStopFrame = 2.0f;
    getOverwriteAttackData()->m_nodeIndex = nodeIndex;
    getOverwriteAttackData()->m_shapeType = soCollision::Shape_Sphere;
    getOverwriteAttackData()->m_attribute = soCollisionAttackData::Attribute_Cutup;
    getOverwriteAttackData()->m_soundLevel = soCollisionAttackData::Sound_Level_Large;
    getOverwriteAttackData()->m_soundAttribute = soCollisionAttackData::Sound_Attribute_Cutup;
    getOverwriteAttackData()->m_isShieldable = false;
    setSleepAttack(true);
    if (withSounds) {
        createSoundWork(7, 1);
        NEWPORK_SOUND(0, snd_se_stage_Newpork_chimera_piki);
        NEWPORK_SOUND(1, snd_se_stage_Newpork_chimera_screem);
        NEWPORK_SOUND(2, snd_se_stage_Newpork_chimera_step_01);
        NEWPORK_SOUND(3, snd_se_stage_Newpork_chimera_step_02);
        NEWPORK_SOUND(4, snd_se_stage_Newpork_chimera_attack);
        NEWPORK_SOUND(5, snd_se_stage_Newpork_enter);
        NEWPORK_SOUND(6, snd_se_stage_Newpork_vanish);
        fn_27_27C150(this, 4);
        fn_27_27C1E8(this, 0, sStepsA, 26);
        fn_27_27C1E8(this, 1, sStepsB, 34);
        fn_27_27C1E8(this, 2, sStepsD, 36);
        fn_27_27C1E8(this, 3, sStepsC, 2);
    }
}

void grNewpork::seEntryUltimateChimera() {
    startGimmickSE(5);
}

void grNewpork::seVanishUltimateChimera() {
    startGimmickSE(6);
}

void grNewpork::seScreemUltimateChimera() {
    startGimmickSE(1);
}

void grNewpork::seAttackUltimateChimera() {
    startGimmickSE(4);
}

void grNewpork::seAttackStopUltimateChimera() {
    stopGimmickSE(4);
}

void grNewpork::thisIsKowareita() {
    createSoundWork(4, 1);
    NEWPORK_SOUND(0, snd_se_stage_Newpork_boad_hit_01);
    NEWPORK_SOUND(1, snd_se_stage_Newpork_boad_hit_02);
    NEWPORK_SOUND(2, snd_se_stage_Newpork_boad_sand);
    NEWPORK_SOUND(3, snd_se_stage_Newpork_boad_break);
}

void grNewpork::seHitKowareita() {
    startGimmickSE(randi(0xFF) & 1);
}

void grNewpork::seSandKowareita() {
    startGimmickSE(2);
}

void grNewpork::seBreakKowareita() {
    startGimmickSE(3);
}

void grNewpork::thisIsLimousine() {
    createSoundWork(1, 1);
    NEWPORK_SOUND(0, snd_se_stage_Newpork_limousin);
}

void grNewpork::seMoveLimousine() {
    startGimmickSE(0);
}

void grNewpork::seMoveLimousineStop() {
    stopGimmickSE(0);
}
