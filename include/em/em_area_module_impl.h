#pragma once

// SHADOW of BrawlHeaders em/em_area_module_impl.h (-I include comes first): the enemy area module
// (mo_enemy/sora_enemy/area/em_area_module_impl.cpp). soAreaModuleImpl (0x4C bytes, the area instances, contact logs and
// the anim cmd / status / situation observers) is a class of sora_melee; the enemy version adds the area data sets of the
// enemy kind, the "target" queries of the AI and the anim cmds of group 'i' (0x69). The class is 0x5D8 bytes and sits
// inside Enemy.
//
// A TARGET is what the AI looks at: an area index 0-7 (TargetKind), or TargetAll (8) for the writers that fill the found
// information of every target at once. The found information (setTargetFoundInfo) is a snapshot of the target's position,
// status and posture that the AI keeps while the target is out of reach.

#include <StaticAssert.h>
#include <ft/builder/ft_dol_instances.h>
#include <mt/mt_vector.h>
#include <so/area/so_area_enviroment_element_checker_impl.h>
#include <so/area/so_area_module_impl.h>
#include <so/so_array.h>
#include <so/so_module_accesser.h>
#include <types.h>

class acAnimCmd;
class Fighter;

// The contact log array is a class of sora_melee (the module only calls its constructor and destructor).
FT_DOL_ARRAY_VECTOR(soAreaContactLog, 44);

// One area of the enemy kind's parameters (emInfo::getAreaParamAryPtrGetId): the enemy areas after the first six.
// HYPOTHESIS: names; the layout comes from createAreaDataSetSubParam.
struct emAreaParam {
    s32 m_group;          // 0x00 gfArea::Group of the area (a value emAreaModuleImpl::isInvalidAreaGroup accepts)
    u8 m_shapeType;       // 0x04
    s32 m_nodeIndex;      // 0x08 model node the area follows, -1: none
    float m_offsetX;      // 0x0C
    float m_offsetY;      // 0x10
    float m_rangeX;       // 0x14
    float m_rangeXScale;  // 0x18 added to the range, times the level scale of the enemy
    float m_rangeY;       // 0x1C
    float m_rangeYScale;  // 0x20
};
static_assert(sizeof(emAreaParam) == 0x24, "Class is wrong size!");

class emAreaModuleImpl : public soAreaModuleImpl {
public:
    // The targets the AI can look at (the area index of the AI's decisions).
    // HYPOTHESIS: names; the first four come from the code that maps them to the fighter manager's entries (player 1's
    // fighter and its sub fighter, then player 2's), 4 and 5 from the create id lookups, 6 from the camera's target.
    enum TargetKind {
        TargetPlayer1 = 0,
        TargetPlayer1Sub = 1,
        TargetPlayer2 = 2,
        TargetPlayer2Sub = 3,
        TargetSelf = 4,
        TargetParent = 5,
        TargetCamera = 6,
        TargetUnk7 = 7,
        TargetAll = 8
    };

    soModuleAccesser* m_moduleAccesser;                 // 0x4C
    emAreaParam* m_areaParams;                          // 0x50, emInfo::getAreaParamAryPtrGetId
    soSet<soAreaData> m_areas;                          // 0x54, the enemy's area work (the data sets) and emInfo::getAreaNum
    // The target ranges of the enemy kind (constant id 0xA81A): the three distance limits of getTargetDistance and, as
    // the fourth float, the opening angle in degrees of the sides of getTargetSide.
    float* m_targetRanges;                              // 0x5C
    soArrayVector<soAreaContactLog, 44> m_contactLogs;  // 0x60
    soAreaEnviromentElementCheckerImpl m_envChecker;    // 0x48C
    // The found information (setTargetFoundInfo), indexed by target kind.
    // [0]: the position of each target (0x4A8), [1]: its center, the hip of a fighter / center of an enemy (0x508).
    Vec3f m_foundPos[2][8];                             // 0x4A8
    s32 m_foundStatusKind[8];                           // 0x568
    float m_foundLr[8];                                 // 0x588
    float m_foundScl[8];                                // 0x5A8
    // The two sides of the cone getTargetSide tests (the facing rotated by +/- half the opening angle).
    Vec2f m_sideLine0;                                  // 0x5C8
    Vec2f m_sideLine1;                                  // 0x5D0

    // wind is the shared null wind object of sora_melee the derived enemies pass on.
    // areaParams is an emAreaParam array, targetRanges a float array and areaWork a soAreaData array (the enemy fetches
    // the first two as constants of its parameters, and the work from the derived enemy).
    emAreaModuleImpl(soModuleAccesser* acc, int areaNum, void* areaParams, void* targetRanges,
                     soArray<soAreaInstance>* instances, void* areaWork, void* wind);
    virtual ~emAreaModuleImpl();
    virtual void activate(int ownerId);
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, soAnimCmdEventS32 unk3);

    // The area data sets and area instances of the enemy (Enemy::init).
    void createAreaDataSet();
    void createAreaInstance();
    void createAreaDataSetSubParam(u32 index, emAreaParam* param);
    void setSideLineVec();
    // Whether the contact log of this module says that target (an area index; kind: 0-7, see the cases of the code) touches.
    bool checkArea(soModuleAccesser* acc, u32 target, u32 kind);

    // The data set of an area index; NULL without data sets or past the area count.
    soAreaData* getAreaData(u32 index);
    // Fills the group dependent parts of a data set (nothing for an invalid group).
    void setAreaDataFromGroup(s32 group, soAreaData* data);
    // The fighter of a target kind 0-3, NULL when it is gone or not accessible.
    static Fighter* getTargetFighterPtr(u32 area);
    // Whether the water area (a group 13 area of this module) with the id is touching it.
    bool checkWaterArea(u32 areaId);

    static bool isInvalidAreaCommonKind(int kind);
    static u32 getAreaCommonKindToGroup(u32 kind);
    static bool isInvalidAreaGroup(int group);
    static bool isInvalidAreaTarget(u32 area);
    static u32 getAreaGroupToGroupMask(int group);
    static u32 getAreaGroupToGroupMaskLocal(int group);
    static u8 getAreaGroupToFlag(int group);
    static bool isFighterTargetKind(u32 area);

    // The queries below take the module accesser of the asking enemy and the target kind. Those with "Found" read the
    // found information instead of the live target.
    static bool isTargetFighter(soModuleAccesser* acc, u32 area);
    static bool isExistTarget(soModuleAccesser* acc, u32 area);
    // center: the center of the enemy / the hip of the fighter instead of its position.
    static Vec3f getTargetPos(soModuleAccesser* acc, u32 area, bool center);
    static s32 getTargetStatusKind(soModuleAccesser* acc, u32 area);
    static float getTargetPostureLr(soModuleAccesser* acc, u32 area);
    static float getTargetPostureScl(soModuleAccesser* acc, u32 area);

    // The angle (in frames of a rotation motion of frameNum frames) from the direction dir to the target; offset is
    // added to the target's position.
    static float getTargetAngleToMotionFrameSub(soModuleAccesser* acc, Vec3f* pos, Vec2f offset, Vec2f dir,
                                                float rotSign, float frameNum);
    static float getTargetAngleToMotionFrame(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f dir,
                                             float rotSign, float frameNum, u32 useLr);
    static s32 getTargetDistanceSub(soModuleAccesser* acc, Vec3f* pos);
    static s32 getTargetDistance(soModuleAccesser* acc, u32 area);
    // MATCH-ONLY: never_inline, the original calls it out of line (also from the anim cmds).
    static float getTargetRealDistanceSub(soModuleAccesser* acc, Vec3f* pos) __attribute__((never_inline));
    static float getTargetRealDistance(soModuleAccesser* acc, u32 area);
    static s32 getTargetDirSub(soModuleAccesser* acc, Vec3f* pos);
    static bool isTargetDirSub(soModuleAccesser* acc, s32 dir, s32 kind);
    static bool isTargetDir(soModuleAccesser* acc, u32 area, s32 kind);
    static s32 getTargetSideSub(soModuleAccesser* acc, Vec3f* pos);
    static bool isTargetSideSub(soModuleAccesser* acc, s32 side, s32 kind);
    static bool isTargetSide(soModuleAccesser* acc, u32 area, s32 kind);
    static s32 getTargetAxisSideSub(soModuleAccesser* acc, Vec3f* pos, Vec2f offset, Vec2f axis);
    static s32 getTargetAxisSide(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, u32 useLr);
    static s32 getTargetRotSpeedDirForAxisAdjustSub(soModuleAccesser* acc, Vec3f* pos, Vec2f offset, Vec2f axis);
    static s32 getTargetRotSpeedDirForAxisAdjust(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, u32 useLr);
    static float getTargetAngleFromAxisSub(soModuleAccesser* acc, Vec3f* pos, Vec2f offset, Vec2f axis);
    static float getTargetAngleFromAxis(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, u32 useLr);
    // limit before useLr: the general term unit's call sets the limit register first, which only this order gives.
    static bool isTargetAngleFromAxis(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, float limit, u32 useLr);
    static float getTargetKineticAngle(soModuleAccesser* acc, u32 area, Vec2f offset, u32 useLr);

    static void setTargetFoundPos(soModuleAccesser* acc, s32 area);
    static Vec3f getTargetFoundPos(soModuleAccesser* acc, s32 area, bool center);
    static float getTargetFoundAngleToMotionFrame(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f dir,
                                                  float rotSign, float frameNum, u32 useLr);
    static s32 getTargetFoundDistance(soModuleAccesser* acc, s32 area);
    static float getTargetFoundRealDistance(soModuleAccesser* acc, s32 area);
    static bool isTargetFoundDir(soModuleAccesser* acc, s32 area, s32 kind);
    static bool isTargetFoundSide(soModuleAccesser* acc, s32 area, s32 kind);
    static s32 getTargetFoundAxisSide(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, u32 useLr);
    static s32 getTargetFoundRotSpeedDirForAxisAdjust(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis,
                                                      u32 useLr);
    static float getTargetFoundAngleFromAxis(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, u32 useLr);
    static bool isTargetFoundAngleFromAxis(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, float limit,
                                           u32 useLr);
    static float getTargetFoundKineticAngleSub(float lr, Vec3f self, Vec3f target);
    static float getTargetFoundKineticAngle(soModuleAccesser* acc, u32 area, Vec2f offset, u32 useLr);
    static void setTargetFoundStatusKind(soModuleAccesser* acc, s32 area);
    static s32 getTargetFoundStatusKind(soModuleAccesser* acc, s32 area);
    static void setTargetFoundPostureLr(soModuleAccesser* acc, s32 area);
    static float getTargetFoundPostureLr(soModuleAccesser* acc, s32 area);
    static void setTargetFoundPostureScl(soModuleAccesser* acc, s32 area);
    static float getTargetFoundPostureScl(soModuleAccesser* acc, s32 area);
    static bool isTargetFighterDownWait(soModuleAccesser* acc, u32 area);
    static bool isTargetFighterSmashHold(soModuleAccesser* acc, u32 area);
    static bool isTargetFighterFoundDownWait(soModuleAccesser* acc, u32 area);
    static bool isTargetFighterFoundSmashHold(soModuleAccesser* acc, u32 area);
    static void setTargetFoundInfo(soModuleAccesser* acc, u32 area);
};
static_assert(sizeof(emAreaModuleImpl) == 0x5D8, "Class is wrong size!");
