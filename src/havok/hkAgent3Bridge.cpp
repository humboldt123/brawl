// Havok translation unit hkAgent3Bridge.o (main.dol 0x802CB484-0x802CB630).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802CB484   428  registerAllAgents   [map: hkAgentRegisterUtil__registerAllAgents]

// TODO: cleanup (shape from jj-link/brawl fn_802CB484)
// fn_802CB484: body from https://github.com/jj-link/brawl src/havok/fn_802CB484.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkBoxBoxAgent.h>
#include <havok/hkBoxBoxAgent3.h>
#include <havok/hkBvAgent.h>
#include <havok/hkBvTreeAgent.h>
#include <havok/hkBvTreeStreamAgent.h>
#include <havok/hkCapsuleCapsuleAgent.h>
#include <havok/hkCapsuleTriangleAgent.h>
#include <havok/hkConvexListAgent.h>
#include <havok/hkHeightFieldAgent.h>
#include <havok/hkListAgent.h>
#include <havok/hkMultiRayConvexAgent.h>
#include <havok/hkMultiSphereAgent.h>
#include <havok/hkMoppAgent.h>
#include <havok/hkPhantomAgent.h>
#include <havok/hkPredGskAgent3.h>
#include <havok/hkPredGskCylinderAgent3.h>
#include <havok/hkPredGskfAgent.h>
#include <havok/hkShapeCollectionAgent.h>
#include <havok/hkSphereBoxAgent.h>
#include <havok/hkSphereCapsuleAgent.h>
#include <havok/hkSphereSphereAgent.h>
#include <havok/hkSphereTriangleAgent.h>
#include <havok/hkTransformAgent.h>
namespace jj_fn_802CB484 {
union ByteSlot { unsigned char b; int align; };
extern "C" {
void fn_802D4EC0(void*);
void fn_802FB894(void*);
void fn_802CB484(void* p) {
 ByteSlot a,b,c,d,e,f;
 fn_802D4EC0(p);
 hkBvAgent::registerAgent((void*)(p));
 hkMultiSphereAgent::registerAgent((hkCollisionDispatcher*)(p));
 a.b=0; ((hkCollisionDispatcher*)(p))->setEnableChecks(*(const hkBool*)(&a.b));
 hkBvTreeAgent::registerAgent((hkCollisionDispatcher*)(p)); hkMoppAgent::registerAgent((hkCollisionDispatcher*)(p)); hkBvTreeStreamAgent::registerAgent((void*)(p)); hkMoppBvTreeStreamAgent::registerAgent((void*)(p));
 b.b=1; ((hkCollisionDispatcher*)(p))->setEnableChecks(*(const hkBool*)(&b.b));
 c.b=0; ((hkCollisionDispatcher*)(p))->setEnableChecks(*(const hkBool*)(&c.b));
 hkShapeCollectionAgent::registerAgent((void*)(p));
 d.b=1; ((hkCollisionDispatcher*)(p))->setEnableChecks(*(const hkBool*)(&d.b));
 hkTransformAgent::registerAgent((hkCollisionDispatcher*)(p)); hkPhantomAgent::registerAgent((hkCollisionDispatcher*)(p));
 e.b=0; ((hkCollisionDispatcher*)(p))->setEnableChecks(*(const hkBool*)(&e.b));
 hkListAgent::registerAgent((void*)(p));
 f.b=1; ((hkCollisionDispatcher*)(p))->setEnableChecks(*(const hkBool*)(&f.b));
 hkConvexListAgent::registerAgent((void*)(p)); hkBvTreeStreamAgent::registerConvexListAgent((void*)(p)); hkHeightFieldAgent::registerAgent((hkCollisionDispatcher*)(p)); hkPredGskfAgent::registerAgent((hkCollisionDispatcher*)(p));
 hkPredGskAgent3::registerAgent3((hkCollisionDispatcher*)(p), 1, 1); hkPredGskCylinderAgent3::registerAgent3((hkCollisionDispatcher*)(p)); hkBoxBoxAgent::registerAgent((hkCollisionDispatcher*)(p)); hkBoxBoxAgent3::registerAgent3((hkCollisionDispatcher*)(p));
 hkSphereSphereAgent::registerAgent((hkCollisionDispatcher*)(p)); hkSphereCapsuleAgent::registerAgent((hkCollisionDispatcher*)(p)); hkSphereTriangleAgent::registerAgent((hkCollisionDispatcher*)(p));
 hkPredGskAgent3::registerAgent3((hkCollisionDispatcher*)(p), 6, 4); hkPredGskAgent3::registerAgent3((hkCollisionDispatcher*)(p), 4, 6);
 hkSphereBoxAgent::registerAgent((hkCollisionDispatcher*)(p)); hkCapsuleCapsuleAgent::registerAgent((hkCollisionDispatcher*)(p)); hkCapsuleTriangleAgent::registerAgent((hkCollisionDispatcher*)(p)); fn_802FB894(p);
 hkMultiSphereTriangleAgent::registerAgent((hkCollisionDispatcher*)(p)); hkMultiRayConvexAgent::registerAgent((hkCollisionDispatcher*)(p)); hkBvTreeStreamAgent::registerMultiRayAgent((void*)(p));
}
}
} // namespace jj_fn_802CB484
