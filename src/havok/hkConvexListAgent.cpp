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
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkSymmetricAgentProcessCollision.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>
#include <havok/hkBvAgent.h>
#include <havok/hkListAgent.h>
#include <havok/hkShapeCollectionAgent.h>
#include <havok/hkCdBody.h>
#include <havok/hkTransform.h>
#include <havok/hkCdPoint.h>
#include <havok/hkFlagCdBodyPairCollector.h>
#include <havok/hkClosestCdPointCollector.h>
#include <havok/hkSimpleClosestContactCollector.h>
#include <havok/hkMonitorStream.h>

// Dispatcher registration entry point (not recovered yet; takes the dispatcher, a registration record
// and the two shape type ids). Same routine as hkPhantomAgent and hkSphereSphereAgent use.
// Stand-ins for the symmetric linear-cast agent functions (other classes, other units).
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


// Stand-in for hkGskCache::init (hkGskCache.o, not recovered yet): fills the cache at 0x0C from the two shapes and
// the transform of B in A's frame.
extern "C" void fn_8031CD64(void* cache, const void* shapeA, const void* shapeB, const hkTransform* aTb);

// Default convex radius of the convex shapes (Havok's hkConvexShapeDefaultRadius, 0.05).
extern hkReal hkConvexShapeDefaultRadius;

// HYPOTHESIS: the list shape's child array (hkListShape::m_childInfo at 0x14 data / 0x18 size, 8 bytes per child; the
// second word of an entry is not read here).
struct hkConvexListChildInfo {
    const hkConvexShape* m_shape; // 0x00
    u32 unk4;                     // 0x04
};
struct hkConvexListShapeView : hkShape {
    u8 unkC[0x14 - 0x0C];
    const hkConvexListChildInfo* m_childInfo; // 0x14
    int m_numChildren;                        // 0x18
    u8 unk1C[0x20 - 0x1C];
    // HYPOTHESIS: Havok's m_minDistanceToUseConvexHullForGetClosestPoints: a hull point farther away than this is
    // reported as is; nearer, the children are queried.
    hkReal m_minDistanceToUseConvexHullForGetClosestPoints; // 0x20
};

// The children of a list shape seen as one convex shape (map name hkConvexListConvexShape, vtable 0x80486AA8): the
// supporting vertex is the best child's, with the child index in the high 24 bits of its id word. Only built on the
// stack by this unit (0x18 bytes).
struct hkConvexListConvexShape : hkConvexShape {
    const hkConvexListChildInfo* m_childInfo; // 0x10
    int m_numChildren;                        // 0x14

    hkConvexListConvexShape() : hkConvexShape(hkConvexShapeDefaultRadius) {}
    hkConvexListConvexShape(const hkConvexListChildInfo* childInfo, int numChildren, hkReal radius)
        : hkConvexShape(radius), m_childInfo(childInfo), m_numChildren(numChildren) {}

    virtual void getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const {}
    virtual hkBool castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const {
        return false;
    }
    virtual void getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const;
    virtual hkVector4* getCollisionSpheres(hkVector4* out) const;
    virtual void getSupportingVertex(const hkVector4& direction, hkVector4& out) const;
    virtual void convertVertexIdsToVertices(const u16* ids, int numIds, hkVector4* out) const;
    virtual void getFirstVertex(hkVector4& out) const;
};

// The second body of the GSK base: the list body with the convex list shape in place of the list (0x28 bytes, a
// stack temporary of the constructor; deleted as class 0x1d with its static size).
struct hkConvexListBody : hkCdBody {
    hkConvexListConvexShape m_convexShape; // 0x10

    hkConvexListBody(const hkCdBody& listBody)
        : hkCdBody(&listBody) {
        const hkConvexListShapeView* list = (const hkConvexListShapeView*)listBody.m_shape;
        u32 key = listBody.m_shapeKey;
        const hkConvexListChildInfo* children = list->m_childInfo;
        hkReal radius = children[0].m_shape->m_radius;
        m_convexShape.m_childInfo = children;
        m_convexShape.m_numChildren = list->m_numChildren;
        m_convexShape.m_radius = radius;
        m_shape = &m_convexShape;
        m_shapeKey = key;
    }
    static void operator delete(void* p, unsigned long nbytes) {
        hkMemory::getInstance().deallocateChunk(p, nbytes, 0x1d);
    }
};

// The w word of a supporting vertex holds its vertex id as an integer.
static inline int& vertexIdWord(hkVector4& v) {
    return *(int*)&v.w;
}

void hkConvexListConvexShape::getSupportingVertex(const hkVector4& direction, hkVector4& out) const {
    hkReal maxDot = -HK_REAL_MAX;
    int best = 0;
    for (int i = 0; i < m_numChildren; i++) {
        hkVector4 support;
        m_childInfo[i].m_shape->getSupportingVertex(direction, support);
        hkReal dot = support.dot3(direction);
        if (dot > maxDot) {
            maxDot = dot;
            out = support;
            best = i;
        }
    }
    vertexIdWord(out) += best << 8;
}

void hkConvexListConvexShape::convertVertexIdsToVertices(const u16* ids, int numIds, hkVector4* out) const {
    for (int i = 0; i < numIds; i++) {
        u16 childId = *ids & 0xFF;
        int child = *ids >> 8;
        m_childInfo[child].m_shape->convertVertexIdsToVertices(&childId, 1, out);
        vertexIdWord(*out) += child << 8;
        ids++;
        out++;
    }
}

void hkConvexListConvexShape::getFirstVertex(hkVector4& out) const {
    m_childInfo[0].m_shape->getFirstVertex(out);
}

void hkConvexListConvexShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 0;
    out->m_flag = 1;
    for (int i = 0; i < m_numChildren; i++) {
        hkCollisionSpheresInfo childInfo;
        m_childInfo[i].m_shape->getCollisionSpheresInfo(&childInfo);
        out->m_numSpheres += childInfo.m_numSpheres;
    }
}

hkVector4* hkConvexListConvexShape::getCollisionSpheres(hkVector4* out) const {
    hkVector4* spheres = out;
    for (int i = 0; i < m_numChildren; i++) {
        const hkConvexShape* child = m_childInfo[i].m_shape;
        child->getCollisionSpheres(spheres);
        hkCollisionSpheresInfo childInfo;
        child->getCollisionSpheresInfo(&childInfo);
        spheres += childInfo.m_numSpheres;
    }
    return out;
}

// The GSK base is built against the whole list as one convex shape; the cache is then filled again from a second
// convex list shape. HYPOTHESIS: the separating-normal w and the 0x18 time are reset to -1 as in hkGskBaseAgent.
hkConvexListAgent::hkConvexListAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                     hkContactMgr* contactMgr)
    : hkPredGskfAgent(bodyA, hkConvexListBody(bodyB), contactMgr) {
    const hkShape* shapeA = bodyA.m_shape;
    m_pairData = input.m_dispatcher;
    m_streamMode = 1;
    unk79 = 0;
    {
        const hkConvexListShapeView* list = (const hkConvexListShapeView*)bodyB.m_shape;
        hkConvexListConvexShape shape(list->m_childInfo, list->m_numChildren, list->m_childInfo[0].m_shape->m_radius);
        hkTransform aTb;
        aTb.setMulInverseMul(*(const hkTransform*)bodyA.m_motion, *(const hkTransform*)bodyB.m_motion);
        fn_8031CD64(unkC, shapeA, &shape, &aTb);
    }
    unk2C = -1.0f;
    unk18 = -1.0f;
}

// Head of the 0x30 sub-object: an empty list whose head points at the inline buffer after it.
struct SubObjectHead {
    void* head;   // 0x00
    int count;    // 0x04
    u32 flags;    // 0x08
};

// Argument block handed to fn_802FEA54 by updateShapeCollectionFilter (lives on the stack there).
struct FilterArgs {
    const hkCdBody* a;         // 0x00
    const hkCdBody* b;         // 0x04
    u32 bValue;                // 0x08 first word of b
    hkCollisionInput* d; // 0x0C
    int unk8;     // 0x10
};

void hkConvexListAgent::registerAgent(void* dispatcher) {
    hkAgentFuncs convexList;
    convexList.create = (hkAgentFunc)createListConvexAgent;
    convexList.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast<hkConvexListAgent>::staticGetPenetrations;
    convexList.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast<hkConvexListAgent>::staticGetClosestPoints;
    convexList.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_17hkConvexListAgent_::staticLinearCast;
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
        return new hkConvexListAgent(*(const hkCdBody*)a, *(const hkCdBody*)b, *(hkCollisionInput*)c,
                                     (hkContactMgr*)flag);
    }
    hkListAgent* agent = (hkListAgent*)hkMemory::getInstance().allocateChunk(sizeof(hkListAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkListAgent);
    new (agent) hkListAgent(a, b, c, flag);
    return agent;
}

// The mirror of createConvexListAgent for the list in A: the symmetric agent (or the symmetric list agent when there
// is no contact manager) with the bodies swapped.
hkCollisionAgent* hkConvexListAgent::createListConvexAgent(void* a, void* b, void* c, int flag) {
    if (flag != 0) {
        return new hkSymmetricAgentLinearCast_17hkConvexListAgent_(*(const hkCdBody*)b, *(const hkCdBody*)a,
                                                                   *(hkCollisionInput*)c, (hkContactMgr*)flag);
    }
    hkListAgent* agent = (hkListAgent*)hkMemory::getInstance().allocateChunk(sizeof(hkListAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkListAgent);
    return new (agent) hkSymmetricAgent_11hkListAgent_(b, a, c, flag);
}

hkConvexListAgent::~hkConvexListAgent() {}

void hkConvexListAgent::invalidateTim(void* arg) {
    if (m_streamMode != 0) {
        fn_802B1A6C(this);
    } else {
        fn_802FC824(subObject());
    }
}

void hkConvexListAgent::warpTime(float t0, float t1, void* arg) {
    if (m_streamMode != 0) {
        fn_802B1B08(this);
    } else {
        fn_802FC988(subObject());
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
void hkConvexListAgent::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& d) {
    if (m_streamMode == 0) {
        FilterArgs args;
        args.a = &a;
        args.b = &b;
        args.d = &d;
        args.unk8 = unk8;
        args.bValue = (u32)b.m_shape;
        fn_802FEA54(&unk30, &args);
    }
}
#pragma dont_inline reset

void hkConvexListAgent::switchToStreamMode() {
    fn_8031830C(subObject(), unk8);
    m_streamMode = 0;
    SubObjectHead* sub = (SubObjectHead*)subObject();
    if (sub != 0) {
        sub->head = (u8*)sub + 0xC;
        sub->count = 0;
        sub->flags = 0x80000001;
    }
    fn_802FCB14(subObject());
    m_unk7A = 0x19;
    streamUnk40() = 0.0f; // HYPOTHESIS: constant loaded from lbl_805A3F24 (value not recovered)
}

void hkConvexListAgent::switchToGskMode() {
    fn_802FC6AC(subObject(), m_pairData, unk8);
    ((SubObjectHead*)subObject())->head = 0;
    m_streamMode = 1;
}

void hkConvexListAgent::cleanup() {
    if (m_streamMode != 0) {
        fn_8031830C(subObject(), unk8);
    } else {
        fn_802FC6AC(subObject(), m_pairData, unk8);
    }
    delete this;
}

// The closest point is first taken from the list as one convex hull: when the hull penetrates, or its point is
// nearer than the shape's threshold, the children are queried instead.
void hkConvexListAgent::staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d) {
    HK_TIMER_BEGIN_LIST("CvxList", "checkHull");
    {
        hkConvexListBody hull(b);
        hkFlagCdBodyPairCollector checker;
        hkGskBaseAgent::staticGetPenetrations(a, hull, c, &checker);
        if (checker.m_hit) {
            HK_TIMER_SPLIT_LIST("children");
            hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetClosestPoints(a, b, c, (hkPenetrationTarget*)d);
        } else {
            hkClosestCdPointCollector closest;
            hkGskBaseAgent::staticGetClosestPoints(a, hull, c, &closest);
            if (closest.hasHit()) {
                const hkConvexListShapeView* list = (const hkConvexListShapeView*)b.m_shape;
                hkReal dist = closest.m_point.m_normal.w;
                if (dist > list->m_minDistanceToUseConvexHullForGetClosestPoints) {
                    hkCdPoint point;
                    point.m_cdBodyA = &a;
                    point.m_cdBodyB = &b;
                    point.m_contact.m_position = closest.m_point.m_position;
                    point.m_contact.m_normal = closest.m_point.m_normal;
                    point.m_contact.m_normal.w = dist;
                    ((hkCdPointCollector*)d)->addCdPoint(&point);
                } else {
                    HK_TIMER_SPLIT_LIST("children");
                    hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetClosestPoints(a, b, c,
                                                                                              (hkPenetrationTarget*)d);
                }
            }
        }
    }
    HK_TIMER_END_LIST();
}

// The cast is first made against the list as one convex hull; only when the hull is hit are the children cast
// against.
void hkConvexListAgent::staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* d,
                                         void* e) {
    HK_TIMER_BEGIN_LIST("CvsListAgent", "checkHull");
    {
        hkConvexListBody hull(b);
        hkSimpleClosestContactCollector checker;
        hkIterativeLinearCastAgent::staticLinearCast(a, hull, c, &checker, &checker);
        if (checker.m_hit) {
            HK_TIMER_SPLIT_LIST("child");
            hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticLinearCast(a, b, c, d, e);
        }
    }
    HK_TIMER_END_LIST();
}

// The whole list is first tested as one convex hull; only when the hull penetrates are the children queried (through
// the shape collection path).
void hkConvexListAgent::staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d) {
    HK_TIMER_BEGIN_LIST("CvxList", "checkHull");
    {
        hkConvexListBody hull(b);
        hkFlagCdBodyPairCollector checker;
        hkGskBaseAgent::staticGetPenetrations(a, hull, c, &checker);
        if (checker.m_hit) {
            HK_TIMER_SPLIT_LIST("children");
            hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetPenetrations(a, b, c, (hkPenetrationTarget*)d);
        }
    }
    HK_TIMER_END_LIST();
}

// BYTES: the symmetric wrappers call these forwarders; without the pragma they are inlined there.
#pragma push
#pragma auto_inline off
void hkConvexListAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d) {
    staticGetClosestPoints(a, b, c, d);
}

void hkConvexListAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d) {
    staticGetPenetrations(a, b, c, d);
}
#pragma pop

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgent_17hkConvexListAgent_::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkConvexListAgent::linearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkConvexListAgent::linearCast(b, a, flippedInput, &flipped, 0);
    }
}

// BYTES: the symmetric linear cast calls this forwarder; without the pragma it is inlined there.
#pragma push
#pragma auto_inline off
void hkConvexListAgent::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* d, void* e) {
    staticLinearCast(a, b, c, d, e);
}
#pragma pop

// Symmetric wrapper of hkListAgent and of this agent (map names hkSymmetricAgentLinearCast_11hkListAgent___
// and hkSymmetricAgentLinearCast_17hkConvexListAgent___updateShapeCollectionFilter): swap the bodies, forward.
template <class T>
void hkSymmetricAgentLinearCast<T>::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c) {
    T::updateShapeCollectionFilter(b, a, c);
}

template void hkSymmetricAgentLinearCast<hkListAgent>::updateShapeCollectionFilter(const hkCdBody&, const hkCdBody&, hkCollisionInput&);
template void hkSymmetricAgentLinearCast<hkConvexListAgent>::updateShapeCollectionFilter(const hkCdBody&, const hkCdBody&, hkCollisionInput&);
template void hkSymmetricAgentLinearCast<hkListAgent>::processCollision(const hkCdBody&, const hkCdBody&, hkProcessCollisionInput&, void*);
template void hkSymmetricAgentLinearCast<hkConvexListAgent>::processCollision(const hkCdBody&, const hkCdBody&, hkProcessCollisionInput&, void*);

template <class T>
void hkSymmetricAgentLinearCast<T>::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    T::getPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    T::getClosestPoints(b, a, c, &wrapped);
}

template void hkSymmetricAgentLinearCast<hkListAgent>::getPenetrations(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
template void hkSymmetricAgentLinearCast<hkListAgent>::getClosestPoints(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgent_11hkListAgent_::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkListAgent::linearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkListAgent::linearCast(b, a, flippedInput, &flipped, 0);
    }
}


// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_17hkConvexListAgent_::staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkConvexListAgent::staticLinearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkConvexListAgent::staticLinearCast(b, a, flippedInput, &flipped, 0);
    }
}

// Same for the shape collection agent (instantiated in this unit); its static cast takes the bodies by reference.
// BYTES: called, not inlined, by hkConvexListAgent::staticLinearCast (the pragma).
#pragma push
#pragma auto_inline off
template <>
void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkShapeCollectionAgent::staticLinearCast(b, a, flippedInput, &flipped,
                                                 &flippedStart);
    } else {
        hkShapeCollectionAgent::staticLinearCast(b, a, flippedInput, &flipped, 0);
    }
}
#pragma pop

template <class T>
void hkSymmetricAgentLinearCast<T>::staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, hkPenetrationTarget* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    T::staticGetPenetrations(b, a, c, (hkPenetrationTarget*)&wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, hkPenetrationTarget* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    T::staticGetClosestPoints(b, a, c, (hkPenetrationTarget*)&wrapped);
}

template void hkSymmetricAgentLinearCast<hkConvexListAgent>::getPenetrations(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
template void hkSymmetricAgentLinearCast<hkConvexListAgent>::staticGetPenetrations(const hkCdBody&, const hkCdBody&, hkCollisionInput&, hkPenetrationTarget*);
template void hkSymmetricAgentLinearCast<hkConvexListAgent>::getClosestPoints(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
template void hkSymmetricAgentLinearCast<hkConvexListAgent>::staticGetClosestPoints(const hkCdBody&, const hkCdBody&, hkCollisionInput&, hkPenetrationTarget*);

// The shape collection agent's static queries take the bodies by reference. BYTES: the convex list queries call
// them; without the pragma they are inlined there.
#pragma push
#pragma auto_inline off
template <>
void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c,
                                                                              hkPenetrationTarget* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    hkShapeCollectionAgent::staticGetPenetrations(b, a, c,
                                                  &wrapped);
}

template <>
void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c,
                                                                               hkPenetrationTarget* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    hkShapeCollectionAgent::staticGetClosestPoints(b, a, c,
                                                   &wrapped);
}
#pragma pop
