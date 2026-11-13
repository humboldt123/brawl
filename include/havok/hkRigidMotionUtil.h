#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

struct hkStepInfo;
struct hkVector4;
struct hkEntity;
struct hkVelocityAccumulator;

// Rigid motion helpers from the hkRigidMotionUtil TU.
struct hkRigidMotionUtil {
    // Writes the solver's velocities back into the motions of the entities and steps them: the step, one
    // accumulator per entity, the entities, their count and the offset of the motion in an entity.
    // HYPOTHESIS: unk5 (both callers pass a sixth word; the function does not read it).
    static void hkRigidMotionUtilApplyAccumulators(const hkStepInfo& info, hkVelocityAccumulator* accumulators,
                                                   hkEntity** entities, int numEntities, int motionOffset,
                                                   void* unk5);
    // Steps the motions of the entities on their own (hkSimulation::integrateInternal, islands without
    // constraints): the step, the velocity change from gravity over it, the entities, and the offset of the motion
    // in an entity. HYPOTHESIS: the parameter types.
    static void hkRigidMotionUtilApplyForcesAndStep(const hkStepInfo& stepInfo, const hkVector4& deltaVelocity,
                                                    hkEntity** entities, int numEntities, int motionOffset);
};
