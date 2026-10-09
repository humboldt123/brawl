// Havok translation unit hkCapsuleTriangleAgent.o (main.dol 0x802AC30C-0x802AEE5C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802AC30C   204  registerAgent   [map: hkCapsuleTriangleAgent__registerAgent]
//   0x802AC3D8   192  createTriangleCapsuleAgent   [map: hkCapsuleTriangleAgent__createTriangleCapsuleAgent]
//   0x802AC498    92  __dt   [map: hkCapsuleTriangleAgent____dt]
//   0x802AC4F4   176  createCapsuleTriangleAgent   [map: hkCapsuleTriangleAgent__createCapsuleTriangleAgent]
//   0x802AC5A4   144  cleanup   [map: hkCapsuleTriangleAgent__cleanup]
//   0x802AC634  1552  getClosestPoints   [map: hkCapsuleTriangleAgent__getClosestPoints]
//   0x802ACC44  1588  staticGetClosestPoints   [map: hkCapsuleTriangleAgent__staticGetClosestPoints]
//   0x802AD278  1400  getPenetrations   [map: hkCapsuleTriangleAgent__getPenetrations]
//   0x802AD7F0  1424  staticGetPenetrations   [map: hkCapsuleTriangleAgent__staticGetPenetrations]
//   0x802ADD80  1624  processCollision   [map: hkCapsuleTriangleAgent__processCollision]
//   0x802AE3D8    72  getPenetrations   [map: hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent___getPenetrations]
//   0x802AE420    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent___staticGetPenetrations]
//   0x802AE468    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent___getClosestPoints]
//   0x802AE4B0    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent___staticGetClosestPoints]
//   0x802AE4F8   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent___staticLinearCast]
//   0x802AE658  1956  processCollision   [map: hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent___processCollision]
//   0x802AEDFC     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent___updateShapeCollectionFilter]
//   0x802AEE00    92  __dt   [map: hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_____dt]

#include <havok/hkCapsuleTriangleAgent.h>
#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>

// Stand-ins for the sub-object constructor and the symmetric linear cast of other units (not written yet).
extern "C" void fn_80325334(void* src, void* dst);
extern "C" void fn_802AE4F8();
extern "C" void fn_802B7FD0();

// MATCH-ONLY: the release slot (0x18 of the contact manager) takes the 16-bit slot value as its argument.
// hkContactMgr in hkCollisionAgent.h declares unk18() without one, so this local view declares it with the key.
struct hkContactMgrKeyed : hkReferencedObject {
    virtual void unk10();
    virtual void unk14();
    virtual void unk18(u16 key);
};

hkCapsuleTriangleAgent::hkCapsuleTriangleAgent(hkContactMgr* contactMgr, void* src) : hkCollisionAgent((int)contactMgr) {
    unkC[0] = 0xFFFF;
    unkC[1] = 0xFFFF;
    unkC[2] = 0xFFFF;
    fn_80325334((u8*)src + 0x10, unk14);
}

hkCapsuleTriangleAgent* hkCapsuleTriangleAgent::createTriangleCapsuleAgent(void* unk0, void* unk1, void* unk2,
                                                                          hkContactMgr* contactMgr) {
    return new hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_(contactMgr, *(void**)unk0);
}

hkCapsuleTriangleAgent* hkCapsuleTriangleAgent::createCapsuleTriangleAgent(void* unk0, void* unk1, void* unk2,
                                                                          hkContactMgr* contactMgr) {
    return new hkCapsuleTriangleAgent(contactMgr, *(void**)unk1);
}

void hkCapsuleTriangleAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs triangle;
    triangle.create = (hkAgentFunc)createTriangleCapsuleAgent;
    triangle.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_::staticGetPenetrations;
    triangle.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_::staticGetClosestPoints;
    triangle.staticLinearCast = (hkAgentFunc)fn_802AE4F8;
    triangle.symmetricA = 1;
    triangle.symmetricB = 0;
    dispatcher->registerCollisionAgent(&triangle, HK_SHAPE_TRIANGLE, HK_SHAPE_CAPSULE);

    hkAgentFuncs capsule;
    capsule.create = (hkAgentFunc)createCapsuleTriangleAgent;
    capsule.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    capsule.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    capsule.staticLinearCast = (hkAgentFunc)fn_802B7FD0;
    capsule.symmetricA = 0;
    capsule.symmetricB = 0;
    dispatcher->registerCollisionAgent(&capsule, HK_SHAPE_CAPSULE, HK_SHAPE_TRIANGLE);
}

void hkCapsuleTriangleAgent::cleanup() {
    for (int i = 0; i < 3; i++) {
        u16 key = unkC[i];
        if (key != 0xFFFF) {
            ((hkContactMgrKeyed*)unk8)->unk18(key);
        }
    }
    delete this;
}

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_::getPenetrations(void* unk0, void* unk1, void* unk2,
                                                                          void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkCapsuleTriangleAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_::staticGetPenetrations(void* unk0, void* unk1, void* unk2,
                                                                                void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkCapsuleTriangleAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_::getClosestPoints(void* unk0, void* unk1, void* unk2,
                                                                           void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkCapsuleTriangleAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_::staticGetClosestPoints(void* unk0, void* unk1, void* unk2,
                                                                                 void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkCapsuleTriangleAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}

hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_::~hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_() {}
