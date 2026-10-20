// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
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
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <ft/ft_outside_event_presenter.h>
#include <so/damage/so_damage.h>
#include <ft/ft_owner.h>

#define FT_NOINLINE_SEARCH_BUILDER
#define FT_BC ftKirbyBuildConfig
#include <ft/builder/ft_builder_noinline.h>

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
    fn_27_195B34(*reinterpret_cast<void**>(reinterpret_cast<u8*>(m_data) + 0x16c), shape, &group);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&group), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    m_moduleAccesser->getAreaModule().addArea(reinterpret_cast<soSet<soAreaData>*>(reinterpret_cast<u8*>(m_data) + 0x17c));
    m_moduleAccesser->getAreaModule().enableArea(9, false, -1);
    m_moduleAccesser->getAreaModule().enableArea(10, false, -1);
    m_moduleAccesser->getAreaModule().enableArea(11, false, -1);
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftKirby is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftKirbyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftKirbyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_96_28D0(u8* p) { return *(u8*)(p + 0x4); }
int fn_96_BC6C(u8* p) { return *(int*)(p + 0x28); }
int fn_96_C17C(u8* p) { return *(int*)(p + 0xc0); }
int fn_96_F3AC(u8* p) { return *(int*)(p + 0x20); }
int fn_96_F488(u8* p) { return *(int*)(p + 0x18); }
int fn_96_115E8() { return 12; }
void fn_96_12530() {}
void fn_96_12C28() {}
void fn_96_12CAC() {}
void fn_96_12FA4() {}
void fn_96_1308C() {}
void fn_96_13174() {}
void fn_96_1325C() {}

#pragma dont_inline on

// Kirby overrides still undefined here (defined later in this unit).
void fn_96_C184(u8* p);

// Event thunks: a second-base adjustment (this - N) followed by a tail call to the Kirby override.
// (fn_96_1359C, the isObserv thunk, takes no second argument in the target and is left out.)
void fn_96_135A4(u8* p) { fn_96_C184(p - 0x54); }

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
    } else if (moduleAccesser->getStatusModule().getStatusKind() == 0x116 && static_cast<u8>(collisionLog->_33) == 1) {
        // MATCH-ONLY: the category is read as a plain byte, not through the bitfield.
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(static_cast<gfTask::Category>(reinterpret_cast<u8*>(collisionLog)[0x22]), collisionLog->m_taskId);
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
        bool spycloak = getOwner()->getSpycloak();
        m_copyAbility.changeCopyAbility(copyKind, manager->getPlayerNo(entryId), spycloak, 0);
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
    bool copyStatus = isCopyStatus();
    bool spycloak = getOwner()->getSpycloak();
    m_copyAbility.changeCopyAbility(-1, manager->getPlayerNo(entryId), spycloak, copyStatus);
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
