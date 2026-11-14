#include <types.h>
#include <ft/robot/ft_robot_article_pools.h>
#include <ft/ft_slot_manager.h>
#include <so/so_archive_db.h>

#include <ft/ft_common_data_accesser.h>
#include <ft/builder/ft_builder_kinetic.h>

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
    void* unk14; // HYPOTHESIS: second article pointer of the public address record
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

template <>
const wnPokeLizardonRockData* ftCommonDataAccesser::getWeaponData<wnPokeLizardonRockData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnPokeLizardonRockData*>(lbl_27_bss_3210[Fighter_PokeLizardon].data->extendParam[6]);
}

template <>
const wnPokeLizardonRockStoneData* ftCommonDataAccesser::getWeaponData<wnPokeLizardonRockStoneData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnPokeLizardonRockStoneData*>(lbl_27_bss_3210[Fighter_PokeLizardon].data->extendParam[7]);
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

template <>
const wnSZerosuitArmorData* ftCommonDataAccesser::getWeaponData<wnSZerosuitArmorData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnSZerosuitArmorData*>(lbl_27_bss_3210[Fighter_SZeroSuit].data->extendParam[7]);
}

void* ftCommonDataAccesser::getWeaponData0(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[42];
}

void* ftCommonDataAccesser::getWeaponData1(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[43];
}

void* ftCommonDataAccesser::getWeaponData2(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[45];
}

void* ftCommonDataAccesser::getWeaponData3(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[46];
}

void* ftCommonDataAccesser::getWeaponData4(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[47];
}

void* ftCommonDataAccesser::getWeaponData5(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[48];
}

void* ftCommonDataAccesser::getWeaponData14(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Link].data->extendParam[11];
}

void* ftCommonDataAccesser::getWeaponData15(ftKind kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_Link:
        return lbl_27_bss_3210[Fighter_Link].data->extendParam[16];
    default:
        return NULL;
    }
}

template <>
const wnSnakeMineData* ftCommonDataAccesser::getWeaponData<wnSnakeMineData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnSnakeMineData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[5]);
}

template <>
const wnSnakeTrenchMortarData* ftCommonDataAccesser::getWeaponData<wnSnakeTrenchMortarData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnSnakeTrenchMortarData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[6]);
}

void* ftCommonDataAccesser::getWeaponData18(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Snake].data->extendParam[7];
}

template <>
const wnSnakeRpg7Data* ftCommonDataAccesser::getWeaponData<wnSnakeRpg7Data>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnSnakeRpg7Data*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[8]);
}

template <>
const wnSnakeCypherData* ftCommonDataAccesser::getWeaponData<wnSnakeCypherData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return static_cast<const wnSnakeCypherData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[9]);
}

template <>
const wnSnakeC4Data* ftCommonDataAccesser::getWeaponData<wnSnakeC4Data>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnSnakeC4Data*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[10]);
}

template <>
const wnSnakeC4SwitchData* ftCommonDataAccesser::getWeaponData<wnSnakeC4SwitchData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA8)
    return static_cast<const wnSnakeC4SwitchData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[11]);
}

template <>
const wnSnakeNikitaData* ftCommonDataAccesser::getWeaponData<wnSnakeNikitaData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnSnakeNikitaData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[12]);
}

void* ftCommonDataAccesser::getWeaponData24(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Snake].data->extendParam[13];
}

template <>
const wnSnakeLadderData* ftCommonDataAccesser::getWeaponData<wnSnakeLadderData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xB4)
    return static_cast<const wnSnakeLadderData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[14]);
}

template <>
const wnSnakeRgb6Data* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6Data>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xB8)
    return static_cast<const wnSnakeRgb6Data*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[15]);
}

template <>
const wnSnakeRgb6SightData* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6SightData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xBC)
    return static_cast<const wnSnakeRgb6SightData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[16]);
}

template <>
const wnSnakeRgb6ClipData* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6ClipData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xC0)
    return static_cast<const wnSnakeRgb6ClipData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[17]);
}

template <>
const wnSnakeRgb6BulletData* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6BulletData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xC4)
    return static_cast<const wnSnakeRgb6BulletData*>(lbl_27_bss_3210[Fighter_Snake].data->extendParam[18]);
}

void* ftCommonDataAccesser::getWeaponData32(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[7];
}

template <>
const wnPikachuCloudData* ftCommonDataAccesser::getWeaponData<wnPikachuCloudData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnPikachuCloudData*>(lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[8]);
}

template <>
const wnPikachuVortexData* ftCommonDataAccesser::getWeaponData<wnPikachuVortexData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return static_cast<const wnPikachuVortexData*>(lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[9]);
}

void* ftCommonDataAccesser::getWeaponData38(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_PokeFushigisou].data->extendParam[4];
}

void* ftCommonDataAccesser::getWeaponData40(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_PokeFushigisou].data->extendParam[6];
}

void* ftCommonDataAccesser::getWeaponData42(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_PokeTrainer].data->extendParam[4];
}

void* ftCommonDataAccesser::getWeaponData43(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[Fighter_PokeTrainer].data->extendParam[5];
}

template <>
const wnZeldaDeinData* ftCommonDataAccesser::getWeaponData<wnZeldaDeinData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnZeldaDeinData*>(lbl_27_bss_3210[Fighter_Zelda].data->extendParam[6]);
}

template <>
const wnZeldaDeinSData* ftCommonDataAccesser::getWeaponData<wnZeldaDeinSData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnZeldaDeinSData*>(lbl_27_bss_3210[Fighter_Zelda].data->extendParam[7]);
}

template <>
const wnZeldaLightingBowData* ftCommonDataAccesser::getWeaponData<wnZeldaLightingBowData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnZeldaLightingBowData*>(lbl_27_bss_3210[Fighter_Zelda].data->extendParam[8]);
}

template <>
const wnZeldaLightingBowArrowData* ftCommonDataAccesser::getWeaponData<wnZeldaLightingBowArrowData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return static_cast<const wnZeldaLightingBowArrowData*>(lbl_27_bss_3210[Fighter_Zelda].data->extendParam[9]);
}

template <>
const wnFoxIllusionData* ftCommonDataAccesser::getWeaponData<wnFoxIllusionData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnFoxIllusionData*>(lbl_27_bss_3210[Fighter_Fox].data->extendParam[10]);
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

template <>
const wnSamusBombData* ftCommonDataAccesser::getWeaponData<wnSamusBombData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnSamusBombData*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[10]);
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

template <>
const wnSamusMissileData* ftCommonDataAccesser::getWeaponData<wnSamusMissileData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnSamusMissileData*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[12]);
}

template <>
const wnSamusSuperMissileData* ftCommonDataAccesser::getWeaponData<wnSamusSuperMissileData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xCC)
    return static_cast<const wnSamusSuperMissileData*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[20]);
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

template <>
const wnSamusLaserData* ftCommonDataAccesser::getWeaponData<wnSamusLaserData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xB4)
    return static_cast<const wnSamusLaserData*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[14]);
}

template <>
const wnSamusLaser2Data* ftCommonDataAccesser::getWeaponData<wnSamusLaser2Data>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xBC)
    return static_cast<const wnSamusLaser2Data*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[16]);
}

template <>
const wnSamusGunData* ftCommonDataAccesser::getWeaponData<wnSamusGunData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xC4)
    return static_cast<const wnSamusGunData*>(lbl_27_bss_3210[Fighter_Samus].data->extendParam[18]);
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

template <>
const wnDonkeyBarrelCongaData* ftCommonDataAccesser::getWeaponData<wnDonkeyBarrelCongaData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnDonkeyBarrelCongaData*>(lbl_27_bss_3210[Fighter_Donkey].data->extendParam[5]);
}

template <>
const wnDonkeySoundEffectData* ftCommonDataAccesser::getWeaponData<wnDonkeySoundEffectData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnDonkeySoundEffectData*>(lbl_27_bss_3210[Fighter_Donkey].data->extendParam[6]);
}

template <>
const wnGameWatchRescueData* ftCommonDataAccesser::getWeaponData<wnGameWatchRescueData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnGameWatchRescueData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[7]);
}

template <>
const wnGameWatchParachuteData* ftCommonDataAccesser::getWeaponData<wnGameWatchParachuteData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnGameWatchParachuteData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[8]);
}

template <>
const wnGameWatchPanelData* ftCommonDataAccesser::getWeaponData<wnGameWatchPanelData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA0)
    return static_cast<const wnGameWatchPanelData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[9]);
}

template <>
const wnGameWatchOilData* ftCommonDataAccesser::getWeaponData<wnGameWatchOilData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnGameWatchOilData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[10]);
}

template <>
const wnGameWatchOctopusData* ftCommonDataAccesser::getWeaponData<wnGameWatchOctopusData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA8)
    return static_cast<const wnGameWatchOctopusData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[11]);
}

const wnGameWatchNormalWeaponData* ftCommonDataAccesser::getGameWatchNormalWeaponData() const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnGameWatchNormalWeaponData*>(lbl_27_bss_3210[Fighter_GameWatch].data->extendParam[12]);
}

template <>
const wnGameWatchNormalWeaponData* ftCommonDataAccesser::getWeaponData<wnGameWatchNormalWeaponData>(int kind, bool* resourceGroup) const
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

extern "C" const wnDummyFighterData lbl_27_rodata_14A4;

const wnDummyFighterData* ftCommonDataAccesser::getDummyFighterData() const
{
    return &lbl_27_rodata_14A4;
}

template <>
const wnDummyFighterData* ftCommonDataAccesser::getWeaponData<wnDummyFighterData>(int kind, bool* resourceGroup) const
{
    return &lbl_27_rodata_14A4;
}

template <>
const wnKirbyPikminData* ftCommonDataAccesser::getWeaponData<wnKirbyPikminData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnKirbyPikminData*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[61]);
}

template <>
const wnMarioPumpWaterData* ftCommonDataAccesser::getWeaponData<wnMarioPumpWaterData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnMarioPumpWaterData*>(lbl_27_bss_3210[kind].data->extendParam[8]);
}

const ftLucarioAuraBallParam* ftCommonDataAccesser::getLucarioAuraBallParam(int kind) const
{
    switch (kind) {
    case Fighter_Lucario:
    default:
        return static_cast<const ftLucarioAuraBallParam*>(lbl_27_bss_3210[Fighter_Lucario].data->extendParam[0]);
    case Fighter_Kirby:
        return static_cast<const ftLucarioAuraBallParam*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[35]);
    }
}

template <>
const wnLucarioQigongData* ftCommonDataAccesser::getWeaponData<wnLucarioQigongData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnLucarioQigongData*>(lbl_27_bss_3210[Fighter_Lucario].data->extendParam[10]);
}

float* ftCommonDataAccesser::getLinkBowParam(int kind) const
{
    switch (kind) {
    case Fighter_Link:
        return static_cast<float*>(lbl_27_bss_3210[Fighter_Link].data->extendParam[0]);
    case Fighter_Kirby:
        return static_cast<float*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[33]);
    default:
        return NULL;
    }
}

template <>
const wnPokeFushigisouSeedData* ftCommonDataAccesser::getWeaponData<wnPokeFushigisouSeedData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_PokeFushigisou:
        return static_cast<const wnPokeFushigisouSeedData*>(lbl_27_bss_3210[Fighter_PokeFushigisou].data->extendParam[5]);
    case Fighter_Kirby:
        return static_cast<const wnPokeFushigisouSeedData*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[54]);
    default:
        return NULL;
    }
}

template <>
const wnPokeZenigameWaterData* ftCommonDataAccesser::getWeaponData<wnPokeZenigameWaterData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case Fighter_PokeZenigame:
        return static_cast<const wnPokeZenigameWaterData*>(lbl_27_bss_3210[Fighter_PokeZenigame].data->extendParam[5]);
    case Fighter_Kirby:
        return static_cast<const wnPokeZenigameWaterData*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[55]);
    default:
        return NULL;
    }
}

float* ftCommonDataAccesser::getToonLinkBowParam(int kind) const
{
    switch (kind) {
    case Fighter_ToonLink:
        return static_cast<float*>(lbl_27_bss_3210[Fighter_ToonLink].data->extendParam[0]);
    case Fighter_Kirby:
        return static_cast<float*>(lbl_27_bss_3210[Fighter_Kirby].data->extendParam[34]);
    default:
        return NULL;
    }
}

template <>
const wnFoxLandMasterShellData* ftCommonDataAccesser::getWeaponData<wnFoxLandMasterShellData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case 6:
        return static_cast<const wnFoxLandMasterShellData*>(lbl_27_bss_3210[6].data->extendParam[12]);
    case 19:
        return static_cast<const wnFoxLandMasterShellData*>(lbl_27_bss_3210[19].data->extendParam[10]);
    case 44:
        return static_cast<const wnFoxLandMasterShellData*>(lbl_27_bss_3210[44].data->extendParam[12]);
    default:
        return NULL;
    }
}

template <>
const wnFalcoIllusionData* ftCommonDataAccesser::getWeaponData<wnFalcoIllusionData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnFalcoIllusionData*>(lbl_27_bss_3210[Fighter_Falco].data->extendParam[8]);
}

template <>
const wnWolfIllusionData* ftCommonDataAccesser::getWeaponData<wnWolfIllusionData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnWolfIllusionData*>(lbl_27_bss_3210[Fighter_Wolf].data->extendParam[10]);
}

template <>
const wnDededeStarData* ftCommonDataAccesser::getWeaponData<wnDededeStarData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnDededeStarData*>(lbl_27_bss_3210[Fighter_Dedede].data->extendParam[8]);
}

template <>
const wnDededeJethammerData* ftCommonDataAccesser::getWeaponData<wnDededeJethammerData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnDededeJethammerData*>(lbl_27_bss_3210[Fighter_Dedede].data->extendParam[10]);
}

template <>
const wnDededeWaddledooBeamData* ftCommonDataAccesser::getWeaponData<wnDededeWaddledooBeamData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnDededeWaddledooBeamData*>(lbl_27_bss_3210[Fighter_Dedede].data->extendParam[6]);
}

template <>
const wnDededeFollowerData* ftCommonDataAccesser::getWeaponData<wnDededeFollowerData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xB4)
    return static_cast<const wnDededeFollowerData*>(lbl_27_bss_3210[Fighter_Dedede].data->extendParam[14]);
}

template <>
const wnDiddyBarrelData* ftCommonDataAccesser::getWeaponData<wnDiddyBarrelData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x9C)
    return static_cast<const wnDiddyBarrelData*>(lbl_27_bss_3210[Fighter_Diddy].data->extendParam[8]);
}

template <>
const wnDiddyPeanutsData* ftCommonDataAccesser::getWeaponData<wnDiddyPeanutsData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnDiddyPeanutsData*>(lbl_27_bss_3210[Fighter_Diddy].data->extendParam[10]);
}

template <>
const wnDiddyExplosionData* ftCommonDataAccesser::getWeaponData<wnDiddyExplosionData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnDiddyExplosionData*>(lbl_27_bss_3210[Fighter_Diddy].data->extendParam[12]);
}

template <>
const wnIkeSwordData* ftCommonDataAccesser::getWeaponData<wnIkeSwordData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnIkeSwordData*>(lbl_27_bss_3210[Fighter_Ike].data->extendParam[7]);
}

template <>
const wnLinkBowData* ftCommonDataAccesser::getWeaponData<wnLinkBowData>(int kind, bool* resourceGroup) const
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Link);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnLinkBowData*>(static_cast<ftRobotBeamArticle*>(address)->unk10);
    }
    switch (kind) {
    case Fighter_Link:
    return static_cast<const wnLinkBowData*>(lbl_27_bss_3210[Fighter_Link].data->extendParam[13]);
    default:
        return NULL;
    }
}

template <>
const wnLinkBowArrowData* ftCommonDataAccesser::getWeaponData<wnLinkBowArrowData>(int kind, bool* resourceGroup) const
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Link);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnLinkBowArrowData*>(static_cast<ftRobotBeamArticle*>(address)->unk14);
    }
    switch (kind) {
    case Fighter_Link:
    return static_cast<const wnLinkBowArrowData*>(lbl_27_bss_3210[Fighter_Link].data->extendParam[15]);
    default:
        return NULL;
    }
}

template <>
const wnToonLinkBowData* ftCommonDataAccesser::getWeaponData<wnToonLinkBowData>(int kind, bool* resourceGroup) const
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_ToonLink);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnToonLinkBowData*>(static_cast<ftRobotBeamArticle*>(address)->unk10);
    }
    switch (kind) {
    case Fighter_ToonLink:
    return static_cast<const wnToonLinkBowData*>(lbl_27_bss_3210[Fighter_ToonLink].data->extendParam[13]);
    default:
        return NULL;
    }
}

template <>
const wnToonLinkBowArrowData* ftCommonDataAccesser::getWeaponData<wnToonLinkBowArrowData>(int kind, bool* resourceGroup) const
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_ToonLink);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnToonLinkBowArrowData*>(static_cast<ftRobotBeamArticle*>(address)->unk14);
    }
    switch (kind) {
    case Fighter_ToonLink:
    return static_cast<const wnToonLinkBowArrowData*>(lbl_27_bss_3210[Fighter_ToonLink].data->extendParam[15]);
    default:
        return NULL;
    }
}

template <>
const wnCaptainBlueFalconData* ftCommonDataAccesser::getWeaponData<wnCaptainBlueFalconData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnCaptainBlueFalconData*>(lbl_27_bss_3210[Fighter_Captain].data->extendParam[6]);
}

template <>
const wnCaptainFinalBgData* ftCommonDataAccesser::getWeaponData<wnCaptainFinalBgData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x98)
    return static_cast<const wnCaptainFinalBgData*>(lbl_27_bss_3210[Fighter_Captain].data->extendParam[7]);
}

template <>
const wnYoshiTamagoData* ftCommonDataAccesser::getWeaponData<wnYoshiTamagoData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case 4:
        return static_cast<const wnYoshiTamagoData*>(lbl_27_bss_3210[4].data->extendParam[6]);
    case Fighter_Kirby: {
        u32 slot = fn_27_1222C8(g_ftSlotManager, 4);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnYoshiTamagoData*>(static_cast<ftRobotBeamArticle*>(address)->unk10);
    }
    default:
        return NULL;
    }
}

template <>
const wnPikminPikminData* ftCommonDataAccesser::getWeaponData<wnPikminPikminData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA4)
    return static_cast<const wnPikminPikminData*>(lbl_27_bss_3210[Fighter_Pikmin].data->extendParam[10]);
}

template <>
const wnPikminDolfinData* ftCommonDataAccesser::getWeaponData<wnPikminDolfinData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xA8)
    return static_cast<const wnPikminDolfinData*>(lbl_27_bss_3210[Fighter_Pikmin].data->extendParam[11]);
}

template <>
const wnPikminChainData* ftCommonDataAccesser::getWeaponData<wnPikminChainData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0xAC)
    return static_cast<const wnPikminChainData*>(lbl_27_bss_3210[Fighter_Pikmin].data->extendParam[12]);
}

template <>
const wnSZerosuitParalyzerData* ftCommonDataAccesser::getWeaponData<wnSZerosuitParalyzerData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnSZerosuitParalyzerData*>(lbl_27_bss_3210[Fighter_SZeroSuit].data->extendParam[4]);
}

template <>
const wnSZerosuitWhipData* ftCommonDataAccesser::getWeaponData<wnSZerosuitWhipData>(int kind, bool* resourceGroup) const
{
    // HYPOTHESIS: ftData's declared extendParam array is shorter than the slot read here (0x94)
    return static_cast<const wnSZerosuitWhipData*>(lbl_27_bss_3210[Fighter_SZeroSuit].data->extendParam[6]);
}

template <>
const wnSheikNeedleData* ftCommonDataAccesser::getWeaponData<wnSheikNeedleData>(int kind, bool* resourceGroup) const
{
    switch (kind) {
    case 14:
        return static_cast<const wnSheikNeedleData*>(lbl_27_bss_3210[14].data->extendParam[5]);
    case Fighter_Kirby: {
        u32 slot = fn_27_1222C8(g_ftSlotManager, 14);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<const wnSheikNeedleData*>(static_cast<ftRobotBeamArticle*>(address)->unk10);
    }
    default:
        return NULL;
    }
}

// HYPOTHESIS: ftData's declared extendParam array is shorter than the slots read below.

extern "C" void* fn_27_147D34(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[5];
}

extern "C" void* fn_27_147D4C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[7];
}

extern "C" void* fn_27_147D7C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[9];
}

extern "C" void* fn_27_148A9C(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Yoshi].data->extendParam[5];
}

extern "C" void* fn_27_148B44(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Yoshi].data->extendParam[7];
}

extern "C" void* fn_27_148B58(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Yoshi].data->extendParam[8];
}

extern "C" void* fn_27_148B6C(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Yoshi].data->extendParam[9];
}

extern "C" void* fn_27_148D7C(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Sheik].data->extendParam[7];
}

extern "C" void* fn_27_148D90(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Sheik].data->extendParam[8];
}

extern "C" void* fn_27_148E3C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[7];
}

extern "C" void* fn_27_148F4C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[11];
}

extern "C" void* fn_27_149194(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[11];
}

extern "C" void* fn_27_1491AC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[12];
}

extern "C" void* fn_27_1491C4(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[12];
}

extern "C" void* fn_27_14925C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[14];
}

extern "C" void* fn_27_149274(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[15];
}

extern "C" void* fn_27_14928C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    return lbl_27_bss_3210[kind].data->extendParam[16];
}

extern "C" void* fn_27_1494BC(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_Wario].data->extendParam[5];
}

template <>
const wnGanonBeastData* ftCommonDataAccesser::getWeaponData<wnGanonBeastData>(int kind, bool* resourceGroup) const
{
    return static_cast<const wnGanonBeastData*>(lbl_27_bss_3210[Fighter_Ganon].data->extendParam[4]);
}

extern "C" void* fn_27_149AF4(const ftCommonDataAccesser*, s32, bool*)
{
    return lbl_27_bss_3210[Fighter_ToonLink].data->extendParam[11];
}

void* ftCommonDataAccesser::getWeaponData128(ftKind kind, bool* resourceGroup) const
{
    return lbl_27_bss_3210[kind].data->extendParam[4];
}

extern "C" void* fn_27_149464(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_Samus:
        return lbl_27_bss_3210[Fighter_Samus].data->extendParam[21];
    default:
        return NULL;
    }
}

extern "C" void* fn_27_1497CC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_GKoopa:
        return lbl_27_bss_3210[Fighter_GKoopa].data->extendParam[5];
    default:
        return NULL;
    }
}

extern "C" void* fn_27_149B08(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_ToonLink:
        return lbl_27_bss_3210[Fighter_ToonLink].data->extendParam[16];
    default:
        return NULL;
    }
}

extern "C" void* fn_27_14948C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Wario) {
        return lbl_27_bss_3210[Fighter_Wario].data->extendParam[4];
    }
    return lbl_27_bss_3210[Fighter_WarioMan].data->extendParam[5];
}

extern "C" void* fn_27_148198(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_PokeLizardon:
        return lbl_27_bss_3210[Fighter_PokeLizardon].data->extendParam[5];
    case Fighter_Kirby:
        return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[53];
    default:
        return NULL;
    }
}

extern "C" void* fn_27_148BD0(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_SZeroSuit:
        return lbl_27_bss_3210[Fighter_SZeroSuit].data->extendParam[5];
    case Fighter_Kirby:
        return lbl_27_bss_3210[Fighter_Kirby].data->extendParam[59];
    default:
        return NULL;
    }
}

extern "C" void* fn_27_1484BC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    switch (kind) {
    case Fighter_Fox:
        return lbl_27_bss_3210[Fighter_Fox].data->extendParam[11];
    case Fighter_Falco:
        return lbl_27_bss_3210[Fighter_Falco].data->extendParam[9];
    case Fighter_Wolf:
        return lbl_27_bss_3210[Fighter_Wolf].data->extendParam[11];
    default:
        return NULL;
    }
}

// Owner's extendParam slot, or Kirby's copy from that fighter's public "data" archive.
#define FT_OWNER_OR_KIRBY(owner, index, field)                                                         \
    switch (kind) {                                                                                    \
    case owner:                                                                                        \
        return lbl_27_bss_3210[owner].data->extendParam[index];                                        \
    case Fighter_Kirby: {                                                                              \
        u32 slot = fn_27_1222C8(g_ftSlotManager, owner);                                               \
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U); \
        return static_cast<ftRobotBeamArticle*>(address)->field;                                       \
    }                                                                                                  \
    default:                                                                                           \
        return NULL;                                                                                   \
    }

#define FT_KIND_OR_KIRBY(owner, index, field)                                                          \
    switch (kind) {                                                                                    \
    case owner:                                                                                        \
        return lbl_27_bss_3210[kind].data->extendParam[index];                                         \
    case Fighter_Kirby: {                                                                              \
        u32 slot = fn_27_1222C8(g_ftSlotManager, owner);                                               \
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U); \
        return static_cast<ftRobotBeamArticle*>(address)->field;                                       \
    }                                                                                                  \
    default:                                                                                           \
        return NULL;                                                                                   \
    }

extern "C" void* fn_27_147C9C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_KIND_OR_KIRBY(Fighter_Mario, 6, unk10)
}

extern "C" void* fn_27_148050(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Pikachu);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    return lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[5];
}

extern "C" void* fn_27_1480CC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Pikachu);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk14;
    }
    return lbl_27_bss_3210[Fighter_Pikachu].data->extendParam[6];
}

extern "C" void* fn_27_148380(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Fox, 8, unk10)
}

extern "C" void* fn_27_148414(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Fox, 9, unk14)
}

extern "C" void* fn_27_14858C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Falco, 6, unk10)
}

extern "C" void* fn_27_148620(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Falco, 7, unk14)
}

extern "C" void* fn_27_1486C8(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Wolf, 8, unk10)
}

extern "C" void* fn_27_14875C(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Wolf, 9, unk14)
}

extern "C" void* fn_27_148910(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Dedede, 12, unk10)
}

extern "C" void* fn_27_1489B8(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Diddy, 6, unk10)
}

extern "C" void* fn_27_148CE8(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Sheik, 6, unk14)
}

extern "C" void* fn_27_148DA4(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_KIND_OR_KIRBY(Fighter_Luigi, 6, unk10)
}

extern "C" void* fn_27_148F64(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Peach);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    return lbl_27_bss_3210[kind].data->extendParam[12];
}

extern "C" void* fn_27_148FE4(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Peach);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk14;
    }
    return lbl_27_bss_3210[kind].data->extendParam[13];
}

extern "C" void* fn_27_149064(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_KIND_OR_KIRBY(Fighter_Pit, 9, unk10)
}

extern "C" void* fn_27_1490FC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_KIND_OR_KIRBY(Fighter_Pit, 10, unk14)
}

extern "C" void* fn_27_1491DC(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Popo);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    return lbl_27_bss_3210[kind].data->extendParam[13];
}

extern "C" void* fn_27_1492F8(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Samus);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    return lbl_27_bss_3210[Fighter_Samus].data->extendParam[8];
}

extern "C" void* fn_27_1494D0(const ftCommonDataAccesser*, s32 kind, bool*)
{
    if (kind == Fighter_Kirby) {
        u32 slot = fn_27_1222C8(g_ftSlotManager, Fighter_Lucario);
        void* address = soArchiveDb::getManager(0)->getPublicAddress(slot, lbl_27_data_2BBD8, 0U, 0U, -1U);
        return static_cast<ftRobotBeamArticle*>(address)->unk10;
    }
    return lbl_27_bss_3210[Fighter_Lucario].data->extendParam[9];
}

extern "C" void* fn_27_1496F0(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_Koopa, 4, unk10)
}

extern "C" void* fn_27_149844(const ftCommonDataAccesser*, s32 kind, bool*)
{
    FT_OWNER_OR_KIRBY(Fighter_GameWatch, 6, unk10)
}

#undef FT_OWNER_OR_KIRBY
#undef FT_KIND_OR_KIRBY

// HYPOTHESIS: ftData + 0x1C is the common status-flag table, eight bytes per status. Nana shares Popo's.
struct ftDataStatusFlags {
    u8 unk0[0x1C];
    const ftStatusFlagData* statusFlags;
};

const ftStatusFlagData* ftCommonDataAccesser::getFtStatusData(ftKind kind, int status) const
{
    if (kind == Fighter_Nana) {
        kind = Fighter_Popo;
    }
    return reinterpret_cast<ftDataStatusFlags*>(lbl_27_bss_3210[kind].data)->statusFlags + status;
}

extern int g_soValueVariation;

// Defined in this file; dont_inline keeps the out-of-line call in getParam / getParamCommon.
#pragma dont_inline on
extern "C" s32 fn_27_1454C0()
{
    return g_soValueVariation;
}
#pragma dont_inline reset
