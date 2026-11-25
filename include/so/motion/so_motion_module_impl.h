// SHADOW of BrawlHeaders/so/motion/so_motion_module_impl.h: identical except for the FT_MODULE_BUILDER-guarded declarations
// (constructors/destructors of sora_melee classes that fighter module builders call). Built only for fighter RELs.
#pragma once

#include <StaticAssert.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <so/anim/so_anim_chr.h>
#include <so/so_array.h>
#include <so/event/so_event_presenter.h>
#include <so/status/so_status_event_presenter.h>
#include <so/motion/so_motion_event_presenter.h>
#include <so/model/so_model_event_presenter.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/transition/so_transition_module_impl.h>
#include <types.h>
#ifdef SO_MOTION_MODULE_IMPL_UNIT
#include <so/motion/so_motion_data_accesser.h>
#include <nw4r/g3d/g3d_anmchrblend.h>
#include <so/templates/so_array_value_soPartialAnim.h>
#include <so/templates/so_array_value_soOtherAnim.h>
#endif

class soModuleAccesser;
#if defined(FT_MODULE_BUILDER) || defined(WN_EM_SIMPLE_UNIT)
struct soPartialAnim;
struct soOtherAnim;
struct soTransitionTermPack;
class soMotionAnimObjCacheModule;
class soTransitionModule;
#endif

#include <so/motion/so_motion_change_param.h>

class soMotionModule {
public:
    // TODO: verify params
    virtual ~soMotionModule();
    virtual void activate();
    virtual void deactivate();
    virtual void setData(void*);
    virtual void updateFrame(float);
    virtual void updateNode();
    virtual float getRate();
    virtual void setRate(float rate);
    virtual void setLoopFlag(bool loopFlag);
    virtual bool isLoopFlag();
    virtual void setFrame(float frame);
    virtual void setFrameSyncAnimCmd(float frame);
    virtual float getFrame();
    virtual bool isAnimResFile(int kind);
    virtual float getEndFrame(int kind);
    virtual float getEndFrame();
    virtual bool isEnd();
    virtual bool isLooped();
    virtual bool isAnimCmd(int);
    virtual char* getName(int kind, bool);
    virtual char* getName();
    virtual int getKind();
    virtual float getWeight();
    virtual float getPrevWeight();
    virtual void setWeight(float weight);
    virtual void setWeightRate(float weightRate);
    virtual bool isBlend();
    virtual void disableNode(int);
    virtual Vec3f calcNode(int, int); // HYPOTHESIS: returns the transformed node position (hidden return buffer in r3)
    virtual void calcNode(int, void*, void*);
    virtual bool changeMotionRequest(soMotionChangeParam* changeParam);
    virtual void processFixPosition();
    virtual void processFixCamera();
    virtual void add2ndAnimChr(int kind, bool, float frame, float rate);
    virtual void remove2ndAnimChr();
    virtual void set2ndRate(float rate);
    virtual float get2ndFrame();
    virtual int get2ndKind();
    virtual bool is2ndEnd();
    // HYPOTHESIS: formal argument order from native caller scheduling; the
    // implementation uses the four integer arguments for part, motion, model
    // node set and loop control, and stores the final control as one byte.
    virtual void addPartialAnimChr(int partKind, int kind, int nodeSet, bool loop, float frame, float rate, bool sync);
    virtual void removePartialAnimChr(int);
    virtual void compRemovePartialAnimChr(int);
    virtual void detachPartialAnimChr(u32, bool);
    virtual void attachPartialAnimChr(u32);
    virtual int getPartialSize();
    virtual int getPartialKind(int);
#ifdef SO_MOTION_PARTIAL_INDEX_FIRST
    virtual void setPartialRate(int part, float rate);
#else
    virtual void setPartialRate(float rate, int);
#endif
    virtual float getPartialRate(int);
#ifdef SO_MOTION_PARTIAL_INDEX_FIRST
    virtual void setPartialFrame(int part, float frame);
#else
    virtual void setPartialFrame(float frame, int);
#endif
    virtual void setPartialFrameSyncAnimCmd(float frame, int);
    virtual float getPartialFrame(int);
    virtual float getPartialEndFrame(int);
    virtual bool isEndPartial(int);
    virtual bool isLoopedPartial(int);
    virtual int getOtherAnimSize();
    virtual int getOtherAnimKind(int);
    // HYPOTHESIS: the native parameter order is (layer, motion, loop, startFrame, rate); MWCC evaluates the arguments in declared order, which fixes the integer-before-float load order.
    virtual void addOtherAnim(int, u32, bool, float, float);
    virtual void addOtherAnim(float, float, int, int, int, bool);
    virtual void removeOtherAnim(int);
    virtual void setOtherAnimRate(int, float rate);
    virtual float getOtherAnimRate(int);
    virtual void setOtherAnimFrame(float frame, int);
    virtual float getOtherAnimFrame(int);
    virtual float getOtherAnimEndFrame(int);
    virtual bool isEndOtherAnim(int);
    virtual bool isLoopedOtherAnim(int);
    virtual Vec3f getTransNTranslateEndFrame(int kind); // MATCH-ONLY: the SDK declares (void*, int) with a float result; the binary returns the Vec3f through the hidden pointer
    virtual Vec3f getTransNTranslate();
    virtual void updateTransMoveSpeed();
    virtual bool isTransMove(); // HYPOTHESIS: takes no argument (the native Pikmin caller passes none)
    virtual Vec3f getTransMoveSpeed();
    virtual Vec3f get2ndTransNTranslate();
    virtual void update2ndTransMoveSpeed();
    virtual bool is2ndTransMove();
    virtual Vec3f get2ndTransMoveSpeed();
    virtual Vec3f getBlendTransMoveSpeed();
    virtual int getNum();
#ifdef FT_FIGHTER_FIELDS
    virtual void setShareAdjustNode(int, float);
#else
    virtual void setShareAdjustNode(float, void*);
#endif
    virtual void setPartialCancelNodeData(void*);
    virtual void enablePartialCancelNode(bool);
    virtual Vec3f getNodeRotate(int nodeId);
    virtual Vec3f getNodeTranslate(int nodeId, bool);
    virtual Vec3f getNodeTranslate(float, int kind, int nodeId, bool);
    virtual void begin();
    virtual void rewind();
    virtual void turnBack();
    virtual bool checkTransition(int);
    virtual void addDefaultTransitionTerm(void*, int numTerms);
    virtual void addTransitionTerm(void*, int numTerms, int);
    virtual void setShareCancelNode(int, int);
    virtual void setOnlyChr(bool);
    virtual void* getLastMotionTransitionInfo();
    virtual void cacheAnimChrForce(int kind);
    virtual void setAnimObjCacheModule(void*);
    virtual void setCheckTexBind(); // TODO
    virtual void* detachAnmObjChrBlend();
    virtual void setAnmObjChrBlend(void*);
    virtual void setResourceId(int, int);
    virtual void clearResourceId();
    virtual void setKindOffset(int kindOffset);
    virtual void setShareAnimObjKind(int shareAnimObjKind);
    virtual void setShareAnimObjPartialInfo(int, int);
    virtual void setNoComp(bool);
    virtual void setTransMoveSpeedNoScale(bool);
    virtual bool isProcessFixPositionDone();
    virtual bool isForceImmediatelyStatusInterpret();
    virtual void changeMotion(soMotionChangeParam* changeParam);
};
static_assert(sizeof(soMotionModule) == 4, "Class is wrong size!");

class soMotionModuleImpl : public soMotionModule, public soStatusEventObserver, public soAnimCmdEventObserver, public soModelEventObserver, public soEventPresenter<soMotionEventObserver> {
#if defined(FT_MODULE_BUILDER) || defined(WN_EM_SIMPLE_UNIT)
public:
    // WN_EM_SIMPLE_UNIT: the enemy weapon's motion builder calls the sora_melee constructor.
    soMotionModuleImpl(soModuleAccesser* acc, soArrayFixed<const soMotionData>* motionData, soTransitionModule* transition, soArray<soPartialAnim>* partialAnims, soArray<soOtherAnim>* otherAnims, soArray<soTransitionTermPack>* termPacks, soArray<u32>* u32s, s16 unitId, bool b, soMotionAnimObjCacheModule* animObjCache);
private:
#endif
public:

#ifdef SO_MOTION_MODULE_IMPL_UNIT
    // setData/getName address the accesser at 52, including its kind offset.
    soMotionDataAccesser m_dataAccesser;
#else
    // 52
    char _spacer1[4];

    // 56
    int m_kindOffset;
#endif

    // 60
    soAnimChr m_mainAnim;

    // 88
    int m_kind;

    // 92
    float m_weight;

    // 96
    float m_prevWeight;

    // 100
#ifdef SO_MOTION_MODULE_IMPL_UNIT
    float m_saveWeight;
#else
    char _100[4];
#endif

    // 104
    float m_weightRate;

    // 108
    soAnimChr m_secondaryAnim;

    // 136
    int m_secondaryKind;

    // 140
#ifdef SO_MOTION_MODULE_IMPL_UNIT
    soArray<soPartialAnim>* m_partialAnims;
    soArray<soOtherAnim>* m_otherAnims;
    nw4r::g3d::AnmObjChrBlend* m_anmObjChrBlend;
    Vec3f m_transNTranslate;
    Vec3f m_transMoveSpeed;
    Vec3f m_secondaryTransNTranslate;
    Vec3f m_secondaryTransMoveSpeed;
    soAnimObj m_otherAnim;
#else
    char _spacer3[100];
#endif

    // 240
    soTransitionModule* m_transitionModule;

    // 244
    soArrayVector<soMotionChangeParam, 4> m_motionChangeParamArray;

    // 320
#ifdef SO_MOTION_MODULE_IMPL_UNIT
    char unk320[24];
    int m_shareAnimObjKind;
    int m_shareAnimObjPartialKind;
    int m_shareAnimObjPartialSlot;
    u8 m_transMove : 1;
    u8 unk356_1 : 1;
    u8 unk356_2 : 1;
    u8 unk356_3 : 1;
    u8 unk356_4 : 1;
    u8 m_secondaryTransMove : 1;
    u8 unk356_6 : 2;
    u8 unk357_0 : 2;
    u8 unk357_2 : 2;
    u8 unk357_4 : 2;
    u8 unk357_6 : 1;
    u8 unk357_7 : 1;
    char unk358[2];
#else
    char _spacer4[40];
#endif

    // 360
    soModuleAccesser* m_moduleAccesser;

    // 364
    char _spacer5[4];

#ifdef SO_MOTION_MODULE_IMPL_UNIT
    // HYPOTHESIS: observed register arguments; the command attribute is passed by reference.
    void syncAnimCmdInterpreterMain(int kind, int group, const soAttributeFlag& attr, bool, float frame);
    void bindResFile(nw4r::g3d::ResFile* resFile);
    void removeAnimChr(bool resetFlag);
#endif
    // TODO: verify params
    virtual ~soMotionModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual void setData(void*);
    virtual void updateFrame(float);
    virtual void updateNode();
    virtual float getRate();
    virtual void setRate(float rate);
    virtual void setLoopFlag(bool loopFlag);
    virtual bool isLoopFlag();
    virtual void setFrame(float frame);
    virtual void setFrameSyncAnimCmd(float frame);
    virtual float getFrame();
    virtual bool isAnimResFile(int kind);
    virtual float getEndFrame(int kind);
    virtual float getEndFrame();
    virtual bool isEnd();
    virtual bool isLooped();
    virtual bool isAnimCmd(int);
    virtual char* getName(int kind, bool);
    virtual char* getName();
    virtual int getKind();
    virtual float getWeight();
    virtual float getPrevWeight();
    virtual void setWeight(float weight);
    virtual void setWeightRate(float weightRate);
    virtual bool isBlend();
    virtual void disableNode(int);
    virtual Vec3f calcNode(int, int); // HYPOTHESIS: returns the transformed node position (hidden return buffer in r3)
    virtual void calcNode(int, void*, void*);
    virtual bool changeMotionRequest(soMotionChangeParam* changeParam);
    virtual void processFixPosition();
    virtual void processFixCamera();
    virtual void add2ndAnimChr(int kind, bool, float frame, float rate);
    virtual void remove2ndAnimChr();
    virtual void set2ndRate(float rate);
    virtual float get2ndFrame();
    virtual int get2ndKind();
    virtual bool is2ndEnd();
    // HYPOTHESIS: formal argument order from native caller scheduling; the
    // implementation uses the four integer arguments for part, motion, model
    // node set and loop control, and stores the final control as one byte.
    virtual void addPartialAnimChr(int partKind, int kind, int nodeSet, bool loop, float frame, float rate, bool sync);
    virtual void removePartialAnimChr(int);
    virtual void compRemovePartialAnimChr(int);
    virtual void detachPartialAnimChr(u32, bool);
    virtual void attachPartialAnimChr(u32);
    virtual int getPartialSize();
    virtual int getPartialKind(int);
#ifdef SO_MOTION_PARTIAL_INDEX_FIRST
    virtual void setPartialRate(int part, float rate);
#else
    virtual void setPartialRate(float rate, int);
#endif
    virtual float getPartialRate(int);
#ifdef SO_MOTION_PARTIAL_INDEX_FIRST
    virtual void setPartialFrame(int part, float frame);
#else
    virtual void setPartialFrame(float frame, int);
#endif
    virtual void setPartialFrameSyncAnimCmd(float frame, int);
    virtual float getPartialFrame(int);
    virtual float getPartialEndFrame(int);
    virtual bool isEndPartial(int);
    virtual bool isLoopedPartial(int);
    virtual int getOtherAnimSize();
    virtual int getOtherAnimKind(int);
    // HYPOTHESIS: the native parameter order is (layer, motion, loop, startFrame, rate); MWCC evaluates the arguments in declared order, which fixes the integer-before-float load order.
    virtual void addOtherAnim(int, u32, bool, float, float);
    virtual void addOtherAnim(float, float, int, int, int, bool);
    virtual void removeOtherAnim(int);
    virtual void setOtherAnimRate(int, float rate);
    virtual float getOtherAnimRate(int);
    virtual void setOtherAnimFrame(float frame, int);
    virtual float getOtherAnimFrame(int);
    virtual float getOtherAnimEndFrame(int);
    virtual bool isEndOtherAnim(int);
    virtual bool isLoopedOtherAnim(int);
    virtual Vec3f getTransNTranslateEndFrame(int kind); // MATCH-ONLY: the SDK declares (void*, int) with a float result; the binary returns the Vec3f through the hidden pointer
    virtual Vec3f getTransNTranslate();
    virtual void updateTransMoveSpeed();
    virtual bool isTransMove(); // HYPOTHESIS: takes no argument (the native Pikmin caller passes none)
    virtual Vec3f getTransMoveSpeed();
    virtual Vec3f get2ndTransNTranslate();
    virtual void update2ndTransMoveSpeed();
    virtual bool is2ndTransMove();
    virtual Vec3f get2ndTransMoveSpeed();
    virtual Vec3f getBlendTransMoveSpeed();
    virtual int getNum();
#ifdef FT_FIGHTER_FIELDS
    virtual void setShareAdjustNode(int, float);
#else
    virtual void setShareAdjustNode(float, void*);
#endif
    virtual void setPartialCancelNodeData(void*);
    virtual void enablePartialCancelNode(bool);
    virtual Vec3f getNodeRotate(int nodeId);
    virtual Vec3f getNodeTranslate(int nodeId, bool);
    virtual Vec3f getNodeTranslate(float, int kind, int nodeId, bool);
    virtual void begin();
    virtual void rewind();
    virtual void turnBack();
    virtual bool checkTransition(int);
    virtual void addDefaultTransitionTerm(void*, int numTerms);
    virtual void addTransitionTerm(void*, int numTerms, int);
    virtual void setShareCancelNode(int, int);
    virtual void setOnlyChr(bool);
    virtual void* getLastMotionTransitionInfo();
    virtual void cacheAnimChrForce(int kind);
    virtual void setAnimObjCacheModule(void*);
    virtual void setCheckTexBind(); // TODO
    virtual void* detachAnmObjChrBlend();
    virtual void setAnmObjChrBlend(void*);
    virtual void setResourceId(int, int);
    virtual void clearResourceId();
    virtual void setKindOffset(int kindOffset);
    virtual void setShareAnimObjKind(int shareAnimObjKind);
    virtual void setShareAnimObjPartialInfo(int, int);
    virtual void setNoComp(bool);
    virtual void setTransMoveSpeedNoScale(bool);
    virtual bool isProcessFixPositionDone();
    virtual bool isForceImmediatelyStatusInterpret();
    virtual void changeMotion(soMotionChangeParam* changeParam);

    virtual void* getResFileData(int);
    virtual bool isObserv(char unk1);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventConstructInstance(bool, soModuleAccesser* moduleAccesser);
    virtual void notifyEventDestructInstance(soModuleAccesser* moduleAccesser);

    // part2/part3
    void clearAnim();
    void change(float, float, int, u8, u8, u8, u8);
    nw4r::g3d::AnmObjChrRes* createNodeAnim(int, int, bool, u32*, bool);
    void syncPartialAnimCmdInterpreterMain(int, int, bool, const soAttributeFlag&, bool, float);
    void applyDefaultTransitionTerm();
    void interpretCmd();
    void interpretCmdPost();
    void interpretCmdSub(const soAttributeFlag&, bool) __attribute__((never_inline));
    void interpretCmdSubPartial(const soAttributeFlag&, bool) __attribute__((never_inline));
    void interpretCmdAdjustSub(const soAttributeFlag&) __attribute__((never_inline));
    void interpretCmdAdjustSubPartial(const soAttributeFlag&) __attribute__((never_inline));
};
static_assert(sizeof(soMotionModuleImpl) == 368, "Class is wrong size!");
