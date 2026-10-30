#pragma once
#include <types.h>
// The capture module (soModuleAccesser slot captureModule): the slots the Enemy unit calls. Later slots are not needed here.
// HYPOTHESIS: the prefix mirrors soCatchModule (so/catch/so_catch_module.h); the Enemy constructor calls the fourth slot
// with (0, node, 0.0f) right after the catch module's setNodes. The other names come from what the Enemy's capture
// handlers (cutCapture, notifyEventCaptureStatus, notifyEventLink) do with them.
class soCaptureModule {
public:
    virtual ~soCaptureModule();
    virtual void activate();
    virtual void deactivate();
    virtual void setNodes(int, int, float);
    virtual void initInfo();                          // 0x18
    virtual bool isCapture();                         // 0x1C
    virtual int getCatchTaskId();                     // 0x20
    virtual bool capture(int taskId, int node, bool); // 0x24
    virtual void unk28();
    virtual float getCatchPosY();                     // 0x2C
    virtual void captureCut(bool, bool constrained);  // 0x30
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();                             // link event 0xB (the capture ends)
    virtual bool isThrowCut();                        // 0x40
    virtual void unk44();
    virtual void cutConstraint();                     // 0x48
    virtual void throwCut(bool constrained);          // 0x4C
    virtual void unk50();
    virtual void unk54(int);                          // link event 0xA, (0) before the capture is cut
};
