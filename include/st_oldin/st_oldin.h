#pragma once

#include <cm/cm_subject.h>
#include <ef/ef_screen_handle.h>
#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <nw4r/ut/ut_Color.h>
#include <snd/snd_3d_generator.h>
#include <snd/snd_id.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Oldin, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Oldin, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Oldin, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// One ground object of the Bridge of Eldin. The first create() argument is the model index: 0 the ground, 1 the bridge,
// 2 Bulblin, 4 King Bulblin, 5 Lord Bullbo, 6 the barrel, 7 a hit box (or attack box), 8 the broken bridge, 9 the
// portal, 0xA the shadow of the bridge, 0xB the sun (see stOldin::createObj).
class grOldin : public grMadein {
public:
    grOldin(const char* taskName) : grMadein(taskName) { setupMelee(); }
    virtual ~grOldin();

    // The flags and fields of grMadein the stage reads and clears.
    void clearHit() { m_isHit = false; }
    float getLastDamage() { return m_hitPointInfo->m_lastDamageTaken; }
    void setYakumonoLr(float lr) { m_yakumono->setLr(lr); }

    static grOldin* create(int mdlIndex, const char* tgtNodeName, const char* taskName) __attribute__((never_inline));
};

// The state of a coroutine of the stage (see stNewporkSeq in st_newpork.h): m_line is the number of the source line to
// resume at, m_tag is a marker that is stored with it.
struct stOldinSeq {
    int m_tag;
    int m_line;

    stOldinSeq(int tag) : m_tag(tag), m_line(0) { }
};

// The numbers the stage is tuned with. HYPOTHESIS: names follow the use in the stage functions.
struct stOldinParam {
    float m_kingStartWait; // 0x00: the wait before King Bulblin's first ride
    float m_kingWaitMin;   // 0x04: the shortest wait between two rides
    float m_kingWaitMax;   // 0x08: the longest wait
    float m_kingChance;    // 0x0C: the chance (0 to 1) the wait is not played out again
    float m_kingHornTime;  // 0x10: the time left on the wait when the horn sounds
    float m_weight[4];     // 0x14: how likely a ride is with (1, 3) or without (0, 2) a Bulblin
    float m_barrelPercent; // 0x24: the chance in percent King Bulblin throws a barrel
    float m_bulblinHp;     // 0x28
    float m_bridgeWaitMin; // 0x2C: the shortest time the bridge stays down
    float m_bridgeWaitMax; // 0x30: the longest
};
static_assert(sizeof(stOldinParam) == 0x34, "Class is wrong size!");

// Bridge of Eldin: King Bulblin rides Lord Bullbo over the bridge, a Bulblin walks over it, a barrel rolls onto it and
// blows it up, and the bridge comes back through the portal.
class stOldin : public stMelee {
    stOldinParam* m_param;          // 0x1D8
    grOldin* m_ground;              // 0x1DC
    grOldin* m_sun;                 // 0x1E0
    grOldin* m_bridge;              // 0x1E4
    grOldin* m_bridgeShadow;        // 0x1E8
    grOldin* m_portal;              // 0x1EC
    grOldin* m_bridgeCrash;         // 0x1F0
    grOldin* m_bulblin;             // 0x1F4
    grOldin* m_kingBulblin;         // 0x1F8
    grOldin* m_lordBullbo;          // 0x1FC
    grOldin* m_taru;                // 0x200
    grOldin* m_kingBulblinHit;      // 0x204
    grOldin* m_lordBullboHit;       // 0x208
    grOldin* m_bulblinHit;          // 0x20C
    grOldin* m_taruAttack;          // 0x210
    grOldin* m_bridgeAttack;        // 0x214
    u32 m_bridgeNode[3];            // 0x218: "StgOldinClash", "OldinClash06", "hashi_koware"
    u32 m_portalNode;               // 0x224
    bool m_noBattle;                  // 0x228: set in the stage of the target test (no enemies)
    bool m_lordActive;                // 0x229: Lord Bullbo is on his way over the bridge
    char _22A[2];
    stOldinSeq m_lordSeq;           // 0x22C
    Matrix m_kingMtx;               // 0x234: where King Bulblin rides in
    Matrix m_lordMtx;               // 0x264
    Matrix m_lordNodeMtx;           // 0x294
    u32 m_lordNode[3];              // 0x2C4
    Vec3f m_lordHitOffset;          // 0x2D0
    float m_lordTimer;              // 0x2DC
    cmSubject m_lordSubject;        // 0x2E0
    stOldinSeq m_kingSeq;           // 0x364
    float m_kingTimer;              // 0x36C
    bool m_kingSide;                  // 0x370: 1 rides in from the left
    bool m_kingLastSide;              // 0x371
    bool m_withBulblin;               // 0x372: a Bulblin walks over the bridge behind King Bulblin
    bool m_barrel;                    // 0x373: King Bulblin throws a barrel on this ride
    bool m_barrelNow;                 // 0x374
    bool m_barrelLast;                // 0x375
    char _376[2];
    Vec3f m_kingHitOffset;          // 0x378
    int m_lordHits;                 // 0x384
    bool m_bulblinActive;             // 0x388
    bool m_bulblinLanded;             // 0x389
    char _38A[2];
    stOldinSeq m_bulblinSeq;        // 0x38C
    float m_bulblinSwayTimer;       // 0x394
    Matrix m_bulblinMtx;            // 0x398
    Matrix m_bulblinNodeMtx;        // 0x3C8
    u32 m_bulblinNode;              // 0x3F8
    float m_bulblinHp;              // 0x3FC
    Vec3f m_bulblinSpeed;           // 0x400
    int m_bulblinHits;              // 0x40C
    float m_bulblinStun;            // 0x410
    bool m_taruActive;                // 0x414
    char _415[3];
    stOldinSeq m_taruSeq;           // 0x418
    float m_taruTimer;              // 0x420
    u32 m_taruNode;                 // 0x424
    u32 m_taruEffect;               // 0x428
    cmSubject m_taruSubject;        // 0x42C
    bool m_bridgeActive;              // 0x4B0
    char _4B1[3];
    stOldinSeq m_bridgeSeq;         // 0x4B4
    float m_bridgeTimer;            // 0x4BC
    cmSubject m_portalSubject;      // 0x4C0
    cmSubject m_bridgeSubject;      // 0x544
    snd3DGenerator m_soundLord;     // 0x5C8
    int m_soundLordStep;            // 0x5D0
    int m_soundLordRoar;            // 0x5D4
    float m_soundLordTimer;         // 0x5D8
    int m_soundLordRumble;          // 0x5DC
    int m_soundLordKing;            // 0x5E0
    snd3DGenerator m_soundTaru;     // 0x5E4
    int m_soundTaruThrow;           // 0x5EC
    int m_soundTaruDrop;            // 0x5F0
    bool m_taruThrown;                // 0x5F4
    char _5F5[3];
    int m_soundBridgeSlide;         // 0x5F8
    snd3DGenerator m_soundBulblin;  // 0x5FC
    efScreenHandle m_screenHandle;  // 0x604
    char _605[3];

public:
    stOldin();
    static stOldin* create();

    virtual ~stOldin();
    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual bool isBamperVector() { return true; }
    virtual GXColor getFinalTechniqColor() { return nw4r::ut::Color(0x14000496); }

    void LordBullboStart(Matrix* matrix, bool full);
    void LordBullboUpdate(float deltaFrame);
    void KingBlublinUpdate(float deltaFrame);
    void BulblinUpdate(float deltaFrame);
    void TaruUpdate(float deltaFrame);
    void BridgeUpdate(float deltaFrame);
    void setLordBullboAttack(Vec3f* offset) __attribute__((never_inline));
    void setTaruAttack(Vec3f* offset) __attribute__((never_inline));
    void setBridgeAttack() __attribute__((never_inline));
    void setLRHang(bool hang) __attribute__((never_inline));

    static stClassInfoImpl<Stages::Oldin, stOldin> bss_loc_14;
};
static_assert(sizeof(stOldin) == 0x608, "Class is wrong size!");
