#pragma once

#include <havok/hkBase.h>
#include <havok/hkWorld.h>
#include <havok/hkWorldOperationQueue.h>

struct hkSimulationIsland;
struct hkConstraintInstance;
struct hkEntity;
struct hkRigidBody;
struct hkPhantom;
struct hkAction;

// Operations on the world's islands, entities, phantoms and constraints, run while the world is locked
// or when queued operations are executed.
struct hkWorldOperationUtil {
    static void removeEntitySI(hkWorld* world, hkEntity* entity);
    static void removeAttachedActionsFromFixedIsland(hkWorld* world, hkEntity* entity,
                                                       hkArray<hkAction*>& actionsToBeMoved);
    static void removeAttachedActionsFromDynamicIsland(hkWorld* world, hkEntity* entity,
                                                       hkArray<hkAction*>& actionsToBeMoved);
    static void addConstraintToCriticalLockedIsland(hkWorld* world, hkConstraintInstance* constraint);
    static void removeConstraintFromCriticalLockedIsland(hkWorld* world, hkConstraintInstance* constraint);
    static void splitSimulationIslands(hkWorld* world);
    // (map name; hkSimulation::integrateInternal runs it before the actions). HYPOTHESIS: what it cleans up.
    static void cleanupDirtyIslands(hkWorld* world);
    static void internalActivateIsland(hkWorld* world, hkSimulationIsland* island);
    static void internalDeactivateIsland(hkWorld* world, hkSimulationIsland* island);
    static void splitSimulationIslands1(hkWorld* world, u8* flag);
    static void removeIslandFromDirtyList(hkWorld* world, hkSimulationIsland* island);
    static void removeIsland(hkWorld* world, hkSimulationIsland* island);
    static void mergeIslands(hkWorld* world, hkEntity* entityA, hkEntity* entityB);
    static void internalMergeTwoIslands(hkWorld* world, hkSimulationIsland* islandA, hkSimulationIsland* islandB);
    static void removeConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback);
    static hkConstraintInstance* addConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback);
    static void markIslandInactive(hkWorld* world, hkSimulationIsland* island);
    static void markIslandActive(hkWorld* world, hkSimulationIsland* island);
    static void updateEntityBP(hkWorld* world, hkEntity* entity);
    static void addEntityBP(hkWorld* world, hkEntity* entity);
    static void removeEntityBP(hkWorld* world, hkEntity* entity);
    static void addPhantomBP(hkWorld* world, hkPhantom* phantom);
    static void removePhantomBP(hkWorld* world, hkPhantom* phantom);
    static void setRigidBodyMotionType(hkRigidBody* body, u8 motionType, u8 a, u8 b);
};
