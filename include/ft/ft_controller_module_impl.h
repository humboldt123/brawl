#pragma once

#include <StaticAssert.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_array.h>
#include <so/status/so_status_event_presenter.h>
#include <so/turn/so_turn_event_presenter.h>
#include <types.h>

class soModuleAccesser;

// MATCH-ONLY: the inline overrides follow the vtable order (MWCC emits deferred inline virtuals backwards).
class ftControllerModuleImpl : public soControllerModuleImpl, public soStatusEventObserver, public soTurnEventObserver {
public:
    soModuleAccesser* m_moduleAccesser;   // 0x2C
    soControllerImpl m_controller;        // 0x30
    soControllerImpl m_unkBC;             // 0xBC HYPOTHESIS: the state setPrev returns past the logged frames
    soArray<soControllerImpl>* m_logs;    // 0x148
    soController* m_currentController;    // 0x14C
    s32 m_logNum;                         // 0x150
    s32 m_logIndex;                       // 0x154 HYPOTHESIS: the last written log entry
    s32 m_prev;                           // 0x158
    s32 m_logCapacity;                    // 0x15C
    s32 m_reverseXFrame;                  // 0x160
    s32 m_reverseXFrameNext;              // 0x164 HYPOTHESIS: started by the next turn
    bool m_logActive;                     // 0x168

    ftControllerModuleImpl(soModuleAccesser* moduleAccesser, s16 unitId, soArray<soControllerImpl>* logs, soArray<soControllerClatter>* clatters);
    virtual ~ftControllerModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual void resetButton() { m_controller.resetButton(); }
    virtual void resetTrigger() { m_controller.resetTrigger(); }
    virtual void resetMainStickX() { m_controller.resetMainStickX(); }
    virtual void setMainStickX(float x) { m_controller.setMainStickX(x); }
    virtual void resetMainStickY() { m_controller.resetMainStickY(); }
    virtual void setMainStickY(float y) { m_controller.setMainStickY(y); }
    virtual void resetMainStick() { m_controller.resetMainStick(); }
    virtual void resetSubStickX() { m_controller.resetSubStickX(); }
    virtual void resetSubStickY() { m_controller.resetSubStickY(); }
    virtual void resetSubStick() { m_controller.resetSubStick(); }
    virtual void update(Input* input, bool p2);
    virtual void resetFlickX() { m_controller.resetFlickX(); }
    virtual void resetFlickY() { m_controller.resetFlickY(); }
    virtual float getStickX() {
        if (m_reverseXFrame == 0) {
            return m_currentController->getStickX();
        }
        return -m_currentController->getStickX();
    }
    virtual float getStickPrevX() { return m_currentController->getStickPrevX(); }
    virtual float getStickY() { return m_currentController->getStickY(); }
    virtual float getStickPrevY() { return m_currentController->getStickPrevY(); }
    virtual u8 getFlickX() { return m_currentController->getFlickX(); }
    virtual s8 getFlickXDir() { return m_currentController->getFlickXDir(); }
    virtual u8 getFlickY() { return m_currentController->getFlickY(); }
    virtual s8 getFlickYDir() { return m_currentController->getFlickYDir(); }
    virtual u8 getFlickNoResetX() { return m_currentController->getFlickNoResetX(); }
    virtual u8 getFlickNoResetY() { return m_currentController->getFlickNoResetY(); }
    virtual u8 getFlickAfterX() { return m_currentController->getFlickAfterX(); }
    virtual s8 getFlickAfterXDir() { return m_currentController->getFlickAfterXDir(); }
    virtual u8 getFlickAfterY() { return m_currentController->getFlickAfterY(); }
    virtual float getSubStickX() {
        if (m_reverseXFrame == 0) {
            return m_currentController->getSubStickX();
        }
        return -m_currentController->getSubStickX();
    }
    virtual float getSubStickPrevX() { return m_currentController->getSubStickPrevX(); }
    virtual float getSubStickY() { return m_currentController->getSubStickY(); }
    virtual float getSubStickPrevY() { return m_currentController->getSubStickPrevY(); }
    virtual int getTrigger() { return m_currentController->getTrigger(); }
    virtual u8 getTriggerCount(u8 index) { return m_currentController->getTriggerCount(index); }
    virtual u8 getTriggerCountPrev(u8 index) { return m_currentController->getTriggerCountPrev(index); }
    virtual int getButton() { return m_currentController->getButton(); }
    virtual int getRelease() { return m_currentController->getRelease(); }
    virtual void setOff(bool off) { m_controller.setOff(off); }
    virtual void setPrev(s32 prev);
    virtual void clearLog();
    virtual s32 getLogNum() { return m_logNum; }
    virtual void setLogActive(bool active);
    virtual float getClatterThreshold(u32 index);
    virtual soController* getController() { return &m_controller; }
    virtual void setRumble(s32 kind, s32 p2, bool loop, s32 p4);
    virtual void stopRumbleKind(s32 kind, s32 p2);
    virtual void stopRumble(bool p1);
    virtual void setRumbleAll(s32, s32, s32);
    virtual void stopRumbleAll(s32, s32);
    virtual void setReverseXFrame(s32 frame, bool next) {
        if (!next) {
            m_reverseXFrame = frame;
        } else {
            m_reverseXFrameNext = frame;
        }
    }
    virtual float getLr() { return m_controller.getLr(); }
    virtual u8 getFlickBonus() { return m_controller.getFlickBonus(); }
    virtual u8 getFlickBonusLr() { return m_controller.getFlickBonusLr(); }
    virtual void resetFlickBonus() { m_controller.resetFlickBonus(); }
    virtual void resetFlickBonusLr() { m_controller.resetFlickBonusLr(); }

    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventTurn(float, float, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(ftControllerModuleImpl) == 0x16C, "Class is wrong size!");
