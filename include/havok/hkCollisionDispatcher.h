#pragma once

#include <havok/hkArray.h>
#include <havok/hkCollisionAgent.h>
#include <havok/hkShapeType.h>
#include <havok/hkCdBody.h>
#include <havok/hkShape.h>
#include <havok/hkCollisionInput.h>

// Agent registration record: the creation function, the two static query functions, the linear cast function and
// two symmetry flags. registerCollisionAgent copies it into the dispatcher's table (0x14 bytes per entry).
typedef void (*hkAgentFunc)();
struct hkAgentFuncs {
    // HYPOTHESIS: a constructor clearing both flags; the dead clears fix the constant order of every registerAgent.
    hkAgentFuncs() : symmetricA(false), symmetricB(false) {}
    hkAgentFunc create;                 // 0x00
    hkAgentFunc staticGetPenetrations;  // 0x04
    hkAgentFunc staticGetClosestPoints; // 0x08
    hkAgentFunc staticLinearCast;       // 0x0C
    hkBool symmetricA;                    // 0x10 HYPOTHESIS: the table entry for (typeA, typeB) is also used by the reverse pair
    hkBool symmetricB;                    // 0x11 HYPOTHESIS: when set, the (typeB, typeA) lookup tables are filled too
};

// Registration record of an agent3 (stream) agent (0x30 bytes, map name hkCollisionDispatcher::Agent3Funcs); the agent
// functions take the stream input and the agent's data block, so they are plain function pointers here.
// HYPOTHESIS: the member names (Havok's Agent3Funcs; the slot order is from the registerAgent3 stores).
struct hkAgent3Funcs {
    // HYPOTHESIS: a constructor clearing the optional slots and flags (as hkAgentFuncs; the zero is created first).
    hkAgent3Funcs()
        : m_updateFilterFunc(0), m_invalidateTimFunc(0), m_warpTimeFunc(0), m_isPredictive(false),
          m_ignoreSymmetricVersion(false) {}
    hkAgentFunc m_createFunc;           // 0x00
    hkAgentFunc m_destroyFunc;          // 0x04
    hkAgentFunc m_cleanupFunc;          // 0x08
    hkAgentFunc m_removePointFunc;      // 0x0C
    hkAgentFunc m_commitPotentialFunc;  // 0x10
    hkAgentFunc m_createZombieFunc;     // 0x14
    hkAgentFunc m_updateFilterFunc;     // 0x18
    hkAgentFunc m_invalidateTimFunc;    // 0x1C
    hkAgentFunc m_warpTimeFunc;         // 0x20
    hkAgentFunc m_sepNormalFunc;        // 0x24
    hkAgentFunc m_processFunc;          // 0x28
    hkBool m_isPredictive;              // 0x2C
    hkBool m_ignoreSymmetricVersion;    // 0x2D
};

// Collision dispatcher: reference counted, owns an 8 x 8 table of reference counted entries and the agent registration
// tables filled by registerCollisionAgent. Only the members used by the matched functions are named so far.
// One collision quality's settings (0x3C bytes; layout from hkCollisionDispatcher::initCollisionQualityInfo's stores:
// floats, one byte at 0x10, a halfword at 0x38). HYPOTHESIS: the struct name, from the map name of the init function.
struct hkCollisionQualityInfo {
    hkReal unk0[4];  // 0x00
    // 0x10 copied into the input's m_createPredictiveAgents (processAgentCollideDiscrete); hkBvTreeAgent grows A's
    // aabb by its motion when it is set
    hkBool unk10;
    hkReal unk14[9]; // 0x14
    u16 unk38;       // 0x38 constraint priority of the contact constraints (HYPOTHESIS: name m_constraintPriority)
};

struct hkCollisionDispatcher : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x25) // hkWorld's constructor allocates 0x1C3C bytes as class 0x25; the dtor frees as 0x25

    int unk8; // 0x08 set by the constructor from its argument (HYPOTHESIS: a shared-object parameter)
    // 0x0C: 8 x 8 contact manager factories indexed by the two entities' material response types
    // (hkWorldAgentUtil::addAgent); reference counted, released by the destructor.
    hkContactMgrFactory* m_contactMgrFactory[8][8];
    // 0x10C: per shape type, one bit for every type it can also be treated as (the alternate types registered with
    // registerAlternateShapeType; hkGroupFilter tests the collection, bv tree and multi-sphere bits).
    u32 m_hasAlternateType[32];
    int m_numAgents; // 0x18C number of registered agent functions (index of the next table entry)

    u8 m_lookup190[0x400]; // 0x190 lookup table of registration indices, filled for the (typeA, typeB) pair
    u8 m_lookup590[0x400]; // 0x590 lookup table filled for the reverse pair when symmetricB is set
    hkAgentFuncs m_agentFuncs[64]; // 0x990, stride 0x14
    u8 unkE90[0xE94 - 0xE90];
    u8 m_lookupE94[0x400]; // 0xE94 lookup table for the (typeA, typeB) pair with value 1
    u8 m_lookup1294[0x400]; // 0x1294 lookup table for the reverse pair with value 1
    u8 unk1694[0x19D4 - 0x1694];
    // 0x19D4 collision quality index for each pair of object quality types (hkSimpleConstraintContactMgr's constructor
    // reads [qualityA][qualityB] and takes the constraint priority from that m_collisionQualityInfo entry).
    s8 m_collisionQualityIndex[9][9];
    u8 unk1A25[0x1A28 - 0x1A25];
    // 0x1A28 quality settings, stride 0x3C (initCollisionQualityInfo fills entries 1 to 5, entry 0 is not written).
    hkCollisionQualityInfo m_collisionQualityInfo[6];
    u8 unk1B90[0x1C08 - 0x1B90];

    int getCollisionQualityIndex(int qualityA, int qualityB) const {
        return m_collisionQualityIndex[qualityA][qualityB];
    }
    hkCollisionQualityInfo* getCollisionQualityInfo(int index) {
        return &m_collisionQualityInfo[index];
    }

    hkBool hasAlternateType(int type, int alternateType) const {
        return (m_hasAlternateType[type] & (1 << alternateType)) != 0;
    }

    u8 m_flagA; // 0x1C08 set to 1 by internalRegisterCollisionAgent
    u8 unk1C09[0x1C0C - 0x1C09];
    // The agent track sizes hkSimulationIsland's constructor copies into its track (as u16s).
    // HYPOTHESIS: the names (0x1C0C becomes the track's sector size and bytes used, 0x1C10 the 0x14 halfword).
    int m_agentSectorSize; // 0x1C0C
    int m_agentEntrySize;  // 0x1C10

    u8 m_enableChecks; // 0x1C14 set by setEnableChecks
    u8 unk1C15[0x1C18 - 0x1C15];
    // One entry per registerAlternateShapeType call: primaryType can also be handled as alternateType.
    struct ShapeInheritance {
        hkShapeType m_primaryType;   // 0x00
        hkShapeType m_alternateType; // 0x04
    };
    hkArray<ShapeInheritance> m_shapeInheritance; // 0x1C18, released by the destructor
    void* m_buffers[4]; // 0x1C24..0x1C30 lookup buffers from the memory allocator; released by disableDebugging

    hkCollisionDispatcher(int unk8Value);
    virtual ~hkCollisionDispatcher();

    // Creation function of a table entry (hkAgentFuncs::create).
    typedef hkCollisionAgent* (*CreateFunc)(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& input,
                                            hkContactMgr* mgr);

    // Creates the agent registered for the shape types of the pair; the predictive table when the input asks for it.
    hkCollisionAgent* getNewCollisionAgent(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& input,
                                           hkContactMgr* mgr) {
        int typeA = a.m_shape->getType();
        int typeB = b.m_shape->getType();
        u8* table = input.m_createPredictiveAgents ? m_lookup590 : m_lookup190;
        int index = ((u8(*)[32])table)[typeA][typeB];
        return ((CreateFunc)m_agentFuncs[index].create)(a, b, input, mgr);
    }

    // Static query functions registered for a shape type pair.
    hkAgentFunc getGetPenetrationsFunc(int typeA, int typeB) const {
        return m_agentFuncs[((const u8(*)[32])m_lookup190)[typeA][typeB]].staticGetPenetrations;
    }
    hkAgentFunc getGetClosestPointsFunc(int typeA, int typeB) const {
        return m_agentFuncs[((const u8(*)[32])m_lookup190)[typeA][typeB]].staticGetClosestPoints;
    }
    hkAgentFunc getLinearCastFunc(int typeA, int typeB) const {
        return m_agentFuncs[((const u8(*)[32])m_lookup190)[typeA][typeB]].staticLinearCast;
    }

    void setEnableChecks(const hkBool& enable);

    // Ors the alternate type's bits into the primary type's mask, then repeats for every type that has the primary
    // type as an alternate (depth is the recursion level).
    void updateHasAlternateType(hkShapeType primaryType, hkShapeType alternateType, int depth);
    // Records that primaryType can be treated as alternateType (a previous identical entry is removed first).
    void registerAlternateShapeType(hkShapeType primaryType, hkShapeType alternateType);
    void disableDebugging();

    // Adds one agent registration for the shape type pair (typeA, typeB); symmetric entries are added when
    // funcs->symmetricB is set.
    void registerCollisionAgent(const hkAgentFuncs* funcs, int typeA, int typeB);
    // Adds an agent3 registration for the shape type pair (map name hkCollisionDispatcher::registerAgent3).
    void registerAgent3(const hkAgent3Funcs& funcs, int typeA, int typeB);
    // HYPOTHESIS: fills the entries of one lookup table for the shape type pair. depth is the recursion level used when
    // a type is -1 (all types).
    void internalRegisterCollisionAgent(u8* table, int value, int typeA, int typeB, int typeA2, int typeB2, void* buffer,
                                        int depth);
};
