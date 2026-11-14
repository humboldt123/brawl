#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_TEAM_TYPED_INTERFACE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_KIRBY_ARTICLE_POOLS
#define MT_VEC3F_SCALE_FRESH_REGS
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_manager.h>
#include <ft/ft_reflector_group.h>
#include <ft/ft_class_info_impl.h>
#include <ft/kirby/ft_kirby.h>
#include <ft/kirby/ft_kirby_extend_param_accesser.h>
#include <mt/mt_prng.h>
#include <so/so_slow.h>
#include <gf/gf_task_scheduler.h>
#include <wn/weapon.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>
#include <st/stage.h>
#include <so/catch/so_catch_module.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <ft/ft_outside_event_presenter.h>
#include <so/damage/so_damage.h>
#include <ft/ft_owner.h>
#include <wn/wn_article_pool.h>
#include <wn/wn_simple.h>

#define FT_NOINLINE_SEARCH_BUILDER
// The fighter owns the shared out-of-line kinetic teardown. The keeper and the abstract work destructor are not in this object.
#define FT_WEAK_KEPT_OBSERVERS
#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftKirbyBuildConfig
#include <ft/builder/ft_builder_noinline.h>
#include <ft/builder/ft_builder_article.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

ftKirbyExtendParamAccesser g_ftKirbyExtendParamAccesser;

ftClassInfoImpl<Fighter_Kirby, ftKirby> g_ftClassInfoKirby;

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt); the copy abilities borrow the processes of other fighters.
extern "C" {
extern soStatusUniqProcess lbl_27_bss_73BC;
extern soStatusUniqProcess lbl_27_bss_73CC;
extern soStatusUniqProcess lbl_27_bss_75C4;
extern soStatusUniqProcess lbl_27_bss_75D4;
extern soStatusUniqProcess lbl_27_bss_75E4;
extern soStatusUniqProcess lbl_27_bss_75F4;
extern soStatusUniqProcess lbl_27_bss_7604;
extern soStatusUniqProcess lbl_27_bss_7704;
extern soStatusUniqProcess lbl_27_bss_7C64;
extern soStatusUniqProcess lbl_27_bss_7CDC;
extern soStatusUniqProcess lbl_27_bss_7CEC;
extern soStatusUniqProcess lbl_27_bss_7CFC;
extern soStatusUniqProcess lbl_27_bss_7D74;
extern soStatusUniqProcess lbl_27_bss_7DD4;
extern soStatusUniqProcess lbl_27_bss_7E14;
extern soStatusUniqProcess lbl_27_bss_7E24;
extern soStatusUniqProcess lbl_27_bss_8004;
extern soStatusUniqProcess lbl_27_bss_8014;
extern soStatusUniqProcess lbl_27_bss_8034;
extern soStatusUniqProcess lbl_27_bss_8044;
extern soStatusUniqProcess lbl_27_bss_8054;
extern soStatusUniqProcess lbl_27_bss_8064;
extern soStatusUniqProcess lbl_27_bss_8074;
extern soStatusUniqProcess lbl_27_bss_80A4;
extern soStatusUniqProcess lbl_27_bss_826C;
extern soStatusUniqProcess lbl_27_bss_82D4;
extern soStatusUniqProcess lbl_27_bss_83CC;
extern soStatusUniqProcess lbl_27_bss_88CC;
extern soStatusUniqProcess lbl_27_bss_88EC;
extern soStatusUniqProcess lbl_27_bss_88FC;
extern soStatusUniqProcess lbl_27_bss_890C;
extern soStatusUniqProcess lbl_27_bss_891C;
extern soStatusUniqProcess lbl_27_bss_897C;
extern soStatusUniqProcess lbl_27_bss_8A14;
extern soStatusUniqProcess lbl_27_bss_8A24;
extern soStatusUniqProcess lbl_27_bss_8A34;
extern soStatusUniqProcess lbl_27_bss_8A44;
extern soStatusUniqProcess lbl_27_bss_8A9C;
extern soStatusUniqProcess lbl_27_bss_8BEC;
extern soStatusUniqProcess lbl_96_bss_C0C;
extern soStatusUniqProcess lbl_96_bss_C1C;
extern soStatusUniqProcess lbl_96_bss_C2C;
extern soStatusUniqProcess lbl_96_bss_C3C;
extern soStatusUniqProcess lbl_96_bss_C4C;
extern soStatusUniqProcess lbl_96_bss_C5C;
extern soStatusUniqProcess lbl_96_bss_C6C;
extern soStatusUniqProcess lbl_96_bss_C7C;
extern soStatusUniqProcess lbl_96_bss_C9C;
extern soStatusUniqProcess lbl_96_bss_CBC;
extern soStatusUniqProcess lbl_96_bss_CCC;
extern soStatusUniqProcess lbl_96_bss_CDC;
extern soStatusUniqProcess lbl_96_bss_CEC;
extern soStatusUniqProcess lbl_96_bss_CFC;
extern soStatusUniqProcess lbl_96_bss_D1C;
}
extern soStatusUniqProcess g_ftMarthStatusUniqProcessSpecialNEnd;
extern soStatusUniqProcess g_ftMarthStatusUniqProcessSpecialNLoop;
extern soStatusUniqProcess g_ftMarthStatusUniqProcessSpecialNStart;
extern soStatusUniqProcess g_ftPurinStatusUniqProcessSpecialNEnd;
extern soStatusUniqProcess g_ftPurinStatusUniqProcessSpecialNHitEnd;
extern soStatusUniqProcess g_ftPurinStatusUniqProcessSpecialNHold;
extern soStatusUniqProcess g_ftPurinStatusUniqProcessSpecialNHoldMax;
extern soStatusUniqProcess g_ftPurinStatusUniqProcessSpecialNRoll;
extern soStatusUniqProcess g_ftPurinStatusUniqProcessSpecialNRollAir;
extern soStatusUniqProcess g_ftPurinStatusUniqProcessSpecialNStart;
extern soStatusUniqProcess g_ftPurinStatusUniqProcessSpecialNTurn;
extern soStatusUniqProcess g_ftRobotStatusUniqProcessSpecialBeam;

// The copy ability module's constructor and its prepareCopyAbility (ftKirbyCopyAbilityModule), defined in this REL.
extern "C" void fn_96_1C244(ftKirbyCopyAbilityModule* module, soModuleAccesser* moduleAccesser);

ftKirby::ftKirby(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftKirbyBuildConfig>(entryId,
                                         Fighter_Kirby,
                                         instHeap,
                                         instHeap,
                                         nwModelInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Kirby)),
    unk1B68C(0),
    unk1B68D(0) {
    fn_96_1C244(&m_copyAbility, m_moduleAccesser);
    m_nwModelHeap = nwModelInstHeap;

    // Kirby owns 174 character action slots: the ones for the copy abilities borrow the processes of the other fighters.
    soStatusUniqProcess* processes[174] = {0};
    processes[0] = &lbl_96_bss_C9C;
    processes[1] = &lbl_96_bss_C4C;
    processes[2] = &lbl_96_bss_C0C;
    processes[3] = &lbl_96_bss_C5C;
    processes[4] = &lbl_96_bss_D1C;
    processes[5] = &lbl_96_bss_C9C;
    processes[7] = &lbl_96_bss_CEC;
    processes[8] = &lbl_96_bss_CEC;
    processes[9] = &lbl_96_bss_CDC;
    processes[10] = &lbl_96_bss_CDC;
    processes[11] = &lbl_96_bss_CDC;
    processes[12] = &lbl_96_bss_CDC;
    processes[13] = &lbl_96_bss_CDC;
    processes[14] = &lbl_96_bss_CDC;
    processes[15] = &lbl_96_bss_CDC;
    processes[16] = &lbl_96_bss_CDC;
    processes[17] = &lbl_96_bss_CDC;
    processes[18] = &lbl_96_bss_CDC;
    processes[19] = &lbl_96_bss_CDC;
    processes[20] = &lbl_96_bss_CBC;
    processes[21] = &lbl_96_bss_CFC;
    processes[22] = &lbl_96_bss_CCC;
    processes[23] = &lbl_96_bss_C1C;
    processes[24] = &lbl_96_bss_C2C;
    processes[25] = &lbl_96_bss_C3C;
    processes[26] = &lbl_96_bss_C6C;
    processes[27] = &lbl_96_bss_C7C;
    processes[28] = &lbl_96_bss_D1C;
    processes[29] = &lbl_96_bss_D1C;
    processes[30] = &lbl_96_bss_D1C;
    processes[31] = &lbl_96_bss_D1C;
    processes[32] = &lbl_96_bss_D1C;
    processes[33] = &lbl_27_bss_7CDC;
    processes[34] = &lbl_27_bss_7CEC;
    processes[35] = &lbl_27_bss_7C64;
    processes[37] = &lbl_27_bss_80A4;
    processes[50] = &lbl_27_bss_82D4;
    processes[54] = &lbl_27_bss_83CC;
    processes[55] = &lbl_27_bss_83CC;
    processes[56] = &lbl_27_bss_83CC;
    processes[57] = &lbl_27_bss_8034;
    processes[58] = &lbl_27_bss_8034;
    processes[60] = &lbl_27_bss_8044;
    processes[61] = &lbl_27_bss_8044;
    processes[62] = &lbl_27_bss_8054;
    processes[63] = &lbl_27_bss_8074;
    processes[64] = &lbl_27_bss_8064;
    processes[65] = &lbl_27_bss_8064;
    processes[66] = &lbl_27_bss_8064;
    processes[67] = &lbl_27_bss_8064;
    processes[72] = &lbl_27_bss_7704;
    processes[73] = &lbl_27_bss_7704;
    processes[75] = &lbl_27_bss_8BEC;
    processes[76] = &lbl_27_bss_8BEC;
    processes[77] = &lbl_27_bss_8BEC;
    processes[78] = &lbl_27_bss_8BEC;
    processes[79] = &lbl_27_bss_8BEC;
    processes[80] = &lbl_27_bss_8BEC;
    processes[81] = &lbl_27_bss_8BEC;
    processes[82] = &lbl_27_bss_8BEC;
    processes[83] = &lbl_27_bss_8BEC;
    processes[84] = &lbl_27_bss_8BEC;
    processes[85] = &lbl_27_bss_8BEC;
    processes[88] = &lbl_27_bss_7DD4;
    processes[89] = &lbl_27_bss_7E14;
    processes[90] = &lbl_27_bss_7E14;
    processes[91] = &lbl_27_bss_7E24;
    processes[92] = &lbl_27_bss_7E14;
    processes[93] = &g_soStatusUniqProcessNull;
    processes[94] = &g_soStatusUniqProcessNull;
    processes[95] = &g_soStatusUniqProcessNull;
    processes[96] = &g_soStatusUniqProcessNull;
    processes[97] = &lbl_27_bss_7CFC;
    processes[98] = &lbl_27_bss_7CFC;
    processes[99] = &lbl_27_bss_7CFC;
    processes[100] = &lbl_27_bss_7CFC;
    processes[104] = &lbl_27_bss_73BC;
    processes[106] = &lbl_27_bss_8A14;
    processes[107] = &lbl_27_bss_8A24;
    processes[108] = &lbl_27_bss_8A34;
    processes[109] = &lbl_27_bss_8A44;
    processes[110] = &g_ftMarthStatusUniqProcessSpecialNStart;
    processes[111] = &g_ftMarthStatusUniqProcessSpecialNLoop;
    processes[112] = &g_ftMarthStatusUniqProcessSpecialNEnd;
    processes[114] = &g_ftPurinStatusUniqProcessSpecialNStart;
    processes[115] = &g_ftPurinStatusUniqProcessSpecialNHold;
    processes[116] = &g_ftPurinStatusUniqProcessSpecialNHoldMax;
    processes[117] = &g_ftPurinStatusUniqProcessSpecialNRoll;
    processes[118] = &g_ftPurinStatusUniqProcessSpecialNRollAir;
    processes[119] = &g_ftPurinStatusUniqProcessSpecialNTurn;
    processes[120] = &g_ftPurinStatusUniqProcessSpecialNEnd;
    processes[121] = &g_ftPurinStatusUniqProcessSpecialNHitEnd;
    processes[122] = &lbl_27_bss_826C;
    processes[123] = &lbl_27_bss_826C;
    processes[124] = &lbl_27_bss_826C;
    processes[125] = &lbl_27_bss_826C;
    processes[126] = &lbl_27_bss_826C;
    processes[127] = &g_ftRobotStatusUniqProcessSpecialBeam;
    processes[128] = &lbl_27_bss_88CC;
    processes[129] = &lbl_27_bss_88CC;
    processes[131] = &lbl_27_bss_891C;
    processes[132] = &lbl_27_bss_891C;
    processes[133] = &lbl_27_bss_890C;
    processes[134] = &lbl_27_bss_890C;
    processes[135] = &lbl_27_bss_890C;
    processes[136] = &lbl_27_bss_890C;
    processes[137] = &lbl_27_bss_890C;
    processes[138] = &lbl_27_bss_890C;
    processes[139] = &lbl_27_bss_890C;
    processes[140] = &lbl_27_bss_890C;
    processes[141] = &lbl_27_bss_890C;
    processes[142] = &lbl_27_bss_890C;
    processes[143] = &lbl_27_bss_890C;
    processes[144] = &lbl_27_bss_890C;
    processes[145] = &lbl_27_bss_88EC;
    processes[146] = &lbl_27_bss_88FC;
    processes[148] = &lbl_27_bss_8004;
    processes[149] = &lbl_27_bss_8014;
    processes[150] = &lbl_27_bss_73CC;
    processes[151] = &lbl_27_bss_8A9C;
    processes[152] = &lbl_27_bss_897C;
    processes[153] = &lbl_27_bss_897C;
    processes[154] = &lbl_27_bss_897C;
    processes[155] = &lbl_27_bss_897C;
    processes[157] = &lbl_27_bss_75C4;
    processes[158] = &lbl_27_bss_75E4;
    processes[160] = &lbl_27_bss_75F4;
    processes[161] = &lbl_27_bss_75D4;
    processes[162] = &lbl_27_bss_7604;
    processes[163] = &lbl_27_bss_7D74;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 174);

    // Prepare the resources of every copy ability that is loaded for this entry.
    int copyCount = g_ftManager->getKirbyCopyResourceCount(m_entryId);
    for (int i = 0; i < copyCount; i++) {
        m_copyAbility.prepareCopyAbility(g_ftManager->getKirbyCopyResourceKind(m_entryId, i));
    }

    // HYPOTHESIS: the common data record holds the shield group at +0x15c (Stone / Hammer shields), group 1, the reflector
    // record at +0x16c (group 2) and the area definitions at +0x17c.
    m_moduleAccesser->getCollisionShieldModule().add(reinterpret_cast<soCollisionShieldGroupData*>(reinterpret_cast<u8*>(m_data) + 0x15c), 1);
    u8 shape[0x20];
    ftReflectorGroupDraft group;
    FT_REFLECTOR_GROUP_CLEAR(group);
    ftUtil::convertReflectorData(*reinterpret_cast<void**>(reinterpret_cast<u8*>(m_data) + 0x16c), shape, &group);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&group), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    m_moduleAccesser->getAreaModule().addArea(reinterpret_cast<soSet<soAreaData>*>(reinterpret_cast<u8*>(m_data) + 0x17c));
    m_moduleAccesser->getAreaModule().enableArea(9, false, -1);
    m_moduleAccesser->getAreaModule().enableArea(10, false, -1);
    m_moduleAccesser->getAreaModule().enableArea(11, false, -1);
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftKirby is done.
// The original object does not define this helper.
__declspec(weak) void testBuilder() {
    soInsideEventManageModuleBuilder<ftKirbyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

// These names are not in the original object. Kept weak so they do not become a second strong definition.
__declspec(weak) u8 fn_96_28D0(u8* p) { return *(u8*)(p + 0x4); }
__declspec(weak) int fn_96_BC6C(u8* p) { return *(int*)(p + 0x28); }
int fn_96_C17C(u8* p) { return *(int*)(p + 0xc0); }
int fn_96_115E8() { return 12; }
void fn_96_12530() {}
__declspec(weak) void fn_96_12C28() {}
__declspec(weak) void fn_96_12CAC() {}
__declspec(weak) void fn_96_12FA4() {}
__declspec(weak) void fn_96_1308C() {}
__declspec(weak) void fn_96_13174() {}
__declspec(weak) void fn_96_1325C() {}

#pragma dont_inline on

// Kirby overrides still undefined here (defined later in this unit).

// Event thunks: a second-base adjustment (this - N) followed by a tail call to the Kirby override.
// (fn_96_1359C, the isObserv thunk, takes no second argument in the target and is left out.)

// Second-base forwarders to the out-of-unit Fighter overrides (MATCH-ONLY: same adjustment and tail call).
#pragma dont_inline off

}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
u8* fn_96_12CB0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_96_12D60(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_96_12E10(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_96_12EC0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_96_12FA8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"


// ---- fighter overrides --------------------------------------------------------------------------------------------------
// HYPOTHESIS: work flags 0x12000042/0x12000043 mark the copy ability being active; variables 0x10000047 (stone shape) and
// 0x10000048/0x10000049 (copy ability slots) are Kirby work.
void ftKirby::onDeactivate() {
    if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000042)) {
        copyCancel(m_moduleAccesser, false);
    }
}

void* ftKirby::getExtendParam() { return reinterpret_cast<u8*>(m_data) + 0x7C; }

void ftKirby::onStart(int param) {
    Fighter::onStart(param);
    soModuleAccesser* acc = m_moduleAccesser;
    soWorkManageModule& work = acc->getWorkManageModule();
    work.offFlag(0x12000042);
    work.offFlag(0x12000043);
    work.setInt(-1, 0x10000048);
    work.setInt(-1, 0x10000049);
    acc->getEffectModule().removeCommon(0x1a);
    int entryId = m_entryId;
    ftManager* manager = g_ftManager;
    bool spycloak = getOwner()->getSpycloak();
    m_copyAbility.changeCopyAbility(-1, manager->getPlayerNo(entryId), spycloak, 0);
}

void ftKirby::processFixPosition() {
    m_copyAbility.processFixPosition();
    Fighter::processFixPosition();
    updateShineEye();
}

void ftKirby::decisionStoneMetamorphosis() {
    soWorkManageModule* work = &m_moduleAccesser->getWorkManageModule();
    int cur = work->getInt(0x10000047);
    cur = (cur + randi(3) + 1) % 5;
    work->setInt(cur, 0x10000047);
}

// HYPOTHESIS: the stone work block starts two bytes into the unknown block.
u8* ftKirby::getWorkStone() { return &unk1B68E[2]; }

// Entering the Kirby-only statuses (the Final Cutter / Stone range 0x12e-0x132) tells the linked articles; the copy ability
// ends when the matching statuses start.
void ftKirby::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    m_copyAbility.notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    switch (statusKind) {
    case 0x10c:
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000042)) {
            copyCancel(moduleAccesser, true);
        }
        break;
    case 0xbd:
        copyCancel(moduleAccesser, false);
        break;
    case 0x12e: case 0x12f: case 0x130: case 0x131: case 0x132: {
        ftKirbyLinkEvent event(0x83a);
        moduleAccesser->getLinkModule().sendEventNodes(1, event, 0);
        break;
    }
    case 0xd:
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000042)) {
            int copyKind = moduleAccesser->getWorkManageModule().getInt(0x10000048);
            if (copyKind != 0xf && copyKind != 0x10 && copyKind != 0x16 && copyKind != 0xe && copyKind != 3 &&
                copyKind != 9 && copyKind != 0x15 && copyKind != 0x31 && copyKind != 0x14) {
                moduleAccesser->getVisibilityModule().set(1, 1);
            }
        } else {
            moduleAccesser->getVisibilityModule().set(1, 1);
        }
        break;
    }
}

void ftKirby::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x132:
        if (kind == 2) {
            ftKirbyLinkEvent event(0x839);
            moduleAccesser->getLinkModule().sendEventNodes(1, event, 0);
        } else {
            ftKirbyLinkEvent event(0x838);
            moduleAccesser->getLinkModule().sendEventNodes(1, event, 0);
        }
        break;
    }
    m_copyAbility.notifyEventChangeSituation(kind, prevKind);
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}

void ftKirby::processUpdate() {
    Fighter::processUpdate();
    m_copyAbility.processUpdate();
    unk1B68D = 0;
    unk1B68C = 0;
    if (soSlow::getInstance()->isEstimate() == true && !m_moduleAccesser->getStopModule().isStop()) {
        if (m_moduleAccesser->getWorkManageModule().getInt(0x1000004c) > 0) {
            m_moduleAccesser->getWorkManageModule().decInt(0x1000004c);
        }
    }
}

void ftKirby::processCollision() {
    m_copyAbility.processCollision();
    StageObject::processCollision();
}

bool ftKirby::isObserv(char kind) { return kind == 12; }

void ftKirby::notifyArticleEventEject(int unk1, int unk2, int* unk3) {
    m_copyAbility.notifyArticleEventEject();
    StageObject::notifyArticleEventEject(unk1, unk2, unk3);
}

void ftKirby::notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    m_copyAbility.notifyEventCollisionAttackFighter(collisionLog, moduleAccesser);
}

bool ftKirby::notifyEventCollisionAttackCheck(u32 flags) {
    if (m_copyAbility.notifyEventCollisionAttackCheck() == true) {
        return true;
    }
    return Fighter::notifyEventCollisionAttackCheck(flags);
}

void ftKirby::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (collisionLog != NULL) {
        m_copyAbility.notifyEventCollisionShield();
    }
    Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftKirby::notifyEventCollisionShieldCheck() {
    if (m_copyAbility.notifyEventCollisionShieldCheck() == true) {
        return true;
    }
    return Fighter::notifyEventCollisionShieldCheck();
}

// HYPOTHESIS: the reflector group 2 is the Stone / Hammer reflect; hits at or above its attack limit set a flag that the
// check below uses to keep the reflector open (unk1B68D) or to deflect (unk1B68C).
void ftKirby::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (groupIndex == 2) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk1B68D = 1;
            return;
        }
        unk1B68C = 1;
    }
    Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftKirby::notifyEventCollisionReflectorCheck() {
    if (unk1B68C == 1) {
        return true;
    }
    if (unk1B68D == 1) {
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}

void ftKirby::notifyEventOnDamage(soDamage* damage, bool reaction, soModuleAccesser* moduleAccesser) {
    m_copyAbility.notifyEventOnDamage();
    if (!moduleAccesser->getLinkModule().isLink(0) &&
        (*reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(&damage->m_attackData) + 0x30) & 0x1f) != soCollisionAttackData::Attribute_Turn) {
        if (reaction) {
            copyCancelDamage(moduleAccesser);
        } else {
            copyCancelDamageNoReaction(moduleAccesser);
        }
    }
    Fighter::notifyEventOnDamage(damage, reaction, moduleAccesser);
}

// HYPOTHESIS: work 0x2200001a marks a Weapon inhaled by the Inhale (Special N) states 0x112 / 0x117; variable 0x20000005
// holds how it was swallowed (0 / 2) and 0x20000004 the weapon's task id. In the Final Smash hit state (0x116) the hit
// fighters are counted in 0x20000006 and tell the target it was inhaled.
void ftKirby::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x112 || moduleAccesser->getStatusModule().getStatusKind() == 0x117) {
        if (!moduleAccesser->getWorkManageModule().isFlag(0x2200001a)) {
            gfTask* task = gfTaskScheduler::getInstance()->getTask(collisionLog->m_taskId);
            if (task != NULL && task->m_taskCategory == gfTask::Category_Weapon) {
                Weapon* weapon = dynamic_cast<Weapon*>(task);
                moduleAccesser->getWorkManageModule().setInt(0, 0x20000005);
                if (weapon->isCanEat()) {
                    switch (weapon->getSubKind()) {
                    case 0x37:
                        moduleAccesser->getWorkManageModule().setInt(2, 0x20000005);
                    case 0x33:
                        moduleAccesser->getWorkManageModule().onFlag(0x2200001a);
                        moduleAccesser->getWorkManageModule().setInt(collisionLog->m_taskId, 0x20000004);
                        break;
                    }
                }
            }
        }
    } else if (moduleAccesser->getStatusModule().getStatusKind() == 0x116 && collisionLog->m_kind == 1) {
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(static_cast<gfTask::Category>(collisionLog->m_taskCategory), collisionLog->m_taskId);
        if (task->m_taskCategory == gfTask::Category_Fighter) {
            soExternalValueAccesser::getTeamNo(&dynamic_cast<StageObject&>(*task));
            Fighter* target = dynamic_cast<Fighter*>(task);
            if (target->getFtKind() != 0x1c) {
                target->onInhaled();
                int count = moduleAccesser->getWorkManageModule().getInt(0x20000006);
                moduleAccesser->getWorkManageModule().setInt(collisionLog->m_taskId, count + 0x20000000);
                moduleAccesser->getWorkManageModule().addInt(1, 0x20000006);
            }
        }
    }
    Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser);
    if (collisionLog != NULL) {
        m_copyAbility.notifyEventCollisionSearch(collisionLog, moduleAccesser);
    }
}

bool ftKirby::notifyEventCollisionSearchCheck() {
    soModuleAccesser* acc = m_moduleAccesser;
    if ((acc->getStatusModule().getStatusKind() == 0x112 || acc->getStatusModule().getStatusKind() == 0x117) &&
        acc->getWorkManageModule().isFlag(0x2200001a)) {
        int taskId = acc->getWorkManageModule().getInt(0x20000004);
        acc->getLinkModule().link(7, taskId);
        ftKirbyLinkEvent event(0x835);
        acc->getLinkModule().sendEventParents(7, event);
        if (acc->getLinkModule().isLink(7) == true) {
            acc->getLinkModule().unlink(7);
        }
        acc->getWorkManageModule().onFlag(0x2200001c);
        acc->getStatusModule().changeStatusRequest(0x126, acc);
        acc->getWorkManageModule().offFlag(0x2200001a);
        return true;
    }
    if (m_copyAbility.notifyEventCollisionSearchCheck() == true) {
        return true;
    }
    return Fighter::notifyEventCollisionSearchCheck();
}

// ---- the copy ability -------------------------------------------------------------------------------------------------
// HYPOTHESIS: work 0x10000048 is the copy ability kind (-1 for none), 0x10000049 the slot (player) it was taken from and the
// flag 0x12000042 is set while Kirby wears a copy ability (0x1200001f: the kind 0x20, the Stone ability).
void ftKirby::copySetup(soModuleAccesser* moduleAccesser, int slotNo, int copyKind, bool effect) {
    if (copyKind != 5 && (copyKind < 0x32 || copyKind > 0x35) && g_ftManager->isReadyKirbyCopyResource(m_entryId, copyKind) == true) {
        moduleAccesser->getWorkManageModule().setInt(copyKind, 0x10000048);
        moduleAccesser->getWorkManageModule().onFlag(0x12000042);
        moduleAccesser->getWorkManageModule().setInt(slotNo, 0x10000049);
        if (copyKind == 0x20) {
            moduleAccesser->getWorkManageModule().onFlag(0x1200001f);
        } else {
            moduleAccesser->getWorkManageModule().offFlag(0x1200001f);
        }
        int entryId = m_entryId;
        ftManager* manager = g_ftManager;
        m_copyAbility.changeCopyAbility(copyKind, manager->getPlayerNo(entryId), getOwner()->getSpycloak(), 0);
        m_outsideEventPresenter.notifyEventKirbyCopySetup(copyKind);
        if (effect) {
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0xc2e), false, false, 0);
        }
    } else if (effect) {
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0xc33), false, false, 0);
    }
}

void ftKirby::copyCancel(soModuleAccesser* moduleAccesser, bool effect) {
    int copyKind = moduleAccesser->getWorkManageModule().getInt(0x10000048);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000048);
    moduleAccesser->getWorkManageModule().offFlag(0x12000042);
    moduleAccesser->getWorkManageModule().offFlag(0x1200001f);
    int entryId = m_entryId;
    ftManager* manager = g_ftManager;
    m_copyAbility.changeCopyAbility(-1, manager->getPlayerNo(entryId), getOwner()->getSpycloak(), isCopyStatus());
    if (effect) {
        moduleAccesser->getWorkManageModule().setInt(2, 0x1000004b);
        soGenerateArticleManageModule* generator = &moduleAccesser->getGenerateArticleManageModule();
        soArticle* article = generator->generate(2, NULL, false);
        if (article->isNull() == false) {
            generator->entry(article);
        }
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0xc2f), false, false, 0);
    }
    m_outsideEventPresenter.notifyEventKirbyCopyCancel(copyKind);
    moduleAccesser->getEffectModule().removeCommon(0x1a);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000049);
}

// HYPOTHESIS: the copy ability can end when Kirby is hit; a (flinching or not) hit has a 1-in-N chance (parameter 0x5dc1).
void ftKirby::copyCancelDamage(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000042) && !moduleAccesser->getWorkManageModule().isFlag(0x12000043)) {
        if (randi(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0)) == 0) {
            copyCancelTransitionStatus();
        }
    }
}

void ftKirby::copyCancelDamageNoReaction(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000042) && !moduleAccesser->getWorkManageModule().isFlag(0x12000043)) {
        if (randi(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0)) == 0) {
            copyCancelTransitionStatus();
        }
    }
}

// Ending the copy ability while wearing it from a copy-only status returns to the idle (air: fall) status first.
void ftKirby::copyCancelTransitionStatus() {
    if (isCopyStatus()) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x12000042);
        if (m_moduleAccesser->getSituationModule().getKind() == 2) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0xe, m_moduleAccesser);
        } else {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0, m_moduleAccesser);
        }
    }
    copyCancel(m_moduleAccesser, true);
}

void ftKirby::notifyEventKirbyResourceLoaded(int index) {
    m_copyAbility.prepareCopyAbility(index);
}

void ftKirby::notifyEventKirbyResourceUnLoaded(int index) {
    if (index == m_moduleAccesser->getWorkManageModule().getInt(0x10000048)) {
        copyCancelTransitionStatus();
    }
    m_copyAbility.disposeCopyAbility(index);
}

void ftKirby::notifyEventExitFighter(int slotNo, int copyKind) {
    if (slotNo == m_moduleAccesser->getWorkManageModule().getInt(0x10000049) &&
        copyKind == m_moduleAccesser->getWorkManageModule().getInt(0x10000048)) {
        copyCancelTransitionStatus();
    }
}

void ftKirby::updateShineEye() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getVisibilityModule().get(0) == 0 || acc->getVisibilityModule().get(0) == 1) {
        if (acc->getVisibilityModule().get(1) == 1) {
            acc->getVisibilityModule().setStatusDefault(0, 1, true);
        } else {
            acc->getVisibilityModule().setStatusDefault(0, 0, true);
        }
    }
}

int ftKirby::getCopyKind() {
    if (!m_moduleAccesser->getWorkManageModule().isFlag(0x12000042)) {
        return -1;
    }
    return m_moduleAccesser->getWorkManageModule().getInt(0x10000048);
}

int ftKirby::getCopySlotNo() {
    if (!m_moduleAccesser->getWorkManageModule().isFlag(0x12000042)) {
        return -1;
    }
    return m_moduleAccesser->getWorkManageModule().getInt(0x10000049);
}

// The copy-only statuses are 0x133-0x1bf.
bool ftKirby::isCopyStatus() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() >= 0x133 && m_moduleAccesser->getStatusModule().getStatusKind() <= 0x1bf) {
        return true;
    } else {
        return false;
    }
}

// HYPOTHESIS: the fighter data holds a table of up to 0x35 (stage kind, limit) pairs; the check passes when the current stage
// has an entry whose limit is at least the value of the query.
bool ftKirby::checkKirbyFinalRayCheck(ftKirbyFinalRayQuery* query) {
    ftKirbyFinalRayEntry* entries;
    u32 count = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(m_data) + 0x188);
    if (count > 0x35) {
        return false;
    }
    entries = *reinterpret_cast<ftKirbyFinalRayEntry**>(reinterpret_cast<u8*>(m_data) + 0x184);
    if (entries == NULL) {
        return false;
    }
    for (u32 i = 0; i < count; i++) {
        ftKirbyFinalRayEntry entry = entries[i];
        if (entry.stageKind < 0 || entry.stageKind > 0x35) {
            return false;
        }
        if (entry.stageKind == g_Stage->m_stageKind && query->limit <= entry.limit) {
            return true;
        }
    }
    return false;
}

// ---- link events --------------------------------------------------------------------------------------------------------
// HYPOTHESIS: the catch module (the target is held by Kirby) has more virtuals than the shared header declares; the call at
// 0x40 is made through this extension.
class ftKirbyCatchModuleExt : public soCatchModule {
public:
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3c();
    virtual void unk40(int);
};

static inline ftKirbyCatchModuleExt& getKirbyCatchModule(soModuleAccesser* moduleAccesser) {
    return *static_cast<ftKirbyCatchModuleExt*>(moduleAccesser->m_enumerationStart->m_catchModule);
}

// Events from the articles that Kirby inhales or copies, from the Stone / Final Cutter articles and from the thrown fighters.
// HYPOTHESIS: statuses 0x112/0x117 are the inhale, 0x119/0x11a hold, 0x14b/0x14c and 0x192/0x193 the copy ability cases.
void ftKirby::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) {
    soStatusModule& status = moduleAccesser->getStatusModule();
    ftKirbyLinkEventIn* event = static_cast<ftKirbyLinkEventIn*>(eventInfo);
    switch (eventInfo->m_eventKind) {
    case 8: {
        int current = status.getStatusKind();
        if (current == 0x112 || current == 0x117) {
            float scl = soExternalValueAccesser::getScl(stageObject);
            if (scl <= moduleAccesser->getPostureModule().getScale()) {
                status.changeStatusRequest(0x119, m_moduleAccesser);
                event->m_result = m_moduleAccesser->getModelModule().getCorrectNodeId(300);
                event->_0x4 = true;
            }
            return;
        }
        if (current == 0x14b || current == 0x14c) {
            float scl = soExternalValueAccesser::getScl(stageObject);
            if (scl <= moduleAccesser->getPostureModule().getScale()) {
                status.changeStatusRequest(0x14e, moduleAccesser);
                event->m_result = moduleAccesser->getModelModule().getCorrectNodeId(300);
                event->_0x4 = true;
            }
            return;
        }
        if (current == 0x159) {
            event->m_result = moduleAccesser->getModelModule().getCorrectNodeId(300);
            event->_0x4 = true;
            getKirbyCatchModule(moduleAccesser).setCatch(-1);
            status.changeStatusRequest(0x15a, moduleAccesser);
            return;
        }
        if (status.getStatusKind() == 0x192 || status.getStatusKind() == 0x193) {
            float scl = soExternalValueAccesser::getScl(stageObject);
            if (scl <= moduleAccesser->getPostureModule().getScale()) {
                status.changeStatusRequest(0x195, m_moduleAccesser);
                event->m_result = m_moduleAccesser->getModelModule().getCorrectNodeId(300);
                event->_0x4 = true;
            }
            return;
        }
        break;
    }
    case 0xf:
        status.changeStatusRequest(0x150, moduleAccesser);
        return;
    case 0x14:
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000042)) {
            if (m_moduleAccesser->getWorkManageModule().getInt(0x10000048) == 0x20) {
                status.changeStatusRequest(0x196, m_moduleAccesser);
            }
        } else {
            status.changeStatusRequest(0x11a, m_moduleAccesser);
        }
        break;
    case 9:
        if (status.getStatusKind() < 0x11b || status.getStatusKind() > 0x125) {
            if (status.getStatusKind() >= 0x197 && status.getStatusKind() <= 0x1a2) {
                getKirbyCatchModule(m_moduleAccesser).unk40(0);
                getKirbyCatchModule(m_moduleAccesser).catchCut(false, true);
                if (moduleAccesser->getSituationModule().getKind() == 2) {
                    status.changeStatusRequest(0xe, m_moduleAccesser);
                } else {
                    status.changeStatusRequest(0, m_moduleAccesser);
                }
                return;
            }
            if (status.getStatusKind() != 0x119 && status.getStatusKind() != 0x11a && status.getStatusKind() != 0x195 &&
                status.getStatusKind() != 0x196 &&
                (status.getStatusKind() == 0x128 || status.getStatusKind() == 0x127 || status.getStatusKind() == 0x1a4)) {
                if (moduleAccesser->getWorkManageModule().isFlag(0x22000016)) {
                    getKirbyCatchModule(moduleAccesser).unk40(0);
                    getKirbyCatchModule(moduleAccesser).catchCut(false, false);
                } else {
                    getKirbyCatchModule(m_moduleAccesser).unk40(0);
                    getKirbyCatchModule(m_moduleAccesser).catchCut(false, true);
                    if (moduleAccesser->getSituationModule().getKind() == 2) {
                        status.changeStatusRequest(0xe, m_moduleAccesser);
                    } else {
                        status.changeStatusRequest(0, m_moduleAccesser);
                    }
                }
                return;
            }
        }
        break;
    case 0x18:
        moduleAccesser->getWorkManageModule().setFloat(event->m_x, 0x21000006);
        moduleAccesser->getWorkManageModule().setFloat(event->m_y, 0x21000007);
        moduleAccesser->getWorkManageModule().onFlag(0x22000017);
        break;
    case 0x2f:
        moduleAccesser->getWorkManageModule().addInt(1, 0x20000008);
        moduleAccesser->getWorkManageModule().addInt(1, 0x20000020);
        moduleAccesser->getWorkManageModule().addInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc9, 0), 0x2000000a);
        {
            soLinkModule& link = moduleAccesser->getLinkModule();
            soEffectModule& effect = moduleAccesser->getEffectModule();
            effect.req(static_cast<EfID>(0xe60008), &link.getParentModelNodeGlobalPosition(6, u32(0), false), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
        }
        switch (soValueAccesser::getConstantInt(moduleAccesser, 0x59e8, 0)) {
        case 0:
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0xba), false, false, 0);
            break;
        case 1:
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0xbb), false, false, 0);
            break;
        case 2:
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0xbc), false, false, 0);
            break;
        }
        moduleAccesser->getControllerModule().setRumble(5, 10, false, -1);
        break;
    case 0x30:
        moduleAccesser->getWorkManageModule().addInt(1, 0x20000009);
        moduleAccesser->getWorkManageModule().addInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dcb, 0), 0x2000000a);
        break;
    case 0x455:
        if (status.getStatusKind() == 0x116) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000018);
        }
        break;
    case 0x456:
        if (status.getStatusKind() == 0x116 || status.getStatusKind() == 0x12e || status.getStatusKind() == 0x12f ||
            status.getStatusKind() == 0x130 || status.getStatusKind() == 0x131 || status.getStatusKind() == 0x132) {
            if (moduleAccesser->getSituationModule().getKind() == 2) {
                status.changeStatusRequest(0xe, m_moduleAccesser);
            } else {
                status.changeStatusRequest(0, m_moduleAccesser);
            }
            return;
        }
    case 0x33: {
        int count = moduleAccesser->getWorkManageModule().getInt(0x20000006);
        for (int i = 0; i < count; i++) {
            if (event->m_result == moduleAccesser->getWorkManageModule().getInt(i + 0x20000000)) {
                moduleAccesser->getWorkManageModule().setInt(-1, i + 0x20000000);
            }
        }
        if (status.getStatusKind() == 0x12e) {
            if (event->m_flag == 1) {
                moduleAccesser->getWorkManageModule().addInt(1, 0x20000020);
            }
        } else if (status.getStatusKind() != 0x12f && status.getStatusKind() != 0x130) {
            status.getStatusKind();
        }
        return;
    }
    case 0x17:
        if (event->m_flag != 0) {
            copyCancel(moduleAccesser, false);
        }
        break;
    }
    if (stageObject->soGetKind() == 2) {
        switch (stageObject->soGetSubKind()) {
        case 0x4d:
            switch (eventInfo->m_eventKind) {
            case 0x838:
                if (moduleAccesser->getStatusModule().getStatusKind() == 0x170) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x172, moduleAccesser);
                }
                break;
            }
            return;
        case 0x66:
            switch (eventInfo->m_eventKind) {
            case 0x838:
                if (moduleAccesser->getStatusModule().getStatusKind() == 0x174) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x176, moduleAccesser);
                }
                break;
            }
            return;
        }
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4);
}

// The kind record of the owner (kind 0, sub kind Kirby) and the heap the articles are built from.
// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument.
struct ftKirbyArticleKind : wnArticleKindInfo {
    ftKirbyArticleKind() {
        kind = 0;
        subKind = Fighter_Kirby;
    }
};
struct ftKirbyArticleBuildInfo : wnArticleBuildInfo {
    ftKirbyArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// Explicit specializations are strong. The original inlines these into the builder, so the out-of-line copy stays weak.
#define FT_KIRBY_SIMPLE_HOLDER(ID)                                                                                      \
    template <>                                                                                                             inline __declspec(weak)                                                                                                 wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 5>, soIntToType<ID> >::wnInstanceHolder(soModuleAccesser* acc) :           m_instance(ID, wnSimpleConstructionInfo(wnSimpleKindInfo(Fighter_Kirby), acc->m_enumerationStart->m_heapModule),                    g_ftCommonDataAccesser.getSimpleData(static_cast<ftKind>(wnSimpleKindInfo(Fighter_Kirby).subKind), &ftArticleDataFlags().get().resourceGroup),                        false) { }

FT_KIRBY_SIMPLE_HOLDER(10)
FT_KIRBY_SIMPLE_HOLDER(9)
FT_KIRBY_SIMPLE_HOLDER(8)
FT_KIRBY_SIMPLE_HOLDER(7)
FT_KIRBY_SIMPLE_HOLDER(6)
#undef FT_KIRBY_SIMPLE_HOLDER

#define FT_KIRBY_WEAPON_HOLDER(WEAPON, DATA, ID, GETTER)                                                                \
    template <>                                                                                                         \
    inline __declspec(weak)                                                                                                    \
    wnInstanceHolder<WEAPON, soKindInfoGeneric<0, 5>, soIntToType<ID> >::wnInstanceHolder(soModuleAccesser* acc) :     \
        m_instance(ID,                                                                                                  \
                   ftKirbyArticleBuildInfo(ftKirbyArticleKind(),                                                        \
                                           static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)),         \
                   static_cast<const DATA*>(g_ftCommonDataAccesser.GETTER(Fighter_Kirby,                               \
                                                                          &ftArticleDataFlags().get().resourceGroup))) { }

FT_KIRBY_WEAPON_HOLDER(wnKirbyCookHat, wnKirbyCookHatData, 5, getWeaponData5)
FT_KIRBY_WEAPON_HOLDER(wnKirbyNabe, wnKirbyNabeData, 4, getWeaponData4)
FT_KIRBY_WEAPON_HOLDER(wnKirbyStone, wnKirbyStoneData, 3, getWeaponData3)
FT_KIRBY_WEAPON_HOLDER(wnKirbyStarmissile, wnKirbyStarmissileData, 2, getWeaponData2)
FT_KIRBY_WEAPON_HOLDER(wnKirbyFinalCutter, wnKirbyFinalCutterData, 1, getWeaponData1)
FT_KIRBY_WEAPON_HOLDER(wnKirbyHammer, wnKirbyHammerData, 0, getWeaponData0)
#undef FT_KIRBY_WEAPON_HOLDER

// The holders and sub pools of the article pools (soKindInfoGeneric<0, 5>); the mediator destroys them out of line.
#pragma dont_inline on
template <class Mediator>
soSelectInstanceHolder<Mediator, soArticleMediatorNull>::~soSelectInstanceHolder() { }
template <class Mediator, int N>
soGenerateArticleManageModuleBuilder<Mediator, N>::~soGenerateArticleManageModuleBuilder() { }
template <class Mediator, int N>
soGenerateArticleManageModuleBuilder<Mediator, N>::soGenerateArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template soSelectInstanceHolder<ftKirbyArticleMediator, soArticleMediatorNull>::~soSelectInstanceHolder();
template soGenerateArticleManageModuleBuilder<ftKirbyArticleMediator, 12>::~soGenerateArticleManageModuleBuilder();
template soGenerateArticleManageModuleBuilder<ftKirbyArticleMediator, 12>::soGenerateArticleManageModuleBuilder(soModuleAccesser*);
template <class PoolList, class Founder>
soArticleMediatorImpl<PoolList, Founder>::~soArticleMediatorImpl() { }
template soArticleMediatorImpl<ftKirbyArticleList, ftKirby>::~soArticleMediatorImpl();
// The flag sits at mediator+0x12828 (builder+0x12930): addis r3, r3, 1; stb r4, 0x2828(r3).
template <class PoolList, class Founder>
void soArticleMediatorImpl<PoolList, Founder>::setAutoRecycle(bool enabled) {
    m_autoRecycle = enabled;
}
template void soArticleMediatorImpl<ftKirbyArticleList, ftKirby>::setAutoRecycle(bool);
template wnInstanceHolder<wnKirbyCookHat, soKindInfoGeneric<0, 5>, soIntToType<5> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnKirbyCookHat, 1, wnInstanceHolder, soIntToType<5> >, soKindInfoGeneric<0, 5> >;
template wnInstanceHolder<wnKirbyNabe, soKindInfoGeneric<0, 5>, soIntToType<4> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnKirbyNabe, 1, wnInstanceHolder, soIntToType<4> >, soKindInfoGeneric<0, 5> >;
template wnInstanceHolder<wnKirbyStone, soKindInfoGeneric<0, 5>, soIntToType<3> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnKirbyStone, 1, wnInstanceHolder, soIntToType<3> >, soKindInfoGeneric<0, 5> >;
template wnInstanceHolder<wnKirbyStarmissile, soKindInfoGeneric<0, 5>, soIntToType<2> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnKirbyStarmissile, 1, wnInstanceHolder, soIntToType<2> >, soKindInfoGeneric<0, 5> >;
template wnInstanceHolder<wnKirbyFinalCutter, soKindInfoGeneric<0, 5>, soIntToType<1> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnKirbyFinalCutter, 1, wnInstanceHolder, soIntToType<1> >, soKindInfoGeneric<0, 5> >;
template wnInstanceHolder<wnKirbyHammer, soKindInfoGeneric<0, 5>, soIntToType<0> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnKirbyHammer, 1, wnInstanceHolder, soIntToType<0> >, soKindInfoGeneric<0, 5> >;
template wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 5>, soIntToType<6> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<6> >, soKindInfoGeneric<0, 5> >;
template wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 5>, soIntToType<7> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<7> >, soKindInfoGeneric<0, 5> >;
template wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 5>, soIntToType<8> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<8> >, soKindInfoGeneric<0, 5> >;
template wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 5>, soIntToType<9> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<9> >, soKindInfoGeneric<0, 5> >;
template wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 5>, soIntToType<10> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<10> >, soKindInfoGeneric<0, 5> >;
template class soInstancePool<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<10> >, soInstancePoolRoot, soKindInfoGeneric<0, 5>, 10>;
#pragma dont_inline off
// The simple article's teardown is a copy in each fighter that pools one. Weak until the animcmd sub-builder
// teardown matches the original: this copy destroys that builder in one call and runs long.
__declspec(weak) wnSimple::~wnSimple() { }

bool ftKirby::activeArticle(wnKirbyHammer* article, soModuleAccesser* acc) {
    soResourceModule& resource = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    article->activate(founderTaskId, resource.getResourceIdAccesser()->getEtcResId(), acc->getSituationModule().getKind());
    return true;
}

inline bool ftKirbyIsAir(soModuleAccesser* acc) {
    return acc->getSituationModule().getKind() == Situation_Air;
}

bool ftKirby::activeArticle(wnKirbyCookHat* article, soModuleAccesser* acc) {
    bool isAir = ftKirbyIsAir(acc);
    soResourceModule& resource = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    float lr = acc->getPostureModule().getLr();
    int resId = dynamic_cast<ftResourceIdAccesserImpl&>(*resource.getResourceIdAccesser()).getFinalResId();
    article->activate(founderTaskId, resId, lr, isAir);
    return true;
}

bool ftKirby::activeArticle(wnKirbyFinalCutter* article, soModuleAccesser* acc) {
    g_ftCommonDataAccesser.getData(Fighter_Kirby);
    soGroundModule& ground = acc->getGroundModule();
    soPostureModule& posture = acc->getPostureModule();
    float angle;
    float lr = posture.getLr();
    angle = 0.0f;
    Vec3f offset;
    offset.m_x = 0.0f;
    offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfb0, 0);
    offset.m_z = soValueAccesser::getConstantFloat(acc, 0xfaf, 0);
    Matrix nodeMtx(true);
    if (ground.isTouch(8, 0)) {
        Vec2f normal;
        normal = ground.getTouchNormal(8, 0);
        float ny = normal.m_y;
        float nx = normal.m_x;
        angle = lr * (float)atan2(nx, ny);
        Matrix rot(true);
        rot.setRotateX(angle);
        rot.mulVec(&offset, &offset);
    }
    Vec3fScale(&offset, &offset, posture.getScale());
    acc->getModelModule().getNodeGlobalMtx(400, &nodeMtx, false);
    Vec3f pos;
    nodeMtx.mulPos(&offset, &pos);
    pos.m_z = acc->getGroundModule().getZ();
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resource = acc->getResourceModule();
    int team;
    int founderTaskId = acc->getStageObject().m_taskId;
    team = fighter.getOwner()->getTeam();
    int resId = resource.getResourceIdAccesser()->getEtcResId();
    article->activate(founderTaskId, resId, team, &pos, lr, angle);
    return false;
}

// ---- the article mediator (slots 0..10: hammer, final cutter, starmissile, stone, nabe, cook hat, five simple articles) ----
typedef soInstancePool<ftKirbyHammerInfo, ftKirbyArticlePool0Parent, ftKirbyArticleKindInfo, 0> ftKirbyHammerPool;
typedef soInstancePool<ftKirbyFinalCutterInfo, ftKirbyArticlePool1Parent, ftKirbyArticleKindInfo, 1> ftKirbyFinalCutterPool;
typedef soInstancePool<ftKirbyStarmissileInfo, ftKirbyArticlePool2Parent, ftKirbyArticleKindInfo, 2> ftKirbyStarmissilePool;
typedef soInstancePool<ftKirbyStoneInfo, ftKirbyArticlePool3Parent, ftKirbyArticleKindInfo, 3> ftKirbyStonePool;
typedef soInstancePool<ftKirbyNabeInfo, ftKirbyArticlePool4Parent, ftKirbyArticleKindInfo, 4> ftKirbyNabePool;
typedef soInstancePool<ftKirbyCookHatInfo, ftKirbyArticlePool5Parent, ftKirbyArticleKindInfo, 5> ftKirbyCookHatPool;
typedef soInstancePool<ftKirbySimple6Info, ftKirbyArticlePool6Parent, ftKirbyArticleKindInfo, 6> ftKirbySimple6Pool;
typedef soInstancePool<ftKirbySimple7Info, ftKirbyArticlePool7Parent, ftKirbyArticleKindInfo, 7> ftKirbySimple7Pool;
typedef soInstancePool<ftKirbySimple8Info, ftKirbyArticlePool8Parent, ftKirbyArticleKindInfo, 8> ftKirbySimple8Pool;
typedef soInstancePool<ftKirbySimple9Info, ftKirbyArticlePool9Parent, ftKirbyArticleKindInfo, 9> ftKirbySimple9Pool;
typedef ftKirbySimple10Pool ftKirbySimple10PoolAlias;

// HYPOTHESIS: the original is a functor over the predicate type that the simple articles' pools count with; its call
// operator stays out of line there. Name as in include/em/em_article_mediator.h (not includable here).
inline bool ftKirbyArticleIsActiveRef(soArticle& article) { return article.isActiveArticle(); }
template <class Fn>
struct soEachCounter {
    Fn* m_fn;
    s32 m_trueNum;
    s32 m_falseNum;
    soEachCounter(Fn* fn) : m_fn(fn), m_trueNum(0), m_falseNum(0) { }
    template <class T>
    bool operator()(T* instance) __attribute__((never_inline)) {
        if (m_fn(static_cast<soArticle&>(*instance)) == true) {
            ++m_trueNum;
        } else {
            ++m_falseNum;
        }
        return true;
    }
};

template <int N, class Level, int S, int L>
inline s32 ftKirbySlotActiveNumEach(Level& pool, const soIntToType<S>&, const soIntToType<L>&) {
    soEachCounter<bool(soArticle&)> counter(ftKirbyArticleIsActiveRef);
    for (s32 i = 0; i < N; ++i) {
        soEachCounter<bool(soArticle&)>* c = &counter;
        if (!(*c)(pool.getSub().getInstanceAt(i))) {
            break;
        }
    }
    return counter.m_trueNum;
}

template <int S, int L>
inline s32 ftKirbySlotRestNum(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    return 2 - acc->getGenerateArticleManageModule().getNum(11);
}

template <class W, int N, class Level, int S, int L>
inline s32 ftKirbySlotActiveNum(Level& pool, const soIntToType<S>&, const soIntToType<L>&) {
    wnArticleSlotCounter counter;
    counter.m_isActive = wnArticleIsActive;
    counter.m_activeNum = 0;
    counter.m_inactiveNum = 0;
    for (s32 i = 0; i < N; ++i) {
        wnArticleSlotCounter* c = &counter;
        if (c->m_isActive(&static_cast<soArticle&>(*pool.getSub().getInstanceAt(i))) == true) {
            ++c->m_activeNum;
        } else {
            ++c->m_inactiveNum;
        }
    }
    return counter.m_activeNum;
}

template <int N, class Level, int S, int L>
inline bool ftKirbySlotIsGeneratable(Level& pool, const soIntToType<S>&, const soIntToType<L>&) {
    bool allActive;
    s32 i;
    for (i = 0; i < N; ++i) {
        if (static_cast<soArticle&>(*pool.getSub().getInstanceAt(i)).isActiveArticle() == false) {
            allActive = false;
            goto done;
        }
    }
    allActive = true;
done:
    return allActive == false;
}

#define FT_KIRBY_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

#define FT_KIRBY_DEACTIVATE(P, W)                                                  \
    for (s32 i = 0; i < 1; ++i) {                                                  \
        W* weapon = static_cast<P&>(m_pools).getSub().getInstanceAt(i);            \
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {         \
            return;                                                                \
        }                                                                          \
    }

template <class PoolList, class Founder>
void soArticleMediatorImpl<PoolList, Founder>::deactivate() {
    FT_KIRBY_DEACTIVATE(ftKirbyHammerPool, wnKirbyHammer)
    FT_KIRBY_DEACTIVATE(ftKirbyFinalCutterPool, wnKirbyFinalCutter)
    FT_KIRBY_DEACTIVATE(ftKirbyStarmissilePool, wnKirbyStarmissile)
    FT_KIRBY_DEACTIVATE(ftKirbyStonePool, wnKirbyStone)
    FT_KIRBY_DEACTIVATE(ftKirbyNabePool, wnKirbyNabe)
    FT_KIRBY_DEACTIVATE(ftKirbyCookHatPool, wnKirbyCookHat)
    FT_KIRBY_DEACTIVATE(ftKirbySimple6Pool, wnSimple)
    FT_KIRBY_DEACTIVATE(ftKirbySimple7Pool, wnSimple)
    FT_KIRBY_DEACTIVATE(ftKirbySimple8Pool, wnSimple)
    FT_KIRBY_DEACTIVATE(ftKirbySimple9Pool, wnSimple)
    FT_KIRBY_DEACTIVATE(ftKirbySimple10PoolAlias, wnSimple)
}

template <class PoolList, class Founder>
s32 soArticleMediatorImpl<PoolList, Founder>::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 8: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 9: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 10: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_KIRBY_UNUSED_SLOTS(0)
    default: return 0;
    }
}

template <class PoolList, class Founder>
s32 soArticleMediatorImpl<PoolList, Founder>::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return ftKirbySlotActiveNum<wnKirbyHammer, 1>(static_cast<ftKirbyHammerPool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 1: return ftKirbySlotActiveNum<wnKirbyFinalCutter, 1>(static_cast<ftKirbyFinalCutterPool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 2: return ftKirbySlotActiveNum<wnKirbyStarmissile, 1>(static_cast<ftKirbyStarmissilePool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 3: return ftKirbySlotActiveNum<wnKirbyStone, 1>(static_cast<ftKirbyStonePool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 4: return ftKirbySlotActiveNum<wnKirbyNabe, 1>(static_cast<ftKirbyNabePool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 5: return ftKirbySlotActiveNum<wnKirbyCookHat, 1>(static_cast<ftKirbyCookHatPool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 6: return ftKirbySlotActiveNumEach<1>(static_cast<ftKirbySimple6Pool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 7: return ftKirbySlotActiveNumEach<1>(static_cast<ftKirbySimple7Pool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 8: return ftKirbySlotActiveNumEach<1>(static_cast<ftKirbySimple8Pool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 9: return ftKirbySlotActiveNumEach<1>(static_cast<ftKirbySimple9Pool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 10: return ftKirbySlotActiveNumEach<1>(static_cast<ftKirbySimple10PoolAlias&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 11: return ftKirbySlotRestNum(acc, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOT(12, 0) WN_ARTICLE_UNUSED_SLOT(13, 0) WN_ARTICLE_UNUSED_SLOT(14, 0)
    WN_ARTICLE_UNUSED_SLOT(15, 0) WN_ARTICLE_UNUSED_SLOT(16, 0)
    default: return 0;
    }
}

template <int S, int L>
inline bool ftKirbySlotGeneratableByCount(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    return acc->getGenerateArticleManageModule().getNum(11) < 2;
}

template <class PoolList, class Founder>
bool soArticleMediatorImpl<PoolList, Founder>::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbyHammerPool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 1: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbyFinalCutterPool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 2: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbyStarmissilePool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 3: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbyStonePool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 4: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbyNabePool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 5: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbyCookHatPool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 6: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbySimple6Pool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 7: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbySimple7Pool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 8: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbySimple8Pool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 9: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbySimple9Pool&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    case 10: return ftKirbySlotIsGeneratable<1>(static_cast<ftKirbySimple10PoolAlias&>(m_pools), WN_ARTICLE_SLOT_TAGS);
    // HYPOTHESIS: the copy ability article is generatable while fewer than two of them exist.
    case 11: return ftKirbySlotGeneratableByCount(acc, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOT(12, false) WN_ARTICLE_UNUSED_SLOT(13, false) WN_ARTICLE_UNUSED_SLOT(14, false)
    WN_ARTICLE_UNUSED_SLOT(15, false) WN_ARTICLE_UNUSED_SLOT(16, false)
    default: return false;
    }
}

// HYPOTHESIS: virtual view of the copy ability module (its first word is a vtable pointer); slot 4 takes the article.
struct ftKirbyCopyAbilityShootView {
    virtual void unk0();
    virtual void unk1();
    virtual void unk2();
    virtual void unk3();
    virtual bool shootArticle(soArticle* article, soModuleAccesser* acc);
};

template <int S, int L>
inline void ftKirbySlotTags(const soIntToType<S>&, const soIntToType<L>&) { }

template <class PoolList, class Founder>
bool soArticleMediatorImpl<PoolList, Founder>::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnKirbyHammer>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnKirbyFinalCutter>(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnKirbyStarmissile>(article, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotShoot<wnKirbyStone>(article, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotShoot<wnKirbyNabe>(article, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotShoot<wnKirbyCookHat>(article, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    case 8: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    case 9: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    case 10: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    case 11: {
        ftKirbySlotTags(WN_ARTICLE_SLOT_TAGS);
        ftKirby* kirby = dynamic_cast<ftKirby*>(&acc->getStageObject());
        if (kirby == NULL) {
            return false;
        }
        return reinterpret_cast<ftKirbyCopyAbilityShootView*>(kirby->getCopyAbility())->shootArticle(article, acc);
    }
    WN_ARTICLE_UNUSED_SLOT(12, true) WN_ARTICLE_UNUSED_SLOT(13, true) WN_ARTICLE_UNUSED_SLOT(14, true)
    WN_ARTICLE_UNUSED_SLOT(15, true) WN_ARTICLE_UNUSED_SLOT(16, true)
    default: return false;
    }
}

template <class W, class Level, class Predicate, int Tag>
inline W* ftKirbyFindInPool(Level& pool, Predicate& checker, const soIntToType<Tag>&) {
    return pool.getSub().find(checker);
}

template <class Activator, class W, class Level, int S, int L>
inline soArticle* ftKirbySlotGenerate(Level& pool, soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    soArticleDeactivateChecker checker;
    W* weapon = ftKirbyFindInPool<W>(pool, checker, soIntToType<0>());
    if (weapon == NULL) {
        weapon = static_cast<W*>(checker.getCandidate());
        if (weapon == NULL) {
            return wnGetNullArticle();
        }
        weapon->deactivateArticle();
    }
    if (Activator::activeArticle(weapon, acc) == true) {
        return &static_cast<soArticle&>(*weapon);
    }
    return wnGetNullArticle();
}

template <int S, int L>
inline soArticle* ftKirbySlotGenerateCopy(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    if (acc->getGenerateArticleManageModule().getNum(11) >= 2) {
        return wnGetNullArticle();
    }
    int ownerTaskId = acc->getStageObject().m_taskId;
    BaseItem* item = soArticleMediatorHelper::createItem(0x59, 11, ownerTaskId);
    if (item == NULL) {
        return wnGetNullArticle();
    }
    return soArticleMediatorHelper::castArticle(item);
}

template <class PoolList, class Founder>
soArticle* soArticleMediatorImpl<PoolList, Founder>::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return ftKirbySlotGenerate<Founder, wnKirbyHammer>(static_cast<ftKirbyHammerPool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftKirbySlotGenerate<Founder, wnKirbyFinalCutter>(static_cast<ftKirbyFinalCutterPool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return ftKirbySlotGenerate<Founder, wnKirbyStarmissile>(static_cast<ftKirbyStarmissilePool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return ftKirbySlotGenerate<Founder, wnKirbyStone>(static_cast<ftKirbyStonePool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 4: return ftKirbySlotGenerate<Founder, wnKirbyNabe>(static_cast<ftKirbyNabePool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 5: return ftKirbySlotGenerate<Founder, wnKirbyCookHat>(static_cast<ftKirbyCookHatPool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 6: return ftKirbySlotGenerate<Founder, wnSimple>(static_cast<ftKirbySimple6Pool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 7: return ftKirbySlotGenerate<Founder, wnSimple>(static_cast<ftKirbySimple7Pool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 8: return ftKirbySlotGenerate<Founder, wnSimple>(static_cast<ftKirbySimple8Pool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 9: return ftKirbySlotGenerate<Founder, wnSimple>(static_cast<ftKirbySimple9Pool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 10: return ftKirbySlotGenerate<Founder, wnSimple>(static_cast<ftKirbySimple10PoolAlias&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    // HYPOTHESIS: copy ability item uses article kind 0x59 when fewer than two exist.
    case 11: return ftKirbySlotGenerateCopy(acc, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOT(12, wnGetNullArticle())
    WN_ARTICLE_UNUSED_SLOT(13, wnGetNullArticle())
    WN_ARTICLE_UNUSED_SLOT(14, wnGetNullArticle())
    WN_ARTICLE_UNUSED_SLOT(15, wnGetNullArticle())
    WN_ARTICLE_UNUSED_SLOT(16, wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template soArticle* soArticleMediatorImpl<ftKirbyArticleList, ftKirby>::generate(s32, soModuleAccesser*);
template void soArticleMediatorImpl<ftKirbyArticleList, ftKirby>::deactivate();
template s32 soArticleMediatorImpl<ftKirbyArticleList, ftKirby>::getGenerateMaxNum(s32);
template s32 soArticleMediatorImpl<ftKirbyArticleList, ftKirby>::getActiveNum(soModuleAccesser*, s32);
template bool soArticleMediatorImpl<ftKirbyArticleList, ftKirby>::isGeneratable(soModuleAccesser*, s32);
template bool soArticleMediatorImpl<ftKirbyArticleList, ftKirby>::shoot(soModuleAccesser*, soArticle*);

ftKineticEnergyController::~ftKineticEnergyController() { }
// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off
