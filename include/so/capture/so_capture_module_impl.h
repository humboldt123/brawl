#pragma once

// Local BrawlHeaders shadow: the BrawlHeaders file only includes the capture event presenter, and units that include it
// (through ft/builder/ft_dol_types.h) declare an opaque soCaptureModuleImpl. The full class below is opt-in
// (SO_CAPTURE_MODULE_IMPL_FULL), defined by the capture module unit.
#include <so/capture/so_capture_event_presenter.h>

#ifdef SO_CAPTURE_MODULE_IMPL_FULL

// soCaptureModuleImpl (sora_melee so_capture_module_impl.o), from its constructor, its virtual table and the
// accessors.

#include <StaticAssert.h>
#include <so/capture/so_capture_event_presenter.h>
#include <so/capture/so_capture_module.h>
#include <so/event/so_event_presenter.h>
#include <types.h>

class soModuleAccesser;

class soCaptureModuleImpl : public soCaptureModule, public soEventPresenter<soCaptureEventObserver> {
public:
    soModuleAccesser* m_moduleAccesser; // 0x10
    int m_catchNode;                    // 0x14
    int m_node1;                        // 0x18: HYPOTHESIS: the setNodes nodes and value
    int m_node2;                        // 0x1C
    float unk20;                        // 0x20
    int m_motionOffset;                 // 0x24
    float unk28;                        // 0x28
    bool m_sendCutEvent;                // 0x2C
    u8 unk2D;                           // 0x2D
    bool m_isThrown;                    // 0x2E
    bool m_updatePos;                   // 0x2F
    bool m_isReaction;                  // 0x30
    u8 unk31;                           // 0x31
    bool m_ignoreCatching;              // 0x32

    soCaptureModuleImpl(soModuleAccesser* moduleAccesser);
    virtual ~soCaptureModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual void setNodes(int node1, int node2, float);
    virtual void initInfo();
    virtual bool isCapture();
    virtual int getCatchTaskId();
    virtual float getCatchPosY();
    // The slots after soCaptureModule's, in table order.
    virtual void setCatchNode(int node);
    virtual void setUpdatePos(bool updatePos);
    virtual bool isReaction();
    virtual int getMotionOffset(); // HYPOTHESIS: return type
    virtual void setIgnoreCatching(bool ignoreCatching);
    virtual bool isIgnoreCatching();
};
static_assert(sizeof(soCaptureModuleImpl) == 0x34, "Class is wrong size!");

#endif
