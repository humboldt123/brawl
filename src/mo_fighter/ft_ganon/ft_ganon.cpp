#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/ganon/ft_ganon.h>
#include <ft/ganon/ft_ganon_extend_param_accesser.h>

#define FT_BC ftGanonBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftGanonExtendParamAccesser g_ftGanonExtendParamAccesser;

ftClassInfoImpl<Fighter_Ganon, ftGanon> g_ftClassInfoGanon;

ftGanon::ftGanon(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftGanonBuildConfig>(entryId,
                                         Fighter_Ganon,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftGanon is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftGanonInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftGanonInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Virtual-slot view used only to emit virtual tail calls; the slots are never defined in this unit.
struct ftGanonVirtualSlots {
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

// Global read (g_soValueVariation is defined elsewhere in the module).
extern void* g_soValueVariation;
void* fn_109_964C() { return g_soValueVariation; }

// Empty virtual overrides and default observers.
void fn_109_8578() {}
void fn_109_8DF0() {}
void fn_109_8DFC() {}
void fn_109_8E3C() {}
void fn_109_8E40() {}
void fn_109_8ED0() {}
void fn_109_8EFC() {}
void fn_109_8F04() {}
void fn_109_8F08() {}
void fn_109_8F0C() {}
void fn_109_8F10() {}
void fn_109_8F14() {}
void fn_109_8F18() {}
void fn_109_8F1C() {}
void fn_109_8F20() {}
void fn_109_8F24() {}
void fn_109_8F28() {}
void fn_109_8F2C() {}
void fn_109_8F30() {}
void fn_109_8F34() {}
void fn_109_8F40() {}
void fn_109_8F44() {}
void fn_109_8F48() {}
void fn_109_8F4C() {}
void fn_109_8F78() {}
void fn_109_8F7C() {}
void fn_109_8F80() {}
void fn_109_8F84() {}
void fn_109_8F88() {}
void fn_109_8FAC() {}
void fn_109_8FB0() {}
void fn_109_8FB4() {}
void fn_109_8FC8() {}
void fn_109_8FCC() {}
void fn_109_8FD0() {}
void fn_109_8FD4() {}
void fn_109_8FD8() {}
void fn_109_8FDC() {}
void fn_109_B144() {}
void fn_109_B850() {}
void fn_109_B8D4() {}
void fn_109_B9BC() {}
void fn_109_BB54() {}
void fn_109_BC3C() {}
void fn_109_BD24() {}
void fn_109_BE0C() {}
void fn_109_BEF4() {}

// Constant returns.
int fn_109_8F38() { return 0; }
int fn_109_8FA4() { return 0; }
int fn_109_8FB8() { return 0; }
int fn_109_8FC0() { return 0; }
int fn_109_9384() { return 0; }
int fn_109_B980() { return 0; }
int fn_109_BA68() { return 0; }
int fn_109_BB18() { return 0; }
int fn_109_BC00() { return 0; }
int fn_109_BCE8() { return 0; }
int fn_109_BDD0() { return 0; }
int fn_109_BEB8() { return 0; }
int fn_109_BFA0() { return 0; }
int fn_109_A338() { return 2; }
int fn_109_B88C() { return 8; }

// Field accessors and element pointers.
u8 fn_109_6868(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_109_8DF4(u8* p) { return *(u8*)(p + 0x44); }
int fn_109_8F8C(u8* p) { return *(int*)(p + 0x110); }
int fn_109_9150(u8* p) { return *(int*)(p + 0x20); }
int fn_109_922C(u8* p) { return *(int*)(p + 0x18); }
int fn_109_8A5C(u8* p) { return *(int*)(p + 0x28); }
int fn_109_92C4(u8* p) { return *(int*)(p + 0x10); }
u8* fn_109_90A0(u8* p) { return p + 0x458; }
u8* fn_109_90A8(u8* p) { return p + 0x3C8; }
u8* fn_109_90B0(u8* p) { return p + 0x8; }
u8* fn_109_8FE0(u8* p, u32 idx) { return p + 4 + (idx << 3); }
u8* fn_109_A348(u8* p) { return p + 0xC3F8; }
u8* fn_109_A368(u8* p) { return p + 0xC434; }
u8* fn_109_A374(u8* p) { return p + 0xC8EC; }
u8* fn_109_A3C4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_109_A3DC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
void fn_109_B22C(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_109_B238(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_109_A340(u8* p, u8 v) { *(u8*)(p + 0x4574) = v; }
void fn_109_B894(s16* dst, s16* src) { *dst = *src; }

// Bit flags in the u32 array at +0x1C.
bool fn_109_91CC(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_109_91E8(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_109_9200(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_109_9214(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_109_9234(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_109_925C(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_109_9274(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_109_928C(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_109_92A4(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_109_92B4(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_109_92CC(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_109_92E4(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_109_92FC(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_109_931C(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_109_9334(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_109_934C(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_109_9364(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }

// Virtual tail calls: slot1 when byte 5 bit 7 is set and byte 6 is clear.
struct ftGanonFlagByte {
    u8 flag : 1;
    u8 rest : 7;
};
void fn_109_B8A0(void* p) {
    u8* b = (u8*)p;
    ftGanonFlagByte* flags = (ftGanonFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftGanonVirtualSlots*)p)->slot1();
        }
    }
}
void fn_109_B988(void* p) {
    u8* b = (u8*)p;
    ftGanonFlagByte* flags = (ftGanonFlagByte*)(b + 5);
    if (flags->flag == true && b[6] == 0) {
        return ((ftGanonVirtualSlots*)p)->slot1();
    }
}
void fn_109_BB20(void* p) {
    u8* b = (u8*)p;
    ftGanonFlagByte* flags = (ftGanonFlagByte*)(b + 5);
    if (flags->flag == true && b[6] == 0) {
        return ((ftGanonVirtualSlots*)p)->slot1();
    }
}
void fn_109_BC08(void* p) {
    u8* b = (u8*)p;
    ftGanonFlagByte* flags = (ftGanonFlagByte*)(b + 5);
    if (flags->flag == true && b[6] == 0) {
        return ((ftGanonVirtualSlots*)p)->slot1();
    }
}
void fn_109_BCF0(void* p) {
    u8* b = (u8*)p;
    ftGanonFlagByte* flags = (ftGanonFlagByte*)(b + 5);
    if (flags->flag == true && b[6] == 0) {
        return ((ftGanonVirtualSlots*)p)->slot1();
    }
}
void fn_109_BDD8(void* p) {
    u8* b = (u8*)p;
    ftGanonFlagByte* flags = (ftGanonFlagByte*)(b + 5);
    if (flags->flag == true && b[6] == 0) {
        return ((ftGanonVirtualSlots*)p)->slot1();
    }
}
void fn_109_BEC0(void* p) {
    u8* b = (u8*)p;
    ftGanonFlagByte* flags = (ftGanonFlagByte*)(b + 5);
    if (flags->flag == true && b[6] == 0) {
        return ((ftGanonVirtualSlots*)p)->slot1();
    }
}
void fn_109_A3B4(void* p) { return ((ftGanonVirtualSlots*)p)->slot20(); }

// Virtual destructors that first destroy one subobject with the given flag.
// Callers are emitted before their callees so the callees are not inlined.
void* fn_109_3BAC(void* p, s16 flags);
void* fn_109_41F0(void* p, s16 flags);
void* fn_109_3B50(void* p, s16 flags);
void* fn_109_3AF0(void* p, s16 flags) {
    if (p != 0) {
        fn_109_3B50((u8*)p + 0x38, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline on
void* fn_109_3B50(void* p, s16 flags) {
    if (p != 0) {
        fn_109_3BAC(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline reset
void* fn_109_6074(void* p, s16 flags) {
    if (p != 0) {
        fn_109_41F0(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline on
void* fn_109_41F0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
#pragma dont_inline reset
#pragma dont_inline on
void* fn_109_3BAC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
#pragma dont_inline reset

// Virtual destructors: delete the object when the flag is positive, then return it.
void* fn_109_1DF4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_109_3BEC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_109_3E98(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_109_44C8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_109_4508(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_109_4A24(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_109_4A64(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_109_4AA4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_109_5AF4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

}
