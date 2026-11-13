// Havok translation unit hkCollideCapsuleUtil.o (main.dol 0x803227F4-0x80325184).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x803227F4  3000  hkCollideCapsuleUtilManifoldCapsVsCaps   [map: hkCollideCapsuleUtil__hkCollideCapsuleUtilManifoldCapsVsCaps]
//   0x803233AC     8  setDistance   [map: hkContactPoint__setDistance]
//   0x803233B4   976  hkCollideCapsuleUtilClostestPointCapsVsCaps   [map: hkCollideCapsuleUtil__hkCollideCapsuleUtilClostestPointCapsVsCaps]
//   0x80323784     4  __ct   [map: hkCollideTriangleUtil27ClosestLineSegLineSegResultFv____ct]
//   0x80323788  2496  hkCollideCapsuleUtilCapsVsTri   [map: hkCollideCapsuleUtil__hkCollideCapsuleUtilCapsVsTri]
//   0x80324148   836  hkCollideCapsuleUtil_calcTrianglePlaneDirections   [map: hkVector4__hkCollideCapsuleUtil_calcTrianglePlaneDirections]
//   0x8032448C  1000  mul4xyz1Points   [map: hkVector4Util__mul4xyz1Points]
//   0x80324874    20  setAll   [map: hkVector4__setAll]
//   0x80324888    84  compareLessThanZero4   [map: hkVector4__compareLessThanZero4]
//   0x803248DC    92  setInterpolate4   [map: hkVector4__setInterpolate4]
//   0x80324938    48  getRow   [map: hkMatrix3__getRow]
//   0x80324968     4  __ct   [map: hkContactPoint____ct]
//   0x8032496C    24  getNextModulo3   [map: hkCollideTriangleUtil__getNextModulo3]
//   0x80324984    20  getPrevModulo3   [map: hkCollideTriangleUtil__getPrevModulo3]
//   0x80324998    68  __as   [map: hkContactPoint____as]
//   0x803249DC  1020  calcBarycentricCoordinates   [map: hkCollideTriangleUtil__calcBarycentricCoordinates]
//   0x80324DD8   604  closestLineSegLineSeg   [map: hkCollideTriangleUtil__closestLineSegLineSeg]
//   0x80325034   336  closestPointLineSeg   [map: hkCollideTriangleUtil__closestPointLineSeg]


#include <havok/hkCollideTriangleUtil.h>
#include <havok/hkContactPoint.h>
#include <havok/hkCdPoint.h>
#include <havok/hkMath.h>
#include <havok/hkVector4Util.h>

#define HK_REAL_EPSILON 1.192092896e-07f

// Lookup table of the modulo-3 successor/predecessor (other unit, referenced by its symbol).
extern "C" const s8 lbl_805A47DC[];

// hkContactPoint::setDistance: writes the w component of the normal (the contact distance).
void hkContactPoint::setDistance(hkReal distance) {
    m_normal.w = distance;
}

// Plane directions of the triangle's three edges (rows of the rotation, w = plane offset) and its normal
// (out-of-line copy at 0x80324148, not written yet).
void hkCollideCapsuleUtil_calcTrianglePlaneDirections(const hkVector4* triVertices,
                                                      const hkCollideTriangleUtil::PointTriangleDistanceCache& cache,
                                                      hkTransform& planeEquationsOut, hkVector4& normalOut);
// Up to three contacts between two capsules (0x803227F4, not written yet).
void hkCollideCapsuleUtilManifoldCapsVsCaps(const hkVector4* capsAPoints, hkReal capsARadius, const hkVector4* capsBPoints,
                                            hkReal capsBRadius, hkContactPoint* points);

// Capsule against a (thick) triangle. The capsule end points are moved into the triangle's plane space: lanes
// x, y, z are the distances to the three edge planes, w the distance to the triangle plane. Up to three contacts.
// Compiled without inlining (every vector accessor is an out-of-line call). 98.9%: the float registers of
// triBRadius, the hoisted loop constants and the edge components are coloured in a different order.
#pragma push
#pragma dont_inline on
void hkCollideCapsuleUtilCapsVsTri(const hkVector4* capsAPoints, hkReal capsARadius, const hkVector4* triVertices,
                                   hkReal triBRadius, const hkCollideTriangleUtil::PointTriangleDistanceCache& cache,
                                   hkReal collisionTolerance, int searchManifold, hkContactPoint* points) {
    hkTransform planeEquations;
    hkVector4 triangleNormal;
    hkCollideCapsuleUtil_calcTrianglePlaneDirections(triVertices, cache, planeEquations, triangleNormal);

    hkVector4 projectedPoint[2];
    projectedPoint[0].setSub4(capsAPoints[0], triVertices[0]);
    projectedPoint[1].setSub4(capsAPoints[1], triVertices[0]);
    hkVector4Util::mul4xyz1Points(planeEquations, projectedPoint, 2, projectedPoint);

    const hkReal radiusSum = capsARadius + triBRadius;
    const hkReal maxDist = radiusSum + collisionTolerance;
    // BYTES: a plain variable (the target keeps the value in a register for the three calls; a literal or a const
    // local is re-loaded each time).
    hkReal realMax = HK_REAL_MAX;
    points[0].setDistance(realMax);
    points[1].setDistance(realMax);
    points[2].setDistance(realMax);

    {
        hkVector4 maxDist4;
        maxDist4.setAll(maxDist);
        int outside0 = maxDist4.compareLessThan4(projectedPoint[0]);
        if ((outside0 & maxDist4.compareLessThan4(projectedPoint[1])) ||
            (projectedPoint[0](3) < -maxDist && projectedPoint[1](3) < -maxDist)) {
            return;
        }
    }

    int mask[2];
    mask[0] = projectedPoint[0].compareLessThanZero4();
    mask[1] = projectedPoint[1].compareLessThanZero4();

    if ((mask[0] ^ mask[1]) & 1) {
        hkReal t = projectedPoint[0](3) / (projectedPoint[0](3) - projectedPoint[1](3));
        hkVector4 intersection;
        intersection.setInterpolate4(projectedPoint[0], projectedPoint[1], t);
        if ((intersection.compareLessThanZero4() & 0xe) == 0xe) {
            points++;
            points->setDistance(-HK_REAL_MAX);
            int* m = mask;
            hkVector4* proj = projectedPoint;
            // BYTES: the target walks three pointers here, the index form shares one offset.
            const hkVector4* capsPoint = capsAPoints;
            for (int i = 0; i < 2; i++, proj++, capsPoint++, m++) {
                if ((*m & 0xe) == 0xe) {
                    hkReal dist = -(hkMath::fabs)((*proj)(3)) - radiusSum;
                    if (dist > points->getDistance()) {
                        if (*m & 1) {
                            points->getSeparatingNormal() = triangleNormal;
                            points->getPosition().setAddMul4(*capsPoint, triangleNormal, triBRadius - (*proj)(3));
                        } else {
                            points->getSeparatingNormal().setNeg4(triangleNormal);
                            points->getPosition().setAddMul4(*capsPoint, triangleNormal, -triBRadius - (*proj)(3));
                        }
                        points->setDistance(dist);
                    }
                }
            }

            hkVector4 dir;
            dir.setSub4(projectedPoint[1], projectedPoint[0]);
            for (int j = 0; j < 3; j++) {
                hkReal dw = dir(3);
                hkReal dj = dir(j);
                hkReal invLen2 = 1.0f / (dw * dw + dj * dj);
                hkReal c = dj * projectedPoint[0](3) - dw * projectedPoint[0](j);
                hkReal distSq = invLen2 * (c * c);
                hkReal maxD = radiusSum + points->getDistance();
                if (distSq < maxD * maxD) {
                    hkReal f = (dj * projectedPoint[0](j) + dw * projectedPoint[0](3)) * -invLen2;
                    if (f > -0.0001f && f < 1.0001f) {
                        hkVector4 row;
                        planeEquations.getRotation().getRow(j, row);
                        if (dw < 0.0f) {
                            dw = -dw;
                            dj = -dj;
                        }
                        row.mul4(dw);
                        points->getSeparatingNormal().setAddMul4(row, triangleNormal, -dj);
                        points->getSeparatingNormal().normalize3();
                        hkVector4 cp;
                        cp.setInterpolate4(capsAPoints[0], capsAPoints[1], f);
                        hkReal d = -(hkMath::sqrt)(distSq);
                        points->getPosition().setAddMul4(cp, points->getNormal(), triBRadius - d);
                        points->setDistance(d - radiusSum);
                    }
                }
            }
            return;
        }
    }

    int andMask;
    int i = 0;
    hkVector4 capsDir;
    capsDir.setSub4(capsAPoints[1], capsAPoints[0]);
    int checkBoth = 0;
    int orMask = mask[0] | mask[1];
    if ((orMask & 0xe) != 0xe) {
        if (searchManifold) {
            points[0].setDistance(collisionTolerance);
            points[1].setDistance(collisionTolerance);
            points[2].setDistance(collisionTolerance);
            hkContactPoint best;
            best.getSeparatingNormal()(3) = HK_REAL_MAX;
            hkVector4 edge[2];
            int bit = 8;
            for (int j = 0; j < 3; j++, bit >>= 1) {
                if (bit & orMask) {
                    continue;
                }
                edge[0] = triVertices[hkCollideTriangleUtil::getNextModulo3(j)];
                edge[1] = triVertices[hkCollideTriangleUtil::getPrevModulo3(j)];
                hkCollideCapsuleUtilManifoldCapsVsCaps(capsAPoints, capsARadius, edge, triBRadius, points);
                if (points->getSeparatingNormal()(3) > best.getSeparatingNormal()(3)) {
                    *points = best;
                } else {
                    best = *points;
                }
            }
            return;
        }
        // HYPOTHESIS: the target enters the edge loop directly with the flag set (a jump into the loop body);
        // a flag test at the top of the loop body instead scores lower.
        checkBoth = 1;
        goto edges;
    }

    for (; i < 2; i++, points++) {
        if ((mask[i] & 0xe) == 0xe) {
            if (mask[i] & 1) {
                const hkVector4& c = capsAPoints[i];
                hkVector4& p = projectedPoint[i];
                points->getPosition().setAddMul4(c, triangleNormal, -triBRadius - p(3));
                points->getSeparatingNormal().setNeg4(triangleNormal);
                points->setDistance(-p(3) - radiusSum);
            } else {
                const hkVector4& c = capsAPoints[i];
                hkVector4& p = projectedPoint[i];
                points->getPosition().setAddMul4(c, triangleNormal, triBRadius - p(3));
                points->getSeparatingNormal() = triangleNormal;
                points->setDistance(p(3) - radiusSum);
            }
        } else {
        edges:
            andMask = mask[0] & mask[1];
            int bit = 8;
            for (int j = 0; j < 3; j++, bit >>= 1) {
                if (checkBoth == 0) {
                    if (bit & mask[i]) {
                        continue;
                    }
                } else if (bit & andMask) {
                    continue;
                }
                int next = hkCollideTriangleUtil::getNextModulo3(j);
                int prev = hkCollideTriangleUtil::getPrevModulo3(j);
                const hkVector4& start = triVertices[next];
                hkVector4 edgeDir;
                edgeDir.setSub4(triVertices[prev], start);
                hkCollideTriangleUtil::ClosestLineSegLineSegResult result;
                int flags = hkCollideTriangleUtil::closestLineSegLineSeg(capsAPoints[0], capsDir, start, edgeDir, result);
                hkReal maxD = radiusSum + points->getDistance();
                if (result.m_distanceSquared < maxD * maxD) {
                if (flags & (1 << i)) {
                    if (checkBoth == 0) {
                        continue;
                    }
                    i = 1; // HYPOTHESIS: the target sets the endpoint index to 1 here (li r29, 1).
                }
                hkVector4 normal;
                if (flags == 0) {
                    normal.setCross(capsDir, edgeDir);
                } else if (result.m_distanceSquared <= HK_REAL_EPSILON * HK_REAL_EPSILON) {
                    planeEquations.getRotation().getRow(j, result.m_closestAminusClosestB);
                    normal.setCross(capsDir, edgeDir);
                    if (normal.lengthSquared3() <= HK_REAL_EPSILON * HK_REAL_EPSILON) {
                        normal = result.m_closestAminusClosestB;
                    }
                } else {
                    normal = result.m_closestAminusClosestB;
                }
                normal.normalize3();
                hkReal dist = normal.dot3(result.m_closestAminusClosestB);
                if (dist < 0.0f) {
                    dist = -dist;
                    normal.setNeg4(normal);
                }
                points->getPosition().setAddMul4(result.m_closestPointA, normal, triBRadius - dist);
                points->getSeparatingNormal() = normal;
                points->setDistance(dist - radiusSum);
                }
            }
        }
        if (checkBoth) {
            return;
        }
    }
}
#pragma pop

// The table holds the predecessors at [0..2] and the successors at [2..4].
int hkCollideTriangleUtil::getNextModulo3(int i) {
    return lbl_805A47DC[i + 2];
}

int hkCollideTriangleUtil::getPrevModulo3(int i) {
    return lbl_805A47DC[i];
}

// Closest point on the segment a..b to p. The parameter is the projection of (p - a) on (b - a).
int hkCollideTriangleUtil::closestPointLineSeg(const hkVector4& p, const hkVector4& a, const hkVector4& b, hkVector4& out) {
    hkVector4 ab;
    ab.setSub4(b, a);
    hkVector4 ap;
    ap.setSub4(p, a);
    hkReal dot = ab.dot3(ap);
    if (dot <= 0.0f) {
        out = a;
        return 8;
    }
    hkReal lenSq = ab.lengthSquared3();
    if (dot >= lenSq) {
        out = b;
        return 4;
    }
    hkReal t = dot / lenSq;
    hkVector4 step;
    step.setMul4(ab, t);
    out.setAdd4(a, step);
    return 0;
}
