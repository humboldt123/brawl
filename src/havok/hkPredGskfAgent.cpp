// Havok translation unit hkPredGskfAgent.o (main.dol 0x802B3694-0x802B4524).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802B3694   108  registerAgent   [map: hkPredGskfAgent__registerAgent]
//   0x802B3700   228  createPredGskfAgent   [map: hkPredGskfAgent__createPredGskfAgent]
//   0x802B37E4  3392  processCollision   [map: hkPredGskfAgent__processCollision]

#include <havok/hkPredGskfAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>

void hkPredGskfAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs funcs;
    funcs.create = (hkAgentFunc)createPredGskfAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)hkGskBaseAgent::staticGetPenetrations;
    funcs.staticGetClosestPoints = (hkAgentFunc)hkGskBaseAgent::staticGetClosestPoints;
    funcs.staticLinearCast = (hkAgentFunc)hkIterativeLinearCastAgent::staticLinearCast;
    funcs.symmetricA = 0;
    funcs.symmetricB = 1;
    dispatcher->registerCollisionAgent(&funcs, HK_SHAPE_CONVEX, HK_SHAPE_CONVEX);
}

hkGskBaseAgent* hkPredGskfAgent::createPredGskfAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                     hkCollisionInput& input, hkContactMgr* contactMgr) {
    if (contactMgr != 0) {
        return new hkPredGskfAgent(bodyA, bodyB, contactMgr);
    }
    return new hkGskBaseAgent(bodyA, bodyB, contactMgr);
}
