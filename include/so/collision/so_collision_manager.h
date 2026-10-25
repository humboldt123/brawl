#pragma once

// Local header (shadows the BrawlHeaders copy): merges upstream's declaration of the class layout (a gfTask with the
// event manage module at +0x8D4, doldecomp/brawl #152) with the log-group members and the singleton helper our fighter
// code uses (sora map: soCollisionManager::soCollisionManager; the fighter manager's setMode creates it).

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <so/event/so_event_manage_module_impl.h>
#include <sr/sr_common.h>
#include <types.h>

class soCollisionManager : public gfTask {
    u8 unk40[0x894];
    soEventManageModuleImpl unk8D4;
    u8 unk8E8[0xC20];
public:
    soCollisionManager();

    soEventManageModule& getEventManageModule() { return unk8D4; }

    static soCollisionManager* get() {
        if (!g_soCollisionManager)
            g_soCollisionManager = new (Heaps::System) soCollisionManager;
        return g_soCollisionManager;
    }

    // HYPOTHESIS: the lazy singleton getter is emitted in the REL that uses it (the fighters keep log groups).
    static soCollisionManager* getInstance();
    int allocateLogGroup();
    void freeLogGroup(int logGroup);
    void leaveFreeRefLogGroup(int logGroup);

    static soCollisionManager* g_soCollisionManager;
};
static_assert(sizeof(soCollisionManager) == 0x1508, "Class is the wrong size!");

// HYPOTHESIS: name of the singleton pointer as the fighter units use it (created lazily by ftManager::setMode)
extern soCollisionManager* g_soCollisionManager;
