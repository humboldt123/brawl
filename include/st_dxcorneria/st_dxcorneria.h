#pragma once

#include <gm/gm_lib.h>
#include <if/if_smash_appear.h>
#include <gr/gr_madein.h>
#include <memory.h>
#include <nw4r/ut/ut_Color.h>
#include <snd/snd_3d_generator.h>
#include <snd/snd_id.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::DxCorneria, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::DxCorneria, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::DxCorneria, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// One ground object of the Corneria stage. The first create() argument is the model index: 0 the terrain, 1 the
// Great Fox, 3 the Arwing, 4 the Wolfen, 5/6 the two motions an Arwing flies, 7 the mesh of an Arwing beam, 8 the mesh
// of a Great Fox shot, 9 a hit box, 0xA the base the Pokemon Trainer stands on (see stDxCorneria::createObj).
class grDxCorneria : public grMadein {
public:
    grDxCorneria(const char* taskName) : grMadein(taskName) { setupMelee(); }
    virtual ~grDxCorneria();

    static grDxCorneria* create(int mdlIndex, const char* tgtNodeName, const char* taskName) __attribute__((never_inline));
};

// The hit box of a beam: it only counts as a hit for the team that last touched the beam.
class grDxCorneriaBeam : public grMadein {
public:
    float m_scale;      // 0x1A4
    float m_angle;      // 0x1A8
    bool m_reflected;   // 0x1AC: a fighter reflected the beam
    bool m_hit;         // 0x1AD
    bool m_reflectable; // 0x1AE
    char _1AF;

    grDxCorneriaBeam(const char* taskName) : grMadein(taskName) {
        setupMelee();
        m_scale = 1.0f;
        m_angle = 0.0f;
        m_reflected = false;
        m_hit = false;
        m_reflectable = true;
    }
    virtual ~grDxCorneriaBeam();

    virtual void onInflict(soCollisionLog* collisionLog, u32 flags, float power);
    void resetBeam();

    static grDxCorneriaBeam* create(int mdlIndex, const char* tgtNodeName, const char* taskName) __attribute__((never_inline));
};

// The coroutine state of the Great Fox falling (see stNewporkSeq in st_newpork.h).
struct stDxCorneriaSeq {
    int m_tag;
    int m_line;

    stDxCorneriaSeq(int tag) : m_tag(tag), m_line(0) { }
};

// The numbers the stage is tuned with (the first float is the shortest, the second the longest wait; HYPOTHESIS:
// names follow the use in the stage functions).
struct stDxCorneriaParam {
    float m_swayWaitMin;       // 0x00
    float m_swayWaitMax;       // 0x04
    float m_swayAccel;         // 0x08
    float m_swaySpeedLimit;    // 0x0C
    float m_swayLimitX;        // 0x10
    float m_swayLimitY;        // 0x14
    float m_cannonWaitMin;     // 0x18
    float m_cannonWaitMax;     // 0x1C
    float m_cannonChargeTime;  // 0x20
    float m_cannonSideChance;  // 0x24
    float m_cannonTimeA;       // 0x28
    float m_cannonTimeB;       // 0x2C
    int m_cannonShotsMin;      // 0x30
    int m_cannonShotsMax;      // 0x34
    float m_cannonHp;          // 0x38
    float m_arwingFirstMin;    // 0x3C
    float m_arwingFirstMax;    // 0x40
    float m_arwingWaitMin;     // 0x44
    float m_arwingWaitMax;     // 0x48
    float m_wolfenChance;      // 0x4C
    int _50[6];                // 0x50
    float _68;                 // 0x68
    float _6C;                 // 0x6C
    float _70;                 // 0x70
    int m_arwingFlightTime;    // 0x74
    int m_arwingShotChance;    // 0x78
    int _7C;                   // 0x7C
    int _80;                   // 0x80
    int _84;                   // 0x84
    float _88;                 // 0x88
};
static_assert(sizeof(stDxCorneriaParam) == 0x8C, "Class is wrong size!");

// One slot of the beams: the mesh, its hit box and what the beam is doing (0 free, 1 start, 2 grow, 3 fly, 4 end).
struct stDxCorneriaBeam {
    grMadein* m_ground;      // 0x00
    grDxCorneriaBeam* m_hit; // 0x04
    int m_power;             // 0x08
    u32 m_node;              // 0x0C: node of the model the beam starts at
    int m_state;             // 0x10
    float m_timer;           // 0x14
    bool m_fromArwing;       // 0x18
    bool m_reflectable;      // 0x19
    bool m_homing;           // 0x1A
    bool m_aimed;            // 0x1B
    Vec3f m_target;          // 0x1C
    Vec3f m_direction;       // 0x28

    void update(float deltaFrame, grMadein* source);
};
static_assert(sizeof(stDxCorneriaBeam) == 0x34, "Class is wrong size!");

// Corneria: the Great Fox in the middle fires its cannon at the fighters, Arwings and Wolfens fly past and shoot, the
// water at the bottom splashes and the ship sways.
class stDxCorneria : public stMelee {
    stDxCorneriaParam* m_param;         // 0x1D8
    grDxCorneria* m_terrain;            // 0x1DC
    grDxCorneria* m_greatFox;           // 0x1E0
    grDxCorneria* m_greatFoxHit;        // 0x1E4: the part of the ship that can be shot
    grDxCorneria* m_greatFoxAttack;     // 0x1E8: the hit box of the cannon beam
    grDxCorneria* m_arwing;             // 0x1EC
    grDxCorneria* m_wolfen;             // 0x1F0
    grDxCorneria* m_arwingMotionA;      // 0x1F4
    grDxCorneria* m_arwingMotionB;      // 0x1F8
    grDxCorneria* m_trainerBase;        // 0x1FC
    stDxCorneriaBeam m_beamA[32];       // 0x200: beams of the Arwings
    int m_beamACount;                   // 0x880
    u32 m_beamAFrontNode;               // 0x884
    u32 m_beamARearNode;                // 0x888
    stDxCorneriaBeam m_beamG[32];       // 0x88C: shots of the Great Fox
    int m_beamGCount;                   // 0xF0C
    u32 m_beamGFrontNode;               // 0xF10
    u32 m_beamGRearNode;                // 0xF14
    float m_arwingTimer;                // 0xF18
    int m_arwingIsWolfen;               // 0xF1C
    grDxCorneria* m_arwingActive;       // 0xF20
    int m_arwingKind;                   // 0xF24
    int m_arwingMotion;                 // 0xF28
    grDxCorneria* m_arwingMotionGround; // 0xF2C
    u32 m_arwingMotionNode;             // 0xF30
    u32 m_arwingMoveNode[2];            // 0xF34
    u32 m_arwingBeamNode[2];            // 0xF3C: "BeamL" / "BeamR"
    u32 m_wolfenBeamNode[2];            // 0xF44
    float m_arwingRoll;                 // 0xF4C
    float m_arwingRollTimer;            // 0xF50
    u32 m_waterHighNode;                // 0xF54
    u32 m_splashANode[3];               // 0xF58: "ShibukiA2N", "ShibukiA1N", "ShibukiA0N"
    u32 m_splashBNode[2];               // 0xF64: "ShibukiB1N", "ShibukiB0N"
    bool m_splashAActive;               // 0xF6C
    bool m_splashBActive;               // 0xF6D
    char _F6E[2];
    u32 m_splashAEffect;                // 0xF70
    u32 m_splashBEffect;                // 0xF74
    u32 m_gunNormalNode;                // 0xF78
    u32 m_gunCrashNode;                 // 0xF7C
    u32 m_chargeNode[2];                // 0xF80: "JyudenLN" / "JyudenRN"
    int m_cannonState;                  // 0xF88
    int m_cannonSide;                   // 0xF8C
    float m_cannonHp;                   // 0xF90
    float m_cannonTimer;                // 0xF94
    int m_cannonShots;                  // 0xF98
    float m_cannonSpeed;                // 0xF9C
    u32 m_chargeEffect[2];              // 0xFA0
    char _FA8[4];
    stDxCorneriaSeq m_seq;              // 0xFAC
    float m_fallTime;                   // 0xFB4
    float m_fallSpeed;                  // 0xFB8
    float m_fallHeight;                 // 0xFBC
    float m_swayTimer;                  // 0xFC0
    float m_swayAccelX;                 // 0xFC4
    float m_swayAccelY;                 // 0xFC8
    float m_swaySpeedX;                 // 0xFCC
    float m_swaySpeedY;                 // 0xFD0
    float m_swayX;                      // 0xFD4
    float m_swayY;                      // 0xFD8
    float m_baseTimer;                  // 0xFDC
    float m_baseAccelX;                 // 0xFE0
    float m_baseAccelY;                 // 0xFE4
    float m_baseSpeedX;                 // 0xFE8
    float m_baseSpeedY;                 // 0xFEC
    float m_baseX;                      // 0xFF0
    float m_baseY;                      // 0xFF4
    u32 m_trainerNode[4];               // 0xFF8
    snd3DGenerator m_soundArwing;       // 0x1008
    int m_soundArwingA;                 // 0x1010
    int m_soundArwingB;                 // 0x1014
    snd3DGenerator m_soundBase;         // 0x1018
    u32 m_nullNode;                     // 0x1020
    int m_soundBaseHandle;              // 0x1024
    snd3DGenerator m_soundSplash;       // 0x1028
    int m_soundSplashHandle;            // 0x1030
    snd3DGenerator m_soundCannon;       // 0x1034
    u32 m_startCounter;                 // 0x103C
    IfSmashAppearTask* m_appearTask;    // 0x1040
    int m_appearKind;                   // 0x1044
    u8 m_appearing;                     // 0x1048
    char _1049[3];

public:
    stDxCorneria();
    static stDxCorneria* create();

    virtual ~stDxCorneria();
    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual GXColor getFinalTechniqColor() { return nw4r::ut::Color(0x1400047d); }
    virtual void setAppearKind(u8 kind);
    virtual bool startAppear();
    virtual void endAppear();
    virtual bool isStartAppearTimming();
    virtual bool isAppear();
    virtual void forceStopAppear();
    virtual IfSmashAppearTask* getAppearTask();

    void GFoxCanonUpdate(float deltaFrame);
    void ArwingAttackStart();
    void ArwingAttackUpdate(float deltaFrame);
    bool GetArwingTarget(Vec3f* target);
    void WaterSplashUpdate();
    bool checkRectInAnyPlayer(Vec3f* a, Vec3f* b);
    void GFoxFallUpdate(float deltaFrame);
    void GFoxUpdate(float deltaFrame);
    void PTBaseUpdate(float deltaFrame);
    void setBeamAAttack(grDxCorneriaBeam* beam) __attribute__((never_inline));
    void setBeamGAttack(grDxCorneriaBeam* beam) __attribute__((never_inline));
    void setGFoxCanonAttack(grDxCorneria* ground) __attribute__((never_inline));

    static stClassInfoImpl<Stages::DxCorneria, stDxCorneria> bss_loc_14;
};
static_assert(sizeof(stDxCorneria) == 0x104C, "Class is wrong size!");
