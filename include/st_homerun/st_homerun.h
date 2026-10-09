#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <st_homerun/gr_homerun.h>
#include <it/item.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

class grCollisionJoint;
class grGimmickBeltConveyorData;
class stTrigger;

template<typename T>
class stClassInfoImpl<Stages::HomeRunContest, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::HomeRunContest, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::HomeRunContest, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// A keyed vector the stage's camera interpolates (HYPOTHESIS names): it moves from m_start to m_end in m_duration frames.
struct stHomerunKey3 {
    Vec3f m_start;    // 0x00
    Vec3f m_end;      // 0x0C
    float m_elapsed;  // 0x18
    float m_duration; // 0x1C
    u8 m_ease;        // 0x20
    char _21[3];
};
static_assert(sizeof(stHomerunKey3) == 0x24, "Class is wrong size!");

// The same for a single number.
struct stHomerunKey1 {
    float m_start;    // 0x00
    float m_end;      // 0x04
    float m_elapsed;  // 0x08
    float m_duration; // 0x0C
    u8 m_ease;        // 0x10
    char _11[3];
};
static_assert(sizeof(stHomerunKey1) == 0x14, "Class is wrong size!");

// Home-Run Contest: the fighter hits a sandbag with the Home-Run Bat and the stage measures how far it flies. The
// stage owns the scroll of the background, the camera and the Smash Ball style "figures" that fly by.
class stHomerun : public stMelee {
    u8 m_phase;                       // 0x1D8: 0 start .. 10 end (see updateSandBag)
    char _1D9[3];
    float m_waitTimer;                // 0x1DC
    float m_landTimer;                // 0x1E0
    float m_sceneFrame;               // 0x1E4: frame of the scene animation (set from the distance)
    float m_endTimer;                 // 0x1E8
    BaseItem* m_sandBag;              // 0x1EC
    BaseItem* m_bat;                  // 0x1F0
    bool m_sandBagWarped;             // 0x1F4
    bool m_batWarped;                 // 0x1F5
    char _1F6[2];
    float m_sandBagHp;                // 0x1F8
    u8 m_cameraState;                 // 0x1FC
    char _1FD[3];
    float m_cameraTimer;              // 0x200
    u8 m_cameraMode;                  // 0x204
    char _205[3];
    float m_cameraBlend;              // 0x208
    stHomerunKey3 m_keyEye;           // 0x20C: camera position (before the throw)
    stHomerunKey3 m_keyTarget;        // 0x230: where the camera looks
    stHomerunKey3 m_keyAngle;         // 0x254: direction of the camera seen from the target
    stHomerunKey1 m_keyDist;          // 0x278: distance of the camera to the target
    stHomerunKey1 m_keyFov;           // 0x28C
    bool m_readyGo;                   // 0x2A0
    bool m_rePlay;                    // 0x2A1
    bool m_eventEnd;                  // 0x2A2
    char _2A3;
    float m_cameraSandBagY;                   // 0x2A4
    Vec3f m_posGround[12];            // 0x2A8
    Vec3f m_posFloor[12];             // 0x338
    Vec3f m_posSky[4];                // 0x3C8
    Vec3f m_posScore[6];              // 0x3F8
    Vec3f m_posNumber[24];            // 0x440
    Vec3f m_posLimit[2];              // 0x560
    Vec3f m_zeroPos;                  // 0x578: where the sandbag was launched
    float m_scroll;                   // 0x584
    float m_scrollPrev;               // 0x588
    float m_sandBagX;                 // 0x58C
    float m_scrollRate;               // 0x590
    u32 m_score[6];                   // 0x594
    float m_dist;                     // 0x5AC
    float m_distPrev;                 // 0x5B0
    float m_scoreBest[2];             // 0x5B4
    float m_scoreBestCombi;           // 0x5BC
    u8 m_barrierState;                // 0x5C0
    char _5C1[3];
    float m_barrierHp;                // 0x5C4
    u32 m_taskIdSandBag;              // 0x5C8
    u32 m_taskIdBat;                  // 0x5CC
    float m_sandBagSpeed;             // 0x5D0
    u8 m_sandBagLanded;               // 0x5D4
    u8 m_figurePhase;                 // 0x5D5
    u8 m_figureRound;                 // 0x5D6
    char _5D7;
    float m_figureX[3];               // 0x5D8
    int m_figureId[3];                // 0x5E4
    gfArchive m_sandBagBrres;         // 0x5F0
    gfArchive m_sandBagParam;         // 0x670
    gfArchive m_batBrres;             // 0x6F0
    gfArchive m_batParam;             // 0x770
    int m_sound[6];                   // 0x7F0
    stTrigger* m_beltTrigger;         // 0x808
    grGimmickBeltConveyorData* m_beltData; // 0x80C

public:
    stHomerun();
    static stHomerun* create();

    virtual ~stHomerun();
    virtual void createObj();
    virtual bool loading();
    virtual void getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID, gfArchive** commonParam = NULL,
                            itCustomizerInterface** customizer = NULL);
    virtual void processFixPosition();
    virtual bool isEnd();
    virtual bool isEventEnd(int param1, int* eventState, int* eventDecision);
    virtual void update(float deltaFrame);

    virtual void createObjMainBg(int index);
    virtual void createObjLoop(int index);
    virtual void createObjScore(int index);
    virtual void createObjNumber(int index);
    virtual void createObjBarrier(int index);
    virtual void createObjFloor();
    virtual void createObjFloor1(u8 index, u8 type);
    virtual void createObjBeltConv();
    virtual void updateSandBag(float deltaFrame);
    virtual void updateSandBagDmg(float deltaFrame);
    virtual void updateCamera(float deltaFrame);
    virtual void updateDist(float deltaFrame);
    virtual void updateFigure(float deltaFrame);
    virtual void updateLimit(float deltaFrame);
    virtual void updateBeltConv(float deltaFrame);
    virtual void fixposSandBag();
    virtual void fixposFigure();
    virtual void fixposCamera();
    virtual void fixposPauseCamera();
    virtual bool isRePlay();
    virtual bool isFly();
    virtual bool isSingle();
    virtual bool isCombi();
    virtual bool isVs();
    virtual bool isVsEnd();
    virtual void setReadyGo(bool readyGo);
    virtual void setRePlay(bool rePlay);
    virtual float getScore();
    virtual u32 getScoreU32();
    virtual void setScoreBest(u8 index, float score);
    virtual void setScoreBestCombi(float score);
    virtual void setItemSandBag(BaseItem* sandBag);
    virtual void entryFigure(u32 instanceId, int itemKind, int variation);
    virtual void interpolateSub(Vec3f* out, stHomerunKey3* key);
    virtual void interpolateSub1(float* out, stHomerunKey1* key);
    virtual void calcCameraParam(float t, Vec3f* out, Vec3f* eye, Vec3f* angle);

    static stClassInfoImpl<Stages::HomeRunContest, stHomerun> bss_loc_14;
};
static_assert(sizeof(stHomerun) == 0x810, "Class is wrong size!");
