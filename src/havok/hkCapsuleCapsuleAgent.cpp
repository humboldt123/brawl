// Havok translation unit hkCapsuleCapsuleAgent.o (main.dol 0x802AA9B8-0x802AC30C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802AA9B8   128  createCapsuleCapsuleAgent   [map: hkCapsuleCapsuleAgent__createCapsuleCapsuleAgent]
//   0x802AAA38   104  registerAgent   [map: hkCapsuleCapsuleAgent__registerAgent]
//   0x802AAAA0   144  cleanup   [map: hkCapsuleCapsuleAgent__cleanup]
//   0x802AAB30    92  __dt   [map: hkCapsuleCapsuleAgent____dt]
//   0x802AAB8C  1392  getClosestPoints   [map: hkCapsuleCapsuleAgent__getClosestPoints]
//   0x802AB0FC  1392  staticGetClosestPoints   [map: hkCapsuleCapsuleAgent__staticGetClosestPoints]
//   0x802AB66C  1540  staticGetPenetrations   [map: hkCapsuleCapsuleAgent__staticGetPenetrations]
//   0x802ABC70    20  getPenetrations   [map: hkCapsuleCapsuleAgent__getPenetrations]
//   0x802ABC84  1672  processCollision   [map: hkCapsuleCapsuleAgent__processCollision]

#include <havok/hkCapsuleCapsuleAgent.h>
#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>

// Stand-in for the linear cast of another unit (not written yet).
extern "C" void fn_802B7FD0();

hkCapsuleCapsuleAgent* hkCapsuleCapsuleAgent::createCapsuleCapsuleAgent(void* unk0, void* unk1, void* unk2,
                                                                       hkContactMgr* contactMgr) {
    return new hkCapsuleCapsuleAgent(contactMgr);
}

void hkCapsuleCapsuleAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs capsule;
    capsule.create = (hkAgentFunc)createCapsuleCapsuleAgent;
    capsule.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    capsule.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    capsule.staticLinearCast = (hkAgentFunc)fn_802B7FD0;
    capsule.symmetricA = 0;
    capsule.symmetricB = 0;
    dispatcher->registerCollisionAgent(&capsule, HK_SHAPE_CAPSULE, HK_SHAPE_CAPSULE);
}

// MATCH-ONLY: the release slot (0x18 of the contact manager) takes the 16-bit slot value as its argument.
// hkContactMgr in hkCollisionAgent.h declares unk18() without one, so this local view declares it with the key.
struct hkContactMgrKeyed : hkReferencedObject {
    virtual void unk10();
    virtual void unk14();
    virtual void unk18(u16 key);
};

void hkCapsuleCapsuleAgent::cleanup() {
    for (int i = 0; i < 3; i++) {
        u16 key = unkC[i];
        if (key != 0xFFFF) {
            ((hkContactMgrKeyed*)unk8)->unk18(key);
        }
    }
    delete this;
}

void hkCapsuleCapsuleAgent::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}
