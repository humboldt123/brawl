#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/poke/ft_poke_lizardon.h>
#include <ft/poke/ft_poke_lizardon_extend_param_accesser.h>

#define FT_BC ftPokeLizardonBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftPokeLizardonExtendParamAccesser g_ftPokeLizardonExtendParamAccesser;

ftClassInfoImpl<Fighter_PokeLizardon, ftPokeLizardon> g_ftClassInfoPokeLizardon;

ftPokeLizardon::ftPokeLizardon(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPokeLizardonBuildConfig>(entryId,
                                         Fighter_PokeLizardon,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftPokeLizardon is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPokeLizardonInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPokeLizardonInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_116_7384(u8* p) { return *(u8*)(p + 0x4); }
void fn_116_929C() {}
void fn_116_9718() {}
void fn_116_9A98() {}
void fn_116_9DA4() {}
void fn_116_9EC4() {}
int fn_116_A148(u8* p) { return *(int*)(p + 0x28); }
int fn_116_A150() { return 0x0; }
void fn_116_A274() {}
void fn_116_A408() {}
u8 fn_116_A40C(u8* p) { return *(u8*)(p + 0x44); }
void fn_116_A414() {}
void fn_116_A454() {}
void fn_116_A458() {}
void fn_116_A510() {}
void fn_116_A514() {}
void fn_116_A518() {}
void fn_116_A51C() {}
void fn_116_A520() {}
void fn_116_A524() {}
void fn_116_A528() {}
void fn_116_A52C() {}
void fn_116_A530() {}
void fn_116_A534() {}
void fn_116_A538() {}
void fn_116_A53C() {}
void fn_116_A540() {}
void fn_116_A56C() {}
void fn_116_A570() {}
void fn_116_A574() {}
void fn_116_A578() {}
void fn_116_A57C() {}
int fn_116_A580(u8* p) { return *(int*)(p + 0x110); }
int fn_116_A598() { return 0x0; }
void fn_116_A5A0() {}
void fn_116_A5A4() {}
void fn_116_A5A8() {}
int fn_116_A5AC() { return 0x0; }
void fn_116_A5B4() {}
void fn_116_A5B8() {}
void fn_116_A5BC() {}
int fn_116_A5C0() { return 0x850; }
void fn_116_A5C8() {}
void fn_116_A5CC() {}
void fn_116_A5D0() {}
int fn_116_A5D4() { return 0x0; }
void fn_116_A5DC() {}
int fn_116_A5E0() { return 0x0; }
void fn_116_A5E8() {}
void fn_116_A5EC() {}
int fn_116_A760(u8* p) { return *(int*)(p + 0x20); }
int fn_116_A83C(u8* p) { return *(int*)(p + 0x18); }
int fn_116_A8D4(u8* p) { return *(int*)(p + 0x10); }
int fn_116_BC58() { return 0x3; }
void fn_116_CB00() {}
void fn_116_D290() {}
int fn_116_D33C() { return 0x0; }
void fn_116_D378() {}
int fn_116_D424() { return 0x0; }
void fn_116_D460() {}
int fn_116_D50C() { return 0x0; }
void fn_116_D548() {}
int fn_116_D5F4() { return 0x0; }
void fn_116_D630() {}
int fn_116_D6DC() { return 0x0; }
void fn_116_D718() {}
int fn_116_D7C4() { return 0x0; }
void fn_116_D800() {}
int fn_116_D8AC() { return 0x0; }
void fn_116_D8E8() {}
int fn_116_D994() { return 0x0; }

}

// Deleting-destructor wrappers and trivial delete helpers of this translation unit.
extern "C" {

void* fn_116_3320(void* p, s16 flags);
void* fn_116_3468(void* p, s16 flags);
void* fn_116_4F18(void* p, s16 flags);
void* fn_116_4648(void* p, s16 flags);
void* fn_116_32C4(void* p, s16 flags);
void* fn_116_3254(void* p, s16 flags);
void* fn_116_53D8(void* p, s16 flags);
void* fn_116_4150(void* p, s16 flags);
void* fn_116_4CE8(void* p, s16 flags);
void* fn_116_45E8(void* p, s16 flags);
void* fn_116_4E4C(void* p, s16 flags);
void* fn_116_4EB8(void* p, s16 flags);
void* fn_116_3408(void* p, s16 flags);
void* fn_27_C31CC(void* p, s16 flags);
void* fn_27_63EBC(void* p, s16 flags);
void* fn_27_5C674(void* p, s16 flags);
void* fn_27_6880C(void* p, s16 flags);
void* fn_27_1988E4(void* p, s16 flags);
void __dt__15soKineticEnergyFv(void* p, s16 flags);
void __dt__16soStopModuleImplFv(void* p, s16 flags);

// Plain delete helpers: delete the object when the flag is positive.
#pragma dont_inline on
void* fn_116_1E04(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_3C04(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_3E10(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_42CC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_4CE8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_53D8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_5418(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_5458(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_4150(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_722C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_726C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

#pragma dont_inline reset

// Destructor wrappers: destroy the subobject(s), then delete when the flag is positive.
void* fn_116_3254(void* p, s16 flags) {
    if (p != 0) {
        fn_116_32C4((u8*)p + 0x8, -1);
        fn_116_53D8(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_31F8(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3254(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_32C4(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3320(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3408(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3468((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_44AC(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3408((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4D28(void* p, s16 flags) {
    if (p != 0) {
        fn_116_45E8((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4DEC(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4E4C((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_5378(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4EB8((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_6B20(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4150(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_394C(void* p, s16 flags) {
    if (p != 0) {
        fn_27_C31CC(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_39A8(void* p, s16 flags) {
    if (p != 0) {
        fn_27_63EBC(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3A04(void* p, s16 flags) {
    if (p != 0) {
        fn_27_5C674(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3F84(void* p, s16 flags) {
    if (p != 0) {
        fn_27_6880C((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_6DB0(void* p, s16 flags) {
    if (p != 0) {
        fn_27_1988E4(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_37E8(void* p, s16 flags) {
    if (p != 0) {
        __dt__15soKineticEnergyFv(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_C120(void* p, s16 flags) {
    if (p != 0) {
        __dt__15soKineticEnergyFv(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3FE4(void* p, s16 flags) {
    if (p != 0) {
        __dt__16soStopModuleImplFv(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}

}

// Virtual-base thunks: subtract the subobject offset from this and tail-call the Fighter member.
extern "C" {

void* fn_116_B938(void* p, int flags);
void* fn_116_A34C(void* p, int flags);
void* fn_116_DB98(void* p, s16 flags) { return fn_116_53D8((u8*)p - 0x4, flags); }
void* fn_116_DBA0(void* p, int flags) { return fn_116_B938((u8*)p - 0x4, flags); }
void* fn_116_DBA8(void* p, s16 flags) { return fn_116_3254((u8*)p - 0x4, flags); }
void* fn_116_DBF0(void* p, int flags) { return fn_116_A34C((u8*)p - 0x40, flags); }

bool fn_116_9B9C(u8* p, acAnimCmd* a, soModuleAccesser* m, int u);
bool fn_116_9D68(u8* p, int c);
void fn_116_9A9C(u8* p, soLinkEventArgs* e, soModuleAccesser* m, StageObject* o, int u);
void fn_116_995C(u8* p, int a, int b, soStatusData* s, soModuleAccesser* m);
bool fn_116_9F2C(u8* p, u32 f);
void fn_116_9EC8(u8* p, float pw, soCollisionLog* l, soModuleAccesser* m);

bool fn_116_DBF8(u8* p, acAnimCmd* a, soModuleAccesser* m, int u) { return fn_116_9B9C((u8*)p - 0x48, a, m, u); }
bool fn_116_DC00(u8* p, int c) { return fn_116_9D68((u8*)p - 0x48, c); }
void fn_116_DC08(u8* p, soLinkEventArgs* e, soModuleAccesser* m, StageObject* o, int u) { fn_116_9A9C((u8*)p - 0x54, e, m, o, u); }
void fn_116_DC10(u8* p, int a, int b, soStatusData* s, soModuleAccesser* m) { fn_116_995C((u8*)p - 0x64, a, b, s, m); }
bool fn_116_DC20(u8* p, u32 f) { return fn_116_9F2C((u8*)p - 0x7C, f); }
void fn_116_DC28(u8* p, float pw, soCollisionLog* l, soModuleAccesser* m) { fn_116_9EC8((u8*)p - 0x7C, pw, l, m); }

bool fn_116_DBB8(u8* p, acAnimCmd* a, soModuleAccesser* m, int u) { return ((Fighter*)((u8*)p - 0x48))->Fighter::notifyEventAnimCmd(a, m, u); }
// Non-inline Fighter::isObserv symbol (the header body is inline, so the member call cannot be used here)
extern "C" bool isObserv__7FighterFc(void* self, int c);
bool fn_116_DBC0(u8* p, int c) { return isObserv__7FighterFc((u8*)p - 0x48, c); }
void fn_116_DBC8(u8* p, soLinkEventArgs* e, soModuleAccesser* m, StageObject* o, int u) { ((Fighter*)((u8*)p - 0x54))->Fighter::notifyEventLink(e, m, o, u); }
void fn_116_DBD0(u8* p, int a, int b, soStatusData* s, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0x64))->Fighter::notifyEventChangeStatus(a, b, s, m); }
bool fn_116_DBD8(u8* p, u32 f) { return ((Fighter*)((u8*)p - 0x7C))->Fighter::notifyEventCollisionAttackCheck(f); }
void fn_116_DBE0(u8* p, float pw, soCollisionLog* l, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0x7C))->Fighter::notifyEventCollisionAttack(pw, l, m); }
void fn_116_DBE8(u8* p, soDamage* d, bool b, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0xE8))->Fighter::notifyEventOnDamage(d, b, m); }
void fn_116_DC18(u8* p, SituationKind k, SituationKind pk, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0x70))->Fighter::notifyEventChangeSituation(k, pk, m); }
void fn_116_DC30(u8* p, int i, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0x88))->Fighter::notifyEventChangeCollisionHit(i, m); }
bool fn_116_DC38(u8* p) { return ((Fighter*)((u8*)p - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
void fn_116_DC40(u8* p, soCollisionSearchModule* s, soCollisionLog* l, u32 g, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0x94))->Fighter::notifyEventCollisionShieldSearch(s, l, g, m); }
bool fn_116_DC50(u8* p) { return ((Fighter*)((u8*)p - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
void fn_116_DC58(u8* p, soCollisionSearchModule* s, soCollisionLog* l, u32 g, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(s, l, g, m); }
bool fn_116_DC68(u8* p) { return ((Fighter*)((u8*)p - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_116_DC78(u8* p) { return ((Fighter*)((u8*)p - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_116_DC80(u8* p, soCollisionLog* l, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0xB8))->Fighter::notifyEventCollisionSearch(l, m); }
bool fn_116_DC88(u8* p, soModuleAccesser* m, int t, int a, int b) { return ((Fighter*)((u8*)p - 0xC4))->Fighter::notifyEventCaptureStatus(m, t, a, b); }
void fn_116_DC90(u8* p, BaseItem* item, int a, bool b, u8 c) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyVisibilityItem(item, a, b, c); }
void fn_116_DC98(u8* p, BaseItem* item, u32 i, bool b) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyEjectAttachItem(item, i, b); }
void fn_116_DCA0(u8* p, BaseItem* item, u32 i, bool b) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyEjectItem(item, i, b); }
void fn_116_DCA8(u8* p, BaseItem* item) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_116_DCB0(u8* p, BaseItem* item) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyDropItem(item); }
void fn_116_DCB8(u8* p, BaseItem* item, u32 n, int* ip) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyThrowItem(item, n, ip); }
void fn_116_DCC0(u8* p, BaseItem* item, u32 n, int* ip) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyUseItem(item, n, ip); }
void fn_116_DCC8(u8* p, BaseItem* item, u32 n, u32 an, bool a, bool b) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyAttachItem(item, n, an, a, b); }
void fn_116_DCD0(u8* p, itParam::SizeKind k, BaseItem* item, u32 n, u32 i, bool b) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyHaveItem(k, item, n, i, b); }
bool fn_116_DCD8(u8* p, BaseItem* item, bool* b) { return ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyHaveItemPreCheck(item, b); }
void fn_116_DCE8(u8* p, soDamage* d, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0xE8))->Fighter::notifyEventAddDamage(d, m); }

}

// Destructor chains of the soInstancePool / soArticleMediator subobjects: each step destroys one subobject, then deletes when the flag is positive.
extern "C" {

void* fn_116_4F18(void* p, s16 flags);
void* fn_116_4648(void* p, s16 flags);
void* fn_116_4E4C(void* p, s16 flags);
void* fn_116_457C(void* p, s16 flags);
void* fn_116_4D88(void* p, s16 flags);
void* fn_116_3408(void* p, s16 flags);
void* fn_116_4368(void* p, s16 flags);
void* fn_116_43D4(void* p, s16 flags);
void* fn_116_4440(void* p, s16 flags);
void* fn_116_44AC(void* p, s16 flags);
void* fn_116_45E8(void* p, s16 flags);
void* fn_116_4D28(void* p, s16 flags);
void* fn_116_4EB8(void* p, s16 flags);
void* fn_116_52A0(void* p, s16 flags);
void* fn_116_530C(void* p, s16 flags);
void* fn_116_5378(void* p, s16 flags);
void* fn_116_339C(void* p, s16 flags);
void* fn_116_450C(void* p, s16 flags);
void* fn_116_38F0(void* p, s16 flags);
void __dt__20soAnimCmdInterpreterFv(void* p, s16 flags);
void __dt__20soResourceModuleImplFv(void* p, s16 flags);
void* fn_116_4CE8(void* p, s16 flags);

void* fn_116_4EB8(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4F18((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_45E8(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4648((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4D88(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) {
            fn_116_4E4C((u8*)p + 0x4, -1);
        }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_450C(void* p, s16 flags) {
    if (p != 0) {
        fn_116_457C((u8*)p + 0x5B98, -1);
        fn_116_4D88(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_339C(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3408((u8*)p + 0x5958, -1);
        fn_116_4368((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4368(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3408((u8*)p + 0x4304, -1);
        fn_116_43D4((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_43D4(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3408((u8*)p + 0x2CB0, -1);
        fn_116_4440((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4440(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3408((u8*)p + 0x165C, -1);
        fn_116_44AC((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_457C(void* p, s16 flags) {
    if (p != 0) {
        fn_116_45E8((u8*)p + 0x18FC, -1);
        fn_116_4D28((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4E4C(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4EB8((u8*)p + 0x44B4, -1);
        fn_116_52A0((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_52A0(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4EB8((u8*)p + 0x2DD0, -1);
        fn_116_530C((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_530C(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4EB8((u8*)p + 0x16EC, -1);
        fn_116_5378((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3320(void* p, s16 flags) {
    if (p != 0) {
        fn_116_339C((u8*)p + 0x8D84, -1);
        if (p != 0) {
            fn_116_450C(p, 0);
        }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3884(void* p, s16 flags) {
    if (p != 0) {
        fn_116_38F0((u8*)p + 0x50, -1);
        __dt__20soAnimCmdInterpreterFv(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4C70(void* p, s16 flags) {
    if (p != 0) {
        __dt__20soResourceModuleImplFv((u8*)p + 0x18, -1);
        if (p != 0) {
            fn_116_4CE8(p, 0);
        }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}

// Remaining tail-call thunks into module-27 functions and a plain setter.
void fn_116_BC60(u8* p, u8 v) { *(u8*)(p + 0xFD34) = v; }
void fn_27_376A58(void* p);
void fn_27_3776C8(void* p);
void fn_27_3776C0(void* p);
void fn_116_93A4(u8* p) { fn_27_376A58(p + 0x17FBC); }
void fn_116_A334(u8* p) { fn_27_3776C8(p + 0x17FBC); }
void fn_116_A340(u8* p) { fn_27_3776C0(p + 0x17FBC); }

}

// soInstancePoolSub getInstanceAt: select the pooled subobject by index.
extern "C" {
void* fn_116_BD30(u8* p, int i) {
    if (i == 1) { return p + 0x1900; }
    if (i == 0) { return p + 0x10; }
    return 0;
}
void* fn_116_BCE8(u8* p, int i) {
    if (i == 3) { return p + 0x44B8; }
    if (i == 2) { return p + 0x2DD8; }
    if (i == 1) { return p + 0x16F8; }
    if (i == 0) { return p + 0x18; }
    return 0;
}
void* fn_116_BD58(u8* p, int i) {
    if (i == 4) { return p + 0x595C; }
    if (i == 3) { return p + 0x430C; }
    if (i == 2) { return p + 0x2CBC; }
    if (i == 1) { return p + 0x166C; }
    if (i == 0) { return p + 0x1C; }
    return 0;
}
// ftPokeLizardon::isObserv: true for the two character codes handled here.
bool fn_116_9D68(u8* p, int c) {
    char ch = (char)c;
    return ch == 0xC || ch == 0x65;
}
extern int g_soValueVariation;
int fn_116_AC5C() { return g_soValueVariation; }
}
