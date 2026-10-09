// Havok translation unit hkGskfAgent.o (main.dol 0x802B2870-0x802B3694).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802B2870    68  __ct   [map: hkGskfAgent____ct]
//   0x802B28B4   220  createGskfAgent   [map: hkGskfAgent__createGskfAgent]
//   0x802B2990    84  cleanup   [map: hkGskfAgent__cleanup]
//   0x802B29E4    64  removePoint   [map: hkGskfAgent__removePoint]
//   0x802B2A24    68  commitPotential   [map: hkGskfAgent__commitPotential]
//   0x802B2A68    60  createZombie   [map: hkGskfAgent__createZombie]
//   0x802B2AA4  2392  processCollisionNoTim   [map: hkGskfAgent__processCollisionNoTim]
//   0x802B33FC   664  processCollision   [map: hkGskfAgent__processCollision]

#include <havok/hkGskfAgent.h>

// Stand-ins for the point list helpers (other units, not recovered yet). The list is the object at 0x30.
extern "C" void fn_8031830C(void* list, hkContactMgr* contactMgr);
extern "C" void fn_8031590C(void* list, int index);

#pragma dont_inline on
hkGskfAgent::hkGskfAgent(hkCdBody* bodyA, hkCdBody* bodyB, hkContactMgr* contactMgr)
    : hkGskBaseAgent(bodyA, bodyB, contactMgr) {
    unk30 = 0;
}
#pragma dont_inline reset

hkGskBaseAgent* hkGskfAgent::createGskfAgent(hkCdBody* bodyA, hkCdBody* bodyB, void* unk2, hkContactMgr* contactMgr) {
    hkGskBaseAgent* agent;
    if (contactMgr != 0) {
        agent = new hkGskfAgent(bodyA, bodyB, contactMgr);
    } else {
        agent = new hkGskBaseAgent(bodyA, bodyB, contactMgr);
    }
    return agent;
}

void hkGskfAgent::cleanup() {
    fn_8031830C(&unk30, (hkContactMgr*)unk8);
    delete this;
}

void hkGskfAgent::removePoint(u16 key) {
    int count = pointCount();
    u8* p = (u8*)this;
    for (int i = 0; i < count; i++, p += 8) {
        if (key == *(u16*)(p + 0x36)) {
            fn_8031590C(&unk30, i);
            return;
        }
    }
}

void hkGskfAgent::commitPotential(u16 key) {
    int count = pointCount();
    u8* p = (u8*)this;
    for (int i = 0; i < count; i++, p += 8) {
        if (*(u16*)(p + 0x36) == 0xFFFF) {
            *(u16*)((u8*)this + i * 8 + 0x36) = key;
            return;
        }
    }
}

void hkGskfAgent::createZombie(u16 key) {
    int count = pointCount();
    hkGskfPoint* p = points();
    for (int i = 0; i < count; i++, p++) {
        if (p->key == key) {
            p->unk0 = 0;
            p->unk1 = 0;
            return;
        }
    }
}
