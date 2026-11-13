#pragma once

#include <havok/hkBoxBoxManifold.h>
#include <havok/hkTransform.h>
#include <havok/hkCollisionAgent.h>
#include <havok/hkCdBody.h>
#include <havok/hkCollisionInput.h>

struct hkProcessCollisionOutput;
struct hkContactPoint;

// B's transform in A's frame as the detector keeps it. MATCH-ONLY: the column-wise copy through four temporaries
// (the box agents keep the temporaries' dead stack stores, at descending addresses); as a global hkTransform copy
// constructor it costs hkTransformAgent's create functions.
struct hkBoxBoxTransform : hkTransform {
    hkBoxBoxTransform(const hkTransform& t) {
        const hkVector4 r0 = t.m_rotation.m_col0;
        const hkVector4 r1 = t.m_rotation.m_col1;
        const hkVector4 r2 = t.m_rotation.m_col2;
        const hkVector4 r3 = t.m_translation;
        m_rotation.m_col0 = r0;
        m_rotation.m_col1 = r1;
        m_rotation.m_col2 = r2;
        m_translation = r3;
    }
};

// Box-box collision detector (hkBoxBoxCollisionDetection.o). A stack object built by the box agents.
// Layout from the unit's functions (asm); HYPOTHESIS: the member names (Havok style).
struct hkBoxBoxCollisionDetection {
    // Closest-feature result of findClosestPoint (0x40 bytes, 16-byte aligned: the callers keep it on an aligned
    // stack). Its constructor is empty (out-of-line copy at 0x80303700).
    struct hkFeaturePointCache {
        hkVector4 unk0;        // 0x00 not identified
        hkVector4 unk10;       // 0x10 not identified
        hkVector4 m_normal;    // 0x20 separating normal (edge-edge case)
        hkReal m_normalScale;  // 0x30 the face normal is the face axis scaled by -m_normalScale
        hkReal m_distance;     // 0x34 contact distance (the w of the contact normal)
        int m_featureIndex;    // 0x38 0..2 face of A, 3..6 face of B, above: edge-edge (then A's edge feature id)
        int m_featureIndexB;   // 0x3C B's edge feature id (edge-edge)

        hkFeaturePointCache() {}
    };

    const hkCdBody& m_bodyA;             // 0x00
    const hkCdBody& m_bodyB;             // 0x04
    const hkProcessCollisionInput* m_env; // 0x08 HYPOTHESIS: the name
    hkContactMgr* m_contactMgr;          // 0x0C receives the removed contact point ids
    hkProcessCollisionOutput* m_result;  // 0x10 output that receives the manifold's contact points
    const hkTransform& m_wTa;            // 0x14 world transform of box A
    const hkTransform& m_wTb;            // 0x18 world transform of box B
    u8 unk1C[0x20 - 0x1C];               // 0x1C not identified
    hkBoxBoxTransform m_aTb;                 // 0x20 B in the frame of A
    hkVector4 m_radiusA;                 // 0x60 half extents of A
    hkVector4 m_radiusB;                 // 0x70 half extents of B
    hkVector4 m_tolerance4;              // 0x80 tolerance in all lanes
    hkVector4 m_radiusPlusTolA;          // 0x90 m_radiusA + m_tolerance4 (HYPOTHESIS: the name; not read in this unit)
    hkVector4 m_radiusPlusTolB;          // 0xA0 m_radiusB + m_tolerance4
    hkReal m_boundaryTolerance;          // 0xB0 limit of the edge-edge separations (only x is read)
    hkReal m_minEdgeNormalLength;        // 0xB4 isValidEdgeEdge rejects edge pairs whose cross product is shorter
    hkVector4 m_dinA;                    // 0xC0 translation of B in A (copy of m_aTb's translation)
    hkVector4 m_dinB;                    // 0xD0 translation of A in B, rotated (initWorkVariables)
    hkVector4 m_sepDist[5];              // 0xE0 separations of the face axes of A, B and the edge axes

    // Built on the stack by the box agents (inline: every caller expands it). The radii already include the shapes'
    // convex radius. BYTES: the tolerance is an hkSimdReal (the aligned typedef): the callers are compiled with copy
    // propagation off (their timer test keeps the neg/or/srwi form), which only an aligned value gives.
    hkBoxBoxCollisionDetection(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkProcessCollisionInput* env,
                               hkContactMgr* mgr, hkProcessCollisionOutput* result, const hkTransform& aTb,
                               const hkTransform& wTa, const hkVector4& radiusA, const hkTransform& wTb,
                               const hkVector4& radiusB, hkSimdReal tolerance)
        : m_bodyA(bodyA), m_bodyB(bodyB), m_env(env), m_contactMgr(mgr), m_result(result), m_wTa(wTa), m_wTb(wTb),
          m_aTb(aTb), m_radiusA(radiusA), m_radiusB(radiusB), m_boundaryTolerance(tolerance),
          m_minEdgeNormalLength(0.01f) {
        m_tolerance4.setAll(tolerance);
        m_radiusPlusTolA.setAdd4(m_tolerance4, m_radiusA);
        m_radiusPlusTolB.setAdd4(m_tolerance4, m_radiusB);
    }

    // Translation of B in A and of A in B (inline: the penetration query and calculateClosestPoint expand it).
    void initWorkVariables() {
        m_dinA = m_aTb.m_translation;
        m_dinB._setRotatedInverseDir(m_aTb.getRotation(), m_aTb.m_translation);
    }
    void calcManifold(hkBoxBoxManifold& manifold);
    // True when the boxes overlap within the tolerance (the penetration query). HYPOTHESIS: the name.
    hkBool getPenetrations() {
        initWorkVariables();
        return checkIntersection(m_tolerance4) == 0;
    }
    void refreshManifold(hkBoxBoxManifold& manifold);
    hkBool queryManifoldNormalConsistency(hkBoxBoxManifold& manifold);
    void calcManifoldNormal(hkVector4& normal, const hkFeatureContactPoint& fcp, hkFeaturePointCache& cache,
                            const hkBool& isEdgeEdge);
    void checkManifoldNormalConsistency(hkBoxBoxManifold& manifold);
    // Separating axis test over the 15 axes; nonzero when the boxes are separated by more than tolerance.
    int checkIntersection(const hkVector4& tolerance);
    void setvdProj(const hkRotation& bRa, int i, int j, hkVector4& out);
    int findClosestPoint(hkBoxBoxManifold& manifold, hkFeatureContactPoint& fcp, hkFeaturePointCache& cache);
    int addPoint(hkBoxBoxManifold& manifold, hkFeaturePointCache& cache, hkFeatureContactPoint& fcp);
    void findAdditionalManifoldPoints(hkBoxBoxManifold& manifold, hkFeatureContactPoint fcp);
    void tryToAddPointFaceA(hkBoxBoxManifold& manifold, hkFeatureContactPoint fcp, u16 planeMask, hkReal minDist);
    void tryToAddPointFaceB(hkBoxBoxManifold& manifold, hkFeatureContactPoint fcp, u16 planeMask, hkReal minDist);
    void tryToAddPointOnEdge(hkBoxBoxManifold& manifold, int edgeA, int edgeB, int nextA, int nextB,
                             const hkVector4& normalA, const hkVector4& normalB, hkReal minDist);
    void addAdditionalEdgeHelper(hkBoxBoxManifold& manifold, hkFeatureContactPoint& fcp, hkReal minDist);
    hkBool calculateClosestPoint(hkContactPoint& contact);
    hkBool isValidEdgeEdge(hkFeaturePointCache& cache);
    void checkCompleteness(hkBoxBoxManifold& manifold, int planeMaskA, int planeMaskB);
};
