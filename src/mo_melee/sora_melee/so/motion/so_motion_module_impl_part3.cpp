#define SO_MOTION_MODULE_IMPL_UNIT
#define SO_MOTION_MODULE_IMPL_PART3_UNIT
#define SO_EVENT_OBSERVER_CHECK_ORDER
#define SO_EFFECT_OBSERVER_ADD_EXTERN
#include <nw4r/g3d/g3d_anmchrblend.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/slow/so_slow_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>

namespace {

class soMotionAnimObjCacheModuleIface {
public:
    virtual bool isNull();
    virtual void unk0C();
    virtual bool setCacheAnimChr(s32 kind, nw4r::g3d::AnmObjChrRes* animObj, u32 cacheSize);
    virtual bool isCached(s32 kind);
};

struct MotionDataAccesserView {
    soArrayFixed<const soMotionData>* data;
    s32 kindOffset;
};

struct MotionExtPart3 {
    void* termPacks;                               // 0x140
    void* partialCancelNodeData;                   // 0x144
    soMotionAnimObjCacheModuleIface* animObjCache; // 0x148
    float shareAdjustNodeVal;                      // 0x14c
    void* shareAdjustNodePtr;                      // 0x150
    void* shareCancelNodes;                        // 0x154
};

struct MotionFlagsPart3 {
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

struct MotionResFileVtableView {
    virtual void pad008(); virtual void pad00c(); virtual void pad010(); virtual void pad014();
    virtual void pad018(); virtual void pad01c(); virtual void pad020(); virtual void pad024();
    virtual void pad028(); virtual void pad02c(); virtual void pad030(); virtual void pad034();
    virtual void pad038(); virtual void pad03c(); virtual void pad040(); virtual void pad044();
    virtual void pad048(); virtual void pad04c(); virtual void pad050(); virtual void pad054();
    virtual void pad058(); virtual void pad05c(); virtual void pad060(); virtual void pad064();
    virtual void pad068(); virtual void pad06c(); virtual void pad070(); virtual void pad074();
    virtual void pad078(); virtual void pad07c(); virtual void pad080(); virtual void pad084();
    virtual void pad088(); virtual void pad08c(); virtual void pad090(); virtual void pad094();
    virtual void pad098(); virtual void pad09c(); virtual void pad0a0(); virtual void pad0a4();
    virtual void pad0a8(); virtual void pad0ac(); virtual void pad0b0(); virtual void pad0b4();
    virtual void pad0b8(); virtual void pad0bc(); virtual void pad0c0(); virtual void pad0c4();
    virtual void pad0c8(); virtual void pad0cc(); virtual void pad0d0(); virtual void pad0d4();
    virtual void pad0d8(); virtual void pad0dc(); virtual void pad0e0(); virtual void pad0e4();
    virtual void pad0e8(); virtual void pad0ec(); virtual void pad0f0(); virtual void pad0f4();
    virtual void pad0f8(); virtual void pad0fc(); virtual void pad100(); virtual void pad104();
    virtual void pad108(); virtual void pad10c(); virtual void pad110(); virtual void pad114();
    virtual void pad118(); virtual void pad11c(); virtual void pad120(); virtual void pad124();
    virtual void pad128(); virtual void pad12c(); virtual void pad130(); virtual void pad134();
    virtual void pad138(); virtual void pad13c(); virtual void pad140(); virtual void pad144();
    virtual void pad148(); virtual void pad14c(); virtual void pad150(); virtual void pad154();
    virtual void pad158(); virtual void pad15c(); virtual void pad160(); virtual void pad164();
    virtual void pad168(); virtual void pad16c(); virtual void pad170(); virtual void pad174();
    virtual void pad178(); virtual void pad17c(); virtual void pad180(); virtual void pad184();
    virtual void pad188(); virtual void pad18c(); virtual void pad190(); virtual void pad194();
    virtual void pad198(); virtual void pad19c(); virtual void pad1a0(); virtual void pad1a4();
    virtual void pad1a8(); virtual void pad1ac(); virtual void pad1b0(); virtual void pad1b4();
    virtual void pad1b8(); virtual void pad1bc(); virtual void pad1c0(); virtual void pad1c4();
    virtual void pad1c8(); virtual void pad1cc(); virtual void pad1d0(); virtual void pad1d4();
    virtual void pad1d8(); virtual void pad1dc(); virtual void pad1e0(); virtual void pad1e4();
    virtual void pad1e8(); virtual void pad1ec(); virtual void pad1f0(); virtual void pad1f4();
    virtual nw4r::g3d::ResFile getResFileData(int, int); // 0x1f8
};

struct WorkManageLockView {
    u32 pad[3];
    s32 lockCount;
};

class soResourceModuleIntIdxView {
    u32 pad[2];
public:
    virtual ~soResourceModuleIntIdxView();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void* getAnmFile(int fileIndex, u32 anmResId, u32 p3); // 0x2c
};

inline MotionDataAccesserView* dataAccesserView(soMotionModuleImpl* self) {
    return reinterpret_cast<MotionDataAccesserView*>(&self->m_dataAccesser);
}

inline nw4r::g3d::AnmObjChrBlend*& anmObjChrBlend(soMotionModuleImpl* self) {
    return self->m_anmObjChrBlend;
}

inline MotionExtPart3* motionExt(soMotionModuleImpl* self) {
    return reinterpret_cast<MotionExtPart3*>(self->unk320);
}

inline MotionFlagsPart3* motionFlags(soMotionModuleImpl* self) {
    return reinterpret_cast<MotionFlagsPart3*>(&self->m_shareAnimObjPartialSlot + 1);
}

} // namespace

void soMotionModuleImpl::changeMotion(soMotionChangeParam* changeParam) {
    WorkManageLockView* lock = reinterpret_cast<WorkManageLockView*>(&m_moduleAccesser->getWorkManageModule());
    lock->lockCount++;
    motionFlags(this)->processFixPositionDone = 1;
    m_moduleAccesser->getSlowModule().resetSkip();
    float rate = changeParam->m_rate;
    change(changeParam->m_frame, rate, changeParam->m_kind,
           changeParam->_12, changeParam->_13, changeParam->_14, changeParam->_15);
    WorkManageLockView* lockEnd = reinterpret_cast<WorkManageLockView*>(&m_moduleAccesser->getWorkManageModule());
    if (lockEnd->lockCount > 0) {
        lockEnd->lockCount--;
    }
}

void soMotionModuleImpl::cacheAnimChrForce(int kind) {
    if (kind < 0 || motionExt(this)->animObjCache->isNull() == true) {
        return;
    }
    if (motionExt(this)->animObjCache->isCached(kind) == true) {
        return;
    }
    u32 cacheSize = 0;
    nw4r::g3d::AnmObjChrRes* animObj = createNodeAnim(kind, -1, true, &cacheSize, true);
    if (animObj != NULL) {
        if (!motionExt(this)->animObjCache->setCacheAnimChr(kind, animObj, cacheSize)) {
            animObj->Destroy();
        }
    }
}

void* soMotionModuleImpl::detachAnmObjChrBlend() {
    clearAnim();
    nw4r::g3d::AnmObjChrBlend* prev = anmObjChrBlend(this);
    anmObjChrBlend(this) = NULL;
    return prev;
}

void* soMotionModuleImpl::getResFileData(int fileIndex) {
    return reinterpret_cast<soResourceModuleIntIdxView*>(&m_moduleAccesser->getResourceModule())->getAnmFile(fileIndex, 0xFFFF, 0);
}

void soMotionModuleImpl::setKindOffset(int kindOffset) {
    dataAccesserView(this)->kindOffset = kindOffset;
}

void soMotionModuleImpl::notifyEventDestructInstance(soModuleAccesser*) {
    clearAnim();
    if (anmObjChrBlend(this) != NULL) {
        anmObjChrBlend(this)->Destroy();
        anmObjChrBlend(this) = NULL;
    }
    nw4r::g3d::ResFile resFile = reinterpret_cast<MotionResFileVtableView*>(this)->getResFileData(0, 0);
    if (resFile.IsValid()) {
        resFile.Release();
    }
}

void soModelEventObserver::addObserver(short manageId, s8 p2) {
    addObserverSub(manageId, this, p2);
}

bool soMotionModuleImpl::isForceImmediatelyStatusInterpret() {
    return motionFlags(this)->forceImmediatelyStatusInterpret;
}

bool soMotionModuleImpl::isProcessFixPositionDone() {
    return motionFlags(this)->processFixPositionDone;
}

void soMotionModuleImpl::setTransMoveSpeedNoScale(bool val) {
    motionFlags(this)->transMoveSpeedNoScale = val;
}

void soMotionModuleImpl::setNoComp(bool val) {
    motionFlags(this)->noComp = val;
}

void soMotionModuleImpl::clearResourceId() {}

void soMotionModuleImpl::setResourceId(int, int) {}

void soMotionModuleImpl::setAnmObjChrBlend(void* blend) {
    anmObjChrBlend(this) = static_cast<nw4r::g3d::AnmObjChrBlend*>(blend);
}

void soMotionModuleImpl::setCheckTexBind() {}

void* soMotionModuleImpl::getLastMotionTransitionInfo() {
    return m_transitionModule->getLastTransitionInfo();
}

void soMotionModuleImpl::setOnlyChr(bool val) {
    motionFlags(this)->onlyChr = val;
}

void soMotionModuleImpl::enablePartialCancelNode(bool val) {
    motionFlags(this)->enablePartialCancelNode = val;
}

void soMotionModuleImpl::setPartialCancelNodeData(void* data) {
    motionExt(this)->partialCancelNodeData = data;
}

void soMotionModuleImpl::setShareAdjustNode(float val, void* ptr) {
    motionExt(this)->shareAdjustNodePtr = ptr;
    motionExt(this)->shareAdjustNodeVal = val;
}

int soMotionModuleImpl::getNum() {
    return dataAccesserView(this)->data->size();
}

int soMotionModuleImpl::getPartialSize() {
    return m_partialAnims->size();
}

int soMotionModuleImpl::get2ndKind() {
    return m_secondaryKind;
}

bool soMotionModuleImpl::isBlend() {
    return m_weight < 1.0f;
}

float soMotionModuleImpl::getPrevWeight() {
    return m_prevWeight;
}

float soMotionModuleImpl::getWeight() {
    return m_weight;
}

char* soMotionModuleImpl::getName() {
    return getName(m_kind, true);
}

soAnimCmdAddressPack::soAnimCmdAddressPack() : unk0(0), unk4(0), unk8(0) {}

soAnimCmdAddressPack::~soAnimCmdAddressPack() {}

template <typename T>
void soEventObserver<T>::addObserver(s16, s8) {}
template void soEventObserver<soModelEventObserver>::addObserver(s16, s8);
template class soArrayVector<soAnimCmdAddressPack, 20>;
