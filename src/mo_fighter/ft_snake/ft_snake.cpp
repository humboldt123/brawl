#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/snake/ft_snake_builder.h>

#define FT_BC ftSnakeBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftClassInfoImpl<Fighter_Snake, ftSnake> g_ftClassInfoSnake;

ftSnake::ftSnake(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftSnakeBuildConfig>(entryId,
                                         Fighter_Snake,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftSnake is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftSnakeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftSnakeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Virtual-slot view used only to emit virtual tail calls; the slots are never defined in this unit.
struct ftSnakeVirtualSlots {
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
};

// Empty virtual overrides.
void fn_122_CF10() {}
void fn_122_EFD0() {}
void fn_122_F1C4() {}
void fn_122_13394() {}
void fn_122_13418() {}
void fn_122_13500() {}
void fn_122_135E8() {}
void fn_122_136D0() {}
void fn_122_137B8() {}
void fn_122_138A0() {}
void fn_122_13988() {}
void fn_122_13A70() {}

// Constant returns.
int fn_122_F094() { return 0; }
int fn_122_11DBC() { return 15; }
int fn_122_133D0() { return 8; }
int fn_122_134C4() { return 0; }
int fn_122_135AC() { return 0; }
int fn_122_13694() { return 0; }
int fn_122_1377C() { return 0; }
int fn_122_13864() { return 0; }
int fn_122_1394C() { return 0; }
int fn_122_13A34() { return 0; }
int fn_122_13B1C() { return 0; }

// Byte store at +0x24900.
void fn_122_11DC4(u8* p, u8 v) { *(u8*)(p + 0x24900) = v; }

// Virtual tail call through slot 20.
void fn_122_11E3C(void* p) { return ((ftSnakeVirtualSlots*)p)->slot20(); }

// Sub-object thunks: adjust this by a constant, then tail-call the sibling function.
void* fn_122_5A94(void* p, s32 flags);
void* fn_122_11708(void* p, s32 flags);
void* fn_122_3254(void* p, s32 flags);
void* fn_122_F284(void* p, s32 flags);
void* fn_122_D7C8(void* p, s32 flags);
void* fn_122_CFC4(void* p, s32 flags);
void* fn_122_D188(void* p, s32 flags);
void* fn_122_DC1C(void* p, s32 flags);
void* fn_122_D25C(void* p, s32 flags);
void* fn_122_EE8C(void* p, s32 flags);
void* fn_122_EDF4(void* p, s32 flags);

void* fn_122_13D20(void* p, s32 flags) { return fn_122_5A94((u8*)p - 0x4, flags); }
void* fn_122_13D28(void* p, s32 flags) { return fn_122_11708((u8*)p - 0x4, flags); }
void* fn_122_13D30(void* p, s32 flags) { return fn_122_3254((u8*)p - 0x4, flags); }
void* fn_122_13D68(void* p, s32 flags) { return fn_122_F284((u8*)p - 0x40, flags); }
void* fn_122_13D80(void* p, s32 flags) { return fn_122_D7C8((u8*)p - 0x54, flags); }
void* fn_122_13D88(void* p, s32 flags) { return fn_122_CFC4((u8*)p - 0x64, flags); }
void* fn_122_13D90(void* p, s32 flags) { return fn_122_D188((u8*)p - 0x70, flags); }
void* fn_122_13DF8(void* p, s32 flags) { return fn_122_DC1C((u8*)p - 0xB8, flags); }
void* fn_122_13E60(void* p, s32 flags) { return fn_122_D25C((u8*)p - 0xE8, flags); }
void* fn_122_13E90(void* p, s32 flags) { return fn_122_F284((u8*)p - 0x2D020, flags); }
void* fn_122_13E9C(void* p, s32 flags) { return fn_122_EE8C((u8*)p - 0x2D020, flags); }
void* fn_122_13EA8(void* p, s32 flags) { return fn_122_EDF4((u8*)p - 0x2D020, flags); }

// Pointer offset returned only for selector 0 (ignores the sub-object if the selector is nonzero).
u8* fn_122_11E74(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11E8C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11EA4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11EBC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11ED4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11EEC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F04(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F1C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F34(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F4C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F64(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F7C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_1341C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_13504(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_135EC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_136D4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_137BC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_138A4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_1398C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_13A74(u8* p, int x) { return x == 0 ? p + 0xC : 0; }

// Selector 0 and 1 sub-object pointers.
u8* fn_122_11E4C(u8* p, int x) {
    if (x == 1) {
        return p + 0x16DC;
    }
    if (x == 0) {
        return p + 0x10;
    }
    return 0;
}

// Delete the object when flags > 0 (no destructor call).
void* fn_122_1D5C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_1DF8(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline on
void* fn_122_3820(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline reset
void* fn_122_3860(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3EE4(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3F94(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_4394(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline on
void* fn_122_48DC(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline reset

// Vec3 copy.
void fn_122_D564(float* dst, float* src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}


// Sub-object destructor wrappers: destroy a sub-object (flag 0 or -1), then delete when flags > 0.
void* fn_122_12BB4(void* p, s16 flags);
void* fn_122_3320(void* p, s16 flags);
void* fn_27_D3B50(void* p, s16 flags);
void* fn_27_64874(void* p, s16 flags);
void* fn_27_C31CC(void* p, s16 flags);
void* fn_27_63EBC(void* p, s16 flags);
void* fn_27_5C674(void* p, s16 flags);
void* fn_27_5AE44(void* p, s16 flags);
void __dt__16soStopModuleImplFv(void* p, s16 flags);
void __dt__22soVisibilityModuleImplFv(void* p, s16 flags);
void __dt__22soColorBlendModuleImplFv(void* p, s16 flags);
void __dt__22soControllerModuleImplFv(void* p, s16 flags);
void __dt__15cmPhotoCallBackFv(void* p, s16 flags);
void __dt__21ftExtendParamAccesserFv(void* p, s16 flags);
void* fn_122_A734(void* p, s16 flags);
void* fn_27_6880C(void* p, s16 flags);
void* fn_122_12054(void* p, s16 flags);
void __dt__25soKineticEnergyWindNormalFv(void* p, s16 flags);
void* fn_122_12828(void* p, s16 flags);
void* fn_122_37C4(void* p, s16 flags);
void* fn_122_12960(void* p, s16 flags);
void* fn_122_33FC(void* p, s16 flags);
void* fn_122_345C(void* p, s16 flags);
void __dt__19soGeneralWorkSimpleFv(void* p, s16 flags);
void* fn_27_B4780(void* p, s16 flags);
void __dt__17soSoundModuleImplFv(void* p, s16 flags);
void* fn_122_4F20(void* p, s16 flags);

void* fn_122_12C2C(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12BB4(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_32C4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_3320(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_38A0(void* p, s16 flags) {
    if (p != 0) {
        fn_27_D3B50(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_38FC(void* p, s16 flags) {
    if (p != 0) {
        fn_27_64874(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3958(void* p, s16 flags) {
    if (p != 0) {
        fn_122_3820(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3BC4(void* p, s16 flags) {
    if (p != 0) {
        fn_27_C31CC(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3C20(void* p, s16 flags) {
    if (p != 0) {
        fn_27_63EBC(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3C7C(void* p, s16 flags) {
    if (p != 0) {
        fn_27_5C674(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_44FC(void* p, s16 flags) {
    if (p != 0) {
        fn_27_5AE44(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5438(void* p, s16 flags) {
    if (p != 0) {
        fn_122_3820(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5754(void* p, s16 flags) {
    if (p != 0) {
        fn_27_63EBC(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_4684(void* p, s16 flags) {
    if (p != 0) {
        __dt__16soStopModuleImplFv(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_593C(void* p, s16 flags) {
    if (p != 0) {
        __dt__22soVisibilityModuleImplFv(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6498(void* p, s16 flags) {
    if (p != 0) {
        __dt__22soColorBlendModuleImplFv(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6B18(void* p, s16 flags) {
    if (p != 0) {
        __dt__22soControllerModuleImplFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_A3D0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_48DC(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_AA80(void* p, s16 flags) {
    if (p != 0) {
        __dt__15cmPhotoCallBackFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_F228(void* p, s16 flags) {
    if (p != 0) {
        __dt__21ftExtendParamAccesserFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_A6D8(void* p, s16 flags) {
    if (p != 0) {
        fn_122_A734(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_4624(void* p, s16 flags) {
    if (p != 0) {
        fn_27_6880C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_120B4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12054((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_12828(void* p, s16 flags) {
    if (p != 0) {
        __dt__25soKineticEnergyWindNormalFv((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_12888(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12828((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_12960(void* p, s16 flags) {
    if (p != 0) {
        fn_122_37C4((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_129C0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12960((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_339C(void* p, s16 flags) {
    if (p != 0) {
        fn_122_33FC((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_33FC(void* p, s16 flags) {
    if (p != 0) {
        fn_122_345C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3DE8(void* p, s16 flags) {
    if (p != 0) {
        __dt__19soGeneralWorkSimpleFv((u8*)p + 0x38, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_42AC(void* p, s16 flags) {
    if (p != 0) {
        __dt__19soGeneralWorkSimpleFv((u8*)p + 0x14, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_449C(void* p, s16 flags) {
    if (p != 0) {
        fn_27_B4780((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_45C4(void* p, s16 flags) {
    if (p != 0) {
        __dt__17soSoundModuleImplFv((u8*)p + 0xC, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_4EC0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_4F20((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

// Sub-object destructor wrappers (batch 2): destroy a sub-object, then delete when flags > 0.
void* fn_122_5D1C(void* p, s16 flags);
void* fn_122_5D7C(void* p, s16 flags);
void* fn_122_5FE4(void* p, s16 flags);
void* fn_122_6044(void* p, s16 flags);
void* fn_122_6648(void* p, s16 flags);
void* fn_122_66A8(void* p, s16 flags);
void* fn_122_6CC4(void* p, s16 flags);
void* fn_122_6D24(void* p, s16 flags);
void* fn_122_6FF0(void* p, s16 flags);
void* fn_122_7050(void* p, s16 flags);
void* fn_122_77B8(void* p, s16 flags);
void* fn_122_7818(void* p, s16 flags);
void* fn_122_7B08(void* p, s16 flags);
void* fn_122_7B68(void* p, s16 flags);
void* fn_122_7ED0(void* p, s16 flags);
void* fn_122_7F30(void* p, s16 flags);
void* fn_122_8224(void* p, s16 flags);
void* fn_122_8284(void* p, s16 flags);
void* fn_122_8628(void* p, s16 flags);
void* fn_122_8688(void* p, s16 flags);
void* fn_122_8944(void* p, s16 flags);
void* fn_122_8A10(void* p, s16 flags);
void* fn_122_89B0(void* p, s16 flags);
void* fn_122_90CC(void* p, s16 flags);
void* fn_122_12304(void* p, s16 flags);
void* fn_122_12364(void* p, s16 flags);

void* fn_122_5CBC(void* p, s16 flags) {
    if (p != 0) {
        fn_122_5D1C((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5D1C(void* p, s16 flags) {
    if (p != 0) {
        fn_122_5D7C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5F84(void* p, s16 flags) {
    if (p != 0) {
        fn_122_5FE4((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5FE4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6044((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_65E8(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6648((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6648(void* p, s16 flags) {
    if (p != 0) {
        fn_122_66A8((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6C64(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6CC4((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6CC4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6D24((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6F90(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6FF0((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6FF0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7050((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_767C(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6B18((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_7758(void* p, s16 flags) {
    if (p != 0) {
        fn_122_77B8((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_77B8(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7818((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_7AA8(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7B08((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_7B08(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7B68((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_7E70(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7ED0((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_7ED0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7F30((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_81C4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8224((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_8224(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8284((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_85C8(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8628((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_8628(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8688((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_88E4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8944((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_89B0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8A10((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_8D58(void* p, s16 flags) {
    if (p != 0) {
        fn_122_89B0((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_FBAC(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12C2C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_12364(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12304((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_123C4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12364((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

// Delete the object when flags > 0 (no destructor call).
void* fn_122_4C3C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_4C7C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5AD4(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5B14(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_CDAC(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

// Sub-object destructor wrapper: destroy the sub-object at +0x4, then delete when flags > 0.
void* fn_122_4F80(void* p, s16 flags);
void* fn_122_4F20(void* p, s16 flags) {
    if (p != 0) {
        fn_122_4F80((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

} // extern "C"
