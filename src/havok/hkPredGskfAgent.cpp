// Havok translation unit hkPredGskfAgent.o (main.dol 0x802B3694-0x802B4524).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802B3694   108  registerAgent   [map: hkPredGskfAgent__registerAgent]
//   0x802B3700   228  createPredGskfAgent   [map: hkPredGskfAgent__createPredGskfAgent]
//   0x802B37E4  3392  processCollision   [map: hkPredGskfAgent__processCollision]

#include <havok/hkPredGskfAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>

// Stand-ins for the static query functions of hkGskBaseAgent and hkIterativeLinearCastAgent (other units, not recovered).
extern "C" void fn_802B2490();
extern "C" void fn_802B1C3C();
extern "C" void fn_802B7FD0();

void hkPredGskfAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs funcs;
    funcs.symmetricA = 0;
    funcs.symmetricB = 1;
    funcs.create = (hkAgentFunc)createPredGskfAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)fn_802B2490;
    funcs.staticGetClosestPoints = (hkAgentFunc)fn_802B1C3C;
    funcs.staticLinearCast = (hkAgentFunc)fn_802B7FD0;
    dispatcher->registerCollisionAgent(&funcs, HK_SHAPE_CONVEX, HK_SHAPE_CONVEX);
}
