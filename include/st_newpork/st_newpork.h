#pragma once

#include <cm/cm_subject.h>
#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <gr/gr_norfair_player_area_check.h>
#include <memory.h>
#include <nw4r/ut/ut_Color.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::NewPork, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::NewPork, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::NewPork, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The destructor of cmSubject takes a hidden delete flag in the main binary (this, flag); the stage passes -1 to
// destroy the member in place. The constructor is the real cmSubject(int kind, int flag) from cm/cm_subject.h.
extern "C" void __dt__9cmSubjectFv(cmSubject* subject, int deleteFlag);

// A camera subject that is built with the stage (the constructor of cmSubject is not in the headers). The stage
// destroys it itself.
class stNewporkSubject : public cmSubject {
public:
    stNewporkSubject() : cmSubject(0, 1) { }
};

// One ground object of New Pork City. The stage builds eight of them from the "grNewpork*" models; the model index
// (the first create() argument) tells which part it is. The class adds no data; the thisIs* functions give a ground its
// special parts (attack and sound tables) and the se* functions play its sounds.
class grNewpork : public grMadein {
public:
    enum Part {
        Part_Chikei = 0,            // the street the fighters stand on (with the Viking boat nodes)
        Part_Viking = 1,            // the Viking boat that carries fighters along the street
        Part_KowareIta = 2,         // the plank of the boat that can be broken
        Part_Limousine = 3,         // the limousine that drives through
        Part_UltimateChimeraArea = 4, // the attack area of the Ultimate Chimera
        Part_LimousineMove = 5,     // the path animation the limousine follows
        Part_UltimateChimera = 6,   // the Ultimate Chimera model
        Part_LimousineBG = 7,
        Part_ChikeiBG = 8,
    };

    grNewpork(const char* taskName) : grMadein(taskName) { setupMelee(); }
    virtual ~grNewpork();

    // Gives a Chimera ground its attack (power 100, a sphere of the given size at the offset on the given node) and, for the model, its sounds and footsteps.
    void thisIsUltimateChimera(float size, Vec3f* offset, u32 nodeIndex, bool withSounds) __attribute__((never_inline));
    void seEntryUltimateChimera() __attribute__((never_inline));
    void seVanishUltimateChimera() __attribute__((never_inline));
    void seScreemUltimateChimera() __attribute__((never_inline));
    void seAttackUltimateChimera() __attribute__((never_inline));
    void seAttackStopUltimateChimera() __attribute__((never_inline));
    void thisIsKowareita() __attribute__((never_inline));
    void seHitKowareita() __attribute__((never_inline));
    void seSandKowareita() __attribute__((never_inline));
    void seBreakKowareita() __attribute__((never_inline));
    void thisIsLimousine() __attribute__((never_inline));
    void seMoveLimousine() __attribute__((never_inline));
    void seMoveLimousineStop() __attribute__((never_inline));

    // The stage reads the landing flag / last damage of the plank and clears the flag after it reacted.
    bool wasHit() { return m_isHit; }
    void clearHit() { m_isHit = false; }
    float getLastDamage() { return m_hitPointInfo->m_lastDamageTaken; }
    void setHitCategory(HitCategory category) { m_hitCategory = (HitCategory)(m_hitCategory | category); }
    // The Chimera's four attack slots: a slot's flag is set while a fighter is hit by it.
    bool isAttackSlotHit(int slot) { return reinterpret_cast<u8*>(m_attackInfo)[0x1C + slot] != 0; }

    static grNewpork* create(int mdlIndex, const char* tgtNodeName, const char* taskName) __attribute__((never_inline));
};

// The state of one scripted hazard of the stage. The hazards are written as coroutines: m_line is the number of the
// source line to resume at (0 = start), m_tag is a marker that is set together with it.
struct stNewporkSeq {
    int m_tag;
    int m_line;

    stNewporkSeq();
    stNewporkSeq(int tag, int line) : m_tag(tag), m_line(line) { }
};

// New Pork City: the Viking boat plank breaks when it is hit often enough, a limousine drives through at random and
// now and then the Ultimate Chimera shows up on the street and attacks. The big update function runs three
// coroutines, one for each hazard.
class stNewpork : public stMelee {
    grNewpork* m_chikei;            // 0x1D8
    grNewpork* m_chikeiBG;          // 0x1DC
    grNewpork* m_viking;            // 0x1E0
    grNewpork* m_kowareIta;         // 0x1E4
    grNewpork* m_limousine;         // 0x1E8
    grNewpork* m_limousineBG;       // 0x1EC
    grNewpork* m_limousineActive;   // 0x1F0: the limousine part that currently drives (m_limousine or m_limousineBG)
    grNewpork* m_limousineMove;     // 0x1F4
    grNewpork* m_chimera;           // 0x1F8
    grNewpork* m_chimeraAttack;     // 0x1FC
    grPlayerAreaCheck* m_chimeraArea;  // 0x200
    grPlayerAreaCheck* m_chimeraArea2; // 0x204
    const float* m_param;           // 0x208: tuning values of the hazards (frames, ratios)
    u32 m_nodeVikingMove;           // 0x20C: "Pos_VIKING_moveParts"
    u32 m_nodeKowareItaPos;         // 0x210: "Pos_kowareIta"
    u32 m_nodeKowareItaModel;       // 0x214: "StgNewpork00_kowareIta"
    u32 m_nodeLimousineAttach;      // 0x218: "Limousine_attach"
    u32 m_nodeChimeraCtrl;          // 0x21C: "StgNewporkUltimateChimera_TR_Ctrl"
    u32 m_nodeChimeraHead;          // 0x220: "StgNewporkUltimateChimera_HeadN"
    stNewporkSeq m_seqPlank;        // 0x224: the breaking plank
    float m_plankTimer;             // 0x22C: strength of the plank, later the time until it comes back
    float m_plankHitCooldown;       // 0x230
    float m_plankBrokenTime;        // 0x234: frames since the plank broke
    bool m_plankBroken;             // 0x238
    stNewporkSeq m_seqLimousine;    // 0x23C: the limousine
    float m_limousineTimer;         // 0x244
    int m_limousineLast;            // 0x248: the last two motions that were used
    int m_limousineLast2;           // 0x24C
    stNewporkSeq m_seqChimera;      // 0x250: the Ultimate Chimera
    bool m_chimeraReady;            // 0x258: the Chimera's matrix follows its control node
    int m_chimeraMotion;            // 0x25C: 0 and 1 stand and wait, 2 ..., 3 and 4 turn around, 5 enter, 6 leave
    float m_chimeraFrame;           // 0x260: frame the Chimera's animation was stopped at
    float m_chimeraStay;            // 0x264: frames the Chimera stays
    float m_chimeraWait;            // 0x268: frames until the Chimera shows up
    Matrix m_chimeraMtx;            // 0x26C
    Vec3f m_chimeraPos;             // 0x29C
    bool m_chimeraHit;              // 0x2A8: the Chimera hit a fighter
    float m_chimeraHitStop;         // 0x2AC: frames the Chimera's animation is frozen after a hit
    Vec3f m_chimeraSpeed;           // 0x2B0: speed of the Chimera when it flies away
    stNewporkSubject m_subject;     // 0x2BC: camera subject that follows the Chimera
    float m_areaCooldown;           // 0x340
    float m_areaCooldown2;          // 0x344
    u8 m_screenFill;                // 0x348: handle of the dark screen fill
    char _349[3];
    grCollisionJoint* m_plankJoint; // 0x34C: collision joint of the plank

public:
    stNewpork();
    static stNewpork* create();

    virtual ~stNewpork();
    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual bool isBamperVector() { return true; }
    virtual GXColor getFinalTechniqColor() { return nw4r::ut::Color(0x14000496); }

    // The Chimera cannot walk across the hole the broken plank leaves, so it leaves when that is about to happen.
    bool isChimeraBlockedByPlank() {
        return m_chimeraMotion == 1 && m_plankBroken && 4.0f <= m_plankBrokenTime && 342.0f <= m_chimeraFrame
               && 576.0f >= m_chimeraFrame;
    }

    static stClassInfoImpl<Stages::NewPork, stNewpork> bss_loc_14;
};
static_assert(sizeof(stNewpork) == 0x350, "Class is wrong size!");
