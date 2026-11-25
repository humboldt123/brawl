#define SO_MOTION_MODULE_IMPL_UNIT
#define SO_ANIM_CMD_MODULE_IMPL_UNIT
#define SO_ATTRIBUTE_FLAG_TRIVIAL_DTOR
#define MT_VEC3F_DIV_SCALE
#include <nw4r/g3d/g3d_calcworld.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <so/anim/so_anim_cmd_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/so_common_data_accesser.h>
#include <so/so_heap_module.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_simple.h>
#include <so/posture/so_posture_module_simple.h>

namespace {

class soMotionAnimObjCacheModuleIface {
public:
    virtual bool isNull();
    virtual void unk0C();
    virtual bool setCacheAnimChr(s32 kind, nw4r::g3d::AnmObjChrRes* animObj, u32 cacheSize);
    virtual bool isCached(s32 kind);
    virtual void destroyAnimChrAll();
};

struct ChrAnmResultData {
    u32 flags;
    float s[3];
    float rawR[3];
    float rt[3][4];
};

class AnmObjChrResView {
public:
    virtual void v08();
    virtual void v0c();
    virtual ~AnmObjChrResView();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual const ChrAnmResultData* GetResult(ChrAnmResultData* result, u32 idx); // 0x38
    virtual void v3c();
    virtual void v40();
    virtual void v44();
    virtual void v48();
    virtual void v4c();
    virtual int Bind(nw4r::g3d::ResMdl mdl, u32 target, int option);              // 0x50
    virtual void Release(nw4r::g3d::ResMdl mdl, u32 target, int option);          // 0x54
};

inline AnmObjChrResView* anmChrView(nw4r::g3d::AnmObjChrRes* p) {
    return reinterpret_cast<AnmObjChrResView*>(p);
}

struct MotionExt {
    void* termPacks;                                // 0x140
    void* partialCancelNodeData;                    // 0x144
    soMotionAnimObjCacheModuleIface* animObjCache;  // 0x148
    float shareAdjustNodeVal;                       // 0x14c
    void* shareAdjustNodePtr;                       // 0x150
    soArray<u32>* shareCancelNodes;                 // 0x154
};

struct MotionFlags {
    u8 transMove : 1;               // 0x164 bit 0 (0x80)
    u8 transMoveSpeedNoScale : 1;   // 0x164 bit 1 (0x40)
    u8 unk356_2 : 2;                // 0x164 bits 2..3
    u8 processFixPositionDone : 1;  // 0x164 bit 4 (0x08)
    u8 secondaryTransMove : 1;      // 0x164 bit 5 (0x04)
    u8 onlyChr : 1;                 // 0x164 bit 6 (0x02)
    u8 enablePartialCancelNode : 1; // 0x164 bit 7 (0x01)

    u8 noComp : 1;                         // 0x165 bit 0 (0x80)
    u8 unk357_1 : 1;                       // 0x165 bit 1 (0x40)
    u8 postModeMain : 2;                   // 0x165 bits 2..3 (0x30)
    u8 postModePartial : 2;                // 0x165 bits 4..5 (0x0c)
    u8 forceImmediatelyStatusInterpret : 1;// 0x165 bit 6 (0x02)
    u8 unk357_7 : 1;                       // 0x165 bit 7 (0x01)
};

inline MotionExt* motionExt(soMotionModuleImpl* self) {
    return reinterpret_cast<MotionExt*>(self->unk320);
}

inline MotionFlags* motionFlags(soMotionModuleImpl* self) {
    return reinterpret_cast<MotionFlags*>(&self->m_shareAnimObjPartialSlot + 1);
}

} // namespace

soPartialAnim::soPartialAnim() : m_kind(-1), m_secondaryKind(-1) {}
soPartialAnim::~soPartialAnim() {}
soOtherAnim::soOtherAnim() : m_kind(-1) {}
soOtherAnim::~soOtherAnim() {}
soMotionModule::~soMotionModule() {}

void soMotionModuleImpl::setData(void* data) {
    m_dataAccesser.setData(static_cast<soSet<const soMotionData>*>(data));
}
int soMotionModuleImpl::getKind() { return m_kind; }
float soMotionModuleImpl::getRate() { return m_mainAnim.m_rate; }
void soMotionModuleImpl::setRate(float rate) { m_mainAnim.m_rate = rate; }
void soMotionModuleImpl::setLoopFlag(bool loop) { m_mainAnim.m_loopFlag = loop; }
bool soMotionModuleImpl::isLoopFlag() { return m_mainAnim.m_loopFlag; }
float soMotionModuleImpl::getFrame() { return m_mainAnim.m_animFrame; }
bool soMotionModuleImpl::isEnd() { return m_mainAnim.isEnd(); }
bool soMotionModuleImpl::isLooped() { return m_mainAnim.isLooped(); }
void soMotionModuleImpl::setWeightRate(float rate) { m_weightRate = rate; }
void soMotionModuleImpl::disableNode(int node) { m_mainAnim.disableNode(node); }
float soMotionModuleImpl::getEndFrame() { return m_mainAnim.getEndFrame(m_mainAnim.m_anmChrRes); }
void soMotionEventObserver::notifyEventChangeMotion(int, int, void*, soModuleAccesser*) {}
void soMotionModuleImpl::setShareAnimObjPartialInfo(int kind, int slot) {
    m_shareAnimObjPartialKind = kind;
    m_shareAnimObjPartialSlot = slot;
}
void soMotionModuleImpl::set2ndRate(float rate) { m_secondaryAnim.m_rate = rate; }
float soMotionModuleImpl::get2ndFrame() { return m_secondaryAnim.m_animFrame; }
bool soMotionModuleImpl::is2ndEnd() { return m_secondaryAnim.isEnd(); }
void soMotionModuleImpl::setPartialRate(float rate, int part) { m_partialAnims->at(part).m_anim.m_rate = rate; }
int soMotionModuleImpl::getOtherAnimSize() { return m_otherAnims->size(); }
bool soMotionModuleImpl::isTransMove() { return m_transMove; }
bool soMotionModuleImpl::is2ndTransMove() { return m_secondaryTransMove; }
bool soMotionModuleImpl::isObserv(char event) { return event == 4; }

char* soMotionModuleImpl::getName(int kind, bool useOffset) {
    return m_dataAccesser.get(kind, useOffset).m_name;
}
void soMotionModuleImpl::removeOtherAnim(int part) {
    soOtherAnim& anim = m_otherAnims->at(part);
    anim.m_anim.reset();
    anim.m_kind = -1;
}

void soMotionModuleImpl::setOtherAnimRate(int part, float rate) {
    if (!m_otherAnims->at(part).m_anim.m_emptyFlag)
        m_otherAnims->at(part).m_anim.m_rate = rate;
}
void soMotionModuleImpl::setOtherAnimFrame(float frame, int part) {
    if (!m_otherAnims->at(part).m_anim.m_emptyFlag)
        m_otherAnims->at(part).m_anim.setFrame(frame);
}

void soMotionModuleImpl::setFrame(float frame) {
    m_mainAnim.setFrame(frame);
    if (m_mainAnim.m_anmChrRes)
        m_moduleAccesser->m_enumerationStart->m_modelModule->clearSetUpMtx();
    m_otherAnim.setFrame(frame);
}
void soMotionModuleImpl::setPartialFrame(float frame, int part) {
    m_partialAnims->at(part).m_anim.setFrame(frame);
    if (m_partialAnims->at(part).m_anim.m_anmChrRes)
        m_moduleAccesser->m_enumerationStart->m_modelModule->clearSetUpMtx();
}


void soMotionModuleImpl::setShareAnimObjKind(int kind) {
    if (kind == -1 && unk357_7 == 1) {
        removePartialAnimChr(m_shareAnimObjPartialSlot);
        unk357_7 = false;
    }
    m_shareAnimObjKind = kind;
}
void soMotionModuleImpl::begin() {
    m_mainAnim.m_startFrame = m_mainAnim.m_animFrame;
    if (m_secondaryKind != -1) {
        m_secondaryAnim.m_startFrame = m_secondaryAnim.m_animFrame;
        m_prevWeight = m_weight;
    }
    int size = m_partialAnims->size();
    for (int i = 0; i < size; i++) {
        soPartialAnim& anim = m_partialAnims->at(i);
        if (anim.m_kind != -1) anim.m_anim.m_startFrame = anim.m_anim.m_animFrame;
    }
    size = m_otherAnims->size();
    for (int i = 0; i < size; i++) {
        soOtherAnim& anim = m_otherAnims->at(i);
        if (anim.m_kind != -1) anim.m_anim.m_startFrame = anim.m_anim.m_animFrame;
    }
    unk356_4 = false;
    unk357_6 = false;
}
void soMotionModuleImpl::rewind() {
    m_mainAnim.rewind();
    if (m_secondaryKind != -1) {
        m_secondaryAnim.rewind();
        m_saveWeight = m_weight;
        setWeight(m_prevWeight);
    }
    int size = m_partialAnims->size();
    for (int i = 0; i < size; i++) {
        if (m_partialAnims->at(i).m_kind != -1) m_partialAnims->at(i).m_anim.rewind();
    }
    size = m_otherAnims->size();
    for (int i = 0; i < size; i++) {
        if (m_otherAnims->at(i).m_kind != -1) m_otherAnims->at(i).m_anim.rewind();
    }
}
void soMotionModuleImpl::turnBack() {
    m_mainAnim.turnBack();
    if (m_secondaryKind != -1) {
        m_secondaryAnim.turnBack();
        setWeight(m_saveWeight);
    }
    int size = m_partialAnims->size();
    for (int i = 0; i < size; i++) {
        if (m_partialAnims->at(i).m_kind != -1) m_partialAnims->at(i).m_anim.turnBack();
    }
    size = m_otherAnims->size();
    for (int i = 0; i < size; i++) {
        if (m_otherAnims->at(i).m_kind != -1) m_otherAnims->at(i).m_anim.turnBack();
    }
    if (m_mainAnim.m_anmChrRes)
        m_moduleAccesser->m_enumerationStart->m_modelModule->clearSetUpMtx();
}
void soMotionModuleImpl::setWeight(float weight) {
    m_weight = weight;
    float secondaryWeight = 1.0f - weight;
    m_anmObjChrBlend->SetWeight(0, weight);
    m_anmObjChrBlend->SetWeight(1, secondaryWeight);
    int size = m_partialAnims->size();
    for (int i = 0; i < size; i++) {
        if (m_partialAnims->at(i).m_secondaryKind != -1) {
            if (m_partialAnims->at(i).unk44 == 1)
                m_anmObjChrBlend->SetWeight(i + 2, 1.0f);
            else
                m_anmObjChrBlend->SetWeight(i + 2, secondaryWeight);
        }
    }
    m_moduleAccesser->m_enumerationStart->m_modelModule->clearSetUpMtx();
}

bool soMotionModuleImpl::isAnimCmd(int kind) {
    soAttributeFlag flag(2);
    return m_moduleAccesser->getAnimCmdModule().isInterpreterAnimCmd(flag, kind);
}

void soMotionModuleImpl::activate() {
    clearAnim();
    m_transitionModule->clearTransitionTermAll(-1);
    *reinterpret_cast<s32*>(reinterpret_cast<u8*>(&m_dataAccesser) + 4) = 0;
    unk356_1 = false;
}

void soMotionModuleImpl::deactivate() {
    clearAnim();
    m_transitionModule->clearTransitionTermAll(-1);
    motionExt(this)->animObjCache->destroyAnimChrAll();
    m_motionChangeParamArray.clear();
}

Vec3f soMotionModuleImpl::calcNode(int kind, int nodeId) {
    soArrayVector<int, 1> nodeIds;
    soArrayVector<Vec3f, 1> outPos;
    nodeIds.push(nodeId);
    outPos.push(Vec3f(0.0f, 0.0f, 0.0f));
    calcNode(kind, &nodeIds, &outPos);
    return outPos.at(0);
}

void soMotionModuleImpl::detachPartialAnimChr(u32 part, bool rebind) {
    soPartialAnim& anim = m_partialAnims->at(part);
    if (anim.m_kind != -1 || anim.m_secondaryKind != -1) {
        if (m_anmObjChrBlend != NULL) {
            m_anmObjChrBlend->Detach(part + 2);
        }
        if (rebind == true && m_mainAnim.m_anmChrRes != NULL) {
            nw4r::g3d::ScnMdl* scnMdl = m_moduleAccesser->getModelModule().getScnMdl();
            anmChrView(m_mainAnim.m_anmChrRes)->Bind(scnMdl->GetResMdl(), anim.unk40, 1);
            if (m_secondaryAnim.m_anmChrRes != NULL) {
                anmChrView(m_secondaryAnim.m_anmChrRes)->Bind(scnMdl->GetResMdl(), anim.unk40, 1);
            }
        }
    }
}

void soMotionModuleImpl::attachPartialAnimChr(u32 part) {
    soPartialAnim& anim = m_partialAnims->at(part);
    if (anim.m_kind != -1 || anim.m_secondaryKind != -1) {
        if (m_mainAnim.m_anmChrRes != NULL) {
            nw4r::g3d::ScnMdl* scnMdl = m_moduleAccesser->getModelModule().getScnMdl();
            anmChrView(m_mainAnim.m_anmChrRes)->Release(scnMdl->GetResMdl(), anim.unk40, 1);
            if (m_secondaryAnim.m_anmChrRes != NULL) {
                anmChrView(m_secondaryAnim.m_anmChrRes)->Release(scnMdl->GetResMdl(), anim.unk40, 1);
            }
        }
        if (anim.m_kind != -1) {
            m_anmObjChrBlend->Attach(part + 2, anim.m_anim.m_anmChrRes);
        } else {
            m_anmObjChrBlend->Attach(part + 2, anim.m_secondaryAnim.m_anmChrRes);
        }
        m_anmObjChrBlend->SetWeight(part + 2, 1.0f);
    }
}

void soMotionModuleImpl::removeAnimChr(bool resetFlag) {
    if (!motionExt(this)->animObjCache->isCached(m_kind)) {
        if (resetFlag == true) {
            m_mainAnim.reset();
        } else {
            m_mainAnim.destroyAnimObj();
        }
    } else {
        int size = m_partialAnims->size();
        for (int i = 0; i < size; i++) {
            soPartialAnim& anim = m_partialAnims->at(i);
            bool active = false;
            if (anim.m_kind != -1 || anim.m_secondaryKind != -1) {
                active = true;
            }
            if (active && m_mainAnim.m_anmChrRes != NULL) {
                soPartialAnim& anim2 = m_partialAnims->at(i);
                nw4r::g3d::ScnMdl* scnMdl = m_moduleAccesser->getModelModule().getScnMdl();
                anmChrView(m_mainAnim.m_anmChrRes)->Bind(scnMdl->GetResMdl(), anim2.unk40, 1);
            }
        }
        m_mainAnim.relinquishAnimObj();
    }
    m_kind = -1;
}

