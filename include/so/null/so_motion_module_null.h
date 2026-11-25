#pragma once

#include <so/motion/so_motion_module_impl.h>

inline soMotionModule::~soMotionModule() { }

class soMotionModuleNull : public soNull, public soNullable {
public:
    virtual ~soMotionModuleNull();
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
    virtual Vec3f calcNode(int, int);
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
    virtual Vec3f getTransNTranslateEndFrame(int kind);
    virtual Vec3f getTransNTranslate();
    virtual void updateTransMoveSpeed();
    virtual bool isTransMove();
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
    virtual bool setAnimObjCacheModule(void*);
    virtual void setCheckTexBind();
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
