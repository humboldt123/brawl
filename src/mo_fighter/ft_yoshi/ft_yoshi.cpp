#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/yoshi/ft_yoshi_builder.h>

#define FT_BC ftYoshiBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftClassInfoImpl<Fighter_Yoshi, ftYoshi> g_ftClassInfoYoshi;

ftYoshi::ftYoshi(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftYoshiBuildConfig>(entryId,
                                         Fighter_Yoshi,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftYoshi is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftYoshiInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftYoshiInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers (ftEntryEventObserver, Fighter, StageObject).
void fn_95_B29C() {}
void fn_95_B424() {}
void fn_95_B430() {}
void fn_95_B470() {}
void fn_95_B474() {}
void fn_95_B504() {}
void fn_95_B530() {}
void fn_95_B534() {}
void fn_95_B538() {}
void fn_95_B53C() {}
void fn_95_B540() {}
void fn_95_B544() {}
void fn_95_B548() {}
void fn_95_B54C() {}
void fn_95_B550() {}
void fn_95_B554() {}
void fn_95_B558() {}
void fn_95_B55C() {}
void fn_95_B560() {}
void fn_95_B564() {}
void fn_95_B568() {}
void fn_95_B574() {}
void fn_95_B578() {}
void fn_95_B57C() {}
void fn_95_B580() {}
void fn_95_B5AC() {}
void fn_95_B5B0() {}
void fn_95_B5B4() {}
void fn_95_B5B8() {}
void fn_95_B5BC() {}
void fn_95_B5E0() {}
void fn_95_B5E4() {}
void fn_95_B5E8() {}
void fn_95_B5FC() {}
void fn_95_B600() {}
void fn_95_B604() {}
void fn_95_B608() {}
void fn_95_B60C() {}
void fn_95_B610() {}

// Constant returns.
int fn_95_AF98() { return 0; }
int fn_95_B56C() { return 0; }
int fn_95_B5D8() { return 0; }
int fn_95_B5EC() { return 0; }
int fn_95_B5F4() { return 0; }
int fn_95_B9B8() { return 0; }

// Field and sub-object accessors.
u8 fn_95_8008(u8* p) { return *(u8*)(p + 0x4); }
u8* fn_95_A108(u8* p) { return *(u8**)(p + 0x22284) + 0x7c; }
u8 fn_95_B428(u8* p) { return *(u8*)(p + 0x44); }
int fn_95_B5C0(u8* p) { return *(int*)(p + 0x110); }
u8* fn_95_B614(u8* p, u32 idx) { return p + 4 + (idx << 3); }
u8* fn_95_B6D4(u8* p) { return p + 0x458; }
u8* fn_95_B6DC(u8* p) { return p + 0x3C8; }
u8* fn_95_B6E4(u8* p) { return p + 0x8; }
u8* fn_95_B6EC(u8* p) { return p + 0x84; }
u8* fn_95_B6F4(u8* p) { return p + 0x70; }
u8* fn_95_B6FC(u8* p) { return p + 0x5C; }
u8* fn_95_B704(u8* p) { return p + 0x48; }
u8* fn_95_B70C(u8* p) { return p + 0x34; }
u8* fn_95_B714(u8* p) { return p + 0x20; }
u8* fn_95_B71C(u8* p) { return p + 0x8; }


// Bit flags in the u32 array at +0x1C.
bool fn_95_B800(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_95_B81C(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_95_B834(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_95_B848(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_95_B890(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_95_B8A8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_95_B8C0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_95_B8D8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_95_B8E8(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_95_B900(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_95_B918(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_95_B930(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_95_B950(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_95_B968(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_95_B980(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_95_B998(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }
int fn_95_B9A8(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); return a[idx]; }

// Work-size getters.
int fn_95_B784(u8* p) { return *(int*)(p + 0x20); }
int fn_95_B860(u8* p) { return *(int*)(p + 0x18); }
int fn_95_B8F8(u8* p) { return *(int*)(p + 0x10); }

// Virtual pointer blocks and methods on the ftYoshi fighter.
void fn_95_A118(ftYoshi* self) { self->deleteGuardColorAnim(); }

// Virtual destructor wrappers without a base destructor call: delete the object when flags > 0.
void* fn_95_1EE4(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_1F80(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_95_233C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_237C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_37C8(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_3864(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_3CC8(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_40B4(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_45D8(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_6138(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_6178(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_61B8(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_727C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

// Wrapper that destroys the Acmd interpreter sub-object, then deletes when flags > 0.
void __dt__16acCmdInterpreterFv(void* p, s16 flags);
void* fn_95_22E0(void* p, s16 flags) {
    if (p != 0) {
        __dt__16acCmdInterpreterFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

// Empty and constant-return slots from the same stage-side observer block.
void fn_95_E65C() {}
void fn_95_E744() {}
void fn_95_E82C() {}
void fn_95_E914() {}
void fn_95_E9FC() {}
void fn_95_EAE4() {}
void fn_95_EBCC() {}
void fn_95_ECB4() {}
int fn_95_E708() { return 0; }
int fn_95_E7F0() { return 0; }
int fn_95_E8D8() { return 0; }
int fn_95_E9C0() { return 0; }
int fn_95_EAA8() { return 0; }
int fn_95_EB90() { return 0; }
int fn_95_EC78() { return 0; }
int fn_95_ED60() { return 0; }

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
void fn_95_EF84(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_95_EF8C(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
void fn_95_EFB4(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
bool fn_95_EFC4(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_95_EFCC(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_95_EFD4(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_95_EFDC(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_95_EFF4(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_95_F00C(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_95_F01C(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_95_F024(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_95_F02C(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_95_F084(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_95_F08C(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_95_F094(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_95_F09C(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_95_F0A4(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_95_F0AC(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_95_F0B4(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }



// Fighter item event thunks, same adjustment and tail call as above.
void fn_95_F034(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_95_F03C(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_95_F044(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_95_F04C(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_95_F054(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_95_F05C(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_95_F064(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_95_F06C(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_95_F074(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_95_F07C(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }

// Kinetic energy and small leaf helpers.
void fn_95_DECC() {}
void fn_95_E5D8() {}
void fn_95_DFB4(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_95_DFC0(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_95_CFFC(u8* p, u8 v) { *(u8*)(p + 0x19DA0) = v; }
void fn_95_B868(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
int fn_95_CFF4() { return 5; }
int fn_95_E614() { return 8; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
bool fn_95_EF9C(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_95_EFE4(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_95_EFEC(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_95_EFFC(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_95_F004(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
void fn_95_F014(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_95_3DA4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_95_41DC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_95_4938(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_95_4978(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
u8* fn_95_E660(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_E748(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_E830(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_E918(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_EA00(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_EAE8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_EBD0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_ECB8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
