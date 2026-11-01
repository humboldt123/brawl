#pragma once

// SHADOW of BrawlHeaders gr/ground.h (-I include comes first): names the tail members read from the Ground unit
// (sora_melee gr/ground.cpp: constructor, startup, setVisibility, renderPre) and declares the unit's helpers.

#include <GX.h>

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_model.h>
#include <gf/gf_task.h>
#include <gr/collision/gr_collision_status.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <gf/gf_3d_scene.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <so/so_array.h>
#include <st/st_trigger.h>
#include <types.h>

class Stage;

class Ground : public gfTask {
public:
    enum CollCategory
    {
        Coll_Category_Fighter = 0x0,
        Coll_Category_Enemy = 0x1,
        Coll_Category_Item = 0x2,
    };
    typedef u32 CollCategoryMask;
    static const CollCategoryMask COLL_CATEGORY_MASK_FIGHTER = 1 << Coll_Category_Fighter;
    static const CollCategoryMask COLL_CATEGORY_MASK_ENEMY = 1 << Coll_Category_Enemy;
    static const CollCategoryMask COLL_CATEGORY_MASK_ITEM = 1 << Coll_Category_Item;
    static const CollCategoryMask COLL_CATEGORY_MASK_ALL = COLL_CATEGORY_MASK_FIGHTER | COLL_CATEGORY_MASK_ENEMY | COLL_CATEGORY_MASK_ITEM;

    struct CollCategoryFlag {
        union {
            struct {
                u32 : 29;
                bool m_item : 1;
                bool m_enemy : 1;
                bool m_fighter : 1;
            };
            CollCategoryMask m_mask;
        };
        inline CollCategoryFlag() {}
        inline CollCategoryFlag(CollCategoryMask bits) : m_mask(bits) {}
    };

    // 0
    nw4r::g3d::ResFile m_resFile;
    // 4
    nw4r::g3d::ScnMdl** m_sceneModels; // TODO: supposed to be a wrapper of some kind
    // 8
    gfModelAnimation** m_modelAnims;
    // C
    grCollision* m_collision;
    // 10
    Ground* m_nextGround;
    // 14
    char _spacer2[0x4];
    // 18
    u32 m_modelAnimNum;
    // 1C
    s16 m_mdlIndex; // -1: no model
    // 1E
    char _spacer3[2];
    // 20
    void* m_stageData;
    // 24
    float m_motionRatio;
    // 28
    float unk68; // HYPOTHESIS: a frame/ratio the constructor zeroes; nothing in gr/ground.cpp reads it

    // 2c: one-bit fields (not bool: the u32 setters insert their argument's low bit directly)
    u8 m_isVisible : 1;
    u8 m_isVisibleByClipping : 1;
    bool m_isEnableCollisionStatus : 1; // bool: stage code compares it with a bool (gr_metalgear_attack)
    u8 m_isEnableCollisionStatusByClipping : 1;
    bool m_isEnableCalcCollision : 1; // bool: the stage units' inline isEnableCalcCollision copies return it as is
    u8 m_hasBindedData : 1;
    u8 m_hasUpdatedG3dCalcWorld : 1;
    u8 m_isUseMdlNameForStartAnim : 1;
    // 2d
    u8 m_isCopyVisibility : 1;
    u8 m_isValid : 1;
    u8 m_noUpdateAnim : 1;
    u8 m_isPauseAnim : 1;
    u8 m_isUpdateG3dCalcWorld : 1;
    u8 m_isGr2 : 1;
    u8 m_isEnableBlendColor : 1;
    u8 m_isSetup : 1;
    // 2e
    char _spacer[2];
    // 30
    HeapType m_heapType;
    // 34: the first scene model's ScnObj options 1..9 (startup saves them, setVisibility restores them); the
    // constructor's defaults are 1 for options 1..8 and 0 for option 9.
    u32 m_scnObjOptions[9];
    // 58: HYPOTHESIS: the blend colour renderPre writes into every material's TEV colour 3 / constant colour 0.
    GXColor m_blendColor;

    Ground(const char* taskName);
    virtual void processAnim();
    virtual void processUpdate();
    virtual void processGameProc();
    virtual void renderPre();
    virtual void renderDebug();
    virtual ~Ground();

    virtual void update(float deltaFrame);
    virtual void loadGroundData(nw4r::g3d::ResFile, u32 unk2, gfSceneRoot::LayerType unk3);
    virtual void bindData(gfArchive* data);
    virtual void setupMelee();
    virtual void setVisibility(u32 unk1);
    virtual void setVisibilityByClipping(u32 unk1);
    virtual void setVisibilityAttachedEffect(u32 unk1);
    virtual void receiveCollMsg(int direction, grCollStatus* collStatus, grCollisionJoint* collisionJoint);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void receiveCollMsg_Wall(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void receiveCollMsg_Attack(grCollStatus* collStatus, grCollisionJoint* collisionJoint);
    virtual void unloadData();
    virtual int getModelCount() {
        if (m_resFile.ptr()) {
            return m_resFile.GetResMdlNumEntries();
        }
        return 0;
    };
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual bool setNode();
    virtual void setStageData(void* stageData) { m_stageData = stageData; }
    virtual void* getStageData() { return m_stageData; }
    virtual void initStageData() { }
    virtual void setMdlIndex(int mdlIndex) { m_mdlIndex = mdlIndex; }
    virtual short getMdlIndex() { return m_mdlIndex; }
    virtual bool isEnableCalcCollision() { return m_isEnableCalcCollision; }
    virtual void enableCalcCollision() { m_isEnableCalcCollision = 1; }
    virtual void disableCalcCollision() { m_isEnableCalcCollision = 0; }
    virtual u32 getNodeIndex(u32 sceneModelIndex, const char* nodeName);
    virtual bool getNodePosition(Vec3f* nodePos, u32 sceneModelIndex, u32 nodeIndex);
    virtual bool getNodePosition(Vec3f* nodePos, u32 sceneModelIndex, const char* nodeName);
    virtual bool getNodeMatrix(Matrix* nodeMtx, u32 sceneModelIndex, u32 nodeIndex);
    virtual bool getNodeMatrix(Matrix* nodeMtx, u32 sceneModelIndex, const char* nodeName);
    virtual bool setNodeVisibility(bool setVisibility, u32 sceneModelIndex, u32 nodeIndex, bool recursive, bool recursive_);
    virtual bool setNodeVisibility(bool setVisibility, u32 sceneModelIndex, const char* nodeName, bool recursive, bool recursive_);
    virtual bool setNodeVisibilityAll(bool unk1, u32 sceneModelIndex);
    virtual bool isNodeVisible(u32 sceneModelIndex, u32 nodeIndex);
    virtual bool setNodeCollision(bool setCollision, u32 sceneModelIndex, u32 nodeIndex, bool recursive);
    virtual bool setNodeCollision(bool setCollision, u32 sceneModelIndex, const char* nodeName, bool recursive);
    virtual bool getNodeScale(Vec3f* nodeScale, u32 sceneModelIndex, u32 nodeIndex);
    virtual bool getNodeScale(Vec3f* nodeScale, u32 sceneModelIndex, const char* nodeName);
    virtual void setValid(u32 unk1);
    virtual void setValidAttachedEffect(u32 unk1);
    virtual void setInitializeFlag() { }
    virtual void setInitializeInfo(int initializeInfo) { }
    virtual bool getInitializeInfo(int initializeInfo) { return false; }
    virtual void setMotionRatio(float ratio);
    virtual void setMotionFrame(float frame, u32 animIndex);
    virtual float getMotionFrame(u32 anim);
    virtual void setMotionLoop(bool shouldLoop, u32 animIndex);
    virtual void setMatAlphaMul(u32 unk1, u32 sceneModelIndex); // TODO
    virtual void setMatAlpha(u32 unk1, u32 sceneModelIndex);    // TODO
    virtual void updateG3dProcCalcWorld();
    virtual void preExit();
    virtual bool adventureEventGetItem(int genParamId, stTriggerData* triggerData) { return false; }
    virtual void invalidatedByCameraClipping();
    virtual void setTransparencyFlag(char flag);

    bool getNodeIndex(u32* nodeIndex, u32 sceneModelIndex, const char* nodeName);
    void addSceneRoot(nw4r::g3d::ScnMdl* sceneModel, int unk2);
    // The original takes the category flags as an int*; the typed overload forwards to it.
    bool isCollisionStatusOwnerTask(grCollStatus* collStatus, int* categories);
    bool isCollisionStatusOwnerTask(grCollStatus* collStatus, CollCategoryFlag* categories) {
        return isCollisionStatusOwnerTask(collStatus, reinterpret_cast<int*>(categories));
    }
    bool searchNode(const char* mdlName, const char* nodeName, int* mdlIndex, u16* nodeIndex);
    void setBlendColorDisable();
    void setBlendColorEnable();
    // HYPOTHESIS: inline (Stage::setBlendColor copies the colour argument to the stack for each ground).
    void setBlendColor(GXColor color) { m_blendColor = color; }
    void setDontMoveGround();
    void setEnableCollisionStatus(bool enableColl);
    void setEnableCollisionStatusByClipping(bool enableColl);
    void setVisibilityByClipping(int unk1, u32 sceneModelIndex);
    void updateG3dProcCalcWorldForce();
};
static_assert(sizeof(Ground) == 0x9C, "Class is wrong size!");

// Size: 196
