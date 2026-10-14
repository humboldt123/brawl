// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#include <math.h>
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/ft_reflector_group.h>
#include <ft/ness/ft_ness_builder.h>
#include <ft/ness/ft_ness_extend_param_accesser.h>

#define FT_NO_REFLECTOR_INSTANTIATION
#define FT_BC ftNessBuildConfig
#include <ft/builder/ft_builder_noinline.h>

#pragma dont_inline on
template soCollisionShieldModuleBuilder<ftNessCollisionReflectorBaseModuleBuildConfig>::soCollisionShieldModuleBuilder(soModuleAccesser*, int, gfTask::Category);
template soCollisionShieldModuleBuilder<ftNessCollisionAbsorberModuleBuildConfig>::soCollisionShieldModuleBuilder(soModuleAccesser*, int, gfTask::Category);
#pragma dont_inline off

ftNessExtendParamAccesser g_ftNessExtendParamAccesser;

ftClassInfoImpl<Fighter_Ness, ftNess> g_ftClassInfoNess;

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_7CFC;
extern soStatusUniqProcess lbl_101_bss_1DC;
extern soStatusUniqProcess lbl_101_bss_1EC;
extern soStatusUniqProcess lbl_101_bss_1FC;
extern soStatusUniqProcess lbl_101_bss_20C;
extern soStatusUniqProcess lbl_101_bss_21C;
extern soStatusUniqProcess lbl_101_bss_22C;
extern soStatusUniqProcess lbl_101_bss_23C;
extern soStatusUniqProcess lbl_101_bss_25C;
}
extern soStatusUniqProcess g_ftStatusUniqProcessAttackAirInheritJumpAerialMotion;
extern soStatusUniqProcess g_ftStatusUniqProcessEscapeAirInheritJumpAerialMotion;
extern soStatusUniqProcess g_ftNessStatusUniqProcessSpecialLw;

ftNess::ftNess(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftNessBuildConfig>(entryId,
                                         Fighter_Ness,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Ness)) {
    unk4AE24 = 0;
    unk4AE25 = 0;

    // Ness owns 15 character action slots; the unused ones keep the shared null process.
    soStatusUniqProcess* processes[15] = {0};
    processes[0] = &lbl_27_bss_7CFC;
    processes[1] = &lbl_101_bss_20C;
    processes[3] = &g_ftNessStatusUniqProcessSpecialLw;
    processes[5] = &lbl_27_bss_7CFC;
    processes[6] = &lbl_27_bss_7CFC;
    processes[7] = &lbl_27_bss_7CFC;
    processes[8] = &lbl_101_bss_21C;
    processes[10] = &lbl_101_bss_22C;
    processes[11] = &lbl_101_bss_23C;
    processes[12] = &lbl_101_bss_25C;
    processes[13] = &lbl_101_bss_25C;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 15);

    // Generic aerial transitions and the smash attack actions override entries outside the consecutive character range.
    m_moduleAccesser->getStatusModule().setUniqProc(0x33, &g_ftStatusUniqProcessAttackAirInheritJumpAerialMotion);
    m_moduleAccesser->getStatusModule().setUniqProc(0x21, &g_ftStatusUniqProcessEscapeAirInheritJumpAerialMotion);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2C, &lbl_101_bss_1FC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x30, &lbl_101_bss_1DC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x31, &lbl_101_bss_1DC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x32, &lbl_101_bss_1EC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2D, &lbl_101_bss_1DC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2E, &lbl_101_bss_1DC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2F, &lbl_101_bss_1EC);

    // HYPOTHESIS: as in Lucas, the common data record (+0x8c) holds a sphere that is installed as the absorber shape (PSI Magnet)
    // and the reflector comes from the record at +0x94.
    soCollisionShieldData absorberShape;
    u8 reflectorShape[0x20];
    ftReflectorGroupDraft reflectorGroup;
    soCollisionShieldGroupData absorberGroup;
    const float* sphere = *reinterpret_cast<const float**>(reinterpret_cast<u8*>(m_data) + 0x8c);
    absorberShape.setOffset(0, sphere[1]);
    absorberShape.setOffset(1, sphere[2]);
    absorberShape.setOffset(2, sphere[3]);
    absorberShape.setOffset(3, sphere[1]);
    absorberShape.setOffset(4, sphere[2]);
    absorberShape.setOffset(5, sphere[3]);
    absorberShape.m_size = sphere[4];
    absorberShape.m_nodeIndex = (u32)sphere[0];
    absorberShape.m_shapeType = 1;
    absorberGroup.m_flags = 0;
    absorberGroup.m_shieldDataSet = soSet<soCollisionShieldData>(&absorberShape, 1);
    soCollisionShieldModule& absorber = m_moduleAccesser->getCollisionAbsorberModule();
    absorber.add(&absorberGroup, 0);
    absorber.setStatus(0, 0, 0);

    void* reflectorRecord = *reinterpret_cast<void**>(reinterpret_cast<u8*>(m_data) + 0x94);
    FT_REFLECTOR_GROUP_CLEAR(reflectorGroup);
    fn_27_195B34(reflectorRecord, reflectorShape, &reflectorGroup);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&reflectorGroup), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
}

void** ftNess::getExtendParam() {
    return m_data->extendParam;
}

void ftNess::processUpdate() {
    Fighter::processUpdate();
    unk4AE25 = 0;
    unk4AE24 = 0;
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftNess is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftNessInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftNessInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_101_92E8(u8* p) { return *(u8*)(p + 0x4); }
void fn_101_B58C() {}
int fn_101_CC78() { return 0; }
void fn_101_CE18() {}
void fn_101_D0C8() {}
void fn_101_D0CC() {}
void fn_101_D0D0() {}
void fn_101_D0D4() {}
int fn_101_D0D8() { return 0; }
void fn_101_D11C() {}
void fn_101_D120() {}
void fn_101_D124() {}
void fn_101_D128() {}
int fn_101_D144() { return 0; }
void fn_101_D14C() {}
int fn_101_D15C() { return 0; }
int fn_101_D164() { return 0; }
u8* fn_101_D240(u8* p) { return p + 0x458; }
u8* fn_101_D248(u8* p) { return p + 0x3C8; }
u8* fn_101_D250(u8* p) { return p + 0x8; }
int fn_101_D2E8(u8* p) { return *(int*)(p + 0x20); }
int fn_101_D3C4(u8* p) { return *(int*)(p + 0x18); }
int fn_101_D45C(u8* p) { return *(int*)(p + 0x10); }
int fn_101_EE70() { return 6; }
u8* fn_101_EE84(u8* p) { return p + 0x4A874; }
u8* fn_101_EEA4(u8* p) { return p + 0x4A8B0; }
u8* fn_101_EEB0(u8* p) { return p + 0x4AD68; }
void fn_101_FE20() {}
void fn_101_FF08(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_101_FF14(u8* p) { *(u8*)(p + 0x31) = 1; }
int fn_101_10660() { return 0; }
int fn_101_10748() { return 0; }
int fn_101_10830() { return 0; }
int fn_101_10918() { return 0; }
int fn_101_10A00() { return 0; }
int fn_101_10AE8() { return 0; }
int fn_101_10BD0() { return 0; }
int fn_101_10CB8() { return 0; }

}

// Placeholders for weak inline methods of library classes (their names exist in sora_melee, so the REL copies cannot be renamed): the shim calls the inline method.
extern "C" {

bool fn_101_D364(const soGeneralWorkSimple* p, u32 a0, u32 a1) { return p->soGeneralWorkSimple::isFlag(a0, a1); }
void fn_101_D380(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::offFlag(a0, a1); }
void fn_101_D398(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::clearFlag(a0); }
void fn_101_D3AC(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::onFlag(a0, a1); }
void fn_101_D3F4(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::mulFloatWork(a0, a1); }
void fn_101_D40C(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::subFloatWork(a0, a1); }
void fn_101_D424(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::addFloatWork(a0, a1); }
void fn_101_D43C(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::setFloatWork(a0, a1); }
float fn_101_D44C(const soGeneralWorkSimple* p, u32 a0) { return p->soGeneralWorkSimple::getFloatWork(a0); }
void fn_101_D464(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::decIntWork(a0); }
void fn_101_D47C(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::incIntWork(a0); }
void fn_101_D494(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::divIntWork(a0, a1); }
void fn_101_D4B4(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::mulIntWork(a0, a1); }
void fn_101_D4CC(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::subIntWork(a0, a1); }
void fn_101_D4E4(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::addIntWork(a0, a1); }
void fn_101_D4FC(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::setIntWork(a0, a1); }
Vec2f fn_101_FF38(soKineticEnergyNormal* p) { return p->soKineticEnergyNormal::getSpeed(); }

}

// Small helpers of this unit (map names in parentheses).
extern "C" {

// (ftness__atan2f)
#pragma scheduling 603 // MATCH-ONLY: the epilogue lwz precedes the frsp
float fn_101_BD7C(double y, double x) { return (float)atan2(y, x); }
#pragma scheduling reset
// (ftness__ABS_f_): __fabs on double, no frsp
double fn_101_CA34(double x) { return __fabs(x); }
// (Vec3f____ct1): copies x and y from src, sets z
void fn_101_C2CC(Vec3f* p, const Vec3f* src, float z) {
    p->m_x = src->m_x;
    p->m_y = src->m_y;
    p->m_z = z;
}

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_101_10EDC(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_101_10EE4(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
bool fn_101_10EEC(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_101_10EFC(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
void fn_101_10EF4(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
void fn_101_10F04(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_101_2078(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_3B0C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_3F84(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_4324(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_4784(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_4A6C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_4AAC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_5D14(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_5D54(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_5D94(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
