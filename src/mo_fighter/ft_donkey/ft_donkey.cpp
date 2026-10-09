#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/donkey/ft_donkey.h>
#include <ft/donkey/ft_donkey_extend_param_accesser.h>

#define FT_BC ftDonkeyBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftDonkeyExtendParamAccesser g_ftDonkeyExtendParamAccesser;

ftClassInfoImpl<Fighter_Donkey, ftDonkey> g_ftClassInfoDonkey;

ftDonkey::ftDonkey(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftDonkeyBuildConfig>(entryId,
                                         Fighter_Donkey,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftDonkey is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftDonkeyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftDonkeyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_92_6E9C(u8* p) { return *(u8*)(p + 0x4); }
void fn_92_8C3C() {}
int fn_92_92F8() { return 0x0; }
void fn_92_947C() {}
void fn_92_95F4() {}
u8 fn_92_95F8(u8* p) { return *(u8*)(p + 0x44); }
void fn_92_9600() {}
void fn_92_9640() {}
void fn_92_9644() {}
void fn_92_96D4() {}
void fn_92_9700() {}
void fn_92_9704() {}
void fn_92_9708() {}
void fn_92_970C() {}
void fn_92_9710() {}
void fn_92_9714() {}
void fn_92_9718() {}
void fn_92_971C() {}
void fn_92_9720() {}
void fn_92_9724() {}
void fn_92_9728() {}
void fn_92_972C() {}
void fn_92_9730() {}
void fn_92_9734() {}
void fn_92_9738() {}
int fn_92_973C() { return 0x0; }
void fn_92_9744() {}
void fn_92_9748() {}
void fn_92_974C() {}
void fn_92_9750() {}
void fn_92_977C() {}
void fn_92_9780() {}
void fn_92_9784() {}
void fn_92_9788() {}
void fn_92_978C() {}
int fn_92_9790(u8* p) { return *(int*)(p + 0x110); }
int fn_92_97A8() { return 0x0; }
void fn_92_97B0() {}
void fn_92_97B4() {}
void fn_92_97B8() {}
void fn_92_97BC() {}
int fn_92_97C0() { return 0x0; }
int fn_92_97C8() { return 0x0; }
void fn_92_97D0() {}
void fn_92_97D4() {}
void fn_92_97D8() {}
void fn_92_97DC() {}
void fn_92_97E0() {}
int fn_92_9954(u8* p) { return *(int*)(p + 0x20); }
int fn_92_9A30(u8* p) { return *(int*)(p + 0x18); }
int fn_92_9AC8(u8* p) { return *(int*)(p + 0x10); }
int fn_92_9B88() { return 0x0; }
int fn_92_AB90() { return 0x2; }
void fn_92_B9D0() {}
void fn_92_C0E0() {}
int fn_92_C11C() { return 0x8; }
void fn_92_C164() {}
int fn_92_C210() { return 0x0; }
void fn_92_C24C() {}
int fn_92_C2F8() { return 0x0; }
void fn_92_C334() {}
int fn_92_C3E0() { return 0x0; }
void fn_92_C41C() {}
int fn_92_C4C8() { return 0x0; }
void fn_92_C504() {}
int fn_92_C5B0() { return 0x0; }
void fn_92_C5EC() {}
int fn_92_C698() { return 0x0; }
void fn_92_C6D4() {}
int fn_92_C780() { return 0x0; }
void fn_92_C7BC() {}
int fn_92_C868() { return 0x0; }

}

// Second placeholder batch: field accessors, flag/array helpers, delete-if-flag destructors and virtual tail calls.
extern "C" {

struct ftDonkeyFlagByte {
    u8 flag : 1;
    u8 rest : 7;
};
struct ftDonkeyVirtualSlots {
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
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
};

// Delete-if-flag virtual destructors.
void* fn_92_1EA4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_395C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_3CA4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_3D80(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_4114(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_448C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_4708(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_4748(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_4F04(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_4F44(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_4F84(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_92_6048(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

// Subobject getters (this + constant).
u8* fn_92_98A4(u8* p) { return p + 0x458; }
u8* fn_92_98AC(u8* p) { return p + 0x3C8; }
u8* fn_92_98B4(u8* p) { return p + 0x8; }
u8* fn_92_98BC(u8* p) { return p + 0x84; }
u8* fn_92_98C4(u8* p) { return p + 0x70; }
u8* fn_92_98CC(u8* p) { return p + 0x5C; }
u8* fn_92_98D4(u8* p) { return p + 0x48; }
u8* fn_92_98DC(u8* p) { return p + 0x34; }
u8* fn_92_98E4(u8* p) { return p + 0x20; }
u8* fn_92_98EC(u8* p) { return p + 0x8; }
u8* fn_92_ABA4(u8* p) { return p + 0x11FEC; }
u8* fn_92_ABC4(u8* p) { return p + 0x12028; }
u8* fn_92_ABD0(u8* p) { return p + 0x124E0; }
u8* fn_92_AC68(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_92_AC20(u8* p, int x) {
    if (x == 3) {
        return p + 0x5F88;
    }
    if (x == 2) {
        return p + 0x3FB8;
    }
    if (x == 1) {
        return p + 0x1FE8;
    }
    if (x == 0) {
        return p + 0x18;
    }
    return 0;
}

// Bit flags in the u32 array at +0x1C.
bool fn_92_99D0(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_92_99EC(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_92_9A04(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_92_9A18(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_92_9A38(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_92_9A60(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_92_9A90(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_92_9AA8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_92_9AB8(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_92_9AD0(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_92_9AE8(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_92_9B00(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_92_9B20(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_92_9B38(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_92_9B50(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_92_9B68(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }

// Byte flags and small copies.
void fn_92_AB98(u8* p, u8 v) { *(u8*)(p + 0x9FF4) = v; }
void fn_92_BAB8(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_92_BAC4(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_92_C124(s16* dst, s16* src) { *dst = *src; }

// Virtual tail calls.
void fn_92_AC10(void* p) { return ((ftDonkeyVirtualSlots*)p)->slot20(); }
void fn_92_ABB0(u8* p) { return ((ftDonkeyVirtualSlots*)(p + 0x11FEC))->slot2(); }










}

// Stage-object thunks: this adjusted by a constant, then a tail call to the Fighter notify method of that base.
extern "C" {

void fn_92_CA8C(u8* p, soLinkEventArgs* a0, soModuleAccesser* a1, StageObject* a2, int a3) { return ((Fighter*)(p - 0x54))->Fighter::notifyEventLink(a0, a1, a2, a3); }
bool fn_92_CA94(u8* p, soModuleAccesser* a0, int a1, int a2, int a3) { return ((Fighter*)(p - 0xC4))->Fighter::notifyEventCaptureStatus(a0, a1, a2, a3); }
void fn_92_CABC(u8* p, int a0, int a1, soStatusData* a2, soModuleAccesser* a3) { return ((Fighter*)(p - 0x64))->Fighter::notifyEventChangeStatus(a0, a1, a2, a3); }
void fn_92_CAC4(u8* p, SituationKind a0, SituationKind a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0x70))->Fighter::notifyEventChangeSituation(a0, a1, a2); }
bool fn_92_CACC(u8* p, u32 a0) { return ((Fighter*)(p - 0x7C))->Fighter::notifyEventCollisionAttackCheck(a0); }
void fn_92_CAD4(u8* p, float a0, soCollisionLog* a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0x7C))->Fighter::notifyEventCollisionAttack(a0, a1, a2); }
void fn_92_CADC(u8* p, int a0, soModuleAccesser* a1) { return ((Fighter*)(p - 0x88))->Fighter::notifyEventChangeCollisionHit(a0, a1); }
bool fn_92_CAE4(u8* p) { return ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
void fn_92_CAEC(u8* p, soCollisionSearchModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3) { return ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldSearch(a0, a1, a2, a3); }
void fn_92_CAF4(u8* p, soCollisionAttackModule* a0, float a1, soCollisionLog* a2, int a3, float a4, float a5, soModuleAccesser* a6) { return ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShield(a0, a1, a2, a3, a4, a5, a6); }
bool fn_92_CAFC(u8* p) { return ((Fighter*)(p - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
void fn_92_CB04(u8* p, soCollisionSearchModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3) { return ((Fighter*)(p - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(a0, a1, a2, a3); }
void fn_92_CB0C(u8* p, soCollisionAttackModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3, float a4, float a5, float a6) { return ((Fighter*)(p - 0xA0))->Fighter::notifyEventCollisionReflector(a0, a1, a2, a3, a4, a5, a6); }
bool fn_92_CB14(u8* p) { return ((Fighter*)(p - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
void fn_92_CB1C(u8* p, soCollisionAttackModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3, float a4, float a5, float a6) { return ((Fighter*)(p - 0xAC))->Fighter::notifyEventCollisionAbsorber(a0, a1, a2, a3, a4, a5, a6); }
bool fn_92_CB24(u8* p) { return ((Fighter*)(p - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_92_CB2C(u8* p, soCollisionLog* a0, soModuleAccesser* a1) { return ((Fighter*)(p - 0xB8))->Fighter::notifyEventCollisionSearch(a0, a1); }
void fn_92_CB3C(u8* p, BaseItem* a0, int a1, bool a2, u8 a3) { return ((Fighter*)(p - 0xD0))->Fighter::notifyVisibilityItem(a0, a1, a2, a3); }
void fn_92_CB44(u8* p, BaseItem* a0, u32 a1, bool a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyEjectAttachItem(a0, a1, a2); }
void fn_92_CB4C(u8* p, BaseItem* a0, u32 a1, bool a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyEjectItem(a0, a1, a2); }
void fn_92_CB54(u8* p, BaseItem* a0) { return ((Fighter*)(p - 0xD0))->Fighter::notifyShootBulletItem(a0); }
void fn_92_CB5C(u8* p, BaseItem* a0) { return ((Fighter*)(p - 0xD0))->Fighter::notifyDropItem(a0); }
void fn_92_CB64(u8* p, BaseItem* a0, u32 a1, int* a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyThrowItem(a0, a1, a2); }
void fn_92_CB6C(u8* p, BaseItem* a0, u32 a1, int* a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyUseItem(a0, a1, a2); }
void fn_92_CB74(u8* p, BaseItem* a0, u32 a1, u32 a2, bool a3, bool a4) { return ((Fighter*)(p - 0xD0))->Fighter::notifyAttachItem(a0, a1, a2, a3, a4); }
void fn_92_CB7C(u8* p, itParam::SizeKind a0, BaseItem* a1, u32 a2, u32 a3, bool a4) { return ((Fighter*)(p - 0xD0))->Fighter::notifyHaveItem(a0, a1, a2, a3, a4); }
bool fn_92_CB84(u8* p, BaseItem* a0, bool* a1) { return ((Fighter*)(p - 0xD0))->Fighter::notifyHaveItemPreCheck(a0, a1); }
void fn_92_CB8C(u8* p, soDamage* a0, soModuleAccesser* a1) { return ((Fighter*)(p - 0xE8))->Fighter::notifyEventAddDamage(a0, a1); }
void fn_92_CB94(u8* p, soDamage* a0, bool a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0xE8))->Fighter::notifyEventOnDamage(a0, a1, a2); }
void fn_92_CB9C(u8* p, float a0, int a1) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventPikminFinalAttack(a0, a1); }
void fn_92_CBA4(u8* p) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_92_CBAC(u8* p) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventBeat(); }
void fn_92_CBB4(u8* p, float a0) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventSetDamage(a0); }
void fn_92_CBBC(u8* p, float a0, float a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0x100))->Fighter::notifyEventTurn(a0, a1, a2); }

}
