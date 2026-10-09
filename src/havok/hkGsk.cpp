// Havok translation unit hkGsk.o (main.dol 0x803183A0-0x8031C8CC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x803183A0   716  findClosestTriangleBackup   [map: hkVector4__findClosestTriangleBackup]
//   0x8031866C  1352  checkTriangleBoundaries   [map: hkGsk__checkTriangleBoundaries]
//   0x80318BB4    28  isNegative   [map: hkMath__isNegative]
//   0x80318BD0     8  setFeatureChange   [map: hkGsk__setFeatureChange]
//   0x80318BD8  1372  findClosestTriangle   [map: hkVector4__findClosestTriangle]
//   0x80319134  1940  processEdgeTriangle   [map: hkGsk__processEdgeTriangle]
//   0x803198C8  1352  getClosestFeature   [map: hkGsk__getClosestFeature]
//   0x80319E10   880  transformPoints   [map: hkVector4Util__transformPoints]
//   0x8031A180  1612  reduceDimension   [map: hkGsk__reduceDimension]
//   0x8031A7CC   724  process_point_edge   [map: hkVector4__process_point_edge]
//   0x8031AAA0    36  __as   [map: hkCdVertex____as]
//   0x8031AAC4   128  hkGskCalcSupportDistSquared   [map: hkVector4__hkGskCalcSupportDistSquared]
//   0x8031AB44   312  exitAndExportCacheImpl   [map: hkGsk__exitAndExportCacheImpl]
//   0x8031AC7C   208  convertFeatureToClosestDistance   [map: hkGsk__convertFeatureToClosestDistance]
//   0x8031AD4C    68  setAddMul4   [map: hkVector4__setAddMul4]
//   0x8031AD90   868  getClosestPoint   [map: hkGsk__getClosestPoint]
//   0x8031B0F4  2312  handlePenetration   [map: hkGsk__handlePenetration]
//   0x8031B9FC   932  reduceDimensionExtended   [map: hkGsk__reduceDimensionExtended]
//   0x8031BDA0  2860  hkGskRecalcContactInternal   [map: hkGsk__hkGskRecalcContactInternal]

#include <havok/hkGsk.h>

void hkGsk::setFeatureChange(int value) {
    unk14 = value;
}
