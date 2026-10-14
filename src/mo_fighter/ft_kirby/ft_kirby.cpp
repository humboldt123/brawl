// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_manager.h>
#include <ft/ft_reflector_group.h>
#include <ft/ft_class_info_impl.h>
#include <ft/kirby/ft_kirby.h>
#include <ft/kirby/ft_kirby_extend_param_accesser.h>

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
extern "C" void fn_96_1C244(ftKirbyCopyAbilityModuleData* module, soModuleAccesser* moduleAccesser);
extern "C" void fn_96_1C4E8(u8* module, int copyKind);

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
        fn_96_1C4E8(reinterpret_cast<u8*>(&m_copyAbility), g_ftManager->getKirbyCopyResourceKind(m_entryId, i));
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

// Kirby work and extend-param getters (map: ftKirby__getExtendParam, ftKirby__getWorkStone, ftKirby__isObserv).
void* fn_96_AF98(u8* p) { return *(u8**)(p + 0x1B678) + 0x7C; }
void* fn_96_B488(u8* p) { return p + 0x1B690; }
#pragma dont_inline on
int fn_96_EFD0(u8* p, s32 kind) { return (s8)kind == (s8)0xc; }

// Kirby overrides still undefined here (defined later in this unit).
void fn_96_B108(u8* p);
void fn_96_B2FC(u8* p);
void fn_96_C184(u8* p);
void fn_96_CE58(u8* p);
bool fn_96_D160(u8* p);
void fn_96_D334(u8* p);
bool fn_96_DA58(u8* p);
bool fn_96_DAEC(u8* p);
bool fn_96_DBA0(u8* p);
void fn_96_DCD8(u8* p);
void fn_96_DD50(u8* p);
bool fn_96_DEC4(u8* p);
bool fn_96_DF94(u8* p);
void fn_96_DBFC(u8* p, int copyKind);
// Fighter overrides defined in other translation units.
void fn_96_1DB1C(u8* p);

// Event thunks: a second-base adjustment (this - N) followed by a tail call to the Kirby override.
// (fn_96_1359C, the isObserv thunk, takes no second argument in the target and is left out.)
void fn_96_135A4(u8* p) { fn_96_C184(p - 0x54); }
void fn_96_135AC(u8* p) { fn_96_B108(p - 0x64); }
void fn_96_135B4(u8* p) { fn_96_B2FC(p - 0x70); }
bool fn_96_135BC(u8* p) { return fn_96_DBA0(p - 0x7c); }
bool fn_96_135D4(u8* p) { return fn_96_DAEC(p - 0x94); }
bool fn_96_135E4(u8* p) { return fn_96_DA58(p - 0x94); }
bool fn_96_135EC(u8* p) { return fn_96_DF94(p - 0xa0); }
bool fn_96_135FC(u8* p) { return fn_96_DEC4(p - 0xa0); }
bool fn_96_13614(u8* p) { return fn_96_D160(p - 0xb8); }
void fn_96_1361C(u8* p) { fn_96_CE58(p - 0xb8); }
void fn_96_13684(u8* p) { fn_96_D334(p - 0xe8); }
void fn_96_1368C(u8* p) { fn_96_DD50(p - 0xf4); }
void fn_96_13694(u8* p) { fn_96_DCD8(p - 0xf4); }
void fn_96_1369C(u8* p, int copyKind) { fn_96_DBFC(p - 0xf4, copyKind); }

// Second-base forwarders to the out-of-unit Fighter overrides (MATCH-ONLY: same adjustment and tail call).
void fn_96_DB94(u8* p) { fn_96_1DB1C(p + 0x1B72C); }
void fn_96_DBFC(u8* p, int copyKind) { fn_96_1C4E8(p + 0x1B72C, copyKind); }
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
