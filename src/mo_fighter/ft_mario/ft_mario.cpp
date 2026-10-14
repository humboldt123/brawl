// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_reflector_group.h>
#include <ft/ft_class_info_impl.h>
#include <ft/mario/ft_mario.h>
#include <ft/mario/ft_mario_extend_param_accesser.h>

#define FT_BC ftMarioBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
// Parameter-table variation getter (soValueAccesser::getValueVariation) under its map address.
extern "C" int fn_91_ABE8() { return g_soValueVariation; }

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_91_bss_1E4;
extern soStatusUniqProcess lbl_91_bss_204;
extern soStatusUniqProcess lbl_27_bss_734C;
extern soStatusUniqProcess lbl_91_bss_1F4;
}

ftMarioExtendParamAccesser g_ftMarioExtendParamAccesser;
ftMarioDExtendParamAccesser g_ftMarioDExtendParamAccesser;

ftClassInfoImpl<Fighter_Mario, ftMario> g_ftClassInfoMario;
ftClassInfoImpl<Fighter_MarioD, ftMarioD> g_ftClassInfoMarioD;

ftMario::ftMario(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftMarioBuildConfig>(entryId,
                                         Fighter_Mario,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Mario)),
    unk2DBD4(0) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[7] = {0};
    processes[0] = &lbl_91_bss_1E4;
    processes[1] = &lbl_91_bss_204;
    processes[2] = &lbl_27_bss_734C;
    processes[6] = &lbl_91_bss_1F4;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 7);
    // The cape: build the reflector group from the common data record (+0x8c) and install it as group 2.
    ftReflectorGroupDraft group;
    FT_REFLECTOR_GROUP_CLEAR(group);
    u8 shape[0x20];
    fn_27_195B34(*reinterpret_cast<void**>(reinterpret_cast<u8*>(m_data) + 0x8c), shape, &group);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&group), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftMario is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftMarioInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftMarioInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Empty virtual overrides of this module (ftEntryEventObserver / Fighter no-ops), under their map addresses.
extern "C" {
void fn_91_A4A4() {}
void fn_91_A4A8() {}
void fn_91_A4AC() {}
void fn_91_A4B0() {}
void fn_91_A4B4() {}
void fn_91_A4B8() {}
void fn_91_A4BC() {}
void fn_91_A4C0() {}
void fn_91_A4C4() {}
void fn_91_A4C8() {}
void fn_91_A4CC() {}
void fn_91_A4D0() {}
void fn_91_A4DC() {}
void fn_91_A4E0() {}
void fn_91_A4E4() {}
void fn_91_A4E8() {}
void fn_91_A514() {}
void fn_91_A518() {}
void fn_91_A51C() {}
void fn_91_A520() {}
void fn_91_A524() {}
void fn_91_A548() {}
void fn_91_A54C() {}
void fn_91_A550() {}
void fn_91_A554() {}
void fn_91_A568() {}
void fn_91_A56C() {}
void fn_91_A570() {}
void fn_91_A574() {}
void fn_91_A578() {}

}

// Field accessors of this module under their map addresses (offsets read from the target code).
extern "C" {

u8 fn_91_22B4(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_91_A0AC(u8* p) { return *(u8*)(p + 0x44); }
int fn_91_A528(u8* p) { return *(int*)(p + 0x110); }
bool fn_91_9D0C(void* self, s8 kind) { return kind == 0xc; }

}

// Weak inline methods of library classes that the REL emits as stand-alone functions. Their names exist in
// sora_melee, so the REL copies cannot be renamed: each shim calls the inline method (as in ft_ike.cpp).
// Virtual-node matrix pool getters (map order: extend, common, hit).
extern "C" {
void* fn_91_A63C(ftVirtualNodeMatrixPoolImpl* p) { return p->ftVirtualNodeMatrixPoolImpl::getExtendMatrix(); }
void* fn_91_A644(ftVirtualNodeMatrixPoolImpl* p) { return p->ftVirtualNodeMatrixPoolImpl::getCommonMatrix(); }
void* fn_91_A64C(ftVirtualNodeMatrixPoolImpl* p) { return p->ftVirtualNodeMatrixPoolImpl::getHitMatrix(); }
}

// soGeneralWorkSimple work-area methods, in map order (flags, floats, ints).
extern "C" {
u32 fn_91_A6EC(soGeneralWorkSimple* p) { return p->soGeneralWorkSimple::getFlagWorkSize(); }
bool fn_91_A768(const soGeneralWorkSimple* p, u32 mask, u32 idx) { return p->soGeneralWorkSimple::isFlag(mask, idx); }
void fn_91_A784(soGeneralWorkSimple* p, u32 mask, u32 idx) { p->soGeneralWorkSimple::offFlag(mask, idx); }
void fn_91_A79C(soGeneralWorkSimple* p, u32 idx) { p->soGeneralWorkSimple::clearFlag(idx); }
void fn_91_A7B0(soGeneralWorkSimple* p, u32 mask, u32 idx) { p->soGeneralWorkSimple::onFlag(mask, idx); }
u32 fn_91_A7C8(soGeneralWorkSimple* p) { return p->soGeneralWorkSimple::getFloatWorkSize(); }
// MATCH-ONLY: divFloatWork is too large for MWCC to inline here (it emits a tail call to a weak copy that the target
// does not have), so its body is written out.
void fn_91_A7D0(soGeneralWorkSimple* p, float v, u32 idx) {
    if (v == 0.0f) {
        return;
    }
    p->m_floatWorks[idx] /= v;
}
void fn_91_A7F8(soGeneralWorkSimple* p, float v, u32 idx) { p->soGeneralWorkSimple::mulFloatWork(v, idx); }
void fn_91_A810(soGeneralWorkSimple* p, float v, u32 idx) { p->soGeneralWorkSimple::subFloatWork(v, idx); }
void fn_91_A828(soGeneralWorkSimple* p, float v, u32 idx) { p->soGeneralWorkSimple::addFloatWork(v, idx); }
void fn_91_A840(soGeneralWorkSimple* p, float v, u32 idx) { p->soGeneralWorkSimple::setFloatWork(v, idx); }
float fn_91_A850(const soGeneralWorkSimple* p, u32 idx) { return p->soGeneralWorkSimple::getFloatWork(idx); }
void fn_91_A868(soGeneralWorkSimple* p, u32 idx) { p->soGeneralWorkSimple::decIntWork(idx); }
void fn_91_A880(soGeneralWorkSimple* p, u32 idx) { p->soGeneralWorkSimple::incIntWork(idx); }
void fn_91_A898(soGeneralWorkSimple* p, s32 v, u32 idx) { p->soGeneralWorkSimple::divIntWork(v, idx); }
void fn_91_A8B8(soGeneralWorkSimple* p, s32 v, u32 idx) { p->soGeneralWorkSimple::mulIntWork(v, idx); }
void fn_91_A8D0(soGeneralWorkSimple* p, s32 v, u32 idx) { p->soGeneralWorkSimple::subIntWork(v, idx); }
void fn_91_A8E8(soGeneralWorkSimple* p, s32 v, u32 idx) { p->soGeneralWorkSimple::addIntWork(v, idx); }
void fn_91_A900(soGeneralWorkSimple* p, s32 v, u32 idx) { p->soGeneralWorkSimple::setIntWork(v, idx); }
}

// Kinetic energy and builder members emitted in this unit.
extern "C" {
void fn_91_CD00(soKineticEnergyNormal* p) { p->soKineticEnergyNormal::offConsiderGroundFriction(); }
void fn_91_CD0C(soKineticEnergyNormal* p) { p->soKineticEnergyNormal::onConsiderGroundFriction(); }
// ftFighterBuilder<ftMarioBuildConfig> virtual getters (map order: cancel module, hit-test query, matrix pool, gimmick pool).
void* fn_91_BD50(ftFighterBuilder<ftMarioBuildConfig>* p) { return p->ftFighterBuilder<ftMarioBuildConfig>::getCancelModule(); }
void* fn_91_BD70(ftFighterBuilder<ftMarioBuildConfig>* p) { return p->ftFighterBuilder<ftMarioBuildConfig>::getVirtualNodeMatrixPool(); }
void* fn_91_BD7C(ftFighterBuilder<ftMarioBuildConfig>* p) { return p->ftFighterBuilder<ftMarioBuildConfig>::getStatusGimmickUniqProcessPool(); }
void fn_91_CC18() {}
void fn_91_D328() {}
void fn_91_D3AC() {}
void fn_91_D494() {}
void fn_91_D57C() {}
void fn_91_D664() {}
void fn_91_D74C() {}
void fn_91_D834() {}
void fn_91_D91C() {}
void fn_91_DA04() {}

}

// Zero-fill of a three-float member (Vec3f-sized).
extern "C" {

void fn_91_CD18(float* p) {
    p[0] = 0.0f;
    p[1] = 0.0f;
    p[2] = 0.0f;
}

}

// Fighter-builder cancel query, forwarding to the cancel module through its virtual interface.
extern "C" bool fn_91_BD5C(ftFighterBuilder<ftMarioBuildConfig>* p) { return p->ftFighterBuilder<ftMarioBuildConfig>::isEnableCancel(); }

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
u8* fn_91_D580(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_91_D668(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_91_D750(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
