#pragma once

#include <types.h>

// The physics module's second interface is not declared yet: fighter constructors call slot 0x54 of the vtable at +8 with the
// module pointer itself and a zero argument (HYPOTHESIS: resets the physics state).
// MATCH-ONLY: eight bytes of padding and nineteen placeholder virtuals put the vptr and the called slot where the original has them;
// a plain function pointer table makes the compiler keep the table pointer in an argument register.
struct ftPhysicsIfacePad {
    int unk0;
    int unk4;
};

struct ftPhysicsIface : ftPhysicsIfacePad {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19(int arg);
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25(int arg);
};

static inline void ftPhysicsModuleCall54(void* module, int arg) {
    static_cast<ftPhysicsIface*>(module)->slot19(arg);
}

// Same interface, slot 0x6c (HYPOTHESIS: selects a physics preset by number).
static inline void ftPhysicsModuleCall6C(void* module, int arg) {
    static_cast<ftPhysicsIface*>(module)->slot25(arg);
}
