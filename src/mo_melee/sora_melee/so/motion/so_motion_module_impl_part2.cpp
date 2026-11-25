#define SO_MOTION_MODULE_IMPL_UNIT
#define SO_MOTION_MODULE_IMPL_PART2_UNIT
#define SO_ANIM_CMD_MODULE_IMPL_UNIT
#define SO_GENERAL_TERM_MANAGER_IMPL
#include <gf/gf_task_scheduler.h>
#include <so/anim/so_anim_cmd_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/slow/so_slow_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/templates/so_array_value_soTransitionTermPack.h>

namespace {

struct MotionExtPart2 {
    soArray<soTransitionTermPack>* termPacks; // 0x140
    void* partialCancelNodeData;              // 0x144
    void* animObjCacheModule;                 // 0x148
    float shareAdjustNodeVal;                 // 0x14c
    void* shareAdjustNodePtr;                 // 0x150
    soArray<u32>* shareCancelNodes;           // 0x154
};

struct MotionFlagsPart2 {
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

inline MotionExtPart2* motionExt(soMotionModuleImpl* self) {
    return reinterpret_cast<MotionExtPart2*>(self->unk320);
}

inline MotionFlagsPart2* motionFlags(soMotionModuleImpl* self) {
    return reinterpret_cast<MotionFlagsPart2*>(&self->m_shareAnimObjPartialSlot + 1);
}

} // namespace

void soMotionModuleImpl::syncPartialAnimCmdInterpreterMain(int, int kind, bool runNow, const soAttributeFlag& attr, bool syncAll, float frame) {
    if (kind == -1) {
        m_moduleAccesser->getAnimCmdModule().resetInterpreter(attr, m_moduleAccesser, -1, false, 0.0f);
        return;
    }
    m_moduleAccesser->getSlowModule().getRate();
    if (syncAll == true) {
        s32 size = m_partialAnims->size();
        for (s32 i = 0; i < size; i++) {
            if (m_partialAnims->at(i).m_kind != -1) {
                soSlowModule& slow = m_moduleAccesser->getSlowModule();
                m_partialAnims->at(i);
                m_partialAnims->at(i);
                slow.getRate();
                break;
            }
        }
    }
    m_moduleAccesser->getAnimCmdModule().resetInterpreter(attr, m_moduleAccesser, kind, false, frame);
    if (runNow == true) {
        soAttributeFlag flag(attr.m_mask);
        m_moduleAccesser->getAnimCmdModule().interpretCmd(m_moduleAccesser, flag, 0.0f);
    }
}

bool soMotionModuleImpl::checkTransition(int arg) {
    soModuleAccesser* moduleAccesser = reinterpret_cast<soModuleAccesser*>(arg);
    u32 targetKind = -1;
    u16 attrMask = 0;
    bool established = (m_transitionModule->checkEstablish(moduleAccesser, &targetKind, -1, &attrMask, NULL) == 1);
    if (established) {
        soAttributeFlag flag(1);
        moduleAccesser->getAnimCmdModule().stopInterpreter(flag, moduleAccesser);
        motionFlags(this)->forceImmediatelyStatusInterpret = 1;
        m_transitionModule->clearTransitionTermAll(-1);
        return true;
    }
    return false;
}

void soMotionModuleImpl::addDefaultTransitionTerm(void* termsPtr, int numTerms) {
    soTransitionTermPack* terms = static_cast<soTransitionTermPack*>(termsPtr);
    if (terms == NULL || numTerms <= 0) {
        return;
    }
    for (int i = 0; i < numTerms; i++) {
        motionExt(this)->termPacks->push(terms[i]);
    }
}

soGeneralTerm::~soGeneralTerm() {}

void soMotionModuleImpl::setShareCancelNode(int index, int nodeId) {
    if (index >= 0 && index < motionExt(this)->shareCancelNodes->size()) {
        motionExt(this)->shareCancelNodes->at(index) = nodeId;
    }
}

void soMotionModuleImpl::processFixPosition() {
    if (m_motionChangeParamArray.isEmpty() != true) {
        while (!m_motionChangeParamArray.isEmpty()) {
            soMotionChangeParam* param = &m_motionChangeParamArray.at(0);
            m_motionChangeParamArray.shift();
            changeMotion(param);
        }
    }
    if (!motionFlags(this)->processFixPositionDone) {
        motionFlags(this)->processFixPositionDone = 1;
        interpretCmd();
        checkTransition(reinterpret_cast<int>(m_moduleAccesser));
    }
}

void soMotionModuleImpl::processFixCamera() {
    interpretCmdPost();
}

void soMotionModuleImpl::interpretCmd() {
    bool adjust = false;
    if (m_moduleAccesser->getSlowModule().isSlow() == true && !m_moduleAccesser->getSlowModule().isSkip()) {
        adjust = true;
    }
    if (adjust) {
        soAttributeFlag mainFlag(0x2);
        interpretCmdAdjustSub(mainFlag);
        soAttributeFlag partialFlag(0x20);
        interpretCmdAdjustSubPartial(partialFlag);
    } else {
        soAttributeFlag mainFlag(0x2);
        interpretCmdSub(mainFlag, false);
        soAttributeFlag partialFlag(0x20);
        interpretCmdSubPartial(partialFlag, false);
    }
}

void soMotionModuleImpl::interpretCmdPost() {
    bool adjust = false;
    if (m_moduleAccesser->getSlowModule().isSlow() == true && !m_moduleAccesser->getSlowModule().isSkip()) {
        adjust = true;
    }
    u8 mainMode = motionFlags(this)->postModeMain;
    if (mainMode == 1) {
        soAttributeFlag flag(0x40);
        interpretCmdSub(flag, true);
    } else if (mainMode == 2 || adjust == true) {
        soAttributeFlag flag(0x40);
        interpretCmdAdjustSub(flag);
    } else {
        soAttributeFlag flag(0x40);
        interpretCmdSub(flag, false);
    }
    u8 partialMode = motionFlags(this)->postModePartial;
    if (partialMode == 1) {
        soAttributeFlag flag(0x80);
        interpretCmdSubPartial(flag, true);
    } else if (partialMode == 2 || adjust == true) {
        soAttributeFlag flag(0x80);
        interpretCmdAdjustSubPartial(flag);
    } else {
        soAttributeFlag flag(0x80);
        interpretCmdSubPartial(flag, false);
    }
}

void soMotionModuleImpl::interpretCmdSub(const soAttributeFlag& attr, bool zeroRate) {
    float rate = 0.0f;
    if (!zeroRate) {
        float slowRate = m_moduleAccesser->getSlowModule().getRate();
        float r = getRate();
        rate = r * slowRate;
    }
    soAttributeFlag flag(attr.m_mask);
    m_moduleAccesser->getAnimCmdModule().interpretCmd(m_moduleAccesser, flag, rate);
}

void soMotionModuleImpl::interpretCmdSubPartial(const soAttributeFlag& attr, bool zeroRate) {
    float slowRate = m_moduleAccesser->getSlowModule().getRate();
    s32 size = m_partialAnims->size();
    for (s32 i = 0; i < size; i++) {
        if (m_partialAnims->at(i).m_kind != -1) {
            float rate = 0.0f;
            if (!zeroRate) {
                rate = slowRate * m_partialAnims->at(i).m_anim.m_rate;
            }
            soAttributeFlag flag(attr.m_mask);
            m_moduleAccesser->getAnimCmdModule().interpretCmd(m_moduleAccesser, flag, rate);
            break;
        }
    }
}

void soMotionModuleImpl::interpretCmdAdjustSub(const soAttributeFlag& attr) {
    if (m_mainAnim.isEnd() == true) {
        soAttributeFlag flag(attr.m_mask);
        interpretCmdSub(flag, false);
        return;
    }
    float endFrame = -1.0f;
    if (m_mainAnim.m_anmChrRes != NULL) {
        endFrame = getEndFrame();
    }
    soAttributeFlag flag(attr.m_mask);
    m_moduleAccesser->getAnimCmdModule().interpretCmdAdjust(m_moduleAccesser, flag, getFrame(), endFrame);
}

bool soMotionModuleImpl::changeMotionRequest(soMotionChangeParam* changeParam) {
    gfTaskScheduler* scheduler = gfTaskScheduler::getInstance();
    if (scheduler != NULL) {
        s32 processType = scheduler->getProcessType();
        if (processType <= 1 || processType >= 6) {
            m_motionChangeParamArray.clear();
            changeMotion(changeParam);
            return true;
        }
    }
    if (m_motionChangeParamArray.isFull() != true) {
        m_motionChangeParamArray.push(*changeParam);
    }
    return false;
}

void soMotionModuleImpl::notifyEventChangeStatus(int, int, soStatusData*, soModuleAccesser*) {
    m_transitionModule->clearTransitionTermAll(-1);
    *reinterpret_cast<s32*>(reinterpret_cast<u8*>(&m_dataAccesser) + 4) = 0;
    motionFlags(this)->unk357_1 = 0;
}
