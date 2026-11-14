// Havok translation unit hkSimpleContactConstraintData.o (main.dol 0x802DD63C-0x802DE624).
// Functions in address order:
//   0x802DD63C   588  allocateContactPoint
//   0x802DD888   428  freeContactPoint
//   0x802DDA34   160  __ct
//   0x802DDAD4    16  getConstraintInfo
//   0x802DDAE4    16  getRuntimeInfo
//   0x802DDAF4     8  getSolverResults
//   0x802DDAFC  1372  (not written: per-point solver setup with a jump table, fn_802DF08C)
//   0x802DE058   224  collisionResponseBeginCallback
//   0x802DE138   248  collisionResponseEndCallback
//   0x802DE230     8  isValid (weak)
//   0x802DE238     8  getType (weak)
//   0x802DE240    28  hkConstraintData::addInstance (weak)
//   0x802DE25C     8  hkConstraintData::getSolverResults (weak; not emitted by this source)
//   0x802DE264   960  hkConstraintData::getConstraintInfoUtil

#include <havok/hkSimpleContactConstraintData.h>
#include <havok/hkConstraintAtom.h>
#include <havok/hkConstraintInstance.h>
#include <havok/hkConstraintInternal.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkEntity.h>
#include <havok/hkWorld.h>

// hkSimpleContactConstraintAtomUtil (unsplit, 0x802DA104-0x802DA49C). HYPOTHESIS: names and parameters.
extern "C" hkSimpleContactConstraintAtom* fn_802DA104(int numReservedContactPoints); // allocateAtom
extern "C" hkSimpleContactConstraintAtom* fn_802DA2C8(hkSimpleContactConstraintAtom* atom); // expandOne
extern "C" void fn_802DA368(hkSimpleContactConstraintAtom* atom, int index);               // removeAtAndCopy
extern "C" hkSimpleContactConstraintAtom* fn_802DA430(hkSimpleContactConstraintAtom* atom, int numFreeElemsLeft); // optimizeCapacity
// hkWorldAgentUtil.o: repoints the constraint's master entry at a moved atom. HYPOTHESIS: name and parameters.
extern "C" void fn_802F58CC(hkConstraintInstance* constraint, hkSimpleContactConstraintAtom* oldAtom,
                            hkSimpleContactConstraintAtom* newAtom, int sizeOfNewAtom);

// hkConstraintOwner's constraint info bookkeeping as inlined here. HYPOTHESIS: names (Havok keeps these on the
// owner; the island's merged info sits at 0x08 and the constraint's internal record mirrors it as u16s).
static inline void addConstraintInfo(hkConstraintInstance* constraint, const hkConstraintInfo& delta) {
    hkConstraintInfo& info = ((hkConstraintOwner*)constraint->m_owner)->m_constraintInfo;
    info.m_maxSizeOfJacobians = info.m_maxSizeOfJacobians > delta.m_maxSizeOfJacobians ? info.m_maxSizeOfJacobians
                                                                                        : delta.m_maxSizeOfJacobians;
    info.m_maxSizeOfJacobians = info.m_maxSizeOfJacobians > delta.m_sizeOfJacobians ? info.m_maxSizeOfJacobians
                                                                                     : delta.m_sizeOfJacobians;
    info.m_sizeOfJacobians += delta.m_sizeOfJacobians;
    info.m_sizeOfSchemas += delta.m_sizeOfSchemas;
    info.m_numSolverResults += delta.m_numSolverResults;
    hkConstraintInternal* internal = (hkConstraintInternal*)constraint->m_internal;
    if (internal) {
        u16 results = internal->m_unk18 + delta.m_numSolverResults;
        u16 schemas = internal->m_unk16 + delta.m_sizeOfSchemas;
        u16 jacobians = internal->m_unk14 + delta.m_sizeOfJacobians;
        internal->m_unk18 = results;
        internal->m_unk14 = jacobians;
        internal->m_unk16 = schemas;
    }
}

static inline void subConstraintInfo(hkConstraintInstance* constraint, const hkConstraintInfo& delta) {
    hkConstraintInfo& info = ((hkConstraintOwner*)constraint->m_owner)->m_constraintInfo;
    info.m_sizeOfJacobians -= delta.m_sizeOfJacobians;
    info.m_sizeOfSchemas -= delta.m_sizeOfSchemas;
    info.m_numSolverResults -= delta.m_numSolverResults;
    hkConstraintInternal* internal = (hkConstraintInternal*)constraint->m_internal;
    if (internal) {
        u16 results = internal->m_unk18 - delta.m_numSolverResults;
        u16 schemas = internal->m_unk16 - delta.m_sizeOfSchemas;
        u16 jacobians = internal->m_unk14 - delta.m_sizeOfJacobians;
        internal->m_unk18 = results;
        internal->m_unk14 = jacobians;
        internal->m_unk16 = schemas;
    }
}

// Reads the constraint member again for each access (a macro-like check in the original).
#define MARK_ISLANDS_FOR_WRITE()                                                           if (((hkSimulationIsland*)m_constraint->m_owner)->m_world->m_unk9C) {                     m_constraint->getEntityA()->m_simulationIsland->markForWrite();                       m_constraint->getEntityB()->m_simulationIsland->markForWrite();                   }

int hkSimpleContactConstraintData::allocateContactPoint(hkContactPoint*& cpOut, hkContactPointProperties*& cpPropsOut) {
    hkConstraintInfo extra;
    hkConstraintInfo delta;
    int oldNumPoints = m_atom->m_numContactPoints;
    m_atom->m_info.m_flags |= 4;

    hkSimpleContactConstraintAtom* oldAtom = m_atom;
    int index = oldAtom->m_numContactPoints;
    m_atom = fn_802DA2C8(oldAtom);

    hkContactPoint* cp = &m_atom->getContactPoints()[index];
    hkContactPointProperties* props = &m_atom->getContactPointProperties()[index];
    props->m_impulseApplied = 0.0f;
    props->m_internalSolverData = 0.0f;
    props->m_flags = 1;
    if (index > 0 && (props[-1].m_flags & 2) == 0) {
        props->m_flags |= 2;
    }
    cpOut = cp;
    cpPropsOut = props;

    delta.m_maxSizeOfJacobians = (oldNumPoints + 1) * 0x30 + 0x80;
    extra.m_sizeOfJacobians = 0;
    extra.m_sizeOfSchemas = 0;
    extra.m_numSolverResults = 0;
    if (oldNumPoints == 1) {
        extra.m_sizeOfSchemas = 4;
        extra.m_sizeOfJacobians = 0x20;
        extra.m_numSolverResults = 1;
    }
    MARK_ISLANDS_FOR_WRITE();
    delta.m_sizeOfJacobians = extra.m_sizeOfJacobians + 0x30;
    delta.m_sizeOfSchemas = extra.m_sizeOfSchemas + 4;
    delta.m_numSolverResults = extra.m_numSolverResults + 1;
    addConstraintInfo(m_constraint, delta);

    if (oldAtom != m_atom) {
        fn_802F58CC(m_constraint, oldAtom, m_atom, m_atom->m_sizeOfAllAtoms);
    }

    for (int i = m_idMgrA.m_values.m_size - 1; i >= 0; i--) {
        if (m_idMgrA.m_values[i] == 0xFF) {
            m_idMgrA.m_values[i] = index;
            return (u16)i;
        }
    }
    m_idMgrA.m_values.pushBack(index);
    return (u16)(m_idMgrA.m_values.m_size - 1);
}

void hkSimpleContactConstraintData::freeContactPoint(u16 id) {
    hkConstraintInfo extra;
    extra.m_sizeOfJacobians = 0;
    extra.m_sizeOfSchemas = 0;
    extra.m_numSolverResults = 0;
    int index = m_idMgrA.m_values[id];
    m_idMgrA.m_values[id] = 0xFF;

    hkSimpleContactConstraintAtom* oldAtom = m_atom;
    int numPoints = oldAtom->m_numContactPoints;
    if (numPoints == 2) {
        extra.m_sizeOfSchemas = 4;
        extra.m_sizeOfJacobians = 0x20;
        extra.m_numSolverResults = 1;
    }
    fn_802DA368(oldAtom, index);
    index[m_atom->getContactPointProperties()].m_flags &= ~2;
    m_atom = fn_802DA430(m_atom, 1);

    for (int i = m_idMgrA.m_values.m_size - 1; i >= 0; i--) {
        u8 v = m_idMgrA.m_values[i];
        if (v != 0xFF && v > index) {
            m_idMgrA.m_values[i] = v - 1;
        }
    }

    MARK_ISLANDS_FOR_WRITE();
    hkConstraintInfo delta;
    delta.m_numSolverResults = extra.m_numSolverResults + 1;
    delta.m_sizeOfSchemas = extra.m_sizeOfSchemas + 4;
    delta.m_sizeOfJacobians = extra.m_sizeOfJacobians + 0x30;
    subConstraintInfo(m_constraint, delta);

    if (oldAtom != m_atom) {
        fn_802F58CC(m_constraint, oldAtom, m_atom, m_atom->m_sizeOfAllAtoms);
    }
    m_atom->m_info.m_flags |= 5;
}

hkSimpleContactConstraintData::hkSimpleContactConstraintData(hkConstraintInstance* constraint) {
    m_constraint = constraint;
    hkSimpleContactConstraintAtom* atom = fn_802DA104(0);
    m_atom = atom;
    atom->m_info.m_flags = 0;
    atom->m_info.m_index = 3;
    // MATCH-ONLY: the seven solver words are cleared as integers (stw of a zero register).
    u32* data = (u32*)&atom->m_info.m_data0C;
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
}

void hkSimpleContactConstraintData::getConstraintInfo(ConstraintInfo& infoOut) const {
    getConstraintInfoUtil((const hkConstraintAtom*)m_atom, m_atom->m_sizeOfAllAtoms, infoOut);
}

void hkSimpleContactConstraintData::getRuntimeInfo(hkBool wantRuntime, hkConstraintRuntimeInfo& infoOut) const {
    infoOut.unk00 = 0;
    infoOut.unk04 = 0;
}

hkSolverResults* hkSimpleContactConstraintData::getSolverResults(hkConstraintRuntime* runtime) const {
    return 0;
}

// Lets the modifier atoms of the constraint adjust a TOI collision response before it is solved.
void hkSimpleContactConstraintData::collisionResponseBeginCallback(const hkContactPoint& cp,
                                                                   hkSimpleConstraintInfoInitInput& inA,
                                                                   hkBodyVelocity& velA,
                                                                   hkSimpleConstraintInfoInitInput& inB,
                                                                   hkBodyVelocity& velB) {
    hkModifierConstraintAtom* modifier = m_constraint->m_constraintModifiers;
    if (modifier) {
        while (modifier->getType() >= hkConstraintAtom::TYPE_MODIFIER_SOFT_CONTACT) {
            switch (modifier->getType()) {
            case hkConstraintAtom::TYPE_MODIFIER_MASS_CHANGER:
                static_cast<hkMassChangerModifierConstraintAtom*>(modifier)->collisionResponseBeginCallback(cp, inA, velA, inB, velB);
                break;
            case hkConstraintAtom::TYPE_MODIFIER_SOFT_CONTACT:
                static_cast<hkSoftContactModifierConstraintAtom*>(modifier)->collisionResponseBeginCallback(cp, inA, velA, inB, velB);
                break;
            case hkConstraintAtom::TYPE_MODIFIER_MOVING_SURFACE:
                static_cast<hkMovingSurfaceModifierConstraintAtom*>(modifier)->collisionResponseBeginCallback(cp, inA, velA, inB, velB);
                break;
            }
            modifier = static_cast<hkModifierConstraintAtom*>(modifier->m_child);
        }
    }
}

void hkSimpleContactConstraintData::collisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied,
                                                                 hkSimpleConstraintInfoInitInput& inA,
                                                                 hkBodyVelocity& velA,
                                                                 hkSimpleConstraintInfoInitInput& inB,
                                                                 hkBodyVelocity& velB) {
    hkModifierConstraintAtom* modifier = m_constraint->m_constraintModifiers;
    if (modifier) {
        while (modifier->getType() >= hkConstraintAtom::TYPE_MODIFIER_SOFT_CONTACT) {
            switch (modifier->getType()) {
            case hkConstraintAtom::TYPE_MODIFIER_MASS_CHANGER:
                static_cast<hkMassChangerModifierConstraintAtom*>(modifier)->collisionResponseEndCallback(cp, impulseApplied, inA, velA, inB, velB);
                break;
            case hkConstraintAtom::TYPE_MODIFIER_SOFT_CONTACT:
                static_cast<hkSoftContactModifierConstraintAtom*>(modifier)->collisionResponseEndCallback(cp, impulseApplied, inA, velA, inB, velB);
                break;
            case hkConstraintAtom::TYPE_MODIFIER_MOVING_SURFACE:
                static_cast<hkMovingSurfaceModifierConstraintAtom*>(modifier)->collisionResponseEndCallback(cp, impulseApplied, inA, velA, inB, velB);
                break;
            }
            modifier = static_cast<hkModifierConstraintAtom*>(modifier->m_child);
        }
    }
}

void hkConstraintData::getConstraintInfoUtil(const hkConstraintAtom* atoms, int sizeOfAllAtoms, ConstraintInfo& infoOut) {
    infoOut.m_atoms = atoms;
    infoOut.m_sizeOfAllAtoms = sizeOfAllAtoms;
    infoOut.m_maxSizeOfJacobians = 0;
    infoOut.m_sizeOfJacobians = 0;
    infoOut.m_numSolverResults = 0;
    infoOut.m_sizeOfSchemas = 0x18;

    if (atoms->isContact()) {
        int n = ((const hkSimpleContactConstraintAtom*)atoms)->m_numContactPoints;
        infoOut.m_numSolverResults = n + 2;
        infoOut.m_maxSizeOfJacobians = n * 0x30 + 0x80;
        infoOut.m_sizeOfJacobians = n * 0x30 + 0x60;
        infoOut.m_sizeOfSchemas = n * 4 + 0x30;
        if (n >= 2) {
            infoOut.m_sizeOfJacobians += 0x20;
            infoOut.m_sizeOfSchemas += 4;
            infoOut.m_numSolverResults += 1;
        }
        return;
    }

    const hkConstraintAtom* atomsEnd = (const hkConstraintAtom*)((const u8*)atoms + sizeOfAllAtoms);
    const u8* atom = (const u8*)atoms;
    while (atom < (const u8*)atomsEnd) {
    nextAtom:
        switch (((const hkConstraintAtom*)atom)->m_type) {
        case hkConstraintAtom::TYPE_INVALID:
            atom = (const u8*)(((u32)atom + 15) & ~15);
            goto nextAtom;
        case hkConstraintAtom::TYPE_BALL_SOCKET:
            atom += 2;
            infoOut.m_sizeOfSchemas += 0xc;
            infoOut.m_sizeOfJacobians += 0x90;
            infoOut.m_numSolverResults += 3;
            break;
        case hkConstraintAtom::TYPE_STIFF_SPRING:
            atom += 8;
            infoOut.m_sizeOfSchemas += 4;
            infoOut.m_sizeOfJacobians += 0x30;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_LIN:
            atom += 4;
            infoOut.m_sizeOfSchemas += 4;
            infoOut.m_sizeOfJacobians += 0x30;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_LIN_SOFT:
            atom += 0xc;
            infoOut.m_sizeOfSchemas += 0xc;
            infoOut.m_sizeOfJacobians += 0x30;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_LIN_LIMIT:
            atom += 0xc;
            infoOut.m_sizeOfSchemas += 0xc;
            infoOut.m_sizeOfJacobians += 0x30;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_LIN_FRICTION:
            atom += 8;
            infoOut.m_sizeOfSchemas += 0xc;
            infoOut.m_sizeOfJacobians += 0x30;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_LIN_MOTOR:
            atom += 0x10;
            infoOut.m_sizeOfSchemas += 0x1c;
            infoOut.m_sizeOfJacobians += 0x30;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_2D_ANG:
            atom += 4;
            infoOut.m_sizeOfSchemas += 8;
            infoOut.m_sizeOfJacobians += 0x40;
            infoOut.m_numSolverResults += 2;
            break;
        case hkConstraintAtom::TYPE_ANG: {
            int numAxes = atom[3];
            atom += 4;
            infoOut.m_sizeOfSchemas += numAxes * 4;
            infoOut.m_sizeOfJacobians += numAxes * 0x20;
            infoOut.m_numSolverResults += numAxes;
            break;
        }
        case hkConstraintAtom::TYPE_ANG_LIMIT:
            atom += 0x10;
            infoOut.m_sizeOfSchemas += 0x10;
            infoOut.m_sizeOfJacobians += 0x20;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_CONE_LIMIT:
            atom += 0x14;
            infoOut.m_sizeOfSchemas += 0x10;
            infoOut.m_sizeOfJacobians += 0x20;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_TWIST_LIMIT:
            atom += 0x14;
            infoOut.m_sizeOfSchemas += 0x10;
            infoOut.m_sizeOfJacobians += 0x20;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_ANG_FRICTION: {
            int numAxes = atom[4];
            atom += 0xc;
            infoOut.m_sizeOfSchemas += numAxes * 0xc;
            infoOut.m_sizeOfJacobians += numAxes * 0x20;
            infoOut.m_numSolverResults += numAxes;
            break;
        }
        case hkConstraintAtom::TYPE_ANG_MOTOR:
            atom += 0x14;
            infoOut.m_sizeOfSchemas += 0x1c;
            infoOut.m_sizeOfJacobians += 0x20;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_RAGDOLL_MOTOR:
            atom += 0x50;
            infoOut.m_sizeOfSchemas += 0x54;
            infoOut.m_sizeOfJacobians += 0x60;
            infoOut.m_numSolverResults += 3;
            break;
        case hkConstraintAtom::TYPE_PULLEY:
            atom += 0x40;
            infoOut.m_sizeOfSchemas += 0xc;
            infoOut.m_sizeOfJacobians += 0x40;
            infoOut.m_numSolverResults += 1;
            break;
        case hkConstraintAtom::TYPE_SET_LOCAL_TRANSFORMS:
            atom += 0x90;
            break;
        case hkConstraintAtom::TYPE_SET_LOCAL_ROTATIONS:
            atom += 0x70;
            break;
        case hkConstraintAtom::TYPE_SET_LOCAL_TRANSLATIONS:
            atom += 0x30;
            break;
        case hkConstraintAtom::TYPE_BRIDGE:
        case hkConstraintAtom::TYPE_MODIFIER_SOFT_CONTACT:
        case hkConstraintAtom::TYPE_MODIFIER_MASS_CHANGER:
        case hkConstraintAtom::TYPE_MODIFIER_VISCOUS_SURFACE:
        case hkConstraintAtom::TYPE_MODIFIER_MOVING_SURFACE:
            atom = (const u8*)atomsEnd;
            break;
        case hkConstraintAtom::TYPE_OVERWRITE_PIVOT:
        case hkConstraintAtom::TYPE_CONTACT:
            break;
        }
    }
}
