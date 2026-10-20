#pragma once

#include <ft/fighter.h>

// Minimal view of Kirby for the status processes of his REL: the real class is declared in ft_kirby.h, whose builder headers cannot be
// combined with the status headers. Only the non-virtual members the statuses call are listed.
class ftKirby : public Fighter {
public:
    int getCopyKind();
    int getCopySlotNo();
    void copySetup(soModuleAccesser* moduleAccesser, int slotNo, int copyKind, bool effect);
    void* getExtendParam();
};
