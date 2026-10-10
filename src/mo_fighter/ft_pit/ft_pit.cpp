#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/pit/ft_pit.h>
#include <ft/pit/ft_pit_extend_param_accesser.h>

#define FT_BC ftPitBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftPitExtendParamAccesser g_ftPitExtendParamAccesser;

ftClassInfoImpl<Fighter_Pit, ftPit> g_ftClassInfoPit;

ftPit::ftPit(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPitBuildConfig>(entryId,
                                         Fighter_Pit,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftPit is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPitInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPitInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Virtual slot table of the kinetic and checkActivate forwarders: slot k sits at vtable offset 8 + 4k.
// Declared only; the slots are never defined here.
class ftPitVirtualSlots {
public:
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
    virtual void slot19();
    virtual void* slot20();
};

// ftPit reads the shared parameter-table variation like Marth does (ftManager::setParamPattern).
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

ftKineticEnergyController::~ftKineticEnergyController() { }

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_112_774C(u8* p) { return *(u8*)(p + 0x4); }
void fn_112_956C() {}
int fn_112_A2E0() { return 0x0; }
void fn_112_A448() {}
void fn_112_A5C0() {}
u8 fn_112_A5C4(u8* p) { return *(u8*)(p + 0x44); }
void fn_112_A5CC() {}
void fn_112_A60C() {}
void fn_112_A610() {}
void fn_112_A6A0() {}
void fn_112_A6CC() {}
void fn_112_A6D0() {}
void fn_112_A6D4() {}
void fn_112_A6D8() {}
void fn_112_A6DC() {}
void fn_112_A6E0() {}
void fn_112_A6E4() {}
void fn_112_A6E8() {}
void fn_112_A6EC() {}
void fn_112_A6F0() {}
void fn_112_A6F4() {}
void fn_112_A6F8() {}
void fn_112_A6FC() {}
void fn_112_A700() {}
void fn_112_A704() {}
int fn_112_A708() { return 0x0; }
void fn_112_A710() {}
void fn_112_A714() {}
void fn_112_A718() {}
void fn_112_A71C() {}
void fn_112_A748() {}
void fn_112_A74C() {}
void fn_112_A750() {}
void fn_112_A754() {}
void fn_112_A758() {}
int fn_112_A75C(u8* p) { return *(int*)(p + 0x110); }
int fn_112_A774() { return 0x0; }
void fn_112_A77C() {}
void fn_112_A780() {}
void fn_112_A784() {}
void fn_112_A788() {}
int fn_112_A78C() { return 0x0; }
int fn_112_A794() { return 0x0; }
void fn_112_A79C() {}
void fn_112_A7A0() {}
void fn_112_A7A4() {}
void fn_112_A7A8() {}
void fn_112_A7AC() {}
int fn_112_A920(u8* p) { return *(int*)(p + 0x20); }
int fn_112_A9FC(u8* p) { return *(int*)(p + 0x18); }
int fn_112_AA94(u8* p) { return *(int*)(p + 0x10); }
int fn_112_BDA8() { return 0x3; }
void fn_112_CC00() {}
void fn_112_D390() {}
int fn_112_D43C() { return 0x0; }
void fn_112_D478() {}
int fn_112_D524() { return 0x0; }
int fn_112_D5D4() { return 0x0; }
void fn_112_D610() {}
int fn_112_D6BC() { return 0x0; }
void fn_112_D6F8() {}
int fn_112_D7A4() { return 0x0; }
void fn_112_D7E0() {}
int fn_112_D88C() { return 0x0; }
void fn_112_D8C8() {}
int fn_112_D974() { return 0x0; }
void fn_112_D9B0() {}
int fn_112_DA5C() { return 0x0; }

// Deleting-destructor leaves: return this, and free it when the flag is positive.
#pragma dont_inline on
void* fn_112_1E8C(void* self, s16 flag) {
    if (self != 0) {
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}
void* fn_112_3BA4(void* self, s16 flag) {
    if (self != 0) {
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}
void* fn_112_3E4C(void* self, s16 flag) {
    if (self != 0) {
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}
void* fn_112_3E8C(void* self, s16 flag) {
    if (self != 0) {
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}
void* fn_112_4480(void* self, s16 flag) {
    if (self != 0) {
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}
void* fn_112_4808(void* self, s16 flag) {
    if (self != 0) {
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}
void* fn_112_4AFC(void* self, s16 flag) {
    if (self != 0) {
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}
void* fn_112_5958(void* self, s16 flag) {
    if (self != 0) {
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}
void* fn_112_5998(void* self, s16 flag) {
    if (self != 0) {
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}
void* fn_112_59D8(void* self, s16 flag) {
    if (self != 0) {
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}

#pragma dont_inline off

// ftTeam deleting destructor: runs the base destructor (fn_112_3BA4) without freeing, then frees if flagged.
void* fn_112_6FC4(void* self, s16 flag) {
    if (self != 0) {
        fn_112_3BA4(self, 0);
        if (flag > 0) {
            ::operator delete(self);
        }
    }
    return self;
}

// Single-field and virtual-slot accessors.
// HYPOTHESIS: the 0x21580 byte is the setAutoRecycle flag of the article mediator stored in the fighter tail.
void fn_112_BDB0(u8* self, u8 value) { *(u8*)(self + 0x21580) = value; }

// Instance-pool getInstanceAt for the Parthena and Bow pools: index 0 yields the first instance slot.
void* fn_112_BE38(u8* self, s32 index) {
    if (index == 0) {
        return self + 0xC;
    }
    return 0;
}
void* fn_112_BE98(u8* self, s32 index) {
    if (index == 0) {
        return self + 0xC;
    }
    return 0;
}

// Bow arrow pool getInstanceAt: each index selects a fixed offset into the pool object.
void* fn_112_BE50(u8* self, s32 index) {
    if (index == 3) {
        return self + 0x5FAC;
    }
    if (index == 2) {
        return self + 0x3FD0;
    }
    if (index == 1) {
        return self + 0x1FF4;
    }
    if (index == 0) {
        return self + 0x18;
    }
    return 0;
}

// Virtual slot 0x58 forwarder (checkActivate).
void* fn_112_BE28(u8* self) {
    return ((ftPitVirtualSlots*)self)->slot20();
}

// Kinetic energy update forwarders: call slot 1 when the top bit of byte 5 is set and byte 6 is clear.
struct ftPitKineticUpdateFlags {
    bool active : 1;
    u8 unk : 7;
};
#define PIT_KINETIC_UPDATE_FORWARDER(name)                                   \
    void name(u8* self) {                                                    \
        bool active = ((ftPitKineticUpdateFlags*)(self + 5))->active;        \
        if (active != true) {                                                \
            return;                                                          \
        }                                                                    \
        if (self[6] != 0) {                                                  \
            return;                                                          \
        }                                                                    \
        ((ftPitVirtualSlots*)self)->slot1();                                 \
    }
PIT_KINETIC_UPDATE_FORWARDER(fn_112_D35C)
PIT_KINETIC_UPDATE_FORWARDER(fn_112_D444)
PIT_KINETIC_UPDATE_FORWARDER(fn_112_D5DC)
PIT_KINETIC_UPDATE_FORWARDER(fn_112_D6C4)
PIT_KINETIC_UPDATE_FORWARDER(fn_112_D7AC)
PIT_KINETIC_UPDATE_FORWARDER(fn_112_D894)
PIT_KINETIC_UPDATE_FORWARDER(fn_112_D97C)

}
