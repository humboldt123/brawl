// Havok translation unit hkCollisionAgent.o (main.dol 0x802B7A00-0x802B85EC).
// Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802B7A00     4  invalidateTim   [map: hkCollisionAgent__invalidateTim]
//   0x802B7A04     4  warpTime   [map: hkCollisionAgent__warpTime]
//   0x802B7A08     4  removePoint   [map: hkCollisionAgent__removePoint]
//   0x802B7A0C     4  commitPotential   [map: hkCollisionAgent__commitPotential]
//   0x802B7A10     4  createZombie   [map: hkCollisionAgent__createZombie]
//   0x802B7A14  1468  linearCast   [map: hkIterativeLinearCastAgent__linearCast]
//   0x802B7FD0  1564  staticLinearCast   [map: hkIterativeLinearCastAgent__staticLinearCast]

#include <havok/hkCollisionAgent.h>

// Default hooks of the base agent: empty in the original.
void hkCollisionAgent::invalidateTim(void* arg) {}
void hkCollisionAgent::warpTime(float t0, float t1, void* arg) {}
void hkCollisionAgent::removePoint(u16 key) {}
void hkCollisionAgent::commitPotential(u16 key) {}
void hkCollisionAgent::createZombie(u16 key) {}
