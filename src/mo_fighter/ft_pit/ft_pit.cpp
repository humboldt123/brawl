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

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_112_DC80(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_112_DC88(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_112_DC90(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_112_DC98(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_112_DCA8(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_112_DCE8(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_112_DCF0(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_112_DCF8(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_112_DD30(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_112_DD40(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_112_DD48(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_112_DD50(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_112_DD58(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_112_DD60(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_112_DD68(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_112_DD70(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_112_DD78(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_112_DD80(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_112_DD88(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_112_DD90(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_112_DD98(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_112_DDA0(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_112_DDA8(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_112_DDB0(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_112_DDB8(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_112_DDC0(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_112_DDC8(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_112_DDD0(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_112_DDD8(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
void fn_112_DCA0(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_112_DCB0(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
bool fn_112_DCC0(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_112_DD08(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_112_DD20(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_112_DD38(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"
