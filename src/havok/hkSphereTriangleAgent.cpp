// Havok translation unit hkSphereTriangleAgent.o (main.dol 0x802C6F98-0x802C8E9C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802C6F98   204  registerAgent   [map: hkSphereTriangleAgent__registerAgent]
//   0x802C7064   184  createTriangleSphereAgent   [map: hkSphereTriangleAgent__createTriangleSphereAgent]
//   0x802C711C    92  __dt   [map: hkSphereTriangleAgent____dt]
//   0x802C7178   168  createSphereTriangleAgent   [map: hkSphereTriangleAgent__createSphereTriangleAgent]
//   0x802C7220   104  cleanup   [map: hkSphereTriangleAgent__cleanup]
//   0x802C7288  1116  getClosestPoints   [map: hkSphereTriangleAgent__getClosestPoints]
//   0x802C76E4  1136  staticGetClosestPoints   [map: hkSphereTriangleAgent__staticGetClosestPoints]
//   0x802C7B54   432  getPenetrations   [map: hkSphereTriangleAgent__getPenetrations]
//   0x802C7D04   440  staticGetPenetrations   [map: hkSphereTriangleAgent__staticGetPenetrations]
//   0x802C7EBC  1240  processCollision   [map: hkSphereTriangleAgent__processCollision]
//   0x802C8394    72  getPenetrations   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___getPenetrations]
//   0x802C83DC    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___staticGetPenetrations]
//   0x802C8424    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___getClosestPoints]
//   0x802C846C    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___staticGetClosestPoints]
//   0x802C84B4   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___staticLinearCast]
//   0x802C8614  1572  processCollision   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___processCollision]
//   0x802C8C38     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___updateShapeCollectionFilter]
//   0x802C8C3C    92  __dt   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_____dt]
//   0x802C8C98   216  addCdPoint   [map: hkSymmetricAgentFlipCollector__addCdPoint]
//   0x802C8D70   216  addCdPoint   [map: hkSymmetricAgentFlipCastCollector__addCdPoint]
//   0x802C8E48    84  addCdBodyPair   [map: hkSymmetricAgentFlipBodyCollector__addCdBodyPair]

#include <havok/hkSphereTriangleAgent.h>
#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>

// Stand-in for the sub-object constructor (other unit): copies the triangle data at src into dst.
extern "C" void fn_80325184(void* src, void* dst);
// Stand-ins for the symmetric linear cast and the linear cast of other units (staticLinearCast not written yet).
extern "C" void fn_802C84B4();
extern "C" void fn_802B7FD0();

void hkSphereTriangleAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs triangle;
    triangle.create = (hkAgentFunc)createTriangleSphereAgent;
    triangle.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::staticGetPenetrations;
    triangle.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::staticGetClosestPoints;
    triangle.staticLinearCast = (hkAgentFunc)fn_802C84B4;
    triangle.symmetricA = 1;
    triangle.symmetricB = 0;
    dispatcher->registerCollisionAgent(&triangle, HK_SHAPE_TRIANGLE, HK_SHAPE_SPHERE);

    hkAgentFuncs sphere;
    sphere.create = (hkAgentFunc)createSphereTriangleAgent;
    sphere.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    sphere.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    sphere.staticLinearCast = (hkAgentFunc)fn_802B7FD0;
    sphere.symmetricA = 0;
    sphere.symmetricB = 0;
    dispatcher->registerCollisionAgent(&sphere, HK_SHAPE_SPHERE, HK_SHAPE_TRIANGLE);
}

hkSphereTriangleAgent::hkSphereTriangleAgent(hkContactMgr* contactMgr, void* src) : hkCollisionAgent((int)contactMgr) {
    unkC = 0xFFFF;
    fn_80325184((u8*)src + 0x10, unk10);
}

hkSphereTriangleAgent* hkSphereTriangleAgent::createTriangleSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSphereTriangleAgent(contactMgr, *(void**)unk0);
}

hkSphereTriangleAgent* hkSphereTriangleAgent::createSphereTriangleAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_(contactMgr, *(void**)unk1);
}

void hkSphereTriangleAgent::cleanup() {
    if (unkC != 0xFFFF) {
        ((hkContactMgr*)unk8)->unk18();
    }
    delete this;
}

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkSphereTriangleAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkSphereTriangleAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkSphereTriangleAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkSphereTriangleAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}

hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::~hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_() {}
