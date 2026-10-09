#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkShapeType.h>

// Agent registration record: the creation function, the two static query functions, the linear cast function and
// two symmetry flags. registerCollisionAgent copies it into the dispatcher's table (0x14 bytes per entry).
typedef void (*hkAgentFunc)();
struct hkAgentFuncs {
    hkAgentFunc create;                 // 0x00
    hkAgentFunc staticGetPenetrations;  // 0x04
    hkAgentFunc staticGetClosestPoints; // 0x08
    hkAgentFunc staticLinearCast;       // 0x0C
    u8 symmetricA;                      // 0x10 HYPOTHESIS: the table entry for (typeA, typeB) is also used by the reverse pair
    u8 symmetricB;                      // 0x11 HYPOTHESIS: when set, the (typeB, typeA) lookup tables are filled too
};

// Collision dispatcher: reference counted, owns an 8 x 8 table of reference counted entries and the agent registration
// tables filled by registerCollisionAgent. Only the members used by the matched functions are named so far.
struct hkCollisionDispatcher : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    int unk8; // 0x08 set by the constructor from its argument (HYPOTHESIS: a shared-object parameter)
    // 0x0C: 8 x 8 reference counted entries, released by the destructor (HYPOTHESIS: indexed by shape type).
    hkReferencedObject* m_refTable[8][8];
    // 0x10C: flags indexed by shape type (HYPOTHESIS).
    u32 m_typeFlags[32];
    int m_numAgents; // 0x18C number of registered agent functions (index of the next table entry)

    u8 m_lookup190[0x400]; // 0x190 lookup table of registration indices, filled for the (typeA, typeB) pair
    u8 m_lookup590[0x400]; // 0x590 lookup table filled for the reverse pair when symmetricB is set
    hkAgentFuncs m_agentFuncs[64]; // 0x990, stride 0x14
    u8 unkE90[0xE94 - 0xE90];
    u8 m_lookupE94[0x400]; // 0xE94 lookup table for the (typeA, typeB) pair with value 1
    u8 m_lookup1294[0x400]; // 0x1294 lookup table for the reverse pair with value 1
    u8 unk1694[0x1C08 - 0x1694];

    u8 m_flagA; // 0x1C08 set to 1 by internalRegisterCollisionAgent
    u8 unk1C09[0x1C14 - 0x1C09];
    u8 m_enableChecks; // 0x1C14 set by setEnableChecks
    u8 unk1C15[0x1C18 - 0x1C15];
    void* m_pairs;      // 0x1C18 pair table (8 bytes per entry), released by the destructor
    int m_pairCount;    // 0x1C1C
    u32 m_pairCapacity; // 0x1C20 HYPOTHESIS: bit 31 marks the pair table as not owned
    void* m_buffers[4]; // 0x1C24..0x1C30 lookup buffers from the memory allocator; released by disableDebugging

    hkCollisionDispatcher(int unk8Value);
    virtual ~hkCollisionDispatcher();

    void setEnableChecks(const hkBool& enable);
    void disableDebugging();

    // Adds one agent registration for the shape type pair (typeA, typeB); symmetric entries are added when
    // funcs->symmetricB is set.
    void registerCollisionAgent(const hkAgentFuncs* funcs, int typeA, int typeB);
    // HYPOTHESIS: fills the entries of one lookup table for the shape type pair. depth is the recursion level used when
    // a type is -1 (all types).
    void internalRegisterCollisionAgent(u8* table, int value, int typeA, int typeB, int typeA2, int typeB2, void* buffer,
                                        int depth);
};
