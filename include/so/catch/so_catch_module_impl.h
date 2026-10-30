#pragma once

// soCatchModuleImpl (sora_melee so_catch_module_impl.o), from its constructor, its virtual table and the flag
// accessors. Not for units that include ft/builder/ft_dol_types.h, which declares an opaque soCatchModuleImpl.

#include <StaticAssert.h>
#include <so/catch/so_catch_module.h>
#include <so/collision/so_collision_catch_event_presenter.h>
#include <types.h>

class soModuleAccesser;

class soCatchModuleImpl : public soCatchModule, public soCollisionCatchEventObserver {
public:
    soModuleAccesser* m_moduleAccesser; // 0x10
    int m_node1;                        // 0x14: HYPOTHESIS: the first setNodes node
    int m_node2;                        // 0x18
    float unk1C;                        // 0x1C
    int m_kind;                         // 0x20: the constructor's second argument
    u8 unk24[0x2C];                     // 0x24
    void* unk50;                        // 0x50: the constructor stores the module itself
    float unk54;                        // 0x54: initInfo resets it to the largest float
    u8 unk58[0x8];                      // 0x58
    bool m_sendCutEvent;                // 0x60
    bool m_catchFlag;                   // 0x61

    soCatchModuleImpl(soModuleAccesser* moduleAccesser, int kind);
    virtual ~soCatchModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual void setNodes(int node1, int node2, float);
    virtual void initInfo();
    virtual bool isCatch();
    virtual void onCatchFlag();
    virtual void offCatchFlag();
    // The slots after soCatchModule's, in table order.
    // HYPOTHESIS: parameter lists of the cling and check slots are not verified.
    virtual void updatePosCling();
    virtual void updateLRCling();
    virtual void clingCut();
    virtual void checkDamage();
    virtual void setSendCutEvent(bool sendCutEvent);
    virtual void check();
};
static_assert(sizeof(soCatchModuleImpl) == 0x64, "Class is wrong size!");
