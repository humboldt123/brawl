// Havok translation unit hkMultiSphereAgent.o (main.dol 0x802BABC4-0x802C0590).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802BABC4   204  registerAgent   [map: hkMultiSphereAgent__registerAgent]
//   0x802BAC90   124  createListAAgent   [map: hkMultiSphereAgent__createListAAgent]
//   0x802BAD0C   132  createListBAgent   [map: hkMultiSphereAgent__createListBAgent]
//   0x802BAD90   152  __dt   [map: hkMultiSphereAgent____dt]
//   0x802BAEB8  1172  __ct   [map: hkMultiSphereAgent____ct]
//   0x802BB3A8   148  cleanup   [map: hkMultiSphereAgent__cleanup]
//   0x802BB43C  1112  processCollision   [map: hkMultiSphereAgent__processCollision]
//   0x802BB894  1016  getClosestPoints   [map: hkMultiSphereAgent__getClosestPoints]
//   0x802BBC8C  1056  staticGetClosestPoints   [map: hkMultiSphereAgent__staticGetClosestPoints]
//   0x802BC0AC  1020  linearCast   [map: hkMultiSphereAgent__linearCast]
//   0x802BC4A8  1064  staticLinearCast   [map: hkMultiSphereAgent__staticLinearCast]
//   0x802BC8D0  1040  getPenetrations   [map: hkMultiSphereAgent__getPenetrations]
//   0x802BCCE0  1080  staticGetPenetrations   [map: hkMultiSphereAgent__staticGetPenetrations]
//   0x802BD118   352  linearCast   [map: hkSymmetricAgent_18hkMultiSphereAgent___linearCast]
//   0x802BD278    72  getPenetrations   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___getPenetrations]
//   0x802BD2C0    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___staticGetPenetrations]
//   0x802BD308    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___getClosestPoints]
//   0x802BD350    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___staticGetClosestPoints]
//   0x802BD398   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___staticLinearCast]
//   0x802BD4F8   412  processCollision   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___processCollision]
//   0x802BD694     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___updateShapeCollectionFilter]
//   0x802BD698   160  __dt   [map: hkSymmetricAgent_18hkMultiSphereAgent_____dt]
//   0x802BD738   204  registerAgent   [map: hkMultiSphereTriangleAgent__registerAgent]
//   0x802BD804   212  createTriangleMultiSphereAgent   [map: hkMultiSphereTriangleAgent__createTriangleMultiSphereAgent]
//   0x802BD8D8    92  __dt   [map: hkMultiSphereTriangleAgent____dt]
//   0x802BD934   196  createMultiSphereTriangleAgent   [map: hkMultiSphereTriangleAgent__createMultiSphereTriangleAgent]
//   0x802BD9F8   144  cleanup   [map: hkMultiSphereTriangleAgent__cleanup]
//   0x802BDA88  1692  getClosestPoints   [map: hkMultiSphereTriangleAgent__getClosestPoints]
//   0x802BE124  1696  staticGetClosestPoints   [map: hkMultiSphereTriangleAgent__staticGetClosestPoints]
//   0x802BE7C4  1432  getPenetrations   [map: hkMultiSphereTriangleAgent__getPenetrations]
//   0x802BED5C  1440  staticGetPenetrations   [map: hkMultiSphereTriangleAgent__staticGetPenetrations]
//   0x802BF2FC  1844  processCollision   [map: hkMultiSphereTriangleAgent__processCollision]
//   0x802BFA30    72  getPenetrations   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___getPenetrations]
//   0x802BFA78    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___staticGetPenetrations]
//   0x802BFAC0    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___getClosestPoints]
//   0x802BFB08    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___staticGetClosestPoints]
//   0x802BFB50   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___staticLinearCast]
//   0x802BFCB0  2176  processCollision   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___processCollision]
//   0x802C0530     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___updateShapeCollectionFilter]
//   0x802C0534    92  __dt   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent_____dt]

#include <havok/hkMultiSphereAgent.h>

hkMultiSphereAgent* hkMultiSphereAgent::createListAAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkMultiSphereAgent(unk0, unk1, unk2, contactMgr);
}

hkMultiSphereAgent* hkMultiSphereAgent::createListBAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSymmetricAgent_18hkMultiSphereAgent_(unk1, unk0, unk2, contactMgr);
}

void hkMultiSphereAgent::cleanup() {
    for (int i = 0; i < m_count; i++) {
        m_entries[i].agent->cleanup();
    }
    delete this;
}
