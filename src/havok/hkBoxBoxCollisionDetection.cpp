// Havok translation unit hkBoxBoxCollisionDetection.o (main.dol 0x80302C78-0x8030880C).
// Partly decompiled. Functions in address order (method names from the Havok TU map):
//   0x80302C78   908  checkCompleteness   [map: hkBoxBoxCollisionDetection__checkCompleteness]
//   0x80303004     8  getNumPoints   [map: hkBoxBoxManifold__getNumPoints]
//   0x8030300C    12  setComplete   [map: hkBoxBoxManifold__setComplete]
//   0x80303018  1768  addAdditionalEdgeHelper   [map: hkBoxBoxCollisionDetection__addAdditionalEdgeHelper]
//   0x80303700     4  __ct   [map: hkBoxBoxCollisionDetection19hkFeaturePointCacheFv____ct]
//   0x80303704  1560  addPoint   [map: hkBoxBoxCollisionDetection__addPoint]
//   0x80303D1C    12  getColumn   [map: hkMatrix3__getColumn1]
//   0x80303D28   592  tryToAddPointOnEdge   [map: hkBoxBoxCollisionDetection__tryToAddPointOnEdge]
//   0x80303F78    12  __ct   [map: hkFeatureContactPoint____ct]
//   0x80303F84  2704  checkIntersection   [map: hkBoxBoxCollisionDetection__checkIntersection]
//   0x80304A14    96  compareLessThan4   [map: hkVector4__compareLessThan4]
//   0x80304A74   108  _setRotatedInverseDir   [map: hkVector4___setRotatedInverseDir]
//   0x80304AE0   228  setvdProj   [map: hkBoxBoxCollisionDetection__setvdProj]
//   0x80304BC4    68  setAdd4   [map: hkVector4__setAdd4]
//   0x80304C08   260  rsqrtAll3   [map: hkBoxBoxUtils__rsqrtAll3]
//   0x80304D0C    68  setMul4   [map: hkVector4__setMul41]
//   0x80304D50    56  cmpAllGT3   [map: hkBoxBoxUtils__cmpAllGT3]
//   0x80304D88   100  selectIfGT3   [map: hkBoxBoxUtils__selectIfGT3]
//   0x80304DEC  3176  findClosestPoint   [map: hkBoxBoxCollisionDetection__findClosestPoint]
//   0x80305A54    20  setAll3   [map: hkVector4__setAll3]
//   0x80305A68   108  calcFaceFeatureBitSetFromAxisMap   [map: hkVector4__calcFaceFeatureBitSetFromAxisMap]
//   0x80305AD4    12  __ct   [map: hkBool____ct1]
//   0x80305AE0  1136  isValidEdgeEdge   [map: hkBoxBoxCollisionDetection__isValidEdgeEdge]
//   0x80305F50  1108  calculateClosestPoint   [map: hkBoxBoxCollisionDetection__calculateClosestPoint]
//   0x803063A4   128  initWorkVariables   [map: hkBoxBoxCollisionDetection__initWorkVariables]
//   0x80306424   568  calcManifold   [map: hkBoxBoxCollisionDetection__calcManifold]
//   0x8030665C  1860  refreshManifold   [map: hkBoxBoxCollisionDetection__refreshManifold]
//   0x80306DA0   336  queryManifoldNormalConsistency   [map: hkBoxBoxCollisionDetection__queryManifoldNormalConsistency]
//   0x80306EF0    12  isComplete   [map: hkBoxBoxManifold__isComplete]
//   0x80306EFC   176  calcManifoldNormal   [map: hkBoxBoxCollisionDetection__calcManifoldNormal]
//   0x80306FAC   652  checkManifoldNormalConsistency   [map: hkBoxBoxCollisionDetection__checkManifoldNormalConsistency]
//   0x80307238    40  getNumContactPoints   [map: hkProcessCollisionData__getNumContactPoints]
//   0x80307260   936  findAdditionalManifoldPoints   [map: hkBoxBoxCollisionDetection__findAdditionalManifoldPoints]
//   0x80307608    76  getMaxPlaneMask3   [map: hkVector4__getMaxPlaneMask3]
//   0x80307654  2248  tryToAddPointFaceA   [map: hkBoxBoxCollisionDetection__tryToAddPointFaceA]
//   0x80307F1C  2256  tryToAddPointFaceB   [map: hkBoxBoxCollisionDetection__tryToAddPointFaceB]
//   0x803087EC    12  getAttemptToFindAllEdges   [map: hkBoxBoxAgent__getAttemptToFindAllEdges]
//   0x803087F8    20  mod3   [map: hkBoxBoxUtils__mod3]

#include <havok/hkBoxBoxCollisionDetection.h>
#include <havok/hkBoxBoxAgent.h>
#include <havok/hkBoxBoxUtils.h>
#include <havok/hkProcessCollisionOutput.h>
#include <havok/hkMath.h>
#include <types.h>

const int hkBoxBoxUtils::s_mod3Table[] = {0, 1, 2, 0, 1, 2, 0, 1, 2};

void hkBoxBoxUtils::cmpAllGT3(const hkVector4& a, const hkReal& b, int& out) {
    out = (a.x > b || a.y > b || a.z > b) ? 1 : 0;
}

void hkBoxBoxUtils::selectIfGT3(hkVector4& dst, const hkReal& value, const hkVector4& cond, const hkReal& threshold) {
    dst.x = (cond.x > threshold) ? value : dst.x;
    dst.y = (cond.y > threshold) ? value : dst.y;
    dst.z = (cond.z > threshold) ? value : dst.z;
}

// The 12 box edges keep a 2-bit count each in an int (edge e at bits 2e, 2e+1); a count stops at 2.
static inline void addEdge(int& edgeMask, int edge) {
    int bit = 1 << (edge * 2);
    if (!((bit << 1) & edgeMask)) {
        edgeMask += bit;
    }
}

// Axis of the face plane bit (0x80, 0x40, 0x20) of getMaxPlaneMask3.
static inline int getAxisFromPlaneMask(int planeMask) {
    int bits = planeMask >> 4;
    if (bits & 8) {
        return 0;
    }
    return (bits & 4) ? 1 : 2;
}

// The edges of a box vertex (bits 0x80/0x40/0x20 of the feature id give the signs) that do not lie in the face
// orthogonal to axis.
static inline void addVertexEdges(int& edgeMask, int featureId, int axis) {
    int bits = featureId >> 4;
    int x = !(bits & 8);
    int y = !(bits & 4);
    int z = !(bits & 2);
    if (axis != 0) {
        addEdge(edgeMask, y + z * 2);
    }
    if (axis != 1) {
        addEdge(edgeMask, x + z * 2 + 4);
    }
    if (axis != 2) {
        addEdge(edgeMask, x + y * 2 + 8);
    }
}

// The edge feature id: direction in the low nibble, the signs of the other two axes in 0x80/0x40/0x20.
static inline void addEdgeFeature(int& edgeMask, int featureId) {
    int dir = featureId & 0xf;
    int bits = featureId >> 4;
    int x = !(bits & 8);
    int y = !(bits & 4);
    int z = !(bits & 2);
    if (dir == 0) {
        addEdge(edgeMask, y + z * 2);
    } else if (dir == 1) {
        addEdge(edgeMask, x + z * 2 + 4);
    } else {
        addEdge(edgeMask, x + y * 2 + 8);
    }
}

void hkBoxBoxCollisionDetection::checkCompleteness(hkBoxBoxManifold& manifold, int planeMaskA, int planeMaskB) {
    int numPoints = manifold.getNumPoints();
    if (numPoints < 3) {
        return;
    }
    if (manifold.unk20 >= 4) {
        manifold.m_complete = true;
        return;
    }
    int axisA = getAxisFromPlaneMask(planeMaskA);
    int axisB = getAxisFromPlaneMask(planeMaskB);
    int edgesA = 0;
    int edgesB = 0;
    for (int i = 0; i < numPoints; i++) {
        const hkBoxBoxManifold::Point& point = manifold.m_points[i];
        if (point.unk0 <= 2) {
            addVertexEdges(edgesB, point.unk1, axisB);
        } else if (point.unk0 <= 6) {
            addVertexEdges(edgesA, point.unk1, axisA);
        } else {
            addEdgeFeature(edgesA, point.unk0);
            addEdgeFeature(edgesB, point.unk1);
        }
    }
    int open = (edgesA & 0x555555) > 0 || (edgesB & 0x555555) > 0;
    manifold.m_complete = open == 0;
}

int hkBoxBoxManifold::getNumPoints() const {
    return m_numPoints;
}

void hkBoxBoxManifold::setComplete(const hkBool& complete) {
    m_complete = complete;
}

hkBool hkBoxBoxManifold::isComplete() const {
    return m_complete;
}

void hkBoxBoxCollisionDetection::tryToAddPointOnEdge(hkBoxBoxManifold& manifold, int edgeA, int edgeB, int nextA,
                                                     int nextB, const hkVector4& normalA, const hkVector4& normalB,
                                                     hkReal minDist) {
    hkVector4 dirB;
    hkVector4 dirA;
    dirA = normalA;
    dirA(edgeA) = 1.0f;
    dirB = normalB;
    hkFeatureContactPoint fcp;
    dirB(edgeB) = 1.0f;
    int maskA = dirA.compareLessThanZero4() & 0xe;
    int maskB = dirB.compareLessThanZero4() & 0xe;
    fcp.m_featureIdA = edgeA | 0x10 | (maskA << 4);
    fcp.m_featureIdB = edgeB + ((maskB ^ 0xe) << 4);
    addAdditionalEdgeHelper(manifold, fcp, minDist);
    int bitA = 1 << (4 + (nextA == 0 ? 3 : (nextA == 1 ? 2 : (nextA == 2 ? 1 : 0))));
    int bitB = 1 << (4 + (nextB == 0 ? 3 : (nextB == 1 ? 2 : (nextB == 2 ? 1 : 0))));
    fcp.m_featureIdA ^= bitA;
    addAdditionalEdgeHelper(manifold, fcp, minDist);
    fcp.m_featureIdB ^= bitB;
    addAdditionalEdgeHelper(manifold, fcp, minDist);
    fcp.m_featureIdA ^= bitA;
    addAdditionalEdgeHelper(manifold, fcp, minDist);
}

void hkBoxBoxCollisionDetection::setvdProj(const hkRotation& bRa, int i, int j, hkVector4& out) {
    hkVector4 a;
    a.setMul4(m_dinA(j), bRa.getColumn(i));
    hkVector4 b;
    b.setMul4(m_dinA(i), bRa.getColumn(j));
    out.setSub4(a, b);
    out.setAbs4(out);
}

// Box vertex signs indexed by the 0x80/0x40/0x20 bits of a feature id (bit set: negative lane).
static const hkReal s_vertexSigns[8][4] __attribute__((aligned(16))) = {
    {1.0f, 1.0f, 1.0f, 0.0f},   {1.0f, 1.0f, -1.0f, 0.0f},   {1.0f, -1.0f, 1.0f, 0.0f},   {1.0f, -1.0f, -1.0f, 0.0f},
    {-1.0f, 1.0f, 1.0f, 0.0f},  {-1.0f, 1.0f, -1.0f, 0.0f},  {-1.0f, -1.0f, 1.0f, 0.0f},  {-1.0f, -1.0f, -1.0f, 0.0f},
};

static inline const hkVector4& getVertexSigns(int featureId) {
    return *(const hkVector4*)s_vertexSigns[(featureId >> 5) & 7];
}

hkBool hkBoxBoxCollisionDetection::isValidEdgeEdge(hkFeaturePointCache& cache) {
    hkVector4 vertexA;
    hkVector4 edgeB;
    hkVector4 diff;
    hkVector4 vertexB;
    hkVector4 normal;
    hkVector4 halfEdgeA;

    int featureB = cache.m_featureIndexB;
    int featureA = cache.m_featureIndex;
    int dirB = featureB & 0xf;
    int dirA = featureA & 0xf;
    const hkVector4& signsA = getVertexSigns(featureA);
    const hkVector4& signsB = getVertexSigns(featureB);

    vertexA.setMul4(signsA, m_radiusA);
    vertexB.setMul4(signsB, m_radiusB);
    edgeB.setMul4(2.0f * m_radiusB(dirB), m_aTb.getColumn(dirB));
    vertexB._setTransformedPos(m_aTb, vertexB);
    diff.setSub4(vertexB, vertexA);

    hkReal lenA = -2.0f * m_radiusA(dirA);
    hkReal dotAB = lenA * edgeB(dirA);
    hkReal lenSqB = 4.0f * (m_radiusB(dirB) * m_radiusB(dirB));
    hkReal lenSqA = lenA * lenA;
    hkReal dotDB = edgeB.dot3(diff);
    hkReal dotDA = lenA * diff(dirA);
    hkReal det = lenSqA * lenSqB - dotAB * dotAB;
    hkReal sNum = dotDA * lenSqB - dotDB * dotAB;
    hkReal absDet = (hkReal)fabs(det);
    if (sNum <= 1.1920929e-7f * absDet || sNum >= 0.9999999f * absDet) {
        return false;
    }
    hkReal s = sNum / absDet;
    hkReal tNum = s * dotAB - dotDB;
    if (tNum <= 1.1920929e-7f * lenSqB || tNum >= 0.9999999f * lenSqB) {
        return false;
    }

    halfEdgeA.setZero4();
    halfEdgeA(dirA) = 0.5f * lenA;
    vertexA(dirA) += lenA * s;

    normal.setCross(halfEdgeA, edgeB);
    hkSimdReal len = normal.length3();
    normal.mul4(1.0f / len);
    if (len < m_minEdgeNormalLength) {
        return false;
    }
    if (normal.dot3(signsA) < 0.0f) {
        normal.setNeg4(normal);
    }
    hkReal dist = normal.dot3(diff);
    if (dist > m_boundaryTolerance) {
        return false;
    }
    cache.unk0 = vertexA;
    cache.m_normal.setNeg4(normal);
    cache.m_distance = dist;
    return true;
}

hkBool hkBoxBoxCollisionDetection::calculateClosestPoint(hkContactPoint& contact) {
    hkBoxBoxManifold manifold;
    hkFeatureContactPoint fcp;
    hkBool found = false;
    initWorkVariables();
    if (!checkIntersection(m_tolerance4)) {
        hkFeaturePointCache cache;
        if (findClosestPoint(manifold, fcp, cache) == 2) {
            found = true;
            hkProcessCdPoint point;
            if (cache.m_featureIndex <= 2) {
                point.m_contact.m_position._setTransformedPos(m_wTb, cache.unk10);
                point.m_contact.m_normal.setMul4(-cache.m_normalScale, m_wTa.getColumn(fcp.m_featureIdA));
                point.m_contact.m_normal.w = cache.m_distance;
                point.m_contactPointId = fcp.m_contactPointId;
            } else if (cache.m_featureIndex <= 6) {
                point.m_contact.m_position._setTransformedPos(m_wTa, cache.unk0);
                point.m_contact.m_normal.setMul4(-cache.m_normalScale, m_wTb.getColumn(fcp.m_featureIdA - 4));
                point.m_contact.m_normal.w = cache.m_distance;
                point.m_contactPointId = fcp.m_contactPointId;
            } else {
                point.m_contact.m_position._setTransformedPos(m_wTa, cache.unk0);
                point.m_contact.m_normal._setRotatedDir(m_wTa.getRotation(), cache.m_normal);
                point.m_contact.m_normal.w = cache.m_distance;
                point.m_contactPointId = fcp.m_contactPointId;
            }
            if (fcp.m_featureIdA > 2) {
                point.m_contact.m_position.addMul4(-point.m_contact.m_normal.w, point.m_contact.m_normal);
            }
            contact = point.m_contact;
        }
    }
    return found;
}

void hkBoxBoxCollisionDetection::calcManifoldNormal(hkVector4& normal, const hkFeatureContactPoint& fcp,
                                                    hkFeaturePointCache& cache, const hkBool& isEdgeEdge) {
    hkSimdReal dist;
    if (cache.m_featureIndex <= 2) {
        normal.setZero4();
        dist = cache.m_normalScale;
        normal(fcp.m_featureIdA) = -dist;
    } else if (cache.m_featureIndex <= 6) {
        dist = cache.m_normalScale;
        normal.setMul4(-dist, m_aTb.getRotation().getColumn(fcp.m_featureIdA - 4));
    } else {
        normal = cache.m_normal;
    }
}

hkBool hkBoxBoxCollisionDetection::queryManifoldNormalConsistency(hkBoxBoxManifold& manifold) {
    if (manifold.getNumPoints() < 2) {
        manifold.m_manifoldNormalValid = false;
        return true;
    }
    if (!(bool)manifold.m_manifoldNormalValid) {
        manifold.m_manifoldNormalB._setRotatedInverseDir(m_aTb.getRotation(), manifold.m_manifoldNormal);
        manifold.m_manifoldNormalValid = true;
    }
    hkVector4 normal;
    normal._setRotatedDir(m_aTb.getRotation(), manifold.m_manifoldNormalB);
    hkSimdReal dot = manifold.m_manifoldNormal.dot3(normal);
    return dot < 0.95f;
}

void hkBoxBoxCollisionDetection::checkManifoldNormalConsistency(hkBoxBoxManifold& manifold) {
    hkVector4 normal;
    normal.setRotatedDir(m_wTa.getRotation(), manifold.m_manifoldNormal);
    int pointIndex = m_result->getNumContactPoints() - manifold.getNumPoints();
    hkReal minDot = 0.95f;
    for (int i = 0; i < manifold.getNumPoints();) {
        hkProcessCdPoint& point = m_result->m_contactPoints[pointIndex];
        hkSimdReal dot = point.m_contact.m_normal.dot3(normal);
        if (dot < minDot) {
            point = m_result->m_firstFreeContactPoint[-1];
            if (manifold.m_points[i].unk0 <= 6) {
                manifold.unk20--;
            }
            m_contactMgr->removeContactPoint(manifold.m_points[i].unk2);
            manifold.removePoint(i);
            m_result->m_firstFreeContactPoint--;
        } else {
            i++;
            pointIndex++;
        }
    }
    if (manifold.getNumPoints() < 2) {
        manifold.m_manifoldNormalValid = false;
    } else {
        manifold.m_manifoldNormalB._setRotatedInverseDir(m_aTb.getRotation(), manifold.m_manifoldNormal);
        manifold.m_manifoldNormalValid = true;
    }
}

#pragma push
#pragma dont_inline on

int hkBoxBoxCollisionDetection::checkIntersection(const hkVector4& tolerance) {
    hkRotation aRbAbs;
    aRbAbs.getColumn(0).setAbs4(m_aTb.getRotation().getColumn(0));
    aRbAbs.getColumn(1).setAbs4(m_aTb.getRotation().getColumn(1));
    aRbAbs.getColumn(2).setAbs4(m_aTb.getRotation().getColumn(2));

    // face axes of A
    hkVector4 projB;
    projB._setRotatedDir(aRbAbs, m_radiusB);
    hkVector4 sepA;
    sepA.setAbs4(m_dinA);
    projB.add4(m_radiusA);
    sepA.sub4(projB);
    if (tolerance.compareLessThan4(sepA) & 0xe) {
        return 1;
    }
    m_sepDist[0] = sepA;

    // face axes of B
    hkVector4 projA;
    projA._setRotatedInverseDir(aRbAbs, m_radiusA);
    hkVector4 sepB;
    sepB.setAbs4(m_dinB);
    projA.add4(m_radiusB);
    sepB.sub4(projA);
    if (tolerance.compareLessThan4(sepB) & 0xe) {
        return 1;
    }
    m_sepDist[1] = sepB;

    // edge axes
    hkRotation bRa;
    bRa.setTranspose(m_aTb.getRotation());
    hkVector4 sq0(bRa.getColumn(0));
    hkVector4 sq1(bRa.getColumn(1));
    hkVector4 sq2(bRa.getColumn(2));
    sq0.mul4(sq0);
    sq1.mul4(sq1);
    sq2.mul4(sq2);

    {
        hkVector4 sep;
        setvdProj(bRa, 1, 2, sep);
        hkVector4 radius;
        radius.set(aRbAbs(2, 0) * m_radiusA(1) + aRbAbs(1, 0) * m_radiusA(2) + aRbAbs(0, 2) * m_radiusB(1) + aRbAbs(0, 1) * m_radiusB(2),
                   aRbAbs(2, 1) * m_radiusA(1) + aRbAbs(1, 1) * m_radiusA(2) + aRbAbs(0, 2) * m_radiusB(0) + aRbAbs(0, 0) * m_radiusB(2),
                   aRbAbs(2, 2) * m_radiusA(1) + aRbAbs(1, 2) * m_radiusA(2) + aRbAbs(0, 1) * m_radiusB(0) + aRbAbs(0, 0) * m_radiusB(1),
                   0.0f);
        hkVector4 invLen;
        invLen.setAdd4(sq2, sq1);
        sep.sub4(radius);
        hkBoxBoxUtils::rsqrtAll3(invLen);
        sep.setMul4(invLen, sep);
        int separated = tolerance.compareLessThan4(sep) & 0xe;
        hkBoxBoxUtils::cmpAllGT3(sep, m_boundaryTolerance, separated);
        if (separated) {
            return 1;
        }
        hkBoxBoxUtils::selectIfGT3(sep, -1e17f, invLen, 999.99994f);
        m_sepDist[2] = sep;
    }
    {
        hkVector4 sep;
        setvdProj(bRa, 2, 0, sep);
        hkVector4 radius;
        radius.set(aRbAbs(2, 0) * m_radiusA(0) + aRbAbs(0, 0) * m_radiusA(2) + aRbAbs(1, 2) * m_radiusB(1) + aRbAbs(1, 1) * m_radiusB(2),
                   aRbAbs(2, 1) * m_radiusA(0) + aRbAbs(0, 1) * m_radiusA(2) + aRbAbs(1, 2) * m_radiusB(0) + aRbAbs(1, 0) * m_radiusB(2),
                   aRbAbs(2, 2) * m_radiusA(0) + aRbAbs(0, 2) * m_radiusA(2) + aRbAbs(1, 1) * m_radiusB(0) + aRbAbs(1, 0) * m_radiusB(1),
                   0.0f);
        hkVector4 invLen;
        invLen.setAdd4(sq2, sq0);
        sep.sub4(radius);
        hkBoxBoxUtils::rsqrtAll3(invLen);
        sep.setMul4(invLen, sep);
        int separated = tolerance.compareLessThan4(sep) & 0xe;
        hkBoxBoxUtils::cmpAllGT3(sep, m_boundaryTolerance, separated);
        if (separated) {
            return 1;
        }
        hkBoxBoxUtils::selectIfGT3(sep, -1e17f, invLen, 999.99994f);
        m_sepDist[3] = sep;
    }
    {
        hkVector4 sep;
        setvdProj(bRa, 0, 1, sep);
        hkVector4 radius;
        radius.set(aRbAbs(1, 0) * m_radiusA(0) + aRbAbs(0, 0) * m_radiusA(1) + aRbAbs(2, 2) * m_radiusB(1) + aRbAbs(2, 1) * m_radiusB(2),
                   aRbAbs(1, 1) * m_radiusA(0) + aRbAbs(0, 1) * m_radiusA(1) + aRbAbs(2, 0) * m_radiusB(2) + aRbAbs(2, 2) * m_radiusB(0),
                   aRbAbs(1, 2) * m_radiusA(0) + aRbAbs(0, 2) * m_radiusA(1) + aRbAbs(2, 1) * m_radiusB(0) + aRbAbs(2, 0) * m_radiusB(1),
                   0.0f);
        hkVector4 invLen;
        invLen.setAdd4(sq1, sq0);
        sep.sub4(radius);
        hkBoxBoxUtils::rsqrtAll3(invLen);
        sep.setMul4(invLen, sep);
        int separated = tolerance.compareLessThan4(sep) & 0xe;
        hkBoxBoxUtils::cmpAllGT3(sep, m_boundaryTolerance, separated);
        if (separated) {
            return 1;
        }
        hkBoxBoxUtils::selectIfGT3(sep, -1e17f, invLen, 999.99994f);
        m_sepDist[4] = sep;
    }
    return 0;
}

void hkBoxBoxCollisionDetection::calcManifold(hkBoxBoxManifold& manifold) {
    initWorkVariables();
    hkProcessCdPoint* firstPoint = m_result->m_firstFreeContactPoint;
    refreshManifold(manifold);
    hkBool normalChanged = queryManifoldNormalConsistency(manifold);
    bool done = !(bool)normalChanged && manifold.isComplete();
    if (done) {
        return;
    }

    int separated;
    if (manifold.m_numPoints) {
        hkReal minDist = firstPoint->m_contact.getDistance();
        for (int i = manifold.m_numPoints - 1; i > 0; i--) {
            minDist = hkMath::min2(minDist, firstPoint[i].m_contact.getDistance());
        }
        minDist -= 0.05f * m_tolerance4(0);
        hkVector4 tolerance;
        tolerance.setAll3(minDist);
        separated = checkIntersection(tolerance);
    } else {
        separated = checkIntersection(m_tolerance4);
    }
    if (separated) {
        return;
    }

    hkFeatureContactPoint fcp;
    hkFeaturePointCache cache;
    int result = findClosestPoint(manifold, fcp, cache);
    if (normalChanged && result) {
        calcManifoldNormal(manifold.m_manifoldNormal, fcp, cache, hkBool(result == 2));
        if (manifold.m_manifoldNormalValid) {
            checkManifoldNormalConsistency(manifold);
        }
    }
    if (result == 2) {
        if (!addPoint(manifold, cache, fcp)) {
            findAdditionalManifoldPoints(manifold, fcp);
        }
    }
}

void hkBoxBoxCollisionDetection::findAdditionalManifoldPoints(hkBoxBoxManifold& manifold, hkFeatureContactPoint fcp) {
    if (manifold.getNumPoints() < 1) {
        return;
    }
    hkVector4 dirA;
    hkVector4 dirB;
    int firstIndex = m_result->getNumContactPoints() - manifold.getNumPoints();
    hkVector4 absA;
    hkVector4 absB;
    const hkVector4& normal = m_result->m_contactPoints[firstIndex].m_contact.getNormal();
    dirA._setRotatedInverseDir(m_wTa.getRotation(), normal);
    dirB._setRotatedInverseDir(m_wTb.getRotation(), normal);
    dirA.setNeg4(dirA);
    absA.setAbs4(dirA);
    absB.setAbs4(dirB);
    int axisA;
    int axisB;
    int planeMaskA = absA.getMaxPlaneMask3(dirA, axisA);
    int planeMaskB = absB.getMaxPlaneMask3(dirB, axisB);
    if (absA(axisA) < 0.98f || absB(axisB) < 0.98f) {
        return;
    }

    hkFeatureContactPoint fcpB;
    hkReal dinA = m_dinA(axisA);
    hkReal dinB = m_dinB(axisB);
    fcp.m_featureIdB = dirB.calcFaceFeatureBitSetFromAxisMap(dinA);
    fcpB.m_featureIdB = dirA.calcFaceFeatureBitSetFromAxisMap(dinB);
    fcp.m_featureIdA = axisA;
    fcpB.m_featureIdA = axisB + 4;

    hkReal minDist = 1.0f;
    for (int i = 0; i < manifold.getNumPoints(); i++) {
        hkReal dist = m_result->m_contactPoints[firstIndex + i].m_contact.getDistance();
        if (dist < minDist) {
            minDist = dist;
        }
    }

    tryToAddPointFaceA(manifold, fcp, planeMaskB, minDist);
    tryToAddPointFaceB(manifold, fcpB, planeMaskA, minDist);

    if (manifold.unk20 >= 4) {
        manifold.setComplete(hkBool(true));
        return;
    }

    if (hkBoxBoxAgent::getAttemptToFindAllEdges()) {
        hkFeatureContactPoint fcpEdge;
        int a1 = hkBoxBoxUtils::mod3(axisA + 1);
        int a2 = hkBoxBoxUtils::mod3(axisA + 2);
        int b1 = hkBoxBoxUtils::mod3(axisB + 1);
        int b2 = hkBoxBoxUtils::mod3(axisB + 2);
        dirB.setNeg4(dirB);
        tryToAddPointOnEdge(manifold, a1, b1, a2, b2, dirA, dirB, minDist);
        tryToAddPointOnEdge(manifold, a1, b2, a2, b1, dirA, dirB, minDist);
        tryToAddPointOnEdge(manifold, a2, b1, a1, b2, dirA, dirB, minDist);
        tryToAddPointOnEdge(manifold, a2, b2, a1, b1, dirA, dirB, minDist);
    }
    checkCompleteness(manifold, planeMaskA, planeMaskB);
}

#pragma pop

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern "C" void fn_80305AD4(unsigned char* r3, unsigned char* r4) {
    *r3 = *r4;
}
