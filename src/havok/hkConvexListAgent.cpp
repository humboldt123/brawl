// Havok translation unit hkConvexListAgent.o (main.dol 0x802AEE5C-0x802B1938).
// Functions in address order (method names from the Havok TU map):
//   0x802AEE5C   260  getSupportingVertex   [map: hkConvexListConvexShape__getSupportingVertex]
//   0x802AEF60   152  convertVertexIdsToVertices   [map: hkConvexListConvexShape__convertVertexIdsToVertices]
//   0x802AEFF8    24  getFirstVertex   [map: hkConvexListConvexShape__getFirstVertex]
//   0x802AF010   160  getCollisionSpheresInfo   [map: hkConvexListConvexShape__getCollisionSpheresInfo]
//   0x802AF0B0   152  getCollisionSpheres   [map: hkConvexListConvexShape__getCollisionSpheres]
//   0x802AF148   312  __ct   [map: hkConvexListAgent____ct]
//   0x802AF2DC    92  __dt   [map: hkGskfAgent____dt]
//   0x802AF3F0   208  createConvexListAgent   [map: hkConvexListAgent__createConvexListAgent]
//   0x802AF4C0   224  createListConvexAgent   [map: hkConvexListAgent__createListConvexAgent]
//   0x802AF5A0    92  __dt   [map: hkConvexListAgent____dt]
//   0x802AF5FC   156  __dt   [map: hkListAgent____dt]
//   0x802AF698   204  registerAgent   [map: hkConvexListAgent__registerAgent]
//   0x802AF764    24  invalidateTim   [map: hkConvexListAgent__invalidateTim]
//   0x802AF77C    24  warpTime   [map: hkConvexListAgent__warpTime]
//   0x802AF794    20  removePoint   [map: hkConvexListAgent__removePoint]
//   0x802AF7A8    20  commitPotential   [map: hkConvexListAgent__commitPotential]
//   0x802AF7BC    20  createZombie   [map: hkConvexListAgent__createZombie]
//   0x802AF7D0   820  staticGetClosestPoints   [map: hkConvexListAgent__staticGetClosestPoints]
//   0x802AFB60    20  getClosestPoints   [map: hkConvexListAgent__getClosestPoints]
//   0x802AFB74   444  staticGetPenetrations   [map: hkConvexListAgent__staticGetPenetrations]
//   0x802AFD30    20  getPenetrations   [map: hkConvexListAgent__getPenetrations]
//   0x802AFD44   488  staticLinearCast   [map: hkConvexListAgent__staticLinearCast]
//   0x802AFF2C    24  linearCast   [map: hkConvexListAgent__linearCast]
//   0x802AFF44    80  updateShapeCollectionFilter   [map: hkConvexListAgent__updateShapeCollectionFilter]
//   0x802AFF94   116  switchToStreamMode   [map: hkConvexListAgent__switchToStreamMode]
//   0x802B0008    72  switchToGskMode   [map: hkConvexListAgent__switchToGskMode]
//   0x802B0050   116  cleanup   [map: hkConvexListAgent__cleanup]
//   0x802B00C4  3000  processCollision   [map: hkConvexListAgent__processCollision]
//   0x802B0D0C     8  getNumVertices   [map: hkConvexShape__getNumVertices]
//   0x802B0D14     8  castRay   [map: hkConvexListConvexShape__castRay]
//   0x802B0D1C     4  getAabb   [map: hkConvexListConvexShape__getAabb]
//   0x802B0D20   352  linearCast   [map: hkSymmetricAgent_11hkListAgent___linearCast]
//   0x802B0E80   352  linearCast   [map: hkSymmetricAgent_17hkConvexListAgent___linearCast]
//   0x802B0FE0    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_22hkShapeCollectionAgent___staticGetPenetrations]
//   0x802B1028    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_22hkShapeCollectionAgent___staticGetClosestPoints]
//   0x802B1070   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_22hkShapeCollectionAgent___staticLinearCast]
//   0x802B11D0    72  getPenetrations   [map: hkSymmetricAgentLinearCast_11hkListAgent___getPenetrations]
//   0x802B1218    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_11hkListAgent___getClosestPoints]
//   0x802B1260   412  processCollision   [map: hkSymmetricAgentLinearCast_11hkListAgent___processCollision]
//   0x802B13FC    16  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_11hkListAgent___updateShapeCollectionFilter]
//   0x802B140C    72  getPenetrations   [map: hkSymmetricAgentLinearCast_17hkConvexListAgent___getPenetrations]
//   0x802B1454    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_17hkConvexListAgent___staticGetPenetrations]
//   0x802B149C    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_17hkConvexListAgent___getClosestPoints]
//   0x802B14E4    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_17hkConvexListAgent___staticGetClosestPoints]
//   0x802B152C   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_17hkConvexListAgent___staticLinearCast]
//   0x802B168C   412  processCollision   [map: hkSymmetricAgentLinearCast_17hkConvexListAgent___processCollision]
//   0x802B1828    16  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_17hkConvexListAgent___updateShapeCollectionFilter]
//   0x802B1838    92  __dt   [map: hkSymmetricAgent_17hkConvexListAgent_____dt]
//   0x802B1894   164  __dt   [map: hkSymmetricAgent_11hkListAgent_____dt]

#include <havok/hkConvexListAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>
#include <havok/hkBvAgent.h>
#include <havok/hkListAgent.h>

// Dispatcher registration entry point (not recovered yet; takes the dispatcher, a registration record
// and the two shape type ids). Same routine as hkPhantomAgent and hkSphereSphereAgent use.
// Stand-ins for the symmetric linear-cast agent functions (other classes, other units).
extern "C" void fn_802B1454();
extern "C" void fn_802B14E4();
extern "C" void fn_802B152C();
// Constructor of hkConvexListAgent (not recovered yet): this, first two pair arguments, pair pointer, flag.
extern "C" void fn_802AF148(hkConvexListAgent* self, void* a, void* b, void* c, int flag);
// hkListAgent constructor (hkListAgent.cpp): the fallback object of createConvexListAgent.
extern "C" void fn_802B87B8(void* self, void* a, void* b, void* c, int flag);
// Stream-mode and GSK-mode helpers (other units). fn_802FC824/fn_802FC988 act on the 0x30 sub-object.
extern "C" void fn_802B1A6C(hkConvexListAgent* self);
extern "C" void fn_802B1B08(hkConvexListAgent* self);
extern "C" void fn_802FC824(void* subObject);
extern "C" void fn_802FC988(void* subObject);
extern "C" void fn_802B29E4(hkConvexListAgent* self);
extern "C" void fn_802B2A24(hkConvexListAgent* self);
extern "C" void fn_802B2A68(hkConvexListAgent* self);
extern "C" void fn_8031830C(void* subObject, int unk8);
extern "C" void fn_802FC6AC(void* subObject, void* pairData, int unk8);
extern "C" void fn_802FEA54(void* subObject, void* args);
extern "C" void fn_802FCB14(void* subObject);


// Head of the 0x30 sub-object: an empty list whose head points at the inline buffer after it.
struct SubObjectHead {
    void* head;   // 0x00
    int count;    // 0x04
    u32 flags;    // 0x08
};

// Argument block handed to fn_802FEA54 by updateShapeCollectionFilter (lives on the stack there).
struct FilterArgs {
    void* a;      // 0x00
    void* b;      // 0x04
    u32 bValue;   // 0x08 first word of b
    void* d;      // 0x0C
    int unk8;     // 0x10
};

void hkConvexListAgent::registerAgent(void* dispatcher) {
    hkAgentFuncs convexList;
    convexList.create = (hkAgentFunc)createListConvexAgent;
    convexList.staticGetPenetrations = (hkAgentFunc)fn_802B1454;
    convexList.staticGetClosestPoints = (hkAgentFunc)fn_802B14E4;
    convexList.staticLinearCast = (hkAgentFunc)fn_802B152C;
    convexList.symmetricA = 1;
    convexList.symmetricB = 1;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&convexList, HK_SHAPE_CONVEX_LIST, HK_SHAPE_CONVEX);

    hkAgentFuncs listConvex;
    listConvex.create = (hkAgentFunc)createConvexListAgent;
    listConvex.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    listConvex.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    listConvex.staticLinearCast = (hkAgentFunc)staticLinearCast;
    listConvex.symmetricA = 0;
    listConvex.symmetricB = 1;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&listConvex, HK_SHAPE_CONVEX, HK_SHAPE_CONVEX_LIST);
}

hkCollisionAgent* hkConvexListAgent::createConvexListAgent(void* a, void* b, void* c, int flag) {
    if (flag != 0) {
        hkConvexListAgent* agent = (hkConvexListAgent*)hkMemory::getInstance().allocateChunk(sizeof(hkConvexListAgent), 0x1d);
        agent->m_memSizeAndFlags = sizeof(hkConvexListAgent);
        if (agent != 0) {
            fn_802AF148(agent, a, b, c, flag);
        }
        return agent;
    }
    hkCollisionAgent* agent = (hkCollisionAgent*)hkMemory::getInstance().allocateChunk(0x20, 0x1d);
    agent->m_memSizeAndFlags = 0x20;
    if (agent != 0) {
        fn_802B87B8(agent, a, b, c, flag);
    }
    return agent;
}

hkConvexListAgent::~hkConvexListAgent() {}

void hkConvexListAgent::invalidateTim(void* arg) {
    if (m_streamMode != 0) {
        fn_802B1A6C(this);
    } else {
        fn_802FC824(m_subObject);
    }
}

void hkConvexListAgent::warpTime(float t0, float t1, void* arg) {
    if (m_streamMode != 0) {
        fn_802B1B08(this);
    } else {
        fn_802FC988(m_subObject);
    }
}

void hkConvexListAgent::removePoint(u16 key) {
    if (m_streamMode != 0) {
        fn_802B29E4(this);
    }
}

void hkConvexListAgent::commitPotential(u16 key) {
    if (m_streamMode != 0) {
        fn_802B2A24(this);
    }
}

void hkConvexListAgent::createZombie(u16 key) {
    if (m_streamMode != 0) {
        fn_802B2A68(this);
    }
}

#pragma dont_inline on
void hkConvexListAgent::updateShapeCollectionFilter(void* a, void* b, void* d) {
    if (m_streamMode == 0) {
        FilterArgs args;
        args.a = a;
        args.bValue = *(u32*)b;
        args.b = b;
        args.d = d;
        args.unk8 = unk8;
        fn_802FEA54(m_subObject, &args);
    }
}
#pragma dont_inline reset

void hkConvexListAgent::switchToStreamMode() {
    fn_8031830C(m_subObject, unk8);
    m_streamMode = 0;
    SubObjectHead* sub = (SubObjectHead*)m_subObject;
    if (sub != 0) {
        sub->head = (u8*)sub + 0xC;
        sub->count = 0;
        sub->flags = 0x80000001;
    }
    fn_802FCB14(m_subObject);
    m_unk7A = 0x19;
    m_unk40 = 0.0f; // HYPOTHESIS: constant loaded from lbl_805A3F24 (value not recovered)
}

void hkConvexListAgent::switchToGskMode() {
    fn_802FC6AC(m_subObject, m_pairData, unk8);
    ((SubObjectHead*)m_subObject)->head = 0;
    m_streamMode = 1;
}

void hkConvexListAgent::cleanup() {
    if (m_streamMode != 0) {
        fn_8031830C(m_subObject, unk8);
    } else {
        fn_802FC6AC(m_subObject, m_pairData, unk8);
    }
    delete this;
}

void hkConvexListAgent::getClosestPoints(void* a, void* b, void* c, void* d) {
    staticGetClosestPoints(a, b, c, d);
}

void hkConvexListAgent::getPenetrations(void* a, void* b, void* c, void* d) {
    staticGetPenetrations(a, b, c, d);
}

void hkConvexListAgent::linearCast(void* a, void* b, void* c, void* d, void* e) {
    staticLinearCast(a, b, c, d, e);
}

// Symmetric wrapper of hkListAgent and of this agent (map names hkSymmetricAgentLinearCast_11hkListAgent___
// and hkSymmetricAgentLinearCast_17hkConvexListAgent___updateShapeCollectionFilter): swap the bodies, forward.
template <class T>
void hkSymmetricAgentLinearCast<T>::updateShapeCollectionFilter(void* a, void* b, void* c) {
    T::updateShapeCollectionFilter(b, a, c);
}

template void hkSymmetricAgentLinearCast<hkListAgent>::updateShapeCollectionFilter(void*, void*, void*);
template void hkSymmetricAgentLinearCast<hkConvexListAgent>::updateShapeCollectionFilter(void*, void*, void*);
