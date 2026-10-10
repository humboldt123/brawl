#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/toonlink/ft_toonlink.h>
#include <ft/toonlink/ft_toonlink_extend_param_accesser.h>

#define FT_BC ftToonLinkBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftToonLinkExtendParamAccesser g_ftToonLinkExtendParamAccesser;

ftClassInfoImpl<Fighter_ToonLink, ftToonLink> g_ftClassInfoToonLink;

ftToonLink::ftToonLink(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftToonLinkBuildConfig>(entryId,
                                         Fighter_ToonLink,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftToonLink is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftToonLinkInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftToonLinkInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers.
void fn_121_91E8() {}
void fn_121_9974() {}
void fn_121_9978() {}
void fn_121_AD2C() {}
void fn_121_AE30() {}
void fn_121_AE3C() {}
void fn_121_AE7C() {}
void fn_121_AE80() {}
void fn_121_AF10() {}
void fn_121_AF3C() {}
void fn_121_AF40() {}
void fn_121_AF44() {}
void fn_121_AF48() {}
void fn_121_AF4C() {}
void fn_121_AF50() {}
void fn_121_AF54() {}
void fn_121_AF58() {}
void fn_121_AF5C() {}
void fn_121_AF60() {}
void fn_121_AF64() {}
void fn_121_AF68() {}
void fn_121_AF6C() {}
void fn_121_AF70() {}
void fn_121_AF74() {}
void fn_121_AF80() {}
void fn_121_AF84() {}
void fn_121_AF88() {}
void fn_121_AF8C() {}
void fn_121_AFB8() {}
void fn_121_AFBC() {}
void fn_121_AFC0() {}
void fn_121_AFC4() {}
void fn_121_AFC8() {}
void fn_121_AFEC() {}
void fn_121_AFF0() {}
void fn_121_B004() {}
void fn_121_B008() {}
void fn_121_B00C() {}
void fn_121_B010() {}
void fn_121_B014() {}
void fn_121_B018() {}
void fn_121_DAAC() {}
void fn_121_E1B8() {}
void fn_121_E23C() {}
void fn_121_E324() {}
void fn_121_E40C() {}
void fn_121_E4F4() {}
void fn_121_E5DC() {}
void fn_121_E6C4() {}
void fn_121_E7AC() {}
void fn_121_E894() {}

// Constant returns.
int fn_121_AC18() { return 0; }
int fn_121_AF78() { return 0; }
int fn_121_AFE4() { return 0; }
int fn_121_AFF4() { return 0; }
int fn_121_AFFC() { return 0; }
int fn_121_B3C0() { return 0; }
int fn_121_E2E8() { return 0; }
int fn_121_E3D0() { return 0; }
int fn_121_E4B8() { return 0; }
int fn_121_E5A0() { return 0; }
int fn_121_E688() { return 0; }
int fn_121_E770() { return 0; }
int fn_121_E858() { return 0; }
int fn_121_E940() { return 0; }
int fn_121_CBAC() { return 7; }
int fn_121_E1F4() { return 8; }

// Field accessors.
u8 fn_121_22F8(u8* p) { return *(u8*)(p + 0x4); }
int fn_121_AA2C(u8* p) { return *(int*)(p + 0x28); }
int fn_121_AC00(u8* p) { return *(int*)(p + 0xC0); }
void fn_121_AC08(u8* p, float x, float y, float z) {
    *(float*)(p + 0x0) = x;
    *(float*)(p + 0x4) = y;
    *(float*)(p + 0x8) = z;
}
u8 fn_121_AE34(u8* p) { return *(u8*)(p + 0x44); }
int fn_121_AFCC(u8* p) { return *(int*)(p + 0x110); }
int fn_121_B18C(u8* p) { return *(int*)(p + 0x20); }
int fn_121_B268(u8* p) { return *(int*)(p + 0x18); }
int fn_121_B300(u8* p) { return *(int*)(p + 0x10); }
u8* fn_121_B01C(u8* p, u32 idx) { return p + 4 + (idx << 3); }
u8* fn_121_B0DC(u8* p) { return p + 0x458; }
u8* fn_121_B0E4(u8* p) { return p + 0x3C8; }
u8* fn_121_B0EC(u8* p) { return p + 0x8; }
u8* fn_121_B0F4(u8* p) { return p + 0x84; }
u8* fn_121_B0FC(u8* p) { return p + 0x70; }
u8* fn_121_B104(u8* p) { return p + 0x5C; }
u8* fn_121_B10C(u8* p) { return p + 0x48; }
u8* fn_121_B114(u8* p) { return p + 0x34; }
u8* fn_121_B11C(u8* p) { return p + 0x20; }
u8* fn_121_B124(u8* p) { return p + 0x8; }
void fn_121_CBB4(u8* p, u8 v) { *(u8*)(p + 0x11BEC) = v; }
u8* fn_121_CBC0(u8* p) { return p + 0x19DE8; }
u8* fn_121_CBE0(u8* p) { return p + 0x19E24; }
u8* fn_121_CBEC(u8* p) { return p + 0x1A2DC; }
void fn_121_DB94(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_121_DBA0(u8* p) { *(u8*)(p + 0x31) = 1; }
u8* fn_121_CC9C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_121_CCB4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_121_CCCC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_121_CCE4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_121_CD44(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_121_CCFC(u8* p, int x) {
    if (x == 3) { return p + 0x5FAC; }
    if (x == 2) { return p + 0x3FD0; }
    if (x == 1) { return p + 0x1FF4; }
    if (x == 0) { return p + 0x18; }
    return 0;
}
void fn_121_E1FC(s16* dst, s16* src) { *dst = *src; }

// Bit flags in the u32 array at +0x1C.
bool fn_121_B208(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_121_B224(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_121_B23C(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_121_B250(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_121_B270(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_121_B298(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_121_B2B0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_121_B2C8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_121_B2E0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_121_B2F0(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_121_B308(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_121_B320(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_121_B338(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_121_B358(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_121_B370(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_121_B388(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_121_B3A0(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }

}

// Destructor wrappers: base destructors or sibling wrappers on sub-objects, then delete when flags > 0.
extern "C" {

void* fn_121_21A0(void* p, s16 flags);
void* fn_121_285C(void* p, s16 flags);
void* fn_121_3D80(void* p, s16 flags);
void* fn_121_4294(void* p, s16 flags);
void* fn_121_482C(void* p, s16 flags);
void* fn_121_59F4(void* p, s16 flags);
void* fn_121_5EE8(void* p, s16 flags);
void* fn_121_5F28(void* p, s16 flags);
void* fn_121_5F68(void* p, s16 flags);
void* fn_121_696C(void* p, s16 flags);
// Sibling and module-27 wrappers used by the functions below (defined elsewhere).
void* fn_27_5C674(void* p, s16 flags);
void* fn_27_5AE44(void* p, s16 flags);
void* fn_27_6880C(void* p, s16 flags);
void* fn_27_63EBC(void* p, s16 flags);
void* fn_27_3A9380(void* p, s16 flags);
// map: wnSimple____dt
void* fn_121_5FA8(void* p, s16 flags);
// map: wnToonLinkHookShot____dt
void* fn_121_6B30(void* p, s16 flags);
void* fn_121_13894(void* p, s16 flags);
void* fn_121_7B94(void* p, s16 flags);
// map: soInstancePool_73soInstancePoolInfo_13wnToonLinkBow_1_16wnInstanceHolder_14soIntToType_0___562soL_______dt
void* fn_121_7C54(void* p, s16 flags);
void* fn_121_6ACC(void* p, s16 flags);
// map: wnToonLinkBow____dt
void* fn_121_79F0(void* p, s16 flags);
void __dt__26ftStatusUniqProcessGimmickFv(void* p, s16 flags);
void __dt__16soTransitionInfoFv(void* p, s16 flags);
void __dt__21soStatusEventObserverFv(void* p, s16 flags);
void __dt__22soAnimCmdEventObserverFv(void* p, s16 flags);
void __dt__22soColorBlendModuleImplFv(void* p, s16 flags);
void __dt__19soPhysicsModuleImplFv(void* p, s16 flags);
void __dt__20soResourceModuleImplFv(void* p, s16 flags);
void __dt__17soSoundModuleImplFv(void* p, s16 flags);
void __dt__16soStopModuleImplFv(void* p, s16 flags);

// Forward declarations for the wrappers below.
void* fn_121_1D2C(void* p, s16 flags);
void* fn_121_1D88(void* p, s16 flags);
void* fn_121_2144(void* p, s16 flags);
void* fn_121_2CE0(void* p, s16 flags);
void* fn_121_486C(void* p, s16 flags);
void* fn_121_58B8(void* p, s16 flags);
void* fn_121_5924(void* p, s16 flags);
void* fn_121_63F0(void* p, s16 flags);
void* fn_121_644C(void* p, s16 flags);
void* fn_121_64A8(void* p, s16 flags);
void* fn_121_6564(void* p, s16 flags);
void* fn_121_664C(void* p, s16 flags);
void* fn_121_6714(void* p, s16 flags);
void* fn_121_68F4(void* p, s16 flags);
void* fn_121_69AC(void* p, s16 flags);
void* fn_121_6A0C(void* p, s16 flags);
void* fn_121_6A6C(void* p, s16 flags);
void* fn_121_7010(void* p, s16 flags);
void* fn_121_7154(void* p, s16 flags);
void* fn_121_71B4(void* p, s16 flags);
void* fn_121_733C(void* p, s16 flags);
void* fn_121_739C(void* p, s16 flags);
void* fn_121_73FC(void* p, s16 flags);
void* fn_121_746C(void* p, s16 flags);
void* fn_121_74CC(void* p, s16 flags);
void* fn_121_752C(void* p, s16 flags);
void* fn_121_75A4(void* p, s16 flags);
void* fn_121_7604(void* p, s16 flags);
void* fn_121_7664(void* p, s16 flags);
void* fn_121_76DC(void* p, s16 flags);
void* fn_121_773C(void* p, s16 flags);
void* fn_121_77A8(void* p, s16 flags);
void* fn_121_78A8(void* p, s16 flags);
void* fn_121_7908(void* p, s16 flags);
void* fn_121_7BF4(void* p, s16 flags);
void* fn_121_7CD0(void* p, s16 flags);
void* fn_121_7D2C(void* p, s16 flags);
void* fn_121_7D9C(void* p, s16 flags);

// Gimmick destructor wrapper
void* fn_121_1D2C(void* p, s16 flags) {
    if (p != 0) {
        __dt__26ftStatusUniqProcessGimmickFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Gimmick destructor wrapper
void* fn_121_1D88(void* p, s16 flags) {
    if (p != 0) {
        __dt__26ftStatusUniqProcessGimmickFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around fn_121_21A0
void* fn_121_2144(void* p, s16 flags) {
    if (p != 0) {
        fn_121_21A0(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// soTransitionInfo sub-object
void* fn_121_2CE0(void* p, s16 flags) {
    if (p != 0) {
        __dt__16soTransitionInfoFv((u8*)p + 0xC, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around fn_121_482C
void* fn_121_486C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_482C(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Status event observer sub-object
void* fn_121_58B8(void* p, s16 flags) {
    if (p != 0) {
        __dt__21soStatusEventObserverFv((u8*)p + 0x20, 0);
        fn_121_5924(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// AnimCmd event observer sub-object
void* fn_121_5924(void* p, s16 flags) {
    if (p != 0) {
        __dt__22soAnimCmdEventObserverFv((u8*)p + 0x8, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around module 27 destructor
void* fn_121_63F0(void* p, s16 flags) {
    if (p != 0) {
        fn_27_5C674(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Color blend module
void* fn_121_644C(void* p, s16 flags) {
    if (p != 0) {
        __dt__22soColorBlendModuleImplFv(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Physics module sub-object
void* fn_121_64A8(void* p, s16 flags) {
    if (p != 0) {
        __dt__19soPhysicsModuleImplFv((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_6564(void* p, s16 flags) {
    if (p != 0) {
        fn_121_486C((u8*)p + 0x38, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around module 27 destructor
void* fn_121_664C(void* p, s16 flags) {
    if (p != 0) {
        fn_27_5AE44(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around module 27 destructor
void* fn_121_6714(void* p, s16 flags) {
    if (p != 0) {
        fn_27_6880C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Resource module sub-object
void* fn_121_68F4(void* p, s16 flags) {
    if (p != 0) {
        __dt__20soResourceModuleImplFv((u8*)p + 0x18, -1);
        if (p != 0) { fn_121_285C(p, 0); }
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_69AC(void* p, s16 flags) {
    if (p != 0) {
// map: wnSimple____dt
        fn_121_5FA8((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_6A0C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_69AC((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_6A6C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_6A0C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around module 27 destructor
void* fn_121_7010(void* p, s16 flags) {
    if (p != 0) {
        fn_27_63EBC(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Sound module sub-object
void* fn_121_7154(void* p, s16 flags) {
    if (p != 0) {
        __dt__17soSoundModuleImplFv((u8*)p + 0xC, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Stop module
void* fn_121_71B4(void* p, s16 flags) {
    if (p != 0) {
        __dt__16soStopModuleImplFv(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_733C(void* p, s16 flags) {
    if (p != 0) {
// map: wnToonLinkHookShot____dt
        fn_121_6B30((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_739C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_733C((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_73FC(void* p, s16 flags) {
    if (p != 0) {
        fn_121_739C((u8*)p + 0x16D8, -1);
        fn_121_6ACC(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_746C(void* p, s16 flags) {
    if (p != 0) {
// map: wnSimple____dt
        fn_121_5FA8((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_74CC(void* p, s16 flags) {
    if (p != 0) {
        fn_121_746C((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_752C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_74CC((u8*)p + 0x4F2C, -1);
        if (p != 0) { fn_121_73FC(p, 0); }
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_75A4(void* p, s16 flags) {
    if (p != 0) {
        fn_121_13894((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_7604(void* p, s16 flags) {
    if (p != 0) {
        fn_121_75A4((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_7664(void* p, s16 flags) {
    if (p != 0) {
        fn_121_7604((u8*)p + 0x6600, -1);
        if (p != 0) { fn_121_752C(p, 0); }
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_76DC(void* p, s16 flags) {
    if (p != 0) {
        fn_121_78A8((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_773C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_78A8((u8*)p + 0x1FE8, -1);
        fn_121_76DC((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_77A8(void* p, s16 flags) {
    if (p != 0) {
        fn_121_78A8((u8*)p + 0x3FC8, -1);
        fn_121_773C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around module 27 destructor
void* fn_121_78A8(void* p, s16 flags) {
    if (p != 0) {
        fn_27_3A9380((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_7908(void* p, s16 flags) {
    if (p != 0) {
        fn_121_78A8((u8*)p + 0x5FA8, -1);
        fn_121_77A8((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_7BF4(void* p, s16 flags) {
    if (p != 0) {
        fn_121_7B94((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper
void* fn_121_7CD0(void* p, s16 flags) {
    if (p != 0) {
// map: soInstancePool_73soInstancePoolInfo_13wnToonLinkBow_1_16wnInstanceHolder_14soIntToType_0___562soL_______dt
        fn_121_7C54(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_7D2C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_7CD0((u8*)p + 0x8, -1);
        fn_121_5F68(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper
void* fn_121_7D9C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_7D2C(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
}

extern "C" {
#pragma dont_inline on
// Virtual destructors: delete the object when the flag is positive, then return it.
void* fn_121_21A0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_285C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_3D80(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_4294(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_482C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_59F4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_5EE8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_5F28(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_5F68(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_696C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
// Virtual destructor of the matrix pool: delete the object when the flag is positive, then return it.
void* fn_121_1AD8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
#pragma dont_inline reset
}

// Wrappers and tail calls defined after their callers.
extern "C" {
struct ftToonLinkVSlotsB {
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
extern void* g_soValueVariation;

// Sub-object destructor wrapper around fn_121_79F0.
void* fn_121_7B94(void* p, s16 flags) {
    if (p != 0) {
// map: wnToonLinkBow____dt
        fn_121_79F0((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object with a redundant pointer check before the call.
void* fn_121_6ACC(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) {
            fn_121_6A0C((u8*)p + 0x4, -1);
        }
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Tail call of virtual slot 20 (vtable offset 0x58).
void fn_121_CC2C(void* p) {
    ((ftToonLinkVSlotsB*)p)->slot20();
}
// Returns the global value-variation table pointer.
void* fn_121_B688() {
    return g_soValueVariation;
}
}
