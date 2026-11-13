#pragma once

#include <havok/hkDynamicsContactMgr.h>
#include <havok/hkSimpleContactConstraintData.h>
#include <havok/hkConstraintInstance.h>

struct hkWorld;
struct hkRigidBody;

// Contact manager that solves its contact points through a contact constraint (0x6C bytes). Layout from the
// constructor and the factory (allocates 0x6C bytes as class 0x20); vtable lbl_804878C8.
// HYPOTHESIS: the member names.
struct hkSimpleConstraintContactMgr : hkDynamicsContactMgr {
    HK_DECLARE_REF_ALLOCATOR(0x20)

    hkWorld* m_world;                                       // 0x08
    u16 m_reservedContactPoints;                            // 0x0C
    u16 m_skipNextNprocessCallbacks;                        // 0x0E
    hkSimpleContactConstraintData m_contactConstraintData;  // 0x10
    hkConstraintInstance m_constraint;                      // 0x40

    hkSimpleConstraintContactMgr(hkWorld* world, hkRigidBody* bodyA, hkRigidBody* bodyB);
    virtual ~hkSimpleConstraintContactMgr();

    virtual void calcStatistics(hkStatisticsCollector* collector) const;
    virtual u16 addContactPoint(const hkCdBody& a, const hkCdBody& b, const hkProcessCollisionInput& input,
                                hkProcessCollisionOutput& output, const hkGskCache* cache, hkContactPoint& cp);
    virtual hkResult reserveContactPoints(int numPoints);
    virtual void removeContactPoint(u16 contactPointId);
    virtual void processContact(const hkCollidable& a, const hkCollidable& b, const hkProcessCollisionInput& input,
                                hkProcessCollisionOutput& output);
    virtual void cleanup() {
        delete this;
    }
    virtual int addToi(const hkCdBody& a, const hkCdBody& b, const hkProcessCollisionInput& input,
                       hkProcessCollisionOutput& output, hkReal toi, hkContactPoint& cp, hkReal& projectedVelocity,
                       hkContactPointMaterial& materialOut);
    virtual void removeToi(const hkCollidable& a, const hkCollidable& b, hkContactPointMaterial& material);
    virtual void processToi(const void* toiEvent, hkReal rotateNormal, void* entitiesOut);

    virtual hkContactPointProperties* getContactPointProperties(hkContactPointId id);
    virtual hkContactPoint* getContactPoint(hkContactPointId id);
    virtual void getAllContactPointIds(hkArray<hkContactPointId>& contactPointIds) const;
    virtual int getType() const {
        return 0;
    }
    virtual void toiCollisionResponseBeginCallback(const hkContactPoint& cp, void* inA, void* velA, void* inB,
                                                   void* velB) {
        m_contactConstraintData.collisionResponseBeginCallback(cp, inA, velA, inB, velB);
    }
    virtual void toiCollisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied, void* inA,
                                                 void* velA, void* inB, void* velB) {
        m_contactConstraintData.collisionResponseEndCallback(cp, impulseApplied, inA, velA, inB, velB);
    }
    virtual hkConstraintInstance* getConstraintInstance() {
        return &m_constraint;
    }

    // Factory registered with the collision dispatcher (0x0C bytes, vtable lbl_80487914).
    struct Factory : hkContactMgrFactory {
        HK_DECLARE_REF_ALLOCATOR(0x20)

        hkWorld* m_world; // 0x08

        Factory(hkWorld* world);
        virtual hkContactMgr* createContactMgr(const hkCollidable& a, const hkCollidable& b,
                                               const hkCollisionInput& input);
    };
};
