#pragma once
#include <types.h>
// Virtual prefix verified against soCatchModuleImpl's constructor-installed
// table at sora_melee data 0x7440. Later slots are not needed here.
class soCatchModule {
public:
    virtual ~soCatchModule();
    virtual void activate();
    virtual void deactivate();
    virtual void setNodes(int, int, float);
    virtual void initInfo();
    virtual void setCatch(int taskId);
    virtual bool isCatch();
    virtual void onCatchFlag();
    virtual void offCatchFlag();
    // HYPOTHESIS: both zero/one-only control arguments are bool; original spelling unknown.
    virtual void catchCut(bool, bool);
#ifdef SO_CATCH_MODULE_CLING_SLOTS
    // Opt-in (default off): the cling slots after catchCut, named from the sora_melee map entries of soCatchModuleImpl's
    // table. clingCut reads its argument (r4) in sora_melee.
    virtual void updatePosCling();
    virtual void updateLRCling();
    // HYPOTHESIS: the argument is a bool (callers pass 0).
    virtual void clingCut(bool);
    virtual bool checkDamage();
    virtual void setSendCutEvent(bool sendCutEvent);
#endif
};
