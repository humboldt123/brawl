#pragma once

#include <havok/hkWorld.h>
#include <havok/hkArray.h>

struct hkAction;
struct hkPhantom;
struct hkEntity;
struct hkConstraintInstance;
struct hkSimulationIsland;

// Static helpers that deliver one world event to every listener of one list (hkWorld keeps one list per
// listener kind). Listeners are called back to front; entries removed during the calls (null) are then
// compacted out of the list. Listener lists sit at fixed hkWorld offsets; only 0x108 is named in hkWorld.h.
// Listener interface names are HYPOTHESIS (from the Havok SDK callback names); no RTTI, so slot 2 is the
// virtual destructor where present and the first callback is at 0x0C.
struct hkWorldCallbackUtil {
    struct ActionListener {
        virtual ~ActionListener();
        virtual void actionAdded(hkAction* action);     // 0x0C
        virtual void actionRemoved(hkAction* action);   // 0x10
    };
    struct EntityListener {
        virtual ~EntityListener();
        virtual void entityAdded(hkEntity* entity);         // 0x0C
        virtual void entityRemoved(hkEntity* entity);       // 0x10
        virtual void entityShapeSet(hkEntity* entity);      // 0x14
    };
    struct PhantomListener {
        virtual ~PhantomListener();
        virtual void phantomAdded(hkPhantom* phantom);      // 0x0C
        virtual void phantomRemoved(hkPhantom* phantom);    // 0x10
    };
    struct ConstraintListener {
        virtual ~ConstraintListener();
        virtual void constraintAdded(hkConstraintInstance* constraint);   // 0x0C
        virtual void constraintRemoved(hkConstraintInstance* constraint); // 0x10
    };
    struct WorldDeletionListener {
        virtual ~WorldDeletionListener();
        virtual void worldDeleted(hkWorld* world);              // 0x0C
    };
    struct PostIntegrateListener {
        virtual ~PostIntegrateListener();
        virtual void postIntegrateCallback(hkWorld* world, void* arg);   // 0x0C, arg not recovered
    };
    struct PostCollideListener {
        virtual ~PostCollideListener();
        virtual void postCollideCallback(hkWorld* world, void* arg);     // 0x0C, arg not recovered
    };
    struct IslandPostListener {
        virtual ~IslandPostListener();
        virtual void islandPostCallback(void* a, void* b);               // 0x0C, args not recovered
    };
    // Contact point event passed to the contact listeners; each firing clears one word first (fields not recovered).
    struct ContactPointEvent {
        u8 m_unk00[0x08];
        u32 m_unk08;    // 0x08
        u32 m_unk0C;    // 0x0C
        u32 m_unk10;    // 0x10
    };
    struct ContactListener {
        virtual void contactPointAdded(ContactPointEvent* event);        // 0x08
        virtual void contactPointConfirmed(ContactPointEvent* event);    // 0x0C
        virtual void contactPointRemoved(ContactPointEvent* event);      // 0x10
        virtual void contactProcess(ContactPointEvent* event);           // 0x14
    };
    // Island activation listeners (world list at 0x114). HYPOTHESIS: Havok SDK callback names.
    struct IslandActivationListener {
        virtual ~IslandActivationListener();
        virtual void islandActivated(hkSimulationIsland* island);     // 0x0C
        virtual void islandDeactivated(hkSimulationIsland* island);   // 0x10
    };
    // Entity activation listeners (hkEntity::m_activationListeners at 0x1C0). Slot order per the asm.
    struct EntityActivationListener {
        virtual ~EntityActivationListener();
        virtual void entityDeactivated(hkEntity* entity);   // 0x0C
        virtual void entityActivated(hkEntity* entity);     // 0x10
    };
    struct PostSimulationListener {
        virtual ~PostSimulationListener();
        virtual void postSimulationCallback(hkWorld* world);    // 0x0C
        virtual void inactiveEntityMoved(hkEntity* entity);     // 0x10
    };

    // Compacts a list of listener pointers by removing its null entries (keeps the order of the rest).
    template <typename T>
    static void removeNullListeners(hkArray<T*>& list);

    // Listener list of hkWorld at a fixed byte offset.
    template <typename T>
    static hkArray<T*>& listenersAt(hkWorld* world, int offset) {
        return *(hkArray<T*>*)((u8*)world + offset);
    }

    static void fireActionAdded(hkWorld* world, hkAction* action);
    static void fireActionRemoved(hkWorld* world, hkAction* action);
    static void fireEntityAdded(hkWorld* world, hkEntity* entity);
    static void fireEntityRemoved(hkWorld* world, hkEntity* entity);
    static void fireEntityShapeSet(hkWorld* world, hkEntity* entity);
    static void firePhantomAdded(hkWorld* world, hkPhantom* phantom);
    static void firePhantomRemoved(hkWorld* world, hkPhantom* phantom);
    static void fireConstraintAdded(hkWorld* world, hkConstraintInstance* constraint);
    static void fireConstraintRemoved(hkWorld* world, hkConstraintInstance* constraint);
    static void fireWorldDeleted(hkWorld* world);
    static void firePostSimulationCallback(hkWorld* world);
    static void fireInactiveEntityMoved(hkWorld* world, hkEntity* entity);
    static void firePostIntegrateCallback(hkWorld* world, void* arg);
    static void firePostCollideCallback(hkWorld* world, void* arg);
    static void fireIslandActivated(hkWorld* world, hkSimulationIsland* island);
    static void fireIslandDeactivated(hkWorld* world, hkSimulationIsland* island);
    static void fireContactPointAdded(hkWorld* world, ContactPointEvent* event);
    static void fireContactPointConfirmed(hkWorld* world, ContactPointEvent* event);
    static void fireContactPointRemoved(hkWorld* world, ContactPointEvent* event);
    static void fireContactProcess(hkWorld* world, ContactPointEvent* event);
    static void fireIslandPostIntegrateCallback(hkWorld* world, void* a, void* b);
    static void fireIslandPostCollideCallback(hkWorld* world, void* a, void* b);
};
