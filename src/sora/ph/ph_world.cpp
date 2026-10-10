// Brawl physics wrapper translation unit ph_world.o (main.dol 0x8009A048-0x8009A104).
// Not yet decompiled. Functions in address order with their map names:
//   0x8009A048    24  __ct   [map: phWorld____ct]
//   0x8009A060    64  __dt   [map: phWorld____dt]
//   0x8009A0A0    92  __dt   [map: hkWorldCinfo____dt]
//   0x8009A0FC     8  stepTime   [map: phWorld__stepTime]
#include <ph/ph_world.h>

// Local declaration (MARKED): hkWorld::stepDeltaTime is not declared in include/havok/hkWorld.h yet.
struct hkWorld : hkReferencedObject {
    void stepDeltaTime(hkReal dt);
};

phWorld::phWorld() {
    m_hkWorld = NULL;
}

phWorld::~phWorld() {
}

void phWorld::stepTime(hkReal dt) {
    m_hkWorld->stepDeltaTime(dt);
}
