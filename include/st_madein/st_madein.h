#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gr/gr_madein.h>
#include <gr/gr_tengan_event.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Madein, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Madein, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Madein, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The grounds of the stage in the order the stage creates them (the stage addresses them by this index).
enum stMadeinGround {
    MadeinGround_YokeroBg = 0,
    MadeinGround_Car01 = 1,
    MadeinGround_Car02 = 2,
    MadeinGround_Car03 = 3,
    MadeinGround_FumarerunaBg = 4,
    MadeinGround_Foot = 5,
    MadeinGround_Shagie = 6,
    MadeinGround_FootShadow = 7,
    MadeinGround_KawaseBg = 8,
    MadeinGround_DepthArrow = 9,  // 20 of them
    MadeinGround_FlyArrow = 29,   // 20 of them
    MadeinGround_Ninja = 49,
    MadeinGround_NurerunaBg = 50,
    MadeinGround_Rain = 51,
    MadeinGround_UmbrellaWide = 52,
    MadeinGround_UmbrellaShort = 53,
    MadeinGround_Snow = 54,
    MadeinGround_Cat = 55,
    MadeinGround_JumpBg = 56,
    MadeinGround_Jump = 57,
    MadeinGround_CrackerYellow = 58,
    MadeinGround_CrackerBlue = 59,
    MadeinGround_CrackerRed = 60,
    MadeinGround_CrackerTapeBg2 = 61,
    MadeinGround_CrackerTapeBg1 = 62,
    MadeinGround_CrackerTapeBg3 = 63,
    MadeinGround_NaraseFloor = 64,
    MadeinGround_UgokunaBg = 65,
    MadeinGround_UgokunaFont = 66,
    MadeinGround_Apirushiro = 67,
    MadeinGround_ApirushiroFloor = 68,
    MadeinGround_Bomb = 69,
    MadeinGround_FalseMark = 70,  // 4 of them
    MadeinGround_TrueMark = 74,   // 4 of them
    MadeinGround_LeftWarnning = 78,
    MadeinGround_RightWarnning = 79,
    MadeinGround_NodeOnly = 80,
    MadeinGround_MainBg = 81,
    MadeinGround_Pig = 82,
    MadeinGround_Title01 = 83,    // 10 of them
    MadeinGround_Attack1 = 93,    // 4 of them
    MadeinGround_Kudake = 97,     // 17 of them
    MadeinGround_GroundAttack = 114,
    MadeinGround_CrackerYellow2 = 115,
    MadeinGround_CrackerBlue2 = 116,
    MadeinGround_CrackerRed2 = 117,
};

// HYPOTHESIS: what the AI reads about the micro-game that is played (see getMadeinAiData). The part from +8 depends on
// the game: most of them tell where the danger is, the games with several things to hit keep a position for each.
struct stMadeinAiData {
    u8 m_active;     // 0x00
    u8 m_unk1;       // 0x01
    u8 m_game;       // 0x02
    char _3;
    float m_time;    // 0x04
    union {
        struct {
            u8 m_flag0;  // 0x08
            u8 m_flag1;  // 0x09
            u8 m_flag2;  // 0x0A
            char _b;
            float m_x;   // 0x0C
            float m_y;   // 0x10
            float m_z;   // 0x14
        } m_single;
        struct {
            u8 m_flag[4]; // 0x08
            struct {
                float m_x;
                float m_y;
            } m_pos[3];
        } m_narase;
        struct {
            u8 m_flag[12]; // 0x08
            struct {
                float m_x;
                float m_y;
            } m_pos[12];
        } m_kudake;
    };
};
static_assert(sizeof(stMadeinAiData) == 0x74, "Class is wrong size!");

// WarioWare, Inc. (the "Gamer" stage): ten micro-games are played one after the other on the same stage. The
// micro-games are named by the map (Yokero = dodge the cars, Fumareruna = don't get stepped on, Kawase = dodge the
// arrows, Nureruna = don't get wet, Umakutobe = jump over the hammer, Narase = pull the crackers, Ugokuna = don't move,
// Apirusiro = strike a pose, Kudake = smash the blocks). The names of the fields are HYPOTHESIS; the ones that are only
// an offset (unkN) are state of the micro-games that is not named yet.
class stMadein : public stMelee {
    grTenganEvent m_event0;           // 0x1D8: the flow of the stage (pick a micro-game, play it, judge the fighters)
    grTenganEvent m_event1;           // 0x284: the micro-game that is played
    grTenganEvent m_event2;           // 0x330: the title of the micro-game flies in
    grTenganEvent m_event3;           // 0x3DC: the bomb, the time the fighters have for the micro-game
    grTenganEvent m_event4;           // 0x488: the sign that warns about the car
    grTenganEvent m_event5;           // 0x534: the marks that tell who got the micro-game right
    grTenganEvent m_event6;           // 0x5E0: the background swings out
    grTenganEvent m_event7;           // 0x68C: the background swings back
    s32 m_unk738;                       // 0x738
    s32 m_unk73C;                       // 0x73C
    u8 m_unk740;                        // 0x740
    char _741[3];
    float m_unk744;                     // 0x744
    float m_unk748;                     // 0x748
    float m_unk74C;                     // 0x74C
    float m_unk750;                     // 0x750
    float m_unk754;                     // 0x754
    float m_unk758;                     // 0x758
    float m_unk75C;                     // 0x75C
    Vec3f m_curvePos[4];              // 0x760
    Vec3f m_curveRot[4];              // 0x790
    Vec3f m_curveScale[4];            // 0x7C0
    float m_curveT;                   // 0x7F0
    u32 m_game;                       // 0x7F4: the micro-game that is played (index of the game table)
    u8 m_unk7F8;                        // 0x7F8
    u8 m_unk7F9;                        // 0x7F9
    char _7FA[2];
    u32 m_arrowNum;                     // 0x7FC: number of arrows of the dodge game
    Vec3f m_arrowPos[20];               // 0x800
    u8 m_arrowHit[20];                  // 0x8F0
    float m_unk904;                     // 0x904
    float m_unk908;                     // 0x908
    float m_unk90C;                     // 0x90C
    float m_unk910;                     // 0x910
    u8 m_unk914;                        // 0x914
    u8 m_unk915;                        // 0x915
    char _916[2];
    Vec3f m_unk918;                     // 0x918
    Vec3f m_unk924;                     // 0x924
    u8 m_unk930;                        // 0x930
    u8 m_unk931;                        // 0x931
    char _932[2];
    float m_unk934;                     // 0x934
    float m_unk938;                     // 0x938
    float m_unk93C;                     // 0x93C
    float m_unk940;                     // 0x940
    u32 m_unk944;                       // 0x944
    u8 m_unk948;                        // 0x948
    char _949[3];
    s32 m_unk94C;                       // 0x94C
    s32 m_unk950;                       // 0x950
    u8 m_unk954[4];                     // 0x954
    s32 m_unk958;                       // 0x958
    u8 m_unk95C[3];                     // 0x95C
    char _95F;
    Vec3f m_standPos[4];                // 0x960
    float m_unk990;                     // 0x990
    float m_unk994;                     // 0x994
    float m_unk998;                     // 0x998
    u8 m_unk99C;                        // 0x99C
    u8 m_unk99D[0xC];                   // 0x99D
    u8 m_unk9A9[4];                     // 0x9A9
    u8 m_unk9AD[6];                     // 0x9AD
    u8 m_unk9B3;                        // 0x9B3
    u32 m_table[10];                  // 0x9B4: the order of the micro-games
    s32 m_tableIndex;                 // 0x9DC
    u8 m_unk9E0;                        // 0x9E0
    bool m_unk9E1;                        // 0x9E1
    u8 m_unk9E2;                        // 0x9E2
    u8 m_unk9E3[4];                     // 0x9E3
    u8 m_unk9E7[4];                     // 0x9E7
    char _9EB;
    stMadeinAiData m_ai;                // 0x9EC
    u8 m_unkA60;                        // 0xA60
    char _A61[3];

public:
    stMadein();
    static stMadein* create();

    virtual ~stMadein();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual bool isEnd();
    virtual u8 getIteamDropStatus();
    virtual void* getMadeinAiData();
    virtual bool isBamperVector();
    virtual int getBgmID();

    void shufuleTable();
    void setYokeroStage();
    void updateBomb(float deltaFrame);
    void updateTitleEvent(float deltaFrame);
    void updateCorrect(float deltaFrame);
    void updateStage(float deltaFrame);
    void updateYokeroWarnning(float deltaFrame);
    bool updateFumareruna(float deltaFrame);
    bool updateYokero(float deltaFrame);
    bool updateKawase(float deltaFrame);
    bool updateNureruna(float deltaFrame);
    bool updateUmakutobe(float deltaFrame);
    bool updateNarase(float deltaFrame);
    bool updateUgokuna(float deltaFrame);
    bool updateApirusiro(float deltaFrame);
    bool updateKudake(float deltaFrame);
    bool updateMiniGame(float deltaFrame);

    static stClassInfoImpl<Stages::Madein, stMadein> bss_loc_14;
};
static_assert(sizeof(stMadein) == 0xA64, "Class is wrong size!");
