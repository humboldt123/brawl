#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/metaknight/ft_metaknight.h>
#include <ft/metaknight/ft_metaknight_extend_param_accesser.h>

#define FT_BC ftMetaknightBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftMetaknightExtendParamAccesser g_ftMetaknightExtendParamAccesser;

ftClassInfoImpl<Fighter_MetaKnight, ftMetaknight> g_ftClassInfoMetaknight;

ftMetaknight::ftMetaknight(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftMetaknightBuildConfig>(entryId,
                                         Fighter_MetaKnight,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_MetaKnight)) {
    // TODO
}

ftMetaknight::~ftMetaknight() { }

// FIXME: Test code present only to emit the shared builder functions; delete once ftMetaknight is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftMetaknightInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftMetaknightInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Virtual-slot view used only to emit virtual tail calls; the slots are never defined in this unit.
struct ftMetaknightVirtualSlots {
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

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

// soValueAccesser::getValueVariation (same global as ft_marth.cpp)
extern int g_soValueVariation;
int fn_111_8CC8() { return g_soValueVariation; }

// soArticle::checkActivate: virtual slot 20 tail call.
void fn_111_9874(void* p) { return ((ftMetaknightVirtualSlots*)p)->slot20(); }

u8 fn_111_5F38(u8* p) { return *(u8*)(p + 0x4); }
void fn_111_8650() {}
void fn_111_8654() {}
void fn_111_8658() {}
int fn_111_87CC(u8* p) { return *(int*)(p + 0x20); }
int fn_111_88A8(u8* p) { return *(int*)(p + 0x18); }
void fn_111_9800(u8* p, u8 v) { *(u8*)(p + 0x20bc) = v; }
void fn_111_A5EC() {}
void fn_111_AE98() {}

}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
u8* fn_111_BB28(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
