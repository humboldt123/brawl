#pragma once

#define SO_CATCH_MODULE_CLING_SLOTS
#include <so/catch/so_catch_module.h>

inline soCatchModule::~soCatchModule() { }

class soCatchModuleNull : public soCatchModule {
public:
    virtual ~soCatchModuleNull();
    virtual void activate();
    virtual void deactivate();
    virtual void setNodes(int, int, float);
    virtual void initInfo();
    virtual void setCatch(int taskId);
    virtual bool isCatch();
    virtual void onCatchFlag();
    virtual void offCatchFlag();
    virtual void catchCut(bool, bool);
    virtual void updatePosCling();
    virtual void updateLRCling();
    virtual void clingCut(bool);
    virtual bool checkDamage();
    virtual void setSendCutEvent(bool sendCutEvent);
    virtual bool check();
};
