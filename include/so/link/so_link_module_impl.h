// SHADOW of BrawlHeaders/so/link/so_link_module_impl.h: identical except for the FT_MODULE_BUILDER-guarded declarations
// (constructors/destructors of sora_melee classes that fighter module builders call, and the members the sora_enemy
// link module reads), the SO_LINK_MODULE_IMPL_INLINE_VIRTUALS opt-in of that module, and the signatures recovered from
// it (getModelConstraintFlag returns the u16 mask, adjustModelConstraintPosture(u16, Vec3f*, Vec3f*), the const event
// of sendEventNodes).
#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <so/so_array.h>
#include <so/anim/so_anim_cmd.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/color/so_color_blend.h>
#include <so/collision/so_collision_attack_part.h>
#include <so/slope/so_slope_module_impl.h>
#include <so/controller/so_controller_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_connection_server.h>
#include <so/situation/so_situation_event_presenter.h>
#include <so/so_null.h>

class StageObject;
class soModuleAccesser;
class soArticle;

class soLinkModule : public soNullable {
public:
    union ModelConstraintFlag {
        u16 m_mask;
        struct {
            bool m_setMatrix : 1;
            bool m_offsetTranslateSelf : 1;
            bool m_offsetTranslate : 1;
            bool m_noUpdateLinkPos : 1;
            bool m_mtxNoScale : 1;
            bool m_offsetScale : 1;
            bool m_offsetRot : 1;
            bool m_flipRotZ : 1;
            bool m_flipRotY : 1;
            bool m_flipRotX : 1;
            bool m_noFlip : 1;
            bool m_oneNode : 1;
            bool m_mtx : 1;
            bool m_orientationPostureRot : 1;
            bool m_orientation : 1;
            bool m_position : 1;
        };
        inline ModelConstraintFlag() {}
        inline ModelConstraintFlag(u16 mask) : m_mask(mask) {}
    };

    soLinkModule() { }
    virtual ~soLinkModule() { };
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual bool link(u32 index, int taskId) = 0;
    virtual void unlinkAll() = 0;
    virtual void unlinkNode(u32 index) = 0;
    virtual void unlinkNodeAll() = 0;
    virtual void unlink(u32 index) = 0;
    virtual bool isLink(int index) = 0;
    virtual bool isLinked(int index) = 0;
    virtual StageObject *getParent(u32 index) = 0;
    virtual StageObject *searchParent(int taskId) = 0;
    virtual StageObject* searchParentAttr(soLinkConnection::Attribute ) = 0;
    virtual StageObject *searchNode(int taskId) = 0;
    virtual void sendEventParents(s32 p1, soLinkEventArgs& p2) = 0;
    // HYPOTHESIS: const. The sora_enemy link module passes a temporary event (it hands the constructor's result to the
    // call), which only binds to a const reference; nothing else in the tree calls this overload.
    virtual void sendEventNodes(s32, const soLinkEventArgs& args, u32 attributeMask) = 0;
    virtual void sendEventNodes(s32, int taskId, soLinkEventArgs &) = 0;
    virtual StageObject *getNode(u8) = 0;
    virtual int getNodeTaskId(u8) = 0;
    virtual bool isNodeDamageReaction(u8) = 0;
    virtual bool isNodeDamageCaptureCut(u8) = 0;
    virtual s8 getParentModelLoupeNo(u32 index) = 0;
    virtual bool chkLinkStop() = 0;
    virtual void toLinkStop() = 0;
    virtual void leaveLinkStop() = 0;
    virtual bool isValidLinkStop() = 0;
    virtual bool chkLinkShake(Vec2f*) = 0;
    virtual soColorBlend *getLinkColorBlend() = 0;
    virtual s8 getLinkLightSet() = 0;
    virtual bool chkLinkVisibility() = 0;
    virtual soController* getLinkController() = 0;
    virtual float getLinkScale() = 0;
    virtual float getParentScale(u32 index) = 0;
    virtual void adjustLinkGroundCollision(int, float*) = 0;
    virtual void setConstraint2ndWind(u8) = 0;
    virtual Vec3f getParentModelNodeGlobalPosition(u32 index, u32 correctNodeId, bool) = 0;
    virtual Vec3f getParentModelNodeGlobalPosition(u32 index, const char *nodeName, bool) = 0;
    virtual float getParentMotionFrame(u32 index) = 0;
    virtual SituationKind getParentSituationKind(u32 index) = 0;
    virtual float getParentLr(u32 index) = 0;
    virtual Vec2f getParentSumSpeed(u32 index) = 0;
    virtual int getParentTaskId(u32 index) = 0;
    virtual Vec3f getParentRot(u32 index) = 0;
    virtual float getParentDamage(u32 index) = 0;
    virtual float getParentDamageReaction(u32 index) = 0;
    virtual float getParentDamagePowerMax(u32 index) = 0;
    virtual void setNodeScalePowerMul(float) = 0;
    virtual bool isParentDamageReaction(u32 index) = 0;
    virtual bool isParentDamageCatchCut(u32 index) = 0;
    virtual int getParentTotalHitStatus(u32 index) = 0;
    virtual nw4r::g3d::ResFile getParentAnmFile(u32 index, u16 fileIndex) = 0;
    virtual bool isValidParentShape(u32 index) = 0;
    virtual Vec2f getParentShapeCenterPos(u32 index) = 0;
    virtual bool getParentShapeCenterPosTaskId(int taskId, Vec2f *outCenterPos) = 0;
    virtual float getOwnerAttackPowerMulArticle(u32 index) = 0;
    virtual bool isParentEnableEnvMap(u32 index) = 0;
    virtual bool isParentRefFB(u32 index) = 0;
    virtual int getParentTexResourceId(u32 index) = 0;
    virtual bool isParentEffectSyncScale(u32 index) = 0;
    virtual void sendEnvMapNodes(bool) = 0;
    virtual bool isParentGlow(u32 index) = 0;
    virtual int getParentGlowAttack(u32 index, soCollisionAttackData*) = 0;
    virtual int getParentWeaponGlowAttack(u32 index) = 0;
    virtual int getParentWeaponGlowDefense(u32 index) = 0;
    virtual int getParentGlowAttackAdd(soCollisionAttackData*) = 0;
    virtual float getParentWeaponGlowEff(u32 index) = 0;
    virtual float getParentWeaponGlowDamageMin(u32 index) = 0;
    virtual float getParentWeaponGlowDamageMax(u32 index) = 0;
    virtual void setParentHitStopFrame(u32, u32, bool) = 0;
    virtual void setParentAttackerInfo(u32 index, StageObject*) = 0;
    virtual u8 getParentAttachItemVisibility(u32 index) = 0;
    virtual bool isParentEnableSubColor(u32 index) = 0;
    virtual GXColor getParentSubColor(u32 index) = 0;
    virtual bool correctParent2ndBoth(u32 index, Vec3f*, float) = 0;
    virtual void setModelConstraintPosOrt(u32 modelConstraintNo, int modelConstraintNode, int modelConstraintTargetNode, u16 modelConstraintFlag, bool
                                          modelConstraintAttribute) = 0;
    virtual void setModelConstraintPosOrt(u32 modelConstraintNo, int modelConstraintNode, const char *modelConstraintTargetNode, u16 modelConstraintFlag, bool
                                          modelConstraintAttribute) = 0;
    virtual void setModelConstraintAttribute(u32 index, bool) = 0;
    // (The flag mask is returned as a halfword in r3, not as the union.)
    virtual u16 getModelConstraintFlag() = 0;
    virtual void setModelConstraintFlag(u16) = 0;
    virtual void offModelConstraintFlag(u16) = 0;
    virtual int getModelConstraintNode() = 0;
    virtual void setModelConstraintNode(int) = 0;
    virtual void setModelConstraintTargetNode(int) = 0;
    virtual u32 getModelConstraintNo() = 0;
    // The callers (Enemy::notifyEventLink, ftStatusUniqProcessTread) pass two Vec3f.
    // HYPOTHESIS: a rotation and a position.
    virtual void adjustModelConstraintPosture(Vec3f* rot, Vec3f* pos) = 0;
    virtual void adjustModelConstraintPosture(u16, Vec3f* rot, Vec3f* pos) = 0;
    virtual void removeModelConstraint(bool) = 0;
    virtual bool isModelConstraint() = 0;
    virtual bool isModelConstraintMutual() = 0;
    virtual void getModelConstraintTargetNodeWorldMtx(Matrix*, bool) = 0;
    virtual Vec3f getModelConstraintTargetNodePosition() = 0;
    virtual Vec3f getModelConstraintNodeGlobalPosition(u32 nodeIndex) = 0;
    virtual Vec3f getModelConstraintNodeGlobalPositionRecursive(u32 nodeIndex) = 0;
    virtual Vec3f getModelConstraintTargetNodeRotation() = 0;
    virtual bool getNodeRotation(u32 index, u32 nodeIndex, Vec3f *rotation) = 0;
    virtual float getConstraintModelScale() = 0;
    virtual bool setUpModelConstraintTargetMtx() = 0;
    virtual void calcModelConstraintNodeMtx(Matrix*) = 0;
    virtual void setAttribute(u32 index, soLinkConnection::Attribute, bool) = 0;
    virtual bool chkAttribute(int index, soLinkConnection::Attribute) = 0;
    virtual bool chkLinkedAttribute(soLinkConnection::Attribute) = 0;
    virtual nw4r::g3d::ResFile getAnmFile(u16 fileIndex) = 0;
    virtual void setAnmFileTransactorLinkNo(int) = 0;
    virtual soSlopeStatusParam::Status getLinkSlopeStatus(int index) = 0;
    virtual bool getLinkSlopeRotation(int index, Vec3f*) = 0;
    virtual soArticle *generateArticleLinkParents(u32 connectionIndex, int id, soModuleAccesser*) = 0;
    virtual void getCmdAddressPackParents(u32 linkConnectionIndex, soArray<soAnimCmdAddressPack>*, int motionKind, short*) = 0;
    virtual bool isLinkParentSlow(int index) = 0;
    virtual bool getLinkParentSlowInfo(float *rate, bool *isSkip, int index) = 0;
    virtual void getLinkParentCameraInfo(u32 index, u8* playerNo, u8* pri, u8* advPri) = 0;
    virtual bool isNodeGroundCrush(float) = 0;
};

class soLinkModuleImpl : public soLinkModule, public soAnimCmdEventObserver {
#ifdef FT_MODULE_BUILDER
public:
    soLinkModuleImpl(soModuleAccesser* moduleAccesser, soArray<soLinkConnection>* connections);
private:
#endif
public:
    soLinkModuleImpl(short unitID) : soAnimCmdEventObserver(unitID) { }
    virtual ~soLinkModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual bool link(u32 index, int taskId);
    virtual void unlinkAll();
    virtual void unlinkNode(u32 index);
    virtual void unlinkNodeAll();
    virtual void unlink(u32 index);
    virtual bool isLink(int index);
    virtual bool isLinked(int index);
    virtual StageObject *getParent(u32 index);
    virtual StageObject *searchParent(int taskId);
    virtual StageObject* searchParentAttr(soLinkConnection::Attribute );
    virtual StageObject *searchNode(int taskId);
    virtual void sendEventParents(s32 p1, soLinkEventArgs& p2);
    virtual void sendEventNodes(s32, const soLinkEventArgs& args, u32 attributeMask);
    virtual void sendEventNodes(s32, int taskId, soLinkEventArgs &);
    virtual StageObject *getNode(u8);
    virtual int getNodeTaskId(u8);
    virtual bool isNodeDamageReaction(u8);
    virtual bool isNodeDamageCaptureCut(u8);
    virtual s8 getParentModelLoupeNo(u32 index);
    virtual bool chkLinkStop();
    virtual void toLinkStop();
    virtual void leaveLinkStop();
    virtual bool isValidLinkStop();
    virtual bool chkLinkShake(Vec2f*);
    virtual soColorBlend *getLinkColorBlend();
    virtual s8 getLinkLightSet();
    virtual bool chkLinkVisibility();
    virtual soController* getLinkController();
    virtual float getLinkScale();
    virtual float getParentScale(u32 index);
    virtual void adjustLinkGroundCollision(int, float*);
    virtual void setConstraint2ndWind(u8);
    virtual Vec3f getParentModelNodeGlobalPosition(u32 index, u32 correctNodeId, bool);
    virtual Vec3f getParentModelNodeGlobalPosition(u32 index, const char *nodeName, bool);
    virtual float getParentMotionFrame(u32 index);
    virtual SituationKind getParentSituationKind(u32 index);
    virtual float getParentLr(u32 index);
    virtual Vec2f getParentSumSpeed(u32 index);
    virtual int getParentTaskId(u32 index);
    virtual Vec3f getParentRot(u32 index);
    virtual float getParentDamage(u32 index);
    virtual float getParentDamageReaction(u32 index);
    virtual float getParentDamagePowerMax(u32 index);
    virtual void setNodeScalePowerMul(float);
    virtual bool isParentDamageReaction(u32 index);
    virtual bool isParentDamageCatchCut(u32 index);
    virtual int getParentTotalHitStatus(u32 index);
    virtual nw4r::g3d::ResFile getParentAnmFile(u32 index, u16 fileIndex);
    virtual bool isValidParentShape(u32 index);
    virtual Vec2f getParentShapeCenterPos(u32 index);
    virtual bool getParentShapeCenterPosTaskId(int taskId, Vec2f *outCenterPos);
    virtual float getOwnerAttackPowerMulArticle(u32 index);
    virtual bool isParentEnableEnvMap(u32 index);
    virtual bool isParentRefFB(u32 index);
    virtual int getParentTexResourceId(u32 index);
    virtual bool isParentEffectSyncScale(u32 index);
    virtual void sendEnvMapNodes(bool);
    virtual bool isParentGlow(u32 index);
    virtual int getParentGlowAttack(u32 index, soCollisionAttackData*);
    virtual int getParentWeaponGlowAttack(u32 index);
    virtual int getParentWeaponGlowDefense(u32 index);
    virtual int getParentGlowAttackAdd(soCollisionAttackData*);
    virtual float getParentWeaponGlowEff(u32 index);
    virtual float getParentWeaponGlowDamageMin(u32 index);
    virtual float getParentWeaponGlowDamageMax(u32 index);
    virtual void setParentHitStopFrame(u32, u32, bool);
    virtual void setParentAttackerInfo(u32 index, StageObject*);
    virtual u8 getParentAttachItemVisibility(u32 index);
    virtual bool isParentEnableSubColor(u32 index);
    virtual GXColor getParentSubColor(u32 index);
    virtual bool correctParent2ndBoth(u32 index, Vec3f*, float);
    virtual void setModelConstraintPosOrt(u32 modelConstraintNo, int modelConstraintNode, int modelConstraintTargetNode, u16 modelConstraintFlag, bool
                                          modelConstraintAttribute);
    virtual void setModelConstraintPosOrt(u32 modelConstraintNo, int modelConstraintNode, const char *modelConstraintTargetNode, u16 modelConstraintFlag, bool
                                          modelConstraintAttribute);
    virtual void setModelConstraintAttribute(u32 index, bool);
#ifdef SO_LINK_MODULE_IMPL_INLINE_VIRTUALS
#ifndef FT_MODULE_BUILDER
#error "SO_LINK_MODULE_IMPL_INLINE_VIRTUALS needs the FT_MODULE_BUILDER layout of soLinkModuleImpl"
#endif
    // Opt-in of the sora_enemy link module (em_link_module_impl.cpp), whose original has weak copies of these
    // accessors (its vtable references them). The fighter units call sora_melee's and must not get the copies.
    virtual u16 getModelConstraintFlag() { return m_modelConstraintFlag.m_mask; }
    virtual void setModelConstraintFlag(u16 flag) { m_modelConstraintFlag.m_mask = flag; }
    virtual void offModelConstraintFlag(u16 flag) { m_modelConstraintFlag.m_mask &= ~flag; }
    virtual int getModelConstraintNode() { return m_modelConstraintNode; }
    virtual void setModelConstraintNode(int);
    virtual void setModelConstraintTargetNode(int);
    virtual u32 getModelConstraintNo() { return m_modelConstraintNo; }
    virtual void adjustModelConstraintPosture(Vec3f* rot, Vec3f* pos) {
        adjustModelConstraintPosture(m_modelConstraintFlag.m_mask, rot, pos);
    }
    virtual void adjustModelConstraintPosture(u16, Vec3f* rot, Vec3f* pos);
    virtual void removeModelConstraint(bool);
    virtual bool isModelConstraint() { return m_modelConstraintNo != -1; }
    virtual bool isModelConstraintMutual() { return m_isModelConstraintMutual; }
#else
    virtual u16 getModelConstraintFlag();
    virtual void setModelConstraintFlag(u16);
    virtual void offModelConstraintFlag(u16);
    virtual int getModelConstraintNode();
    virtual void setModelConstraintNode(int);
    virtual void setModelConstraintTargetNode(int);
    virtual u32 getModelConstraintNo();
    virtual void adjustModelConstraintPosture(Vec3f* rot, Vec3f* pos);
    virtual void adjustModelConstraintPosture(u16, Vec3f* rot, Vec3f* pos);
    virtual void removeModelConstraint(bool);
    virtual bool isModelConstraint();
    virtual bool isModelConstraintMutual();
#endif
    virtual void getModelConstraintTargetNodeWorldMtx(Matrix*, bool);
    virtual Vec3f getModelConstraintTargetNodePosition();
    virtual Vec3f getModelConstraintNodeGlobalPosition(u32 nodeIndex);
    virtual Vec3f getModelConstraintNodeGlobalPositionRecursive(u32 nodeIndex);
    virtual Vec3f getModelConstraintTargetNodeRotation();
    virtual bool getNodeRotation(u32 index, u32 nodeIndex, Vec3f *rotation);
    virtual float getConstraintModelScale();
    virtual bool setUpModelConstraintTargetMtx();
    virtual void calcModelConstraintNodeMtx(Matrix*);
    virtual void setAttribute(u32 index, soLinkConnection::Attribute, bool);
    virtual bool chkAttribute(int index, soLinkConnection::Attribute);
    virtual bool chkLinkedAttribute(soLinkConnection::Attribute);
    virtual nw4r::g3d::ResFile getAnmFile(u16 fileIndex);
    virtual void setAnmFileTransactorLinkNo(int);
    virtual soSlopeStatusParam::Status getLinkSlopeStatus(int index);
    virtual bool getLinkSlopeRotation(int index, Vec3f*);
    virtual soArticle *generateArticleLinkParents(u32 connectionIndex, int id, soModuleAccesser*);
    virtual void getCmdAddressPackParents(u32 linkConnectionIndex, soArray<soAnimCmdAddressPack>*, int motionKind, short*);
    virtual bool isLinkParentSlow(int index);
    virtual bool getLinkParentSlowInfo(float *rate, bool *isSkip, int index);
    virtual void getLinkParentCameraInfo(u32 index, u8* playerNo, u8* pri, u8* advPri);
    virtual bool isNodeGroundCrush(float);

    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual bool isObserv(char unk1);

#ifdef FT_MODULE_BUILDER
    // The members the sora_enemy link module reads (offsets from the RTTI-recovered layout; the names follow the
    // SO_LINK_MODULE_IMPL_FULL layout below). HYPOTHESIS: the builder view stops at 0x50, while the sora_melee
    // setModelConstraintTargetNode stores to 0x50 (sizeof 0x54 there); the rest is not reconstructed yet.
    u8 m_unk14[0x38 - 0x14];
    u32 m_modelConstraintNo;                    // 0x38 (-1: no model constraint)
    ModelConstraintFlag m_modelConstraintFlag;  // 0x3C
    bool m_isModelConstraintMutual;               // 0x3E
    u8 m_unk3F[0x44 - 0x3F];
    soModuleAccesser* m_moduleAccesser;         // 0x44
    u8 m_unk48[0x4C - 0x48];
    int m_modelConstraintNode;                  // 0x4C
#endif
#if defined(SO_LINK_MODULE_IMPL_FULL) && !defined(FT_MODULE_BUILDER)
    soLinkModuleImpl(soModuleAccesser* moduleAccesser, soArray<soLinkConnection>* connections);
#endif
#ifdef SO_LINK_MODULE_IMPL_FULL
    // The layout of the sora_melee link units (verified against the constructor and the accessors).
    StageObject* getOwner();
    void onUnlink(int id);

    utList m_nodes;                           // 0x14: the child ends linked to this module
    utList m_parents;                         // 0x20: the parent ends this module is linked to
    utListNode m_serverNode;                  // 0x2C: this module's entry in the connection server
    soLinkModuleImpl* m_serverNodeOwner;      // 0x34
    int m_modelConstraintNo;                  // 0x38
    ModelConstraintFlag m_modelConstraintFlag; // 0x3C
    bool m_isModelConstraintMutual;           // 0x3E
    int m_anmFileTransactorLinkNo;            // 0x40
    soModuleAccesser* m_moduleAccesser;       // 0x44
    soArray<soLinkConnection>* m_connections; // 0x48
    int m_modelConstraintNode;                // 0x4C
    int m_modelConstraintTargetNode;          // 0x50
#endif
};
#ifdef SO_LINK_MODULE_IMPL_FULL
static_assert(sizeof(soLinkModuleImpl) == 0x54, "Class is wrong size!");
#endif
