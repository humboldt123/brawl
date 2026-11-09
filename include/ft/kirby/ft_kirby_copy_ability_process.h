#pragma once

#include <types.h>

class soModuleAccesser;
class soCollisionLog;
class soCollisionAttackModule;

// Primary base of the Yoshi and Wario transactors, verified by their RTTI and the
// default functions in its vtable. Only the hooks used here are reconstructed;
// HYPOTHESIS: argument types on the unused unk slots retain the old interface
// view until their callers are audited. Do not call those slots through this view.
class ftKirbyCopyAbilityProcess {
public:
    virtual ~ftKirbyCopyAbilityProcess() { }
    virtual void init(soModuleAccesser* moduleAccesser);
    virtual void exit(soModuleAccesser* moduleAccesser);
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    virtual void unk4(soModuleAccesser* moduleAccesser);
    virtual void processFixPosition(soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeSituation(int kind, int prevKind, soModuleAccesser* moduleAccesser); // slot 6, named from Peach's override
    virtual void unk7(soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser); // slot 8
    virtual bool notifyEventCollisionShieldCheck(soModuleAccesser* moduleAccesser); // slot 9
    virtual void notifyEventCollisionSearch(soCollisionLog* log, soModuleAccesser* moduleAccesser);
    virtual bool unk11(soModuleAccesser* moduleAccesser);
    virtual bool activeArticle(class soArticle* article, soModuleAccesser* moduleAccesser);
    virtual bool unk13(soModuleAccesser* moduleAccesser);
    virtual void unk14(soModuleAccesser* moduleAccesser);
    virtual void unk15(soModuleAccesser* moduleAccesser);
    virtual bool unk16(soModuleAccesser* moduleAccesser);
    virtual void unk17(soModuleAccesser* moduleAccesser);
};
