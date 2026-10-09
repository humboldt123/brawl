// Havok translation unit hkListAgent.o (main.dol 0x802B85EC-0x802B8E78).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802B85EC   204  registerAgent   [map: hkListAgent__registerAgent]
//   0x802B86B8   132  createListAAgent   [map: hkListAgent__createListAAgent]
//   0x802B873C   124  createListBAgent   [map: hkListAgent__createListBAgent]
//   0x802B87B8   132  __ct   [map: hkListAgent____ct]
//   0x802B883C    88  cleanup   [map: hkListAgent__cleanup]
//   0x802B8894     8  invalidateTim   [map: hkListAgent__invalidateTim]
//   0x802B889C     8  warpTime   [map: hkListAgent__warpTime]
//   0x802B88A4   832  processCollision   [map: hkListAgent__processCollision]
//   0x802B8BE4    20  getClosestPoints   [map: hkListAgent__getClosestPoints]
//   0x802B8BF8   176  staticGetClosestPoints   [map: hkListAgent__staticGetClosestPoints]
//   0x802B8CA8    24  linearCast   [map: hkListAgent__linearCast]
//   0x802B8CC0   176  staticLinearCast   [map: hkListAgent__staticLinearCast]
//   0x802B8D70    20  getPenetrations   [map: hkListAgent__getPenetrations]
//   0x802B8D84   176  staticGetPenetrations   [map: hkListAgent__staticGetPenetrations]
//   0x802B8E34    68  updateShapeCollectionFilter   [map: hkListAgent__updateShapeCollectionFilter]

#include <havok/hkListAgent.h>

hkListAgent* hkListAgent::createListAAgent(void* a, void* b, void* c, int d) {
    hkListAgent* agent = (hkListAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkListAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkListAgent);
    new (agent) hkListAgentVariant(b, a, c, d);
    return agent;
}

hkListAgent* hkListAgent::createListBAgent(void* a, void* b, void* c, int d) {
    hkListAgent* agent = (hkListAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkListAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkListAgent);
    new (agent) hkListAgent(a, b, c, d);
    return agent;
}

// Stand-ins for the member functions of the embedded object at 0x10 (other unit, not recovered yet).
extern "C" void fn_802FC824(void* self, void* arg);
extern "C" void fn_802FC988(void* self, float t0, float t1, void* arg);
extern "C" void fn_802FC6AC(void* subObject, int pairData, int unk8);
extern "C" void fn_802FEA54(void* subObject, void* args);

// Argument block handed to fn_802FEA54 by updateShapeCollectionFilter (lives on the stack there).
struct FilterArgs {
    void* a;      // 0x00
    void* b;      // 0x04
    u32 bValue;   // 0x08 first word of b
    void* d;      // 0x0C
    int unk8;     // 0x10
};

void hkListAgent::cleanup() {
    fn_802FC6AC(m_subObject, unkC, unk8);
    delete this;
}

void hkListAgent::updateShapeCollectionFilter(void* a, void* b, void* d) {
    FilterArgs args;
    u32 first = *(u32*)b;
    args.a = a;
    args.b = b;
    args.d = d;
    args.bValue = first;
    args.unk8 = unk8;
    fn_802FEA54(m_subObject, &args);
}

void hkListAgent::invalidateTim(void* arg) {
    fn_802FC824((char*)this + 0x10, arg);
}

void hkListAgent::warpTime(float t0, float t1, void* arg) {
    fn_802FC988((char*)this + 0x10, t0, t1, arg);
}

void hkListAgent::getClosestPoints(void* a, void* b, void* c, void* target) {
    staticGetClosestPoints(a, b, c, (hkPenetrationTarget*)target);
}

void hkListAgent::getPenetrations(void* a, void* b, void* c, void* target) {
    staticGetPenetrations(a, b, c, (hkPenetrationTarget*)target);
}

void hkListAgent::linearCast(void* a, void* b, void* c, void* target, void* d) {
    staticLinearCast(a, b, c, (hkPenetrationTarget*)target, d);
}
