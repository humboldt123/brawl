#pragma once

#include <havok/hkBase.h>

struct hkConstraintData;
struct hkModifierConstraintAtom;
struct hkEntity;

// A constraint between two entities (0x2C bytes). Layout and names from main.dol's hkConstraintInstanceClass;
// the owner and internal pointers are runtime-only (not serialized). Only the methods other units call are
// declared.
struct hkConstraintInstance : hkReferencedObject {
    enum ConstraintPriority {
        PRIORITY_INVALID = 0,
        PRIORITY_PSI = 1,
        PRIORITY_TOI = 2,
        PRIORITY_TOI_HIGHER = 3,
        PRIORITY_TOI_FORCED = 4
    };
    enum InstanceType {
        TYPE_NORMAL = 0,
        TYPE_CHAIN = 1
    };
    enum AddReferences {
        DO_NOT_ADD_REFERENCES = 0,
        DO_ADD_REFERENCES = 1
    };

    void* m_owner;                                   // 0x08
    hkConstraintData* m_data;                        // 0x0C
    hkModifierConstraintAtom* m_constraintModifiers; // 0x10
    hkEntity* m_entities[2];                         // 0x14
    s8 m_priority;                                   // 0x1C (ConstraintPriority)
    hkBool m_wantRuntime;                            // 0x1D
    const char* m_name;                              // 0x20
    u32 m_userData;                                  // 0x24
    void* m_internal;                                // 0x28

    // Out of line in the (unsplit) hkConstraintInstance unit: 0x802DE764 / 0x802DE7E0 / 0x802DE79C.
    hkConstraintInstance(ConstraintPriority priority);
    virtual ~hkConstraintInstance();
    // calcStatistics                                          0x0C (inherited)
    virtual void entityAddedCallback(hkEntity* entity);   // 0x10
    virtual void entityRemovedCallback(hkEntity* entity); // 0x14 (detaches the constraint, see fireEntityRemoved)
    virtual void entityDeletedCallback(hkEntity* entity); // 0x18
    virtual InstanceType getType() const;                 // 0x1C (TYPE_CHAIN for hkConstraintChainInstance)
    void setPriority(ConstraintPriority priority);

    hkConstraintData* getData() const { return m_data; }
    hkEntity* getEntityA() const { return m_entities[0]; }
    hkEntity* getEntityB() const { return m_entities[1]; }
};
