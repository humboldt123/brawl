#pragma once

// Definitions shared by the four Zako fighter units (each fighter was compiled on its own in the original).
#include <ft/ft_class_info_impl.h>
#include <ft/ft_util.h>
#include <ft/zako/ft_zako.h>

#define FT_ZAKO_IMPL(NAME)                                                                  \
    ftZako##NAME::ftZako##NAME(s32 entryId,                                                 \
                               Heaps::HeapType instHeap,                                    \
                               Heaps::HeapType nwModelInstHeap,                             \
                               Heaps::HeapType nwMotionInstHeap) :                          \
        ftFighterBuilder<ftZako##NAME##BuildConfig>(entryId,                                \
                                                    Fighter_Zako_##NAME,                    \
                                                    instHeap,                               \
                                                    nwModelInstHeap,                        \
                                                    nwMotionInstHeap) {                     \
        m_commonData = g_ftCommonDataAccesser.getData(Fighter_Zako_##NAME);                 \
    }                                                                                       \
    bool ftZako##NAME::checkTransitionStatus(u32 status) {                                  \
        bool result = false;                                                                \
        if (Fighter::checkTransitionStatus(status)) {                                       \
            if (ftUtil::isValidStatusKindZako(m_moduleAccesser, status)) {                  \
                result = true;                                                              \
            }                                                                               \
        }                                                                                   \
        return result;                                                                      \
    }                                                                                       \
    bool ftZako##NAME::isHeartSwapEnableCondition() { return false; }                       \
    bool ftZako##NAME::setMetal(bool setStatus, float health, int unk3) { return false; }


// Plays the common "other" animation layer and sets four work flags on activation.
// HYPOTHESIS: the flags are the invincibility/ground-state flags set by the common statuses; names unknown.
#define FT_ZAKO_ON_ACTIVATE(NAME)                                                           \
    void ftZako##NAME::onActivate() {                                                       \
        soModuleAccesser* acc = m_moduleAccesser;                                           \
        float one = 1.0f;                                                                   \
        acc->getMotionModule().addOtherAnim(0.0f, one, 1, 0x1CE, true);                     \
        acc->getMotionModule().setOtherAnimRate(one, 1);                                    \
        acc->getWorkManageModule().onFlag(0x12000021);                                     \
        acc->getWorkManageModule().onFlag(0x12000022);                                     \
        acc->getWorkManageModule().onFlag(0x12000023);                                     \
        acc->getWorkManageModule().onFlag(0x1200001D);                                     \
    }

