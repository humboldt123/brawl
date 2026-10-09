// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Marth.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/captain/ft_captain.h>
#include <ft/captain/ft_captain_extend_param_accesser.h>
#include <ft/ft_common_data_accesser.h>

#define FT_BC ftCaptainBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftCaptainExtendParamAccesser g_ftCaptainExtendParamAccesser;

ftClassInfoImpl<Fighter_Captain, ftCaptain> g_ftClassInfoCaptain;

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt); the first and the other
// entries in module 27 are shared ft status processes.
extern "C" {
extern soStatusUniqProcess lbl_27_bss_7CDC;
extern soStatusUniqProcess lbl_27_bss_7CEC;
extern soStatusUniqProcess lbl_100_bss_194;
extern soStatusUniqProcess lbl_100_bss_1A4;
extern soStatusUniqProcess lbl_100_bss_1B4;
extern soStatusUniqProcess lbl_100_bss_1C4;
extern soStatusUniqProcess lbl_100_bss_1D4;
extern soStatusUniqProcess lbl_100_bss_1E4;
extern soStatusUniqProcess lbl_100_bss_1F4;
extern soStatusUniqProcess lbl_100_bss_204;
extern soStatusUniqProcess lbl_100_bss_214;
extern soStatusUniqProcess lbl_100_bss_224;
extern soStatusUniqProcess lbl_100_bss_234;
extern soStatusUniqProcess lbl_100_bss_244;
}

ftCaptain::ftCaptain(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftCaptainBuildConfig>(entryId,
                                         Fighter_Captain,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Captain)) {
    // Register the status processes in action order; entry 10 has no process.
    soStatusUniqProcess* processes[16] = {0};
    processes[0] = &lbl_27_bss_7CDC;
    processes[1] = &lbl_100_bss_194;
    processes[2] = &lbl_100_bss_1C4;
    processes[3] = &lbl_100_bss_1A4;
    processes[4] = &lbl_100_bss_1F4;
    processes[5] = &lbl_27_bss_7CEC;
    processes[6] = &lbl_100_bss_194;
    processes[7] = &lbl_100_bss_1D4;
    processes[8] = &lbl_100_bss_1E4;
    processes[9] = &lbl_100_bss_1B4;
    processes[11] = &lbl_100_bss_204;
    processes[12] = &lbl_100_bss_214;
    processes[13] = &lbl_100_bss_224;
    processes[14] = &lbl_100_bss_234;
    processes[15] = &lbl_100_bss_244;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 16);
}

ftCaptain::~ftCaptain() { }

// FIXME: Test code present only to emit the shared builder functions; delete once ftCaptain is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftCaptainInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftCaptainInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

// HYPOTHESIS: extend param group 3 (ftData + 0x88) holds the Falcon Kick parameters.
void* fn_100_8C4C() { return g_ftCommonDataAccesser.getData(Fighter_Captain)->extendParam[3]; }

u8 fn_100_6C60(u8* p) { return *(u8*)(p + 0x4); }
void fn_100_9670() {}
void fn_100_9674() {}
int fn_100_97E8(u8* p) { return *(int*)(p + 0x20); }
int fn_100_98C4(u8* p) { return *(int*)(p + 0x18); }
void fn_100_A9D8(u8* p, u8 v) { *(u8*)(p + 0x405c) = v; }
void fn_100_B7DC() {}
void fn_100_BF74() {}
void fn_100_C05C() {}
void fn_100_C1F4() {}
void fn_100_C2DC() {}
void fn_100_C3C4() {}
void fn_100_C4AC() {}
void fn_100_C594() {}

}
