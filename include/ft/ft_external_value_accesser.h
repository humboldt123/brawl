// Local BrawlHeaders shadow: expose the fighter collision-hit accessor.
#pragma once

#include <StaticAssert.h>
#include <ft/fighter.h>
#include <types.h>

class soDamageModule;

namespace ftExternalValueAccesser {
    soCollisionHitModule* getsoCollisionHitModule(Fighter* fighter);
    soDamageModule* getDamageModule(Fighter* fighter);
    float getWeight(Fighter* fighter);
    Vec3f getHipPos(Fighter* fighter);
    Vec3f getCursorPos(Fighter* fighter);
    int getNoDiscretionFinalCount();
    // HYPOTHESIS: parameter and return types; the fighter's Final Smash hit target entry for the index, 0 outside the table.
    int getHitTarget(Fighter* fighter, int index);
    // HYPOTHESIS: maps a hit number of the attack module to the model node it hits (used by the Pikmin chain-up).
    int getNodeIdFromHitNo(u8 hitNo);
}