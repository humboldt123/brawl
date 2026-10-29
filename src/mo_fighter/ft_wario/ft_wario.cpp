#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
// MATCH-ONLY: the custom kinetic helper calls the shared vector constructor.
#define MT_VEC3F_CTOR_NOINLINE
#define FT_NOINLINE_SEARCH_BUILDER
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_SHARED_STATUS_TABLE_CTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/wario/ft_wario_builder.h>
#include <ft/wario/ft_wario_extend_param_accesser.h>
#include <ft/wario/ft_wario_common.h>
#include <ft/wario/ft_wario_transactor.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_owner.h>
#include <wn/wario/wn_wario_garlic.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/so_kinetic_utility.h>
#include <wn/wn_article_pool.h>

#define FT_BC ftWarioBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

ftWarioExtendParamAccesser g_ftWarioExtendParamAccesser;

ftClassInfoImpl<Fighter_Wario, ftWario> g_ftClassInfoWario;

// Wario extends the common fighter data with an area set and model settings
// used by the second six-color costume group. Calls through the real area and
// model interfaces establish both the record offsets and the argument types.
struct ftWarioData : ftData {
    soSet<soAreaData> additionalAreas; // +0x94
    u32** costumeNodeConvertData; // +0x9C
    int costumeShadowNode; // +0xA0
};
static_assert(sizeof(ftWarioData) == 0xA4, "Wario data prefix");

// Only the primary status-process view is needed for registration, as in the
// other fighters. Concrete implementations remain in their status units.
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNOpen;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialS;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialHiStart;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialLw;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNBite;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNBiteEnd;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNEatStart;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNEat;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSStart;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSDrive;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSWheelie;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSCommon;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSEscape;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSDown;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialHiJump;

ftWario::ftWario(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftWarioBuildConfig>(entryId,
                                         Fighter_Wario,
                                         instHeap,
                                         instHeap, // Wario uses the instance heap for models too.
                                         nwMotionInstHeap),
    m_data(static_cast<ftWarioData*>(g_ftCommonDataAccesser.getData(Fighter_Wario))) {
    // Chomp's open, bite and eating phases share handlers where their behavior
    // agrees. Null entries install no character-specific process.
    soStatusUniqProcess* processes[30] = {0};
    processes[0] = &g_ftWarioStatusUniqProcessSpecialNOpen;
    processes[1] = &g_ftWarioStatusUniqProcessSpecialS;
    processes[2] = &g_ftWarioStatusUniqProcessSpecialHiStart;
    processes[3] = &g_ftWarioStatusUniqProcessSpecialLw;
    processes[5] = &g_ftWarioStatusUniqProcessSpecialNOpen;
    processes[7] = &g_ftWarioStatusUniqProcessSpecialNBite;
    processes[8] = &g_ftWarioStatusUniqProcessSpecialNBite;
    processes[9] = &g_ftWarioStatusUniqProcessSpecialNBiteEnd;
    processes[10] = &g_ftWarioStatusUniqProcessSpecialNEatStart;
    processes[11] = &g_ftWarioStatusUniqProcessSpecialNEat;
    processes[12] = &g_ftWarioStatusUniqProcessSpecialNEat;
    processes[13] = &g_ftWarioStatusUniqProcessSpecialNEat;
    processes[14] = &g_ftWarioStatusUniqProcessSpecialNEat;

    // Mounting, riding, wheelies, dismounting and the shared bike states.
    processes[15] = &g_ftWarioStatusUniqProcessSpecialSStart;
    processes[17] = &g_ftWarioStatusUniqProcessSpecialSStart;
    processes[18] = &g_ftWarioStatusUniqProcessSpecialSDrive;
    processes[19] = &g_ftWarioStatusUniqProcessSpecialSWheelie;
    processes[20] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[21] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[22] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[23] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[24] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[25] = &g_ftWarioStatusUniqProcessSpecialSEscape;
    processes[26] = &g_ftWarioStatusUniqProcessSpecialSDown;
    processes[27] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[28] = &g_ftWarioStatusUniqProcessSpecialHiJump;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 30);

    m_moduleAccesser->getAreaModule().addArea(&m_data->additionalAreas);
    // HYPOTHESIS: colors 6..11 are the classic-costume group. Their distinct
    // node conversion and shadow anchor are explicit in the native setup.
    if (getOwner()->getFighterColor() >= 6 && getOwner()->getFighterColor() <= 11) {
        m_moduleAccesser->getModelModule().setNodeConvertData(m_data->costumeNodeConvertData);
        m_moduleAccesser->getModelModule().setShadowNode(m_data->costumeShadowNode);
    }
}

// The common implementation keeps Waft charge, bike links and transformation
// state consistent between normal Wario and Wario-Man.
// MATCH-ONLY: the kinetic setup uses the shared getter call boundary.
#pragma dont_inline on
ftWarioExtendParam* ftWario::getExtendParam() {
    return reinterpret_cast<ftWarioExtendParam*>(&m_data->extendParam[0]);
}
#pragma dont_inline reset

void ftWario::onActivate() {
    ftWarioExtendParam* param = reinterpret_cast<ftWarioExtendParam*>(&m_data->extendParam[0]);
    g_ftWarioCommon.onActivate(m_moduleAccesser, &m_waftAnim, param->bikeRider->remountCooldownFrames);
}

void ftWario::onDeactivate() {
    g_ftWarioCommon.onDeactivate(m_moduleAccesser, &m_waftAnim);
}

void ftWario::onStart(int startKind) {
    Fighter::onStart(startKind);
    g_ftWarioCommon.onStart(m_moduleAccesser, startKind);
}

void ftWario::toDead(Dead::Reason reason) {
    g_ftWarioCommon.toDead(m_moduleAccesser, reason);
    Fighter::toDead(reason);
}

void ftWario::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc, StageObject* sender, int selector) {
    if (!g_ftWarioCommon.notifyEventLink(static_cast<ftWarioLinkEventArgs*>(event), acc, sender, selector))
        Fighter::notifyEventLink(event, acc, sender, selector);
}

void ftWario::notifyEventChangeStatus(int status, int previous, soStatusData* data, soModuleAccesser* acc) {
    Fighter::notifyEventChangeStatus(status, previous, data, acc);
    g_ftWarioCommon.notifyEventChangeStatus(status, previous, data, acc);
}

bool ftWario::notifyEventAnimCmd(acAnimCmd* command, soModuleAccesser* acc, s32 index) {
    if (!isObserv(command->getGroup())) return false;
    switch (command->getType()) {
    case 5:
        g_ftWarioCommon.toChange(acc, m_entryId, 1, 1);
        return true;
    default:
        return Fighter::notifyEventAnimCmd(command, acc, index);
    }
}

// MATCH-ONLY: native addresses the guard, singleton and destructor registration
// separately, as in the other fighter transactor callbacks.
#pragma pool_data off
void ftWario::notifyEventCollisionSearch(soCollisionLog* log, soModuleAccesser* acc) {
    static ftWarioTransactor transactor;
    static_cast<ftKirbyCopyAbilityProcess&>(transactor).notifyEventCollisionSearch(log, acc);
    Fighter::notifyEventCollisionSearch(log, acc);
}
#pragma pool_data reset

// Restore the saved fighter state when returning from Wario-Man. The native
// virtual calls preserve overrides of both the start and restoration hooks.
void ftWario::change(void* work, int startKind) {
    onStart(startKind);
    changeSucceedCore(work, 0x130);
}

void ftWario::setupChangeSucceedWork(void* work) {
    Fighter::setupChangeSucceedWork(work);
    g_ftWarioCommon.setupChangeSucceedWork(m_moduleAccesser, work);
}

void ftWario::changeSucceedCore(void* work, int kind) {
    Fighter::changeSucceedCore(work, kind);
    g_ftWarioCommon.changeSucceedCore(m_moduleAccesser, work, kind);
}

void ftWario::processUpdate() {
    Fighter::processUpdate();
    ftWarioExtendParam* param = reinterpret_cast<ftWarioExtendParam*>(&m_data->extendParam[0]);
    g_ftWarioCommon.processUpdate(param->specialLw, m_moduleAccesser, &m_waftAnim);
}

bool ftWario::activeArticle(wnWarioGarlic* article, soModuleAccesser* acc) {
    int founderTaskId;
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resource = acc->getResourceModule();
    founderTaskId = fighter.m_taskId;
    article->activate(founderTaskId, resource.getResourceIdAccesser()->getEtcResId(),
                      fighter.getOwner()->getTeam());
    return true;
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftWario is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftWarioInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftWarioInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers.
void fn_110_9464() {}
void fn_110_9470() {}
void fn_110_94B0() {}
void fn_110_94B4() {}
void fn_110_9544() {}
void fn_110_9570() {}
void fn_110_9574() {}
void fn_110_9578() {}
void fn_110_957C() {}
void fn_110_9580() {}
void fn_110_9584() {}
void fn_110_9588() {}
void fn_110_958C() {}
void fn_110_9590() {}
void fn_110_9594() {}
void fn_110_9598() {}
void fn_110_959C() {}
void fn_110_95A0() {}
void fn_110_95A4() {}
void fn_110_95A8() {}
void fn_110_95B4() {}
void fn_110_95B8() {}
void fn_110_95BC() {}
void fn_110_95C0() {}
void fn_110_95EC() {}
void fn_110_95F0() {}
void fn_110_95F4() {}
void fn_110_95F8() {}
void fn_110_95FC() {}
void fn_110_9620() {}
void fn_110_9624() {}
void fn_110_9628() {}
void fn_110_9634() {}
void fn_110_9638() {}
void fn_110_963C() {}
void fn_110_9648() {}
void fn_110_9654() {}
void fn_110_9658() {}
void fn_110_B824() {}
void fn_110_BF34() {}
void fn_110_BFB8() {}
void fn_110_C0A0() {}
void fn_110_C188() {}
void fn_110_C270() {}
void fn_110_C358() {}
void fn_110_C440() {}
void fn_110_C528() {}
void fn_110_C610() {}

// Work-array sizes and field read.
u8 fn_110_9468(u8* p) { return *(u8*)(p + 0x44); }
int fn_110_97CC(u8* p) { return *(int*)(p + 0x20); }
int fn_110_98A8(u8* p) { return *(int*)(p + 0x18); }
int fn_110_9940(u8* p) { return *(int*)(p + 0x10); }

// Bit flags in the u32 array at +0x1C (soGeneralWorkSimple).
bool fn_110_9848(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_110_9864(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_110_987C(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_110_9890(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_110_98B0(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_110_98D8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_110_98F0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_110_9908(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_110_9920(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_110_9930(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_110_9948(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_110_9960(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_110_9978(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_110_9998(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_110_99B0(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_110_99C8(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_110_99E0(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }
int fn_110_99F0(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); return a[idx]; }

// Sub-object accessors (ftVirtualNodeMatrixPool, ftStatusGimmickUniqProcessPool, ftSound3dGeneratorAccesser).
u8* fn_110_971C(u8* p) { return p + 0x458; }
u8* fn_110_9724(u8* p) { return p + 0x3C8; }
u8* fn_110_972C(u8* p) { return p + 0x8; }
u8* fn_110_9734(u8* p) { return p + 0x84; }
u8* fn_110_973C(u8* p) { return p + 0x70; }
u8* fn_110_9744(u8* p) { return p + 0x5C; }
u8* fn_110_974C(u8* p) { return p + 0x48; }
u8* fn_110_9754(u8* p) { return p + 0x34; }
u8* fn_110_975C(u8* p) { return p + 0x20; }
u8* fn_110_9764(u8* p) { return p + 0x8; }
u8* fn_110_965C(u8* p, u32 idx) { return p + 4 + (idx << 3); }

// Field reads and constant returns.
int fn_110_9600(u8* p) { return *(int*)(p + 0x110); }
int fn_110_9618(u8* p) { return 0; }
int fn_110_91B8(u8* p) { return 0; }
bool fn_110_9A00(u8* p) { return false; }
bool fn_110_8EF4(u8* p, s8 kind) { return kind == 12; }
int fn_110_A9BC(u8* p) { return 2; }
void fn_110_A9C4(u8* p, u8 v) { *(u8*)(p + 0x4B0C) = v; }

// Cancel / pool sub-object accessors on the fighter (+0x10000 base adjusted).
u8* fn_110_A9CC(u8* p) { return p + 0xCD4C; }
u8* fn_110_A9EC(u8* p) { return p + 0xCD88; }
u8* fn_110_A9F8(u8* p) { return p + 0xD240; }

// Single-node gate on an index.
u8* fn_110_AA48(u8* p, u32 idx) { return idx == 0 ? p + 0xC : 0; }
u8* fn_110_AA60(u8* p, u32 idx) { return idx == 0 ? p + 0xC : 0; }

// Ground friction flag at +0x31.
void fn_110_B90C(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_110_B918(u8* p) { *(u8*)(p + 0x31) = 1; }

// Destructor wrappers: delete the object when the flag is positive, then return it.
// Sub-object destructor wrappers first run the sibling wrapper with flags 0.
#pragma dont_inline on
void* fn_110_1E6C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_1F08(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_22C4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_2304(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_3A60(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_3D64(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_3E14(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_40EC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_41B4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_451C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4804(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4844(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4FB4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4FF4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_5034(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_5074(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_6000(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

void* fn_110_1EAC(void* p, s16 flags) {
    if (p != 0) {
        fn_110_1F08(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_3D08(void* p, s16 flags) {
    if (p != 0) {
        fn_110_3D64(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_408C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_40EC((u8*)p + 0xC, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_50B4(void* p, s16 flags) {
    if (p != 0) {
        fn_110_3D08((u8*)p + 0x1C0, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_5C9C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_3D08((u8*)p + 0x88, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_6620(void* p, s16 flags) {
    if (p != 0) {
        fn_110_451C(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AAD8(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_ACB8(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AD14(void* p, s16 flags) {
    if (p != 0) {
        fn_110_ACB8((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AD74(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AD14((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B4A0(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B4FC(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B4A0((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B55C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B4FC((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B634(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B690(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B634((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B6F0(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B690((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AA78(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AB34(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AA78((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AB94(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AB34((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_ABF4(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AB94((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AC54(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_AB94((u8*)p + 0x4, -1); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AE44(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AEA4(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AE44((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AF04(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AEA4((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AFDC(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B03C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AFDC((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B09C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B03C((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B174(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AFDC(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B1D0(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B174((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B230(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B1D0((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B308(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B368(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B308((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B3C8(void* p, s16 flags) {
    if (p != 0) {
        fn_110_B368((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_65C0(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_451C(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
#pragma dont_inline reset


// Field read, constant returns and a global value-variation pointer.
u8 fn_110_6E04(u8* p) { return *(u8*)(p + 0x4); }
int fn_110_962C(u8* p) { return 0; }
int fn_110_9640(u8* p) { return 0x7FAD; }
int fn_110_964C(u8* p) { return 0; }
extern void* g_soValueVariation;
void* fn_110_9CC8() { return g_soValueVariation; }

// Virtual slot tail calls (vtable offset 8+4k for slot k).
struct ftWarioVSlotsB {
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
struct ftWarioVSlotsI {
    virtual int slot0();
    virtual int slot1();
    virtual int slot2();
    virtual int slot3();
};
void fn_110_AA38(u8* p) { ((ftWarioVSlotsB*)p)->slot20(); }
int fn_110_A9D8(u8* p) { return ((ftWarioVSlotsI*)(p + 0xCD4C))->slot2(); }


// Vector zeroing (float triple at the start of the object).
void fn_110_B924(float* p) { p[0] = 0.0f; p[1] = 0.0f; p[2] = 0.0f; }

// Virtual slot 3 result compared against zero.
bool fn_110_AA04(u8* p) { return ((ftWarioVSlotsI*)p)->slot3() == 0; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_110_C914(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_110_C91C(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_110_C924(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
void fn_110_C954(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_110_C95C(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_110_C964(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_110_C96C(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_110_C974(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_110_C98C(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_110_C9A4(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_110_C9B4(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
bool fn_110_C9C4(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_110_C9CC(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_110_C9D4(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_110_C9DC(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_110_C9E4(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_110_C9EC(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_110_C9F4(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_110_C9FC(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_110_CA04(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_110_CA0C(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_110_CA14(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_110_CA1C(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_110_CA24(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_110_CA2C(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_110_CA34(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_110_CA3C(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_110_CA44(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_110_CA4C(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_110_C90C(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_110_C97C(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_110_C984(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_110_C994(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_110_C99C(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_110_C9AC(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
u8* fn_110_BFBC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C0A4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C18C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C274(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C35C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C444(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C52C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C614(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"


// Standard modes use the common fighter dispatcher. Every custom request runs
// preprocessing; only the five recognized modes re-enable outside energy.
// MATCH-ONLY: preserve the shared preprocessing flag and the native stores
// left by the otherwise empty custom-mode handlers.
#pragma push
#pragma opt_propagation off
void ftWarioKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    Vec2f speed;
    soModuleAccesser* processAccesser = acc;
    bool process = true;
    Vec2f::copy(speed, ftKineticTransactHelper::preHelpProcess(processAccesser, process, process));
    switch (mode) {
    case 0x64: {
        bool result = false;
        changeKineticSub(&result, pools, &speed, acc);
        break;
    }
    case 0x65: { volatile bool result = false; break; }
    case 0x66: { volatile bool result = false; break; }
    case 0x67: { volatile bool result = false; break; }
    case 0x68: { volatile bool result = false; break; }
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(acc);
}
#pragma pop

// Mode 0x64 preserves vertical velocity in Gravity and horizontal velocity in
// Stop, enables Damage, and installs Wario's own gravity tuning. The gameplay
// name of this mode is unresolved; its selection may come from motion scripts.
void ftWarioKineticTransactor::changeKineticSub(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f initialSpeed;
    Vec2f::copy(initialSpeed, *speed);
    Vec2f gravitySpeed;
    float verticalSpeed = initialSpeed.m_y;
    gravitySpeed.m_x = 0.0f;
    gravitySpeed.m_y = verticalSpeed;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravitySpeed, &Vec3f(0.0f, 0.0f, 0.0f));
    Vec2f stopSpeed;
    stopSpeed.m_x = initialSpeed.m_x;
    stopSpeed.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(3, acc, 6, &stopSpeed, &Vec3f(0.0f, 0.0f, 0.0f));
    acc->getKineticModule().getEnergy(4)->enable();
    ftWario& fighter = dynamic_cast<ftWario&>(acc->getStageObject());
    ftWarioBikeRiderParam* param = fighter.getExtendParam()->bikeRider;
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(
        *acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = -param->gravity;
    gravity.unk1C = param->unk4;
}

// The holders and sub pools of the article pools (soKindInfoGeneric<0, 21>); the mediator destroys them out of line.
#pragma dont_inline on
template wnInstanceHolder<wnWarioGarlic, soKindInfoGeneric<0, 21>, soIntToType<1> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnWarioGarlic, 1, wnInstanceHolder, soIntToType<1> >, soKindInfoGeneric<0, 21> >;
template class soInstancePool<soInstancePoolInfo<wnWarioGarlic, 1, wnInstanceHolder, soIntToType<1> >, soInstancePoolRoot, soKindInfoGeneric<0, 21>, 1>;
#pragma dont_inline off
