#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_manager.h>
#include <so/work/so_general_work_simple.h>
#include <ft/ft_class_info_impl.h>
#include <ft/samus/ft_samus.h>
#include <ft/samus/ft_samus_extend_param_accesser.h>

#define FT_BC ftSamusBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftSamusExtendParamAccesser g_ftSamusExtendParamAccesser;

ftClassInfoImpl<Fighter_Samus, ftSamus> g_ftClassInfoSamus;

ftSamus::ftSamus(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftSamusBuildConfig>(entryId,
                                         Fighter_Samus,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// Symbols for these members are not in symbols.txt yet, so they are emitted under their placeholder names.
extern "C" {

void fn_94_8FF4(ftSamus* self, int startKind) {
    soModuleAccesser* acc = self->m_moduleAccesser;
    soWorkManageModule* workManage = &acc->getWorkManageModule();
    if (startKind == 8) {
        // HYPOTHESIS: int 0x10000040 holds an effect handle that is removed on this start kind
        int effectId = workManage->getInt(0x10000040);
        if (effectId != 0) {
            self->m_moduleAccesser->getEffectModule().remove(effectId);
        }
    }
    workManage->setInt(0, 0x10000040);
    workManage->setInt(0, 0x10000041);
    workManage->setInt(0, 0x10000042);
    workManage->offFlag(0x1200003e);
    workManage->offFlag(0x1200003d);
    acc->getEffectModule().removeCommon(0x1a);
    self->Fighter::onStart(startKind);
}

void fn_94_9134(ftSamus* self) {
    soModuleAccesser* acc = self->m_moduleAccesser;
    soWorkManageModule& workManage = acc->getWorkManageModule();
    if (workManage.isFlag(0x1200003d)) {
        workManage.offFlag(0x1200003d);
        if (static_cast<u8>(g_ftManager->getFighterCount(self->m_entryId)) > 1) {
            int fighterNo = g_ftManager->getFighterNo(self->m_entryId, self->m_taskId);
            // The motion module is set back to frame 0 before the next fighter is chosen.
            acc->getMotionModule().setFrame(0.0f);
            g_ftManager->toChange(self->m_entryId, static_cast<u8>(fighterNo) == 0, 0, 0);
        }
    }
    self->Fighter::onDeadEnd();
}

}

// FIXME: Test code present only to emit the shared builder functions; delete once ftSamus is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftSamusInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftSamusInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Shared getters and setters reached through this module (placeholder names; offsets are the object's own fields).
extern int g_soValueVariation;
extern "C" {

int fn_94_B398() { return g_soValueVariation; }
u8* fn_94_CCF0(u8* self) { return self + 0x2565c; }
u8* fn_94_CD10(u8* self) { return self + 0x25698; }
u8* fn_94_CD1C(u8* self) { return self + 0x25b50; }
void fn_94_DC30(u8* self) {
    Vec2f zero(0.0f, 0.0f);
    *reinterpret_cast<Vec2f*>(self + 0x8) = zero;
}
void fn_94_DD14(u8* self) { self[0x31] = 0; }
void fn_94_DD20(u8* self) { self[0x31] = 1; }

}

// Pool-slot lookups by index (placeholder names; each returns a sub-object of this, or null for unknown indices).
extern "C" {

void fn_94_CCE4(u8* self, u8 value) { self[0x1d6b4] = value; }

u8* fn_94_CD84(u8* self, int index) {
    if (index == 1) return self + 0x1938;
    if (index == 0) return self + 0x10;
    return 0;
}
u8* fn_94_CDC4(u8* self, int index) {
    if (index == 3) return self + 0x477c;
    if (index == 2) return self + 0x2fb0;
    if (index == 1) return self + 0x17e4;
    if (index == 0) return self + 0x18;
    return 0;
}
u8* fn_94_CE24(u8* self, int index) {
    if (index == 2) return self + 0x3264;
    if (index == 1) return self + 0x193c;
    if (index == 0) return self + 0x14;
    return 0;
}
u8* fn_94_CE5C(u8* self, int index) {
    if (index == 2) return self + 0x354c;
    if (index == 1) return self + 0x1ab0;
    if (index == 0) return self + 0x14;
    return 0;
}
u8* fn_94_CE94(u8* self, int index) {
    if (index == 3) return self + 0x489c;
    if (index == 2) return self + 0x3070;
    if (index == 1) return self + 0x1844;
    if (index == 0) return self + 0x18;
    return 0;
}
u8* fn_94_E590(u8* self, int index) {
    if (index == 0) return self + 0xc;
    return 0;
}
u8* fn_94_E678(u8* self, int index) {
    if (index == 0) return self + 0xc;
    return 0;
}
u8* fn_94_E760(u8* self, int index) {
    if (index == 0) return self + 0xc;
    return 0;
}

}

extern "C" {

// HYPOTHESIS: the first call dispatches through the vtable stored at +0x3C with the value as its argument.
void fn_94_A3D8(ftSamus* self, void* data, int value) {
    reinterpret_cast<void (*)(ftSamus*, int)>((*reinterpret_cast<void***>(reinterpret_cast<u8*>(self) + 0x3c))[0x29c / 4])(self, value);
    self->Fighter::changeSucceedCore(data, 0x126);
}

}

// soGeneralWorkSimple accessors and updaters (shared inline helpers) under their placeholder names.
extern "C" {

u32 fn_94_AEA4(soGeneralWorkSimple* self, u32 flag, u32 index) {
    u32 result;
    if ((result = self->isFlag(flag, index)) == 1) {
        self->offFlag(flag, index);
    }
    return result;
}

bool fn_94_AF18(soGeneralWorkSimple* self, u32 flag, u32 index) { return self->m_flagWorks[index] & flag; }
void fn_94_AF34(soGeneralWorkSimple* self, u32 flag, u32 index) { self->m_flagWorks[index] &= ~flag; }
void fn_94_AF4C(soGeneralWorkSimple* self, u32 index) { self->m_flagWorks[index] = 0; }
void fn_94_AF60(soGeneralWorkSimple* self, u32 flag, u32 index) { self->m_flagWorks[index] |= flag; }

void fn_94_AF80(soGeneralWorkSimple* self, float value, u32 index) {
    if (value != 0.0f) {
        self->m_floatWorks[index] /= value;
    }
}
void fn_94_AFA8(soGeneralWorkSimple* self, float value, u32 index) { self->m_floatWorks[index] *= value; }
void fn_94_AFC0(soGeneralWorkSimple* self, float value, u32 index) { self->m_floatWorks[index] -= value; }
void fn_94_AFD8(soGeneralWorkSimple* self, float value, u32 index) { self->m_floatWorks[index] += value; }
void fn_94_AFF0(soGeneralWorkSimple* self, float value, u32 index) { self->m_floatWorks[index] = value; }
float fn_94_B000(soGeneralWorkSimple* self, u32 index) { return self->m_floatWorks[index]; }

void fn_94_B018(soGeneralWorkSimple* self, u32 index) { self->m_intWorks[index]--; }
void fn_94_B030(soGeneralWorkSimple* self, u32 index) { self->m_intWorks[index]++; }
void fn_94_B048(soGeneralWorkSimple* self, s32 value, u32 index) {
    if (value != 0) {
        self->m_intWorks[index] /= value;
    }
}
void fn_94_B068(soGeneralWorkSimple* self, s32 value, u32 index) { self->m_intWorks[index] *= value; }
void fn_94_B080(soGeneralWorkSimple* self, s32 value, u32 index) { self->m_intWorks[index] -= value; }
void fn_94_B098(soGeneralWorkSimple* self, s32 value, u32 index) { self->m_intWorks[index] += value; }
void fn_94_B0B0(soGeneralWorkSimple* self, s32 value, u32 index) { self->m_intWorks[index] = value; }

}

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_94_6CF8(u8* p) { return *(u8*)(p + 0x4); }
int fn_94_A630(u8* p) { return *(int*)(p + 0x28); }
void fn_94_AAA8() {}
void fn_94_AAAC() {}
int fn_94_AAB0() { return 32683; }
void fn_94_AAB8() {}
int fn_94_AE9C(u8* p) { return *(int*)(p + 0x20); }
int fn_94_AF78(u8* p) { return *(int*)(p + 0x18); }
void fn_94_DC2C() {}
void fn_94_E338() {}
void fn_94_E3BC() {}
void fn_94_E4A4() {}
void fn_94_E58C() {}
void fn_94_E674() {}
void fn_94_E75C() {}
void fn_94_E844() {}
void fn_94_E92C() {}
void fn_94_EA14() {}

}
