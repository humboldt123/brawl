#pragma once

// Local BrawlHeaders shadow (supersedes include/Brawl/Include/st/st_melee.h): names the stMelee members its
// constructor, destructor and accessors use (sora_melee st/st_melee.o, 0x238BDC-0x23A164) and declares the
// position helpers from the map.

#include <StaticAssert.h>
#include <st/st_common_gimmick.h>
#include <types.h>

class stCollisionWork {
    grCollData::VtxData* m_vtxDatas;
    grCollData::LineData* m_lineDatas;
    grCollData::JointData* m_jointDatas;

public:
    u16 m_vtxLen;
    bool m_isClosed;
    char _0xf;

    void initialize();
};
static_assert(sizeof(stCollisionWork) == 0x10, "Class is wrong size!");

class stMelee : public stCommonGimmick {
protected:
    // 1B4: the start position each player uses (shuffled by the constructor in some modes)
    int m_playerPositionIndex[4];
    stTrigger* m_wind2ndTrigger;         // +0x1C4
    grGimmickWindData2nd* m_wind2ndData; // +0x1C8
    // 1CC: the restart point picked last for index 30 (0xFFFF until the first pick)
    u32 m_lastReStartPosIndex;
    // 1D0: the camera rotation factors zoomOutCamera saves and zoomInCamera restores
    float m_savedVerticalRotationFactor;
    float m_savedHorizontalRotationFactor;

public:
    stMelee(char const* name, srStageKind stageKind);
    virtual void processBegin();
    virtual void processUpdate();
    virtual void getItemGenPac(gfArchive** archive);
    virtual void getFighterStartPos(Vec3f* startPos, int fighterIndex);
    virtual void getFighterReStartPos(Vec3f* startPos, int fighterIndex);
    virtual bool createWind2ndOnly();
    virtual grGimmickWindData2nd* getWind2ndOnlyData() { return m_wind2ndData; }
    virtual void updateWind2ndOnly();
    virtual bool isReStartSamePoint() { return true; }
    virtual void update(float deltaFrame);
    virtual void setCameraLimitRange(float unk1, float unk2, float unk3, float unk4); // TODO
    virtual void resetCameraLimitRange();                                             // TODO
    virtual bool checkChangeScene();                                                  // TODO
    virtual void resetChangeScene();                                                  // TODO
    virtual void setChangeSceneNumber(s32 n);                                         // TODO
    ~stMelee();

    void zoomInCamera();
    void zoomOutCamera(float verticalRotationFactor, float horizontalRotationFactor);
    void setPlayerPositionIndexSerial();
    void setPlayerPositionIndexFixed(int index);
    int playSeBasic(SndID id, float pan);
    void stopSeBasic(int handle, float fadeFrames);
    void setSePan(int handle, float pan);
    void setGravityHalf();
    void setGravityNormal();
    grCollision* createCollisionSelf(stCollisionWork* collisionWork, Ground* ground, const char*, const char*, u16);
    void initCollData_fromCollWork(grCollData* collData, stCollisionWork* collisionWork, const char*, const char*, u16);
    static bool getPlayerPositionCore(int playerNo, Vec3f* pos, int kind);
    bool getPlayerPositionEntryId(int entryId, Vec3f* pos);
    static bool getPlayerPosition(int playerNo, Vec3f* pos);
    static bool getPlayerHipPosition(int playerNo, Vec3f* pos);
    static bool getPlayerCursorPosition(int playerNo, Vec3f* pos);
};
static_assert(sizeof(stMelee) == 472, "Class is wrong size!");
