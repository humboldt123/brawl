#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkConstraintData.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkMemory.h>
#include <havok/hkMultiThreadLock.h>

struct hkEntity;
struct hkWorld;
struct hkAction; // include/havok/hkAction.h; the island stores itself in the action at 0x0C
struct hkStatisticsCollector;
struct hkConstraintInstance;
struct hkStepInfo;

// The island's state bits at 0x26 (MWCC allocates bit fields from the top: m_stateA is 0xC0, m_stateB 0x30).
// HYPOTHESIS: the names. Setting m_stateA to 1 marks the island for a split check (internalRemoveEntity,
// hkWorldAgentUtil::removeAgent).
struct hkSimulationIslandFlags {
    unsigned char m_stateA : 2;
    unsigned char m_stateB : 2;
    unsigned char m_stateC : 2; // 0x0C cleared by the constructor
    unsigned char m_stateD : 2; // 0x03 set to 1 by the constructor

    hkSimulationIslandFlags() {
        m_stateA = 0;
        m_stateB = 0;
        m_stateD = 1;
    }
};

// The bit fields of 0x27 (the constructor sets the top two bits to 1). HYPOTHESIS: the names.
struct hkSimulationIslandFlags27 {
    unsigned char m_active : 2;
    unsigned char m_rest : 6;

    hkSimulationIslandFlags27() { m_active = 1; }
};

// Simulation island (0x70 bytes; the constructor writes every field but m_storageIndex). Fields used by the island code:
// 0x26 bit field (bits 0xC0 and 0x30, written as flags by internalRemoveEntity/removeAction),
// 0x38 entity array (pointers), 0x60 action array (pointers).
// Base of the island: the constraint bookkeeping slots of its vtable (lbl_80488300: 0x10 add, 0x14 remove,
// 0x18 checkAccessRw; checkAccessRw is a bare return emitted in hkSimulationIsland.o, map name
// hkConstraintOwner::checkAccessRw). HYPOTHESIS: add/remove being pure, and m_constraintInfo living here.
struct hkConstraintOwner : hkReferencedObject {
    hkConstraintInfo m_constraintInfo; // 0x08 merged constraint info (see mergeConstraintInfo)

    virtual void addConstraintToCriticalLockedIsland(hkConstraintInstance* constraint) = 0;
    virtual void removeConstraintFromCriticalLockedIsland(hkConstraintInstance* constraint) = 0;
    virtual void checkAccessRw() {}
};

struct hkSimulationIsland : hkConstraintOwner {
    hkWorld* m_world;                // 0x18 (hkWorldAgentUtil::removeAgentAndItsToiEvents reads the simulation off it)
    u32 m_unk1C;                     // 0x1C (zeroed by the constructor)
    u16 m_storageIndex;              // 0x20 index in the world's island list; 0xFFFF for the fixed island
                                     // HYPOTHESIS: name (hkWorldAgentUtil::addAgent never puts agents there)
    u16 m_dirtyListIndex;            // 0x22 HYPOTHESIS: name (0xFFFF from the constructor)
    u8 m_unk24;                      // 0x24
    u8 m_unk25;                      // 0x25
    hkSimulationIslandFlags m_flags;     // 0x26 bit fields, see above
    hkSimulationIslandFlags27 m_flags27; // 0x27
    hkMultiThreadLock m_multithreadLock; // 0x28
    float m_unk30;                   // 0x30
    float m_unk34;                   // 0x34
    hkInplaceArray<hkEntity*, 1> m_entities; // 0x38 (inline storage at 0x44)
    hkAgentNnTrack m_agentTrack;     // 0x48 the island's agent entries (hkSimulation::collideInternal processes it)
    hkArray<hkAction*> m_actions;    // 0x60
    float m_unk6C;                   // 0x6C (-10 from the constructor)

    HK_DECLARE_REF_ALLOCATOR(0x30)
    hkSimulationIsland(hkWorld* world);
    virtual ~hkSimulationIsland();

    // The 0x26 bit fields as one byte.
    u8& flags26() { return *(u8*)&m_flags; }
    u8 flags26() const { return *(const u8*)&m_flags; }
    hkBool isFixed() const { return m_storageIndex == 0xFFFF; }
    void internalAddEntity(hkEntity* entity);
    void internalRemoveEntity(hkEntity* entity);
    void addAction(hkAction* action);
    void removeAction(hkAction* action);
    virtual void addConstraintToCriticalLockedIsland(hkConstraintInstance* constraint);
    virtual void removeConstraintFromCriticalLockedIsland(hkConstraintInstance* constraint);
    void mergeConstraintInfo(const hkSimulationIsland* other);
    void markForWrite();
    // whether the island can go to sleep after this step (map name; hkDefaultWorldMaintenanceMgr asks it)
    // HYPOTHESIS: the parameter type
    hkBool shouldDeactivate(const hkStepInfo& stepInfo);
};
