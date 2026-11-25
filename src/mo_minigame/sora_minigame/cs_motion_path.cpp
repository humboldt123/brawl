#include <types.h>
#include <sr/sr_common.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <havok/hkEntity.h>
#include <ph/ph_actor.h>

void getScalingFactor(Matrix* matrix, Vec3f* scale);
nw4r::g3d::ScnMdl* ScnMdl_Construct(MEMAllocator* allocator, u32* size, nw4r::g3d::ResMdl mdl, u32 bufferOption, int nView,
                                    void (*callback)());
extern "C" void fn_801B0A28();

class csElementInterface {
public:
    virtual ~csElementInterface() { }
};

class csElement : public csElementInterface {
public:
    u8 m_isActive; // 0x04

    csElement();
    csElement(int group);
    virtual ~csElement();
    virtual void beginOperation() = 0;
    virtual void afterOperation() = 0;
    virtual void debugDraw() = 0;
};

struct hkContactPointAddedEvent;
struct hkContactPointConfirmedEvent;
struct hkContactPointRemovedEvent;
struct hkContactProcessEvent;

class hkCollisionListener {
public:
    virtual void contactPointAddedCallback(hkContactPointAddedEvent& event) = 0;
    virtual void contactPointConfirmedCallback(hkContactPointConfirmedEvent& event) = 0;
    virtual void contactPointRemovedCallback(hkContactPointRemovedEvent& event) = 0;
    virtual void contactProcessCallback(hkContactProcessEvent& event) = 0;
    virtual ~hkCollisionListener() { }
};

class hkEntityListener {
public:
    virtual ~hkEntityListener() { }
    virtual void entityAddedCallback(hkEntity* entity);
    virtual void entityRemovedCallback(hkEntity* entity);
    virtual void entityShapeSetCallback(hkEntity* entity);
    virtual void entityDeletedCallback(hkEntity* entity);
};

class phCollisionEvent : public hkCollisionListener, public hkEntityListener {
public:
    virtual void contactPointAddedCallback(hkContactPointAddedEvent& event);
    virtual void contactPointConfirmedCallback(hkContactPointConfirmedEvent& event);
    virtual void contactPointRemovedCallback(hkContactPointRemovedEvent& event);
    virtual void contactProcessCallback(hkContactProcessEvent& event);
    virtual ~phCollisionEvent() { }
};

class csArticle : public csElement, public phActor, public phCollisionEvent {
public:
    u8 m_unk20; // 0x20

    csArticle();
    virtual ~csArticle();
    virtual void beginOperation();
    virtual void afterOperation();
    virtual void debugDraw();
    virtual void applyDestroy();
};

class csMotionPath : public csArticle {
public:
    u8 m_state;                      // 0x21
    float m_frame;                   // 0x24
    int m_nodeIndex;                 // 0x28
    float m_unk2C;                   // 0x2C
    float m_frameSpeed;              // 0x30
    u32 m_frameCount;                // 0x34
    nw4r::g3d::ScnMdl** m_sceneModels; // 0x38
    gfModelAnimation** m_modelAnims; // 0x3C
    Vec3f m_pos;                     // 0x40
    Vec3f m_rot;                     // 0x4C
    Vec3f m_scale;                   // 0x58
    float m_unk64;                   // 0x64
    float m_unk68;                   // 0x68
    float m_unk6C;                   // 0x6C
    float m_endThreshold;            // 0x70

    csMotionPath();
    virtual ~csMotionPath();
    virtual void applyDestroy();
    virtual void update(float deltaFrame);
    virtual void processAnim();
    virtual int getModelCount();
    virtual void startMove();
    virtual bool isMoveEnd();
    virtual bool isReturnEnd();
    virtual void setFrameModelAnim();
    virtual void clearFrame();

    void setTRS();
    void setFrame(float frame);
    void getTRS(Vec3f* pos, Vec3f* rotation, float* scale);
    void loadPathData(nw4r::g3d::ResFileData* resFileData, int index);
    void createAttachMotionPath();
};

csMotionPath::csMotionPath() {
    m_frameCount = 0;
    m_unk64 = 1.0f;
    m_unk68 = 1.0f;
    m_unk6C = 1.0f;
    m_endThreshold = 0.1f;
    m_state = 0;
    m_frame = 0.0f;
    m_unk2C = 0.0f;
    m_nodeIndex = 0;
    m_frameSpeed = 1.0f;
}

csMotionPath::~csMotionPath() {
    gfModelAnimation* modelAnim = m_modelAnims[0];
    if (modelAnim != NULL) {
        if (modelAnim->m_anmObjChrRes != NULL) {
            modelAnim->m_anmObjChrRes->Destroy();
        }
        if (modelAnim->m_anmObjVisRes != NULL) {
            modelAnim->m_anmObjVisRes->Destroy();
        }
        if (modelAnim->m_anmObjTexPatRes != NULL) {
            modelAnim->m_anmObjTexPatRes->Destroy();
        }
        if (modelAnim->m_anmObjTexSrtRes != NULL) {
            modelAnim->m_anmObjTexSrtRes->Destroy();
        }
        if (modelAnim->m_anmObjMatClrRes != NULL) {
            modelAnim->m_anmObjMatClrRes->Destroy();
        }
        if (modelAnim->m_anmObjShpRes != NULL) {
            modelAnim->m_anmObjShpRes->Destroy();
        }
        if (modelAnim->m_resFile.ptr() != NULL && (u8)modelAnim->_spacer[0] != 0) {
            gfHeapManager::free(modelAnim->m_resFile.ptr());
            modelAnim->m_resFile = nw4r::g3d::ResFile();
        }
        modelAnim->m_anmObjChrRes = NULL;
        modelAnim->m_anmObjVisRes = NULL;
        modelAnim->m_anmObjTexPatRes = NULL;
        modelAnim->m_anmObjTexSrtRes = NULL;
        modelAnim->m_anmObjMatClrRes = NULL;
        modelAnim->m_anmObjShpRes = NULL;
        delete modelAnim;
    }
    delete m_modelAnims;
}

void csMotionPath::processAnim() {
    nw4r::g3d::ScnMdl* model = m_sceneModels[0];
    gfModelAnimation* animation = m_modelAnims[0];
    if (model == NULL || animation == NULL) {
        return;
    }
    if (m_frame >= (float)m_frameCount || isMoveEnd() == 1) {
        if (m_frame > (float)m_frameCount) {
            m_frame = (float)m_frameCount;
        }
        m_state = 0;
        m_frameSpeed = 0.0f;
    }
    animation->setFrame(m_frame);
    animation->setUpdateRate(0.0f);
    static_cast<nw4r::g3d::ScnMdlSimple*>(model)->UpdateFrame();
    model->G3dProc(1, 0, NULL);
    setTRS();
}

void csMotionPath::update(float deltaFrame) {
    switch (m_state) {
    case 1:
        m_frameSpeed = m_frameSpeed * m_unk64;
        break;
    }
}

void csMotionPath::setFrameModelAnim() {
    nw4r::g3d::ScnMdl* model = m_sceneModels[0];
    gfModelAnimation* animation = m_modelAnims[0];
    if (model == NULL || animation == NULL) {
        return;
    }
    animation->setFrame(m_frame);
    animation->setUpdateRate(0.0f);
    static_cast<nw4r::g3d::ScnMdlSimple*>(model)->UpdateFrame();
    model->G3dProc(1, 0, NULL);
    setTRS();
}

bool csMotionPath::isMoveEnd() {
    return false;
}

bool csMotionPath::isReturnEnd() {
    return false;
}

void csMotionPath::setFrame(float frame) {
    m_frame = frame;
}

void csMotionPath::getTRS(Vec3f* pos, Vec3f* rotation, float* scale) {
    Vec3f currentPos = m_pos;
    pos->m_x = currentPos.m_x;
    pos->m_y = currentPos.m_y;
    pos->m_z = currentPos.m_z;
    Vec3f currentRot = m_rot;
    rotation->m_x = currentRot.m_x;
    rotation->m_y = currentRot.m_y;
    rotation->m_z = currentRot.m_z;
    *scale = m_scale.m_x;
}

void csMotionPath::startMove() {
    if (m_state == 0) {
        m_state = 1;
    }
}

void csMotionPath::applyDestroy() {
    if (m_sceneModels[0] != NULL) {
        m_sceneModels[0]->Destroy();
    }
    if (m_sceneModels != NULL) {
        delete m_sceneModels;
        m_sceneModels = NULL;
    }
}

void csMotionPath::createAttachMotionPath() {
    m_frameCount = m_modelAnims[0]->getFrameCount();
    startMove();
}

void csMotionPath::clearFrame() {
    m_state = 0;
    m_frame = 0.0f;
    m_frameSpeed = 0.0f;
    setFrameModelAnim();
}

int csMotionPath::getModelCount() {
    return 1;
}
