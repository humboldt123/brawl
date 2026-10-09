// Havok translation unit hkGenericConstraintData.o (main.dol 0x802DBC80-0x802DD570).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802DBC80    52  finishLoadedObjecthkGenericConstraintData   [map: hkGenericConstraintData__finishLoadedObjecthkGenericConstraintData]
//   0x802DBCB4    20  cleanupLoadedObjecthkGenericConstraintData   [map: hkGenericConstraintData__cleanupLoadedObjecthkGenericConstraintData]
//   0x802DBCC8    72  getVtablehkGenericConstraintData   [map: hkGenericConstraintData__getVtablehkGenericConstraintData]
//   0x802DBD10    88  __ct   [map: hkGenericConstraintData____ct]
//   0x802DBD68   416  __dt   [map: hkGenericConstraintData____dt]
//   0x802DBF08    76  getConstraintInfo   [map: hkGenericConstraintData__getConstraintInfo]
//   0x802DBF54    20  getRuntimeInfo   [map: hkGenericConstraintData__getRuntimeInfo]
//   0x802DBF68   104  buildJacobian   [map: hkGenericConstraintData__buildJacobian]
//   0x802DBFD0   260  constrainAllLinearW   [map: hkGenericConstraintData__constrainAllLinearW]
//   0x802DC0D4   112  calcDeltaAngleAroundAxis   [map: hkGenericConstraintDataParameters__calcDeltaAngleAroundAxis]
//   0x802DC144   248  setLinearFrictionW   [map: hkGenericConstraintData__setLinearFrictionW]
//   0x802DC23C  1212  hatchScheme   [map: hkGenericConstraintData__hatchScheme]
//   0x802DC6F8   108  _setRotatedDir   [map: hkVector4___setRotatedDir]
//   0x802DC764   208  constrainLinearW   [map: hkGenericConstraintData__constrainLinearW]
//   0x802DC834   100  __as   [map: hkRotation____as]
//   0x802DC898   404  constrainToAngularW   [map: hkGenericConstraintData__constrainToAngularW]
//   0x802DCA2C   428  constrainAllAngularW   [map: hkGenericConstraintData__constrainAllAngularW]
//   0x802DCBD8   240  setLinearLimitW   [map: hkGenericConstraintData__setLinearLimitW]
//   0x802DCCC8   344  setAngularLimitW   [map: hkGenericConstraintData__setAngularLimitW]
//   0x802DCE20   452  setConeLimitW   [map: hkGenericConstraintData__setConeLimitW]
//   0x802DCFE4   228  setTwistLimitW   [map: hkGenericConstraintData__setTwistLimitW]
//   0x802DD0C8   512  setAngularMotorW   [map: hkGenericConstraintData__setAngularMotorW]
//   0x802DD2C8   404  setLinearMotorW   [map: hkGenericConstraintData__setLinearMotorW]
//   0x802DD45C   152  setAngularFrictionW   [map: hkGenericConstraintData__setAngularFrictionW]
//   0x802DD4F4     4  __ct   [map: hkGenericConstraintDataParameters____ct]
//   0x802DD4F8     8  begin   [map: hkArray_i___begin]
//   0x802DD500     8  begin   [map: hkArray_9hkVector4___begin]
//   0x802DD508     8  begin   [map: hkArray_P20hkConstraintModifier___begin]
//   0x802DD510     8  isValid   [map: hkGenericConstraintData__isValid]
//   0x802DD518     8  getType   [map: hkGenericConstraintData__getType]
//   0x802DD520    80  __sinit_\hkGenericConstraintData_cpp   [map: hkGenericConstraintDatacpp____sinit_]

#include <havok/hkGenericConstraintData.h>

// Stand-in for a helper in another TU (not yet named).
extern "C" void fn_80288B40(void* a, void* b, u32 c, u32 d);

#pragma fp_contract on

void hkGenericConstraintData::finishLoadedObjecthkGenericConstraintData(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkGenericConstraintData(flag);
}

void hkGenericConstraintData::cleanupLoadedObjecthkGenericConstraintData(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

const void* hkGenericConstraintData::getVtablehkGenericConstraintData() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x58] __attribute__((aligned(16)));
    ::new (buf) hkGenericConstraintData(flag);
    return *(const void**)buf;
}

hkGenericConstraintData::hkGenericConstraintData(hkFinishLoadedObjectFlag flag)
    : hkConstraintData(flag), m_scheme(flag) {
    m_bridgeAtom.init(m_bridgeAtom.m_constraintData);
    m_bridgeAtom.init(this);
}

// Releases the motors (reference counted), then the member arrays are destroyed in reverse order.
hkGenericConstraintData::~hkGenericConstraintData() {
    for (int i = 0; i < m_scheme.m_motors.m_size; i++) {
        ((hkReferencedObject**)m_scheme.m_motors.m_data)[i]->removeReference();
    }
}

void hkGenericConstraintData::getConstraintInfo(hkConstraintInfo* info) {
    info->unk10 = &m_bridgeAtom;
    info->unk14 = (u32)((u8*)&m_scheme - (u8*)&m_bridgeAtom);
    info->unk00 = 0;
    info->unk04 = 0;
    info->unk08 = 0;
    info->unk0C = 0;
    info->unk00 = m_scheme.m_info[0];
    info->unk04 = m_scheme.m_info[1];
    info->unk08 = m_scheme.m_info[2];
    info->unk0C = m_scheme.m_info[3];
}

void hkGenericConstraintData::getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out) {
    u32 n = m_scheme.m_info[3];
    out->unk04 = n;
    out->unk00 = n * 8;
}

void hkGenericConstraintData::buildJacobian(void* a, void* b) {
    u32 n = *(u32*)((u8*)a + 0x44);
    fn_80288B40(a, b, n, 8);
    hatchScheme(&m_scheme, a, b);
}

hkGenericConstraintDataParameters::hkGenericConstraintDataParameters() {
}

hkBool hkGenericConstraintData::isValid() const {
    return hkBool(true);
}

u32 hkGenericConstraintData::getType() const {
    return 10;
}
