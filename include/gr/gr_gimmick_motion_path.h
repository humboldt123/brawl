#pragma once
// Shadows include/Brawl/Include/gr/gr_gimmick_motion_path.h (-I include comes first) to declare getTRS's scale as a
// float*: the function stores one float through it (sora_melee, getTRS__19grGimmickMotionPathFP5Vec3fP5Vec3fPf), and
// its callers keep a 4-byte slot for it (emKineticEnergy::startEnergy / updateEnergySpeed). Upstream declares a Vec3f*.

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>

class grGimmickMotionPath : public grGimmick {

public:
    char _spacer[4];
    float m_frame;
    char _spacer2[12];
    unsigned int m_frameCount;
    grGimmickMotionPathData* m_motionPathData;
    char _spacer3[20];
    int m_nodeIndex;
    char m_nodeName[32];

    grGimmickMotionPath(const char* taskName, const char* nodeName);
    virtual void processAnim();
    virtual void loadGroundData(nw4r::g3d::ResFile, u32 unk2, gfSceneRoot::LayerType unk3);
    virtual int getModelCount();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual ~grGimmickMotionPath();

    virtual bool isMoveEnd();
    virtual bool isReturnEnd();
    virtual void setFrameModelAnim();
    virtual void clearFrame();

    static grGimmickMotionPath* create(int mdlIndex, const char* nodeName, const char* taskName);

    void startMove();
    void startMove(float deltaFrame);
    void setFrameUpdate(float);
    void setFrame(float frame);
    void applyEndFrame();
    float getFrame();
    bool isEndFrame();
    bool isStartFrame();
    void getTRS(Vec3f* pos, Vec3f* rotation, float* scale);
    void stopStartSE();
};