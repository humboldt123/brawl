// Havok translation unit hkPredGskCylinderAgent3.o (main.dol 0x803020E0-0x80302C78).
// Functions in address order (method names from the Havok TU map):
//   0x803020E0   224  registerAgent3   [map: hkPredGskCylinderAgent3__registerAgent3]
//   0x803021C0   176  create   [map: hkPredGskCylinderAgent3__create]
//   0x80302270  1656  process   [map: hkPredGskCylinderAgent3__process]
//   0x803028E8     4  __ct   [map: hkCdBody____ct]
//   0x803028EC    16  getVertex   [map: hkCylinderShape__getVertex]
//   0x803028FC     8  getTransform   [map: hkCdBody__getTransform]
//   0x80302904    76  resetGskManifold   [map: hkGskManifold__resetGskManifold]
//   0x80302950   312  replaceCylindersWithCapsulesInCdBodies   [map: hkCdBody__replaceCylindersWithCapsulesInCdBodies]
//   0x80302A88    32  __dl   [map: hkShape____dl]
//   0x80302AA8   216  convertCylinderCacheToCapsuleCache   [map: hkGskCache__convertCylinderCacheToCapsuleCache]
//   0x80302B80   180  restoreOriginalShapesInCdBodies   [map: hkCdBody__restoreOriginalShapesInCdBodies]
//   0x80302C34    68  __as   [map: hkGskCache____as]

#include <havok/hkPredGskCylinderAgent3.h>
#include <havok/hkGskManifoldUtil.h>
#include <havok/hkGskCache.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkAgent1nTrack.h>
#include <havok/hkCylinderShape.h>
#include <havok/hkCapsuleShape.h>
#include <havok/hkCollidable.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkProcessCollisionOutput.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkMath.h>

// The predictive GSK agent's functions with this agent's create and process, for cylinder x convex in both orders.
void hkPredGskCylinderAgent3::registerAgent3(hkCollisionDispatcher* dispatcher) {
    hkAgent3Funcs f;
    f.m_createFunc = (hkAgentFunc)create;
    f.m_processFunc = (hkAgentFunc)process;
    f.m_sepNormalFunc = (hkAgentFunc)hkPredGskAgent3::sepNormal;
    f.m_cleanupFunc = (hkAgentFunc)hkPredGskAgent3::cleanup;
    f.m_removePointFunc = (hkAgentFunc)hkPredGskAgent3::removePoint;
    f.m_commitPotentialFunc = (hkAgentFunc)hkPredGskAgent3::commitPotential;
    f.m_createZombieFunc = (hkAgentFunc)hkPredGskAgent3::createZombie;
    f.m_destroyFunc = (hkAgentFunc)hkPredGskAgent3::destroy;
    f.m_isPredictive = true;
    f.m_ignoreSymmetricVersion = true;
    dispatcher->registerAgent3(f, HK_SHAPE_CYLINDER, HK_SHAPE_CONVEX);
    dispatcher->registerAgent3(f, HK_SHAPE_CONVEX, HK_SHAPE_CYLINDER);
}

void* hkPredGskCylinderAgent3::create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData) {
    entry->unk4 = 0;
    if (input.m_bodyA->getShape()->getType() == HK_SHAPE_CYLINDER) {
        entry->unk4 |= 1;
    }
    if (input.m_bodyB->getShape()->getType() == HK_SHAPE_CYLINDER) {
        entry->unk4 |= 2;
    }
    return hkPredGskAgent3::create(input, entry, agentData);
}

static void replaceCylindersWithCapsulesInCdBodies(const hkCdBody** bodies[2], const int useCapsule[2],
                                                   hkCdBody tempBodies[2], const hkCdBody* origBodies[2]);
static void convertCylinderCacheToCapsuleCache(hkGskCache* cache, const int useCapsule[2]);

// The cylinder's full radius (cylinder radius plus convex radius). HYPOTHESIS: an inline helper; the convex radius is
// read before the out-of-line getCylinderRadius call (after the capsule's allocation), which a plain sum in the
// argument does not give.
static inline hkReal getCapsuleRadius(const hkCylinderShape* cylinder) {
    hkReal convexRadius = cylinder->m_radius;
    hkReal cylRadius = cylinder->getCylinderRadius();
    return cylRadius + convexRadius;
}
static void restoreOriginalShapesInCdBodies(const hkCdBody** bodies[2], const hkCdBody* origBodies[2]);

// BYTES: the whole function is compiled without inlining; every accessor is a call to its out-of-line copy.
#pragma push
#pragma dont_inline on
void* hkPredGskCylinderAgent3::process(const hkAgent3ProcessInput& input, hkAgentEntry* entry, void* agentData,
                                       hkVector4* separatingNormal, hkProcessCollisionOutput& output) {
    HK_TIMER_BEGIN_ACCESSORS("PredGskf3");
    HK_TIMER_BEGIN_LIST_ACCESSORS("intern", "init");

    hkGskCache* gskCache = (hkGskCache*)agentData;
    hkGskManifold* gskManifold = (hkGskManifold*)(gskCache + 1);
    hkReal distAtT1 = input.m_distAtT1;
    const hkCdBody** bodies[2] = {(const hkCdBody**)&input.m_bodyA, (const hkCdBody**)&input.m_bodyB};
    int isCylinder[2];
    isCylinder[0] = entry->unk4 & 1;
    isCylinder[1] = entry->unk4 & 2;
    int useCapsule[2];
    useCapsule[0] = entry->unk4 & 4;
    useCapsule[1] = entry->unk4 & 8;
    const hkCdBody* origBodies[2];
    origBodies[0] = 0;
    origBodies[1] = 0;
    hkCdBody tempBodies[2];

    hkVector4 sepNormal;
    hkPredGskAgent3::sepNormal(input, agentData, sepNormal);

    for (int i = 0; i < 2; i++) {
        if (isCylinder[i]) {
            const hkCylinderShape* cylinder = (const hkCylinderShape*)(*bodies[i])->getShape();
            hkVector4 axis;
            axis.setSub4(cylinder->getVertex(1), cylinder->getVertex(0));
            axis.normalize3();
            axis.setRotatedDir((*bodies[i])->getTransform().getRotation(), axis);
            hkReal cosAngle = (hkMath::fabs)(axis.dot3(sepNormal));
            if (useCapsule[i]) {
                if (cosAngle > 0.17f) {
                    useCapsule[i] = 0;
                    entry->unk4 &= ~(4 << i);
                    hkGskManifold::resetGskManifold(gskManifold, entry, input.m_contactMgr);
                }
            } else if (cosAngle < 0.085f) {
                useCapsule[i] = 1;
                entry->unk4 |= 4 << i;
                hkGskManifold::resetGskManifold(gskManifold, entry, input.m_contactMgr);
            }
        }
    }

    {
        const hkCollisionQualityInfo& qi = *input.m_input->m_collisionQualityInfo;
        if (qi.unk10) {
            hkReal radiusA = ((const hkCollidable*)input.m_bodyA->getRootCollidable())->m_allowedPenetrationDepth;
            hkReal radiusB = ((const hkCollidable*)input.m_bodyB->getRootCollidable())->m_allowedPenetrationDepth;
            hkReal allowedPenetrationDepth = hkMath::min2(radiusA, radiusB);
            hkReal distAtT0 = (*separatingNormal)(3);
            hkReal minSeparation = hkMath::min2(qi.unk14[0] * allowedPenetrationDepth,
                                                distAtT0 + qi.unk14[1] * allowedPenetrationDepth);
            if (distAtT1 >= minSeparation) {
                goto QUICK_VERIFY_MANIFOLD;
            }
            hkReal toiSeparation = hkMath::min2(qi.unk14[4] * allowedPenetrationDepth,
                                                distAtT0 + qi.unk14[5] * allowedPenetrationDepth);
            HK_TIMER_SPLIT_LIST_ACCESSORS("toi");
            hk4dGskCollideCalcToi(input, allowedPenetrationDepth, minSeparation, toiSeparation, *gskCache,
                                  *separatingNormal, output);
            goto PROCESS_AT_T1;
        }
    QUICK_VERIFY_MANIFOLD:
        if (distAtT1 > qi.unk0[3]) {
            (*separatingNormal)(3) = distAtT1;
            if (gskManifold->m_countC) {
                HK_TIMER_SPLIT_LIST_ACCESSORS("getPoints");
                replaceCylindersWithCapsulesInCdBodies(bodies, useCapsule, tempBodies, origBodies);
                hkGskManifoldWork work;
                hkGskManifold_init(*gskManifold, *separatingNormal, *input.m_bodyA, *input.m_bodyB,
                                   input.m_input->getTolerance(), work);
                hkGskManifold_verifyAndGetPoints(*gskManifold, work, 0, output, input.m_contactMgr);
                if (gskManifold->m_countC && output.m_potentialContacts) {
                    *output.m_potentialContacts->m_firstFreeRepresentativeContact++ =
                        output.m_firstFreeContactPoint - gskManifold->m_countC;
                }
            }
            goto END;
        }
    }
PROCESS_AT_T1:
    HK_TIMER_SPLIT_LIST_ACCESSORS("process");
    replaceCylindersWithCapsulesInCdBodies(bodies, useCapsule, tempBodies, origBodies);
    {
        hkGskCache* cache = gskCache;
        hkGskCache capsuleCache;
        if (useCapsule[0] || useCapsule[1]) {
            cache = &capsuleCache;
            capsuleCache = *gskCache;
            convertCylinderCacheToCapsuleCache(cache, useCapsule);
        }
        hkGskAgentUtil_processCollisionNoTim(input, entry, agentData, *cache, *gskManifold, *separatingNormal, output);
        *separatingNormal = sepNormal;
    }
END:
    restoreOriginalShapesInCdBodies(bodies, origBodies);
    entry->m_numContactPoints = gskManifold->m_countC;
    HK_TIMER_END_LIST_ACCESSORS();
    HK_TIMER_END_ACCESSORS();
    return hkAddByteOffset(agentData, (gskManifold->getTotalSizeInBytes() + sizeof(hkGskCache) + 15) & ~15);
}
#pragma pop

void hkGskManifold::resetGskManifold(hkGskManifold* manifold, hkAgentEntry* entry, void* arg) {
    hkGskManifoldUtil::hkGskManifold_cleanup(manifold, arg);
    entry->m_numContactPoints = manifold->m_countC;
    *(u32*)manifold = 0;
}

// Replaces each body flagged in useCapsule by a temporary body with a new capsule along the cylinder's axis (radius:
// cylinder radius + convex radius + 0.002) and remembers the original body.
static void replaceCylindersWithCapsulesInCdBodies(const hkCdBody** bodies[2], const int useCapsule[2],
                                                   hkCdBody tempBodies[2], const hkCdBody* origBodies[2]) {
    for (int i = 0; i < 2; i++) {
        if (useCapsule[i]) {
            const hkCylinderShape* cylinder = (const hkCylinderShape*)(*bodies[i])->getShape();
            hkCapsuleShape* capsule =
                new hkCapsuleShape(cylinder->m_vertexA, cylinder->m_vertexB, 0.002f + getCapsuleRadius(cylinder));
            hkCdBody* body = new (&tempBodies[i]) hkCdBody(*bodies[i], (*bodies[i])->getMotionState());
            body->m_shapeKey = body->m_parent->m_shapeKey;
            body->m_shape = capsule;
            origBodies[i] = *bodies[i];
            *bodies[i] = body;
        } else {
            origBodies[i] = 0;
        }
    }
}

// Turns the cylinder vertex ids of the cache into capsule ids (a cylinder's edge or face becomes the capsule's
// segment) for each body flagged in useCapsule.
static void convertCylinderCacheToCapsuleCache(hkGskCache* cache, const int useCapsule[2]) {
    if (useCapsule[0]) {
        cache->m_vertices[0] = (u16)((1 - ((cache->m_vertices[0] >> 7) & 1)) << 4);
        if (cache->m_dimA > 1) {
            cache->m_vertices[0] = 0x10;
            cache->m_vertices[1] = 0;
            if (cache->m_dimA == 3) {
                cache->m_dimA = 2;
                cache->m_vertices[2] = cache->m_vertices[3];
            }
        }
    }
    if (useCapsule[1]) {
        u16* v = &cache->m_vertices[cache->m_dimA];
        v[0] = (u16)((1 - ((v[0] >> 7) & 1)) << 4);
        if (cache->m_dimB > 1) {
            cache->m_vertices[cache->m_dimA] = 0x10;
            cache->m_vertices[cache->m_dimA + 1] = 0;
            if (cache->m_dimB == 3) {
                cache->m_dimB = 2;
            }
        }
    }
}

// Releases the temporary capsules and puts the original bodies back.
static void restoreOriginalShapesInCdBodies(const hkCdBody** bodies[2], const hkCdBody* origBodies[2]) {
    for (int i = 0; i < 2; i++) {
        if (origBodies[i]) {
            ((hkShape*)(*bodies[i])->getShape())->removeReference();
            *bodies[i] = origBodies[i];
        }
    }
}

void hkGskCache::operator=(const hkGskCache& other) {
    m_vertices[0] = other.m_vertices[0];
    m_vertices[1] = other.m_vertices[1];
    m_vertices[2] = other.m_vertices[2];
    m_vertices[3] = other.m_vertices[3];
    m_dimA = other.m_dimA;
    m_dimB = other.m_dimB;
    unkA = other.unkA;
    unkB = other.unkB;
}
