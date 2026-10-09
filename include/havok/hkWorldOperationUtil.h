#pragma once

#include <havok/hkBase.h>
#include <havok/hkWorld.h>
#include <havok/hkWorldOperationQueue.h>

struct hkSimulationIsland;
struct hkConstraintInstance;
struct hkEntity;
struct hkRigidBody;

// Operations on the world's islands, entities, phantoms and constraints, run while the world is locked
// or when queued operations are executed.
struct hkWorldOperationUtil {
    static void addConstraintToCriticalLockedIsland(hkWorld* world, hkConstraintInstance* constraint);
    static void removeConstraintFromCriticalLockedIsland(hkWorld* world, hkConstraintInstance* constraint);
    static void splitSimulationIslands(hkWorld* world);
    static void splitSimulationIslands1(hkWorld* world, u8* flag);
    static void removeIslandFromDirtyList(hkWorld* world, hkSimulationIsland* island);
    static void removeIsland(hkWorld* world, hkSimulationIsland* island);
    static void mergeIslands(hkWorld* world, hkEntity* entityA, hkEntity* entityB);
    static void internalMergeTwoIslands(hkWorld* world, hkSimulationIsland* islandA, hkSimulationIsland* islandB);
    static void removeConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback);
    static hkConstraintInstance* addConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback);
    static void markIslandInactive(hkWorld* world, hkSimulationIsland* island);
    static void markIslandActive(hkWorld* world, hkSimulationIsland* island);
    static void setRigidBodyMotionType(hkRigidBody* body, u8 motionType, u8 a, u8 b);
};
