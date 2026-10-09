// Havok translation unit hkBoxBoxCollisionDetection.o (main.dol 0x80302C78-0x8030880C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
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

#include <havok/hkBoxBoxManifold.h>
#include <havok/hkBoxBoxUtils.h>

void hkBoxBoxUtils::cmpAllGT3(const hkVector4& a, const hkVector4& b, int& out) {
    out = (a.x > b.x || a.y > b.x || a.z > b.x) ? 1 : 0;
}

void hkBoxBoxUtils::selectIfGT3(hkVector4& dst, const hkVector4& src, const hkVector4& cond, const hkVector4& threshold) {
    dst.x = (cond.x > threshold.x) ? src.x : dst.x;
    dst.y = (cond.y > threshold.x) ? src.x : dst.y;
    dst.z = (cond.z > threshold.x) ? src.x : dst.z;
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
