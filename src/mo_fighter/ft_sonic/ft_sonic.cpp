#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/sonic/ft_sonic.h>

#define FT_BC ftSonicBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftClassInfoImpl<Fighter_Sonic, ftSonic> g_ftClassInfoSonic;

ftSonic::ftSonic(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftSonicBuildConfig>(entryId,
                                         Fighter_Sonic,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftSonic is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftSonicInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftSonicInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;
#include <so/work/so_general_work_simple.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Targets owned by other units of this module (tail-call thunks).
void fn_123_5094(u8* p);
void fn_123_B2B8(u8* p);
void fn_123_3264(u8* p);
void fn_123_18D8(u8* p);
void fn_123_9F64(u8* p);
bool fn_123_9F48(u8* p, s32 unk1);
void fn_123_9888(u8* p);
void fn_123_8EEC(u8* p);
void fn_123_9944(u8* p);
void fn_123_91E4(u8* p);
void fn_123_9588(u8* p);
void fn_123_9AC4(u8* p);

// Fighter (ftEntryEventObserver / Fighter) empty virtuals.
void fn_123_A184() {}
void fn_123_A188() {}
void fn_123_A18C() {}
void fn_123_A190() {}
void fn_123_A194() {}
void fn_123_A1B8() {}
void fn_123_A1BC() {}
void fn_123_A1D0() {}
void fn_123_A1D4() {}
void fn_123_A1D8() {}
void fn_123_A1DC() {}
void fn_123_A1E0() {}
void fn_123_A1E4() {}
void fn_123_A1E8() {}

// Constant returns.
s32 fn_123_A1B0(u8* p) { return 0; }
bool fn_123_A1C0(u8* p) { return false; }
u32 fn_123_A1C8(u8* p) { return 0; }
s32 fn_123_A590(u8* p) { return 0; }
s32 fn_123_9BC4(u8* p) { return 0; }
s32 fn_123_A144() { return 0; }

// Field getters.
s32 fn_123_A198(u8* p) { return *(s32*)(p + 0x110); }
u8 fn_123_A004(u8* p) { return *(u8*)(p + 0x44); }
u8 fn_123_6E08(u8* p) { return *(u8*)(p + 0x4); }
void* fn_123_A1EC(u8* p, u32 index) { return p + index * 8 + 4; }
void* fn_123_A2AC(u8* p) { return p + 0x458; }
void* fn_123_A2B4(u8* p) { return p + 0x3c8; }
void* fn_123_A2BC(u8* p) { return p + 0x8; }
void* fn_123_A2C4(u8* p) { return p + 0x84; }
void* fn_123_A2CC(u8* p) { return p + 0x70; }
void* fn_123_A2D4(u8* p) { return p + 0x5c; }
void* fn_123_A2DC(u8* p) { return p + 0x48; }
void* fn_123_A2E4(u8* p) { return p + 0x34; }
void* fn_123_A2EC(u8* p) { return p + 0x20; }
void* fn_123_A2F4(u8* p) { return p + 0x8; }
void fn_123_B550(u8* p, u8 value) { *(u8*)(p + 0x4f40) = value; }
s32 fn_123_B548(u8* p) { return 2; }

// Sub-object and collision-state helpers.
void fn_123_C3CC(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_123_C3D8(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_123_C2F8() {}

// soGeneralWorkSimple accessors (flags at +0x1C, ints at +0x0C, floats at +0x14).
bool fn_123_A3D8(const soGeneralWorkSimple* p, u32 flag, u32 index) { return p->soGeneralWorkSimple::isFlag(flag, index); }
void fn_123_A3F4(soGeneralWorkSimple* p, u32 flag, u32 index) { p->soGeneralWorkSimple::offFlag(flag, index); }
void fn_123_A420(soGeneralWorkSimple* p, u32 flag, u32 index) { p->soGeneralWorkSimple::onFlag(flag, index); }
u32 fn_123_A35C(soGeneralWorkSimple* p) { return p->soGeneralWorkSimple::getFlagWorkSize(); }
u32 fn_123_A438(soGeneralWorkSimple* p) { return p->soGeneralWorkSimple::getFloatWorkSize(); }
u32 fn_123_A4D0(soGeneralWorkSimple* p) { return p->soGeneralWorkSimple::getIntWorkSize(); }
void fn_123_A468(soGeneralWorkSimple* p, float value, u32 index) { p->soGeneralWorkSimple::mulFloatWork(value, index); }
void fn_123_A480(soGeneralWorkSimple* p, float value, u32 index) { p->soGeneralWorkSimple::subFloatWork(value, index); }
void fn_123_A498(soGeneralWorkSimple* p, float value, u32 index) { p->soGeneralWorkSimple::addFloatWork(value, index); }
void fn_123_A4B0(soGeneralWorkSimple* p, float value, u32 index) { p->soGeneralWorkSimple::setFloatWork(value, index); }
float fn_123_A4C0(const soGeneralWorkSimple* p, u32 index) { return p->soGeneralWorkSimple::getFloatWork(index); }
void fn_123_A4D8(soGeneralWorkSimple* p, u32 index) { p->soGeneralWorkSimple::decIntWork(index); }
void fn_123_A4F0(soGeneralWorkSimple* p, u32 index) { p->soGeneralWorkSimple::incIntWork(index); }
void fn_123_A528(soGeneralWorkSimple* p, s32 value, u32 index) { p->soGeneralWorkSimple::mulIntWork(value, index); }
void fn_123_A540(soGeneralWorkSimple* p, s32 value, u32 index) { p->soGeneralWorkSimple::subIntWork(value, index); }
void fn_123_A558(soGeneralWorkSimple* p, s32 value, u32 index) { p->soGeneralWorkSimple::addIntWork(value, index); }
void fn_123_A570(soGeneralWorkSimple* p, s32 value, u32 index) { p->soGeneralWorkSimple::setIntWork(value, index); }
s32 fn_123_A580(const soGeneralWorkSimple* p, u32 index) { return p->soGeneralWorkSimple::getIntWork(index); }

// Fighter notifyEvent thunks: subtract the sub-object offset, then tail-call the Fighter method.
void fn_123_EF30(u8* p) { return fn_123_5094(p - 0x4); }
void fn_123_EF38(u8* p) { return fn_123_B2B8(p - 0x4); }
void fn_123_EF40(u8* p) { return fn_123_3264(p - 0x4); }
void fn_123_EF48(u8* p) { return fn_123_18D8(p - 0x40); }
void fn_123_EF50(u8* p, soLinkEventArgs* a0, soModuleAccesser* a1, StageObject* a2, int a3) { return ((Fighter*)(p - 0x54))->Fighter::notifyEventLink(a0, a1, a2, a3); }
void fn_123_EF58(u8* p, int a0, int a1, soStatusData* a2, soModuleAccesser* a3) { return ((Fighter*)(p - 0x64))->Fighter::notifyEventChangeStatus(a0, a1, a2, a3); }
void fn_123_EF60(u8* p, SituationKind a0, SituationKind a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0x70))->Fighter::notifyEventChangeSituation(a0, a1, a2); }
bool fn_123_EF68(u8* p, u32 a0) { return ((Fighter*)(p - 0x7C))->Fighter::notifyEventCollisionAttackCheck(a0); }
void fn_123_EF70(u8* p, soCollisionLog* a0, soModuleAccesser* a1) { return ((Fighter*)(p - 0xB8))->Fighter::notifyEventCollisionSearch(a0, a1); }
void fn_123_EF78(u8* p, soDamage* a0, bool a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0xE8))->Fighter::notifyEventOnDamage(a0, a1, a2); }
void fn_123_EF80(u8* p) { return fn_123_9F64(p - 0x40); }
bool fn_123_EF88(u8* p, acAnimCmd* a0, soModuleAccesser* a1, int a2) { return ((Fighter*)(p - 0x48))->Fighter::notifyEventAnimCmd(a0, a1, a2); }
bool fn_123_EF90(u8* p, s32 unk1) { return fn_123_9F48(p - 0x48, unk1); }
void fn_123_EF98(u8* p) { return fn_123_9888(p - 0x54); }
void fn_123_EFA0(u8* p) { return fn_123_8EEC(p - 0x64); }
void fn_123_EFA8(u8* p) { return fn_123_9944(p - 0x70); }
void fn_123_EFB0(u8* p) { return fn_123_91E4(p - 0x7C); }
void fn_123_EFB8(u8* p, float a0, soCollisionLog* a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0x7C))->Fighter::notifyEventCollisionAttack(a0, a1, a2); }
void fn_123_EFC0(u8* p, int a0, soModuleAccesser* a1) { return ((Fighter*)(p - 0x88))->Fighter::notifyEventChangeCollisionHit(a0, a1); }
bool fn_123_EFC8(u8* p) { return ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
void fn_123_EFD0(u8* p, soCollisionSearchModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3) { return ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldSearch(a0, a1, a2, a3); }
void fn_123_EFD8(u8* p, soCollisionAttackModule* a0, float a1, soCollisionLog* a2, int a3, float a4, float a5, soModuleAccesser* a6) { return ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShield(a0, a1, a2, a3, a4, a5, a6); }
bool fn_123_EFE0(u8* p) { return ((Fighter*)(p - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
void fn_123_EFE8(u8* p, soCollisionSearchModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3) { return ((Fighter*)(p - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(a0, a1, a2, a3); }
void fn_123_EFF0(u8* p, soCollisionAttackModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3, float a4, float a5, float a6) { return ((Fighter*)(p - 0xA0))->Fighter::notifyEventCollisionReflector(a0, a1, a2, a3, a4, a5, a6); }
bool fn_123_EFF8(u8* p) { return ((Fighter*)(p - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
void fn_123_F000(u8* p, soCollisionAttackModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3, float a4, float a5, float a6) { return ((Fighter*)(p - 0xAC))->Fighter::notifyEventCollisionAbsorber(a0, a1, a2, a3, a4, a5, a6); }
bool fn_123_F008(u8* p) { return ((Fighter*)(p - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_123_F010(u8* p) { return fn_123_9588(p - 0xB8); }
bool fn_123_F018(u8* p, soModuleAccesser* a0, int a1, int a2, int a3) { return ((Fighter*)(p - 0xC4))->Fighter::notifyEventCaptureStatus(a0, a1, a2, a3); }
void fn_123_F020(u8* p, BaseItem* a0, int a1, bool a2, u8 a3) { return ((Fighter*)(p - 0xD0))->Fighter::notifyVisibilityItem(a0, a1, a2, a3); }
void fn_123_F028(u8* p, BaseItem* a0, u32 a1, bool a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyEjectAttachItem(a0, a1, a2); }
void fn_123_F030(u8* p, BaseItem* a0, u32 a1, bool a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyEjectItem(a0, a1, a2); }
void fn_123_F038(u8* p, BaseItem* a0) { return ((Fighter*)(p - 0xD0))->Fighter::notifyShootBulletItem(a0); }
void fn_123_F040(u8* p, BaseItem* a0) { return ((Fighter*)(p - 0xD0))->Fighter::notifyDropItem(a0); }
void fn_123_F048(u8* p, BaseItem* a0, u32 a1, int* a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyThrowItem(a0, a1, a2); }
void fn_123_F050(u8* p, BaseItem* a0, u32 a1, int* a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyUseItem(a0, a1, a2); }
void fn_123_F058(u8* p, BaseItem* a0, u32 a1, u32 a2, bool a3, bool a4) { return ((Fighter*)(p - 0xD0))->Fighter::notifyAttachItem(a0, a1, a2, a3, a4); }
void fn_123_F060(u8* p, itParam::SizeKind a0, BaseItem* a1, u32 a2, u32 a3, bool a4) { return ((Fighter*)(p - 0xD0))->Fighter::notifyHaveItem(a0, a1, a2, a3, a4); }
bool fn_123_F068(u8* p, BaseItem* a0, bool* a1) { return ((Fighter*)(p - 0xD0))->Fighter::notifyHaveItemPreCheck(a0, a1); }
void fn_123_9B94(u8* p, s32 kind);
void fn_123_F070(u8* p, s32 kind) { return fn_123_9B94(p - 0xDC, kind); }
void fn_123_F078(u8* p, soDamage* a0, soModuleAccesser* a1) { return ((Fighter*)(p - 0xE8))->Fighter::notifyEventAddDamage(a0, a1); }
void fn_123_F080(u8* p) { return fn_123_9AC4(p - 0xE8); }
void fn_123_F088(u8* p, float a0, int a1) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventPikminFinalAttack(a0, a1); }
void fn_123_F090(u8* p) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_123_F098(u8* p) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventBeat(); }
void fn_123_F0A0(u8* p, float a0) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventSetDamage(a0); }
void fn_123_F0A8(u8* p, float a0, float a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0x100))->Fighter::notifyEventTurn(a0, a1, a2); }

// Empty notifyEvent observers of ftEntryEventObserver.
void fn_123_A108() {}
void fn_123_A10C() {}
void fn_123_A110() {}
void fn_123_A114() {}
void fn_123_A118() {}
void fn_123_A11C() {}
void fn_123_A120() {}
void fn_123_A124() {}
void fn_123_A128() {}
void fn_123_A12C() {}
void fn_123_A130() {}
void fn_123_A134() {}
void fn_123_A138() {}
void fn_123_A13C() {}
void fn_123_A140() {}
void fn_123_A14C() {}
void fn_123_A150() {}
void fn_123_A154() {}
void fn_123_A158() {}
void fn_123_A000() {}
void fn_123_A00C() {}
void fn_123_A04C() {}
void fn_123_A050() {}

// Sub-object getters and instance-pool lookups (fixed offsets into the unit's object).
void* fn_123_B558(u8* p) { return p + 0xD028; }
void* fn_123_B578(u8* p) { return p + 0xD064; }
void* fn_123_B584(u8* p) { return p + 0xD51C; }
void* fn_123_B5D4(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_B5EC(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_D08C(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_D488(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_D884(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_DC74(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_E078(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_E47C(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_E880(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_EC84(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_D134(u8* p) { return 0; }
void* fn_123_D530(u8* p) { return 0; }
void* fn_123_D92C(u8* p) { return 0; }
void* fn_123_DD1C(u8* p) { return 0; }
void* fn_123_E120(u8* p) { return 0; }
void* fn_123_E524(u8* p) { return 0; }
void* fn_123_E928(u8* p) { return 0; }
void* fn_123_ED2C(u8* p) { return 0; }

// Kinetic transactor/mediator thunks (tail calls into ftKineticTransactor members).
extern "C" void addSpeed__19ftKineticTransactorFPvPvP16soModuleAccesser(void* a, void* b, void* c);
extern "C" void addSpeedOutside__19ftKineticTransactorFiPvPvP16soModuleAccesser(void* a, s32 b, void* c, void* d);
extern "C" void notifyEventChangeStatus__19ftKineticTransactorFPvPvPvPv(void* a, void* b, void* c, void* d);
void fn_123_CCEC(u8* p, void* a, void* b) { return addSpeed__19ftKineticTransactorFPvPvP16soModuleAccesser(a, p + 4, b); }
void fn_123_CCFC(u8* p, void* a, s32 b, void* c) { return addSpeedOutside__19ftKineticTransactorFiPvPvP16soModuleAccesser(a, b, p + 4, c); }
void fn_123_CD10(u8* p, void* a, void* b, void* c, void* d) { return notifyEventChangeStatus__19ftKineticTransactorFPvPvPvPv(a, b, c, d); }
s32 fn_123_CD24() { return 8; }
void fn_123_CCE8() {}

// Fixed-offset virtual slot view (slot k at vtable offset 8 + 4k); only used to emit virtual tail calls.
struct ftSonicVirtualSlots {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual s32 slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6(s32 kind);
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
};

void fn_123_B564(u8* p) { return ((ftSonicVirtualSlots*)(p + 0xD028))->slot2(); }
void fn_123_B5C4(u8* p) { return ((ftSonicVirtualSlots*)p)->slot20(); }
bool fn_123_B590(u8* p) { return ((ftSonicVirtualSlots*)p)->slot3() == 0; }
void fn_123_9BC0() {}
void fn_123_9E88() {}
void fn_123_9BBC() {}

// soGeneralWorkSimple divide helpers (zero-divisor guard is in the header).
void fn_123_A440(soGeneralWorkSimple* p, float value, u32 index) { p->soGeneralWorkSimple::divFloatWork(value, index); }
void fn_123_A508(soGeneralWorkSimple* p, s32 value, u32 index) { p->soGeneralWorkSimple::divIntWork(value, index); }

// Two-word value read from the object (returned in r3:r4).
struct ftSonicWordPair {
    u32 first;
    u32 second;
};
ftSonicWordPair fn_123_9564(u8* p) { return *(ftSonicWordPair*)(p + 0x8); }

extern void* g_soValueVariation;
void* fn_123_A858() { return g_soValueVariation; }

}
