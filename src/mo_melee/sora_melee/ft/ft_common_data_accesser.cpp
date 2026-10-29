#include <types.h>
#include <ft/robot/ft_robot_article_pools.h>
#include <ft/ft_slot_manager.h>
#include <so/so_archive_db.h>

#include <ft/ft_common_data_accesser.h>

extern "C" s32 fn_27_1454C0();
extern "C" u32 fn_27_12006C(ftSlot* slot, int type, ftKind kind, bool flag);
extern "C" u32 fn_27_120218(ftSlot* slot, ftKind kind, int copyKind);
extern "C" u32 fn_27_122218(ftSlotManager* slotManager, ftKind kind, u8 colorNo);
extern "C" u32 fn_27_122270(ftSlotManager* slotManager, ftKind kind, u8 colorNo);
extern "C" s32 fn_27_1222C8(ftSlotManager* slotManager, s32 slot);
extern "C" void fn_27_145AA0(ftCommonData* commonData);
// HYPOTHESIS: 12-byte ftKindData entries (kind * 0xC), one per ftKind
extern "C" ftKindData lbl_27_bss_3210[];
extern "C" char lbl_27_data_2BBD8[];
extern "C" wnSimpleData lbl_27_bss_4530;

ftCommonDataAccesser g_ftCommonDataAccesser;

// HYPOTHESIS: public address record whose pointer at +0x10 is the article data
struct ftRobotBeamArticle {
    u8 unk0[0x10];
    void* unk10;
};

// HYPOTHESIS: ftData continues past extendParam; the pointer read for kind 5 sits at 0x10C
struct ftDataExtended {
    u8 unk0[0x10C];
    void* unk10C;
};

ftData* ftCommonDataAccesser::getData(ftKind kind) const
{
    return lbl_27_bss_3210[kind].data;
}

ftParam* ftCommonDataAccesser::getParamCommon() const
{
    // HYPOTHESIS: the common attribute block is returned through the ftParam view its callers cast from.
    return reinterpret_cast<ftParam*>(g_ftCommonData.commonParams[fn_27_1454C0()]);
}

ftParam* ftCommonDataAccesser::getParam(ftKind kind) const
{
    s32 index = fn_27_1454C0();
    return lbl_27_bss_3210[kind].params[index];
}

ftParam* ftCommonDataAccesser::getParam(ftKind kind, int variation) const
{
    return lbl_27_bss_3210[kind].params[variation];
}

ftParamEtc* ftCommonDataAccesser::getParamEtc(ftKind kind) const
{
    return lbl_27_bss_3210[kind].ParamEtc;
}

float ftCommonDataAccesser::getParamCommonFloat(u32 variation, u32 id) const
{
    return g_ftCommonData.commonFloatParams[variation].m_0[id - 3117];
}

s32 ftCommonDataAccesser::getParamCommonInt(u32 variation, u32 id) const
{
    return g_ftCommonData.commonIntParams[variation].m_0[id - 23030];
}

s32* ftCommonDataAccesser::getParamCommonIndefinite(u32 variation, u32 id) const
{
    return g_ftCommonData.commonIndefiniteParams[variation].m_0[id - 43026];
}

void ftCommonDataAccesser::setupCommon(ftDataCommon* dataCommon)
{
    g_ftCommonData.dataCommon = dataCommon;
    g_ftCommonData.commonParams = dataCommon->ftCommonParams;
    fn_27_145AA0(&g_ftCommonData);
}

bool ftCommonDataAccesser::isValidDat(ftKind kind)
{
    return lbl_27_bss_3210[kind].data != NULL;
}

u32 ftCommonDataAccesser::getResId(ftKind kind) const
{
    return g_ftSlotManager->m_resId[kind];
}

u32 ftCommonDataAccesser::getFinalResId(ftKind kind) const
{
    return g_ftSlotManager->m_finalResId[kind];
}

u32 ftCommonDataAccesser::getMotionResId(int slotNo, ftKind kind) const
{
    if (kind == Fighter_WarioMan) {
        return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 6, Fighter_Wario, false);
    }
    return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 6, kind, false);
}

u32 ftCommonDataAccesser::getModelResId(int slotNo, ftKind kind, bool flag) const
{
    return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 8, kind, flag);
}

u32 ftCommonDataAccesser::getEtcResId(int slotNo, ftKind kind) const
{
    if (kind == Fighter_WarioMan) {
        return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 5, Fighter_Wario, false);
    }
    if (kind == Fighter_GKoopa) {
        return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 5, Fighter_Koopa, false);
    }
    return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 5, kind, false);
}

u32 ftCommonDataAccesser::getMotionEtcResId(int slotNo, ftKind kind) const
{
    if (kind == Fighter_WarioMan) {
        return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 7, Fighter_Wario, false);
    }
    if (kind == Fighter_GKoopa) {
        return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 7, Fighter_Koopa, false);
    }
    return fn_27_12006C(&g_ftSlotManager->m_slots[slotNo], 7, kind, false);
}

u32 ftCommonDataAccesser::getEntryResId(ftKind kind, u8 colorNo) const
{
    return fn_27_122218(g_ftSlotManager, kind, colorNo);
}

u32 ftCommonDataAccesser::getResultResId(ftKind kind, u8 colorNo) const
{
    return fn_27_122270(g_ftSlotManager, kind, colorNo);
}

u32 ftCommonDataAccesser::getKirbyResId(int slotNo, ftKind kind, int copyKind) const
{
    if (kind == Fighter_WarioMan) {
        return fn_27_120218(&g_ftSlotManager->m_slots[slotNo], Fighter_Wario, copyKind);
    }
    if (kind == Fighter_GKoopa) {
        return fn_27_120218(&g_ftSlotManager->m_slots[slotNo], Fighter_Koopa, copyKind);
    }
    return fn_27_120218(&g_ftSlotManager->m_slots[slotNo], kind, copyKind);
}

void* ftCommonDataAccesser::getKirbyBinData(ftKind kind) const
{
    u32 slot = fn_27_1222C8(g_ftSlotManager, kind);
    return soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
}

void ftCommonDataAccesser::reset(ftKind kind)
{
    lbl_27_bss_3210[kind].data = NULL;
    lbl_27_bss_3210[kind].params = NULL;
    lbl_27_bss_3210[kind].ParamEtc = NULL;
}

void ftCommonDataAccesser::reset()
{
    g_ftCommonData.dataCommon = NULL;
    g_ftCommonData.commonParams = NULL;
    for (int i = 0; i < Fighter_Count; i++) {
        reset(static_cast<ftKind>(i));
    }
}

void* ftCommonDataAccesser::getKirbyStoneData() const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x134)
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[46];
}

void* ftCommonDataAccesser::getPikachuThunderData() const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[7];
}

void* ftCommonDataAccesser::getPokeLizardonBreathParam(ftKind kind) const
{
    switch (kind) {
    case Fighter_PokeLizardon:
        return lbl_27_bss_3210[Fighter_PokeLizardon].data->extendParam[1];
    case Fighter_Kirby:
        // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
        return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[12];
    default:
        return NULL;
    }
}

void* ftRobotArticleDataAccesser::getGyroData(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return lbl_27_bss_3210[Fighter_Robot].data->extendParam[6];
}

void* ftRobotArticleDataAccesser::getBeamData(ftKind kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_Robot:
        // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
        return lbl_27_bss_3210[Fighter_Robot].data->extendParam[8];
    case 5: {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Robot);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    default:
        return NULL;
    }
}

void* ftRobotArticleDataAccesser::getGyroHolderData(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return lbl_27_bss_3210[Fighter_Robot].data->extendParam[10];
}

void* ftRobotArticleDataAccesser::getFinalBeamData(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return lbl_27_bss_3210[Fighter_Robot].data->extendParam[12];
}

void* ftCommonDataAccesser::getSZerosuitArmorData() const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return lbl_27_bss_3210[Fighter_SZeroSuit].data->extendParam[7];
}

void* ftCommonDataAccesser::getWeaponData91(ftKind kind, bool* resourceGroup) const
{
    switch (kind) {
    case 26:
        return lbl_27_bss_3210[26].data->extendParam[7];
    case 5: {
        u32 slot = fn_27_1222C8(g_ftSlotManager, 26);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    default:
        return NULL;
    }
}

void* ftCommonDataAccesser::getWeaponData92(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return lbl_27_bss_3210[Fighter_Lucas].data->extendParam[8];
}

void* ftCommonDataAccesser::getWeaponData93(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return lbl_27_bss_3210[Fighter_Lucas].data->extendParam[9];
}

void* ftCommonDataAccesser::getWeaponData94(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return lbl_27_bss_3210[Fighter_Lucas].data->extendParam[10];
}

void* ftCommonDataAccesser::getWeaponData95(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA8)
    return lbl_27_bss_3210[Fighter_Lucas].data->extendParam[11];
}

void* ftCommonDataAccesser::getWeaponData96(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return lbl_27_bss_3210[Fighter_Lucas].data->extendParam[12];
}

void* ftCommonDataAccesser::getParamSamusChargeShot(ftKind kind) const
{
    switch (kind) {
    case Fighter_Samus:
        return lbl_27_bss_3210[Fighter_Samus].data->extendParam[0];
    case 5:
        return reinterpret_cast<ftDataExtended*>(lbl_27_bss_3210[5].data)->unk10C;
    default:
    case 4:
        return NULL;
    }
}

void* ftCommonDataAccesser::getParamSamusSpecialLw(ftKind kind) const
{
    switch (kind) {
    case Fighter_Samus:
        return lbl_27_bss_3210[Fighter_Samus].data->extendParam[2];
    default:
        return NULL;
    }
}

void* ftCommonDataAccesser::getParamSamusSpecialS(ftKind kind) const
{
    switch (kind) {
    case Fighter_Samus:
        return lbl_27_bss_3210[Fighter_Samus].data->extendParam[3];
    default:
        return NULL;
    }
}

void* ftCommonDataAccesser::getParamSamusFinal(ftKind kind) const
{
    switch (kind) {
    case Fighter_Samus:
        return lbl_27_bss_3210[Fighter_Samus].data->extendParam[4];
    default:
        return NULL;
    }
}

void* ftCommonDataAccesser::getWeaponData121(ftKind kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_Ness:
        return lbl_27_bss_3210[Fighter_Ness].data->extendParam[7];
    case 5: {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Ness);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    default:
        return NULL;
    }
}

void* ftCommonDataAccesser::getWeaponData122(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[8];
}

void* ftCommonDataAccesser::getWeaponData123(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[9];
}

void* ftCommonDataAccesser::getWeaponData124(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[10];
}

void* ftCommonDataAccesser::getWeaponData125(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA8)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[11];
}

void* ftCommonDataAccesser::getWeaponData126(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[12];
}

void* ftCommonDataAccesser::getWeaponData127(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xB0)
    return lbl_27_bss_3210[Fighter_Ness].data->extendParam[13];
}

void* ftCommonDataAccesser::getWeaponData129(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return lbl_27_bss_3210[kind].data->extendParam[6];
}

void* ftCommonDataAccesser::getWeaponData130(ftKind kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return lbl_27_bss_3210[kind].data->extendParam[7];
}

void* ftCommonDataAccesser::getKoopaBreathParam(ftKind kind) const
{
    switch (kind) {
    case Fighter_Koopa:
        return lbl_27_bss_3210[Fighter_Koopa].data->extendParam[0];
    case Fighter_Kirby:
        // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x110)
        return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[37];
    default:
        return NULL;
    }
}

void* ftCommonDataAccesser::getGKoopaBreathParam(ftKind kind) const
{
    switch (kind) {
    case Fighter_GKoopa:
        return lbl_27_bss_3210[Fighter_GKoopa].data->extendParam[1];
    default:
        return NULL;
    }
}

const wnGameWatchNormalWeaponData* ftCommonDataAccesser::getGameWatchNormalWeaponData() const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnGameWatchNormalWeaponData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[12]);
}

void* ftCommonDataAccesser::getGanonBeastData() const
{
    return lbl_27_bss_3210[Fighter_Ganon].data->extendParam[4];
}

wnSimpleData* ftCommonDataAccesser::getSimpleData(ftKind kind, bool* resourceGroup) const
{
    return &lbl_27_bss_4530;
}

ftCommonDataAccesser::~ftCommonDataAccesser() { }
