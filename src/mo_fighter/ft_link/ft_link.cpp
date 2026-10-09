#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/link/ft_link.h>
#include <ft/link/ft_link_extend_param_accesser.h>

#define FT_BC ftLinkBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftLinkExtendParamAccesser g_ftLinkExtendParamAccesser;

ftClassInfoImpl<Fighter_Link, ftLink> g_ftClassInfoLink;

ftLink::ftLink(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftLinkBuildConfig>(entryId,
                                         Fighter_Link,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftLink is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftLinkInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftLinkInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers.
void fn_93_91E8() {}
void fn_93_9974() {}
void fn_93_9978() {}
void fn_93_AD30() {}
void fn_93_AE34() {}
void fn_93_AE40() {}
void fn_93_AE80() {}
void fn_93_AE84() {}
void fn_93_AF14() {}
void fn_93_AF40() {}
void fn_93_AF44() {}
void fn_93_AF48() {}
void fn_93_AF4C() {}
void fn_93_AF50() {}
void fn_93_AF54() {}
void fn_93_AF58() {}
void fn_93_AF5C() {}
void fn_93_AF60() {}
void fn_93_AF64() {}
void fn_93_AF68() {}
void fn_93_AF6C() {}
void fn_93_AF70() {}
void fn_93_AF74() {}
void fn_93_AF78() {}
void fn_93_AF84() {}
void fn_93_AF88() {}
void fn_93_AF8C() {}
void fn_93_AF90() {}
void fn_93_AFBC() {}
void fn_93_AFC0() {}
void fn_93_AFC4() {}
void fn_93_AFC8() {}
void fn_93_AFCC() {}
void fn_93_AFF0() {}
void fn_93_AFF4() {}
void fn_93_B008() {}
void fn_93_B00C() {}
void fn_93_B010() {}
void fn_93_B014() {}
void fn_93_B018() {}
void fn_93_B01C() {}
void fn_93_DAB0() {}

// Constant returns.
int fn_93_AC1C() { return 0; }
int fn_93_AF7C() { return 0; }
int fn_93_AFE8() { return 0; }
int fn_93_AFF8() { return 0; }
int fn_93_B000() { return 0; }
int fn_93_B3C4() { return 0; }
int fn_93_CBB0() { return 7; }

// Field accessors.
u8 fn_93_22F8(u8* p) { return *(u8*)(p + 0x4); }
int fn_93_AA2C(u8* p) { return *(int*)(p + 0x28); }
int fn_93_AC04(u8* p) { return *(int*)(p + 0xC0); }
void fn_93_AC0C(u8* p, float x, float y, float z) {
    *(float*)(p + 0x0) = x;
    *(float*)(p + 0x4) = y;
    *(float*)(p + 0x8) = z;
}
u8 fn_93_AE38(u8* p) { return *(u8*)(p + 0x44); }
int fn_93_AFD0(u8* p) { return *(int*)(p + 0x110); }
u8* fn_93_B020(u8* p, u32 idx) { return p + 4 + (idx << 3); }
u8* fn_93_B0E0(u8* p) { return p + 0x458; }
u8* fn_93_B0E8(u8* p) { return p + 0x3C8; }
u8* fn_93_B0F0(u8* p) { return p + 0x8; }
u8* fn_93_B0F8(u8* p) { return p + 0x84; }
u8* fn_93_B100(u8* p) { return p + 0x70; }
u8* fn_93_B108(u8* p) { return p + 0x5C; }
u8* fn_93_B110(u8* p) { return p + 0x48; }
u8* fn_93_B118(u8* p) { return p + 0x34; }
u8* fn_93_B120(u8* p) { return p + 0x20; }
u8* fn_93_B128(u8* p) { return p + 0x8; }
int fn_93_B190(u8* p) { return *(int*)(p + 0x20); }
int fn_93_B26C(u8* p) { return *(int*)(p + 0x18); }
int fn_93_B304(u8* p) { return *(int*)(p + 0x10); }
u8* fn_93_CBC4(u8* p) { return p + 0x19DAC; }
u8* fn_93_CBE4(u8* p) { return p + 0x19DE8; }
u8* fn_93_CBF0(u8* p) { return p + 0x1A2A0; }
void fn_93_CBB8(u8* p, u8 v) { *(u8*)(p + 0x11BEC) = v; }
u8* fn_93_CCA0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_93_CCB8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_93_CCD0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_93_CCE8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_93_CD48(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
void fn_93_DB98(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_93_DBA4(u8* p) { *(u8*)(p + 0x31) = 1; }

// Bit flags in the u32 array at +0x1C.
bool fn_93_B20C(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_93_B228(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_93_B240(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_93_B254(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_93_B274(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_93_B29C(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_93_B2B4(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_93_B2CC(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_93_B2E4(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_93_B2F4(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_93_B30C(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_93_B324(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_93_B33C(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_93_B35C(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_93_B374(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_93_B38C(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_93_B3A4(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }
}
// Second placeholder batch: empty and constant virtuals, guarded virtual calls and pointer-returning destructors.
extern "C" {

typedef void (*ftLinkVFunc)(void*);
struct ftLinkFlagByte {
    u8 flag : 1;
    u8 rest : 7;
};
// Polymorphic view used only to emit virtual calls; the slots are never defined in this unit.
struct ftLinkVirtualSlots {
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

void fn_93_E1BC() {}
void fn_93_E240() {}
void fn_93_E328() {}
void fn_93_E410() {}
void fn_93_E4F8() {}
void fn_93_E5E0() {}
void fn_93_E6C8() {}
void fn_93_E7B0() {}
void fn_93_E898() {}
int fn_93_E2EC() { return 0; }
int fn_93_E3D4() { return 0; }
int fn_93_E4BC() { return 0; }
int fn_93_E5A4() { return 0; }
int fn_93_E68C() { return 0; }
int fn_93_E774() { return 0; }
int fn_93_E85C() { return 0; }
int fn_93_E944() { return 0; }
int fn_93_E1F8() { return 8; }
void fn_93_E200(s16* dst, s16* src) { *dst = *src; }

// Calls the slot-3 virtual of the object when byte 5 bit 7 is set and byte 6 is clear.
void fn_93_E20C(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E2F4(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E3DC(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E4C4(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E5AC(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E694(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E77C(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E864(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}

// Virtual destructors: delete the object when the flag is positive, then return it.
void* fn_93_21A0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_285C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_3D80(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_4294(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_482C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_59F4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_5EE8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_5F28(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_5F68(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_696C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

// Virtual calls through a subobject vtable and tail calls.
void fn_93_CC30(void* p) { return ((ftLinkVirtualSlots*)p)->slot20(); }
void fn_93_DBB0(u8* p) { fn_93_AC0C(p, 0.0f, 0.0f, 0.0f); }
void fn_93_CBD0(u8* p) { return ((ftLinkVirtualSlots*)(p + 0x19DAC))->slot2(); }


}
