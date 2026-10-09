// Havok translation unit hkNullAgent.o (main.dol 0x802C0590-0x802C068C).
// Functions in address order (method names from the Havok TU map):
//   0x802C0590    32  __ct   [map: hkNullAgent____ct]
//   0x802C05B0     4  staticGetClosestPoints   [map: hkNullAgent__staticGetClosestPoints]
//   0x802C05B4     4  staticGetPenetrations   [map: hkNullAgent__staticGetPenetrations]
//   0x802C05B8     4  staticLinearCast   [map: hkNullAgent__staticLinearCast]
//   0x802C05BC    12  createNullAgent   [map: hkNullAgent__createNullAgent]
//   0x802C05C8    12  getNullAgent   [map: hkNullAgent__getNullAgent]
//   0x802C05D4     4  cleanup   [map: hkNullAgent__cleanup]
//   0x802C05D8     4  processCollision   [map: hkNullAgent__processCollision]
//   0x802C05DC     4  linearCast   [map: hkNullAgent__linearCast]
//   0x802C05E0     4  getClosestPoints   [map: hkNullAgent__getClosestPoints]
//   0x802C05E4     4  getPenetrations   [map: hkNullAgent__getPenetrations]
//   0x802C05E8    92  __dt   [map: hkNullAgent____dt]
//   0x802C0644    72  __sinit_\hkNullAgent_cpp   [map: hkNullAgentcpp____sinit_]

#include <havok/hkNullAgent.h>

hkNullAgent::hkNullAgent() {}

static hkNullAgent s_nullAgent;

void hkNullAgent::staticGetClosestPoints() {}
void hkNullAgent::staticGetPenetrations() {}
void hkNullAgent::staticLinearCast() {}

hkNullAgent* hkNullAgent::createNullAgent() {
    return &s_nullAgent;
}

hkNullAgent* hkNullAgent::getNullAgent() {
    return &s_nullAgent;
}

void hkNullAgent::cleanup() {}
void hkNullAgent::processCollision(void* a, void* b, void* c) {}
void hkNullAgent::linearCast(void* a, void* b, void* c, void* target, void* d) {}
void hkNullAgent::getClosestPoints(void* a, void* b, void* c, void* target) {}
void hkNullAgent::getPenetrations(void* a, void* b, void* c, void* target) {}

hkNullAgent::~hkNullAgent() {}
