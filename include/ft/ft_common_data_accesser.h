// Local BrawlHeaders shadow: expose the Final Smash resource identifier and extend parameters.
#pragma once

#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <it/it_create.h>
#include <so/so_common_data_accesser.h>
#include <types.h>

// HYPOTHESIS: per-fighter item preload table at ftData + 0x60
struct ftDataItemList {
    itKind* m_items;
    int m_count;
};

// One (action id, command list) pair; the table ends at the first negative id (see soAnimCmdControlUnitBuilderDisguise::setupDisguiseList).
struct soAnimCmdDisguiseList {
    s32 actionId;
    void* commandList;
};

struct ftData {
    ftMotionData* motionData;
    ftVisibilityData* visibilityData;
    u8 unk8[0xC]; // HYPOTHESIS: possibly the attribute, SSE attribute and misc-section pointers (matches the moveset header order at 0x8/0xC/0x10)
    ftStatusData* statusData;
    void* uniqStatusData; // 0x18: character-specific soStatusData table, UniqueStatusCount entries
    u8 unk1C[8]; // HYPOTHESIS: possibly the unknown-7 table and the action interrupt table pointers (0x1C/0x20 in the moveset header order)
    void* uniqActionEntryScripts; // 0x24: per-status entry anim-cmd table that follows the common one
    void* uniqActionExitScripts; // 0x28: per-status exit anim-cmd table that follows the common one
    ftPreCheckAnimCmdData* preCheckAnimCmdData;
    void* subactionMainScripts;  // 0x30: anim-cmd threads 1 and 5 (see ftFighterBuildData::getAnimCmdData)
    void* subactionGfxScripts;   // 0x34: threads 2 and 6
    void* subactionSfxScripts;   // 0x38: threads 3 and 7
    void* subactionOtherScripts; // 0x3C: threads 4 and 8
#ifdef FT_FIGHTER_FIELDS
    u8 unk40[0xC];
    // HYPOTHESIS: node sets indexed by item hold kind, used by Fighter::attachItemHoldAnim.
    int* itemHoldNodeSets;
#else
    u8 unk40[0x10]; // HYPOTHESIS: possibly two bone-float tables and two bone-reference tables (0x40..0x4C in the moveset header order)
#endif
    soAnimCmdDisguiseList* entryActionOverrides; // 0x50: per-action replacement entry scripts
    soAnimCmdDisguiseList* exitActionOverrides;  // 0x54: per-action replacement exit scripts
    union {
        u8 unk58[0x24]; // HYPOTHESIS: possibly unknown-22, bone floats 3, unknown-24, static articles, entry article, two unknowns and two flag words (0x58..0x78 in the moveset header order)
        struct {
            u8 unk58_pad[0x8];
            ftDataItemList* unk60; // HYPOTHESIS: fighter item preload list
#ifdef FT_KIRBY_ARTICLE_POOLS
            struct wnSimpleData** unk64;
#endif
        };
    };
    void* extendParam[6];
#ifdef FT_DATA_SIMPLE_ARTICLE_DATA
    u8 unk94[0x34];
    // HYPOTHESIS: the fighter's simple-article data (ftSnake::activeArticle12 passes it to the copy-ability process).
    const struct wnSimpleData* simpleArticleData; // 0xC8
#endif
#ifdef FT_KIRBY_ARTICLE_POOLS
    u8 unk94_kirby[0xB8]; // 0x94..0x14C
    // HYPOTHESIS: copy-kind integer parameter table read by Kirby's simple article 10 activator.
    const s32* unk14C;
#endif
};


// The fighter attribute block (an array of 4-byte entries, 0x2E4 bytes in the moveset file). modelScale lines up with the
// attribute list's size entry, and 0x1FC..0x208 are the camera extents read by ftManager::getCameraRange.
struct ftParam {
    u8 unk0[0xB4]; // HYPOTHESIS: possibly the walk, dash, run, jump, air-drift and gravity attributes plus weight at 0xB0
    float modelScale;
#ifdef FT_KIRBY_ARTICLE_POOLS
    u8 unkB8[0xEC];
    float unk1A4;
    s32 unk1A8;
    u8 unk1AC[0xEC];
#else
    u8 unkB8[0x1E0]; // HYPOTHESIS: possibly shield size at 0xC4, edge jump speed at 0xF8/0xFC, item throw strength at 0x118, landing lag at 0x13C..0x150, wall jump speed at 0x164/0x168
#endif
    float slopeAngleLimit;
#ifdef FT_KIRBY_ARTICLE_POOLS
    u8 unk29C[0x48]; // tail of the 0x2E4-byte fighter attribute block copied onto the stack in Starmissile
#endif
};

// HYPOTHESIS: per-fighter extra parameters; unk3C points at floats used by the squat walk (status 0x13 / 0x14 speed limits).
struct ftParamEtc {
    u8 unk00[0x3C];
    float* unk3C;
};

struct ftKindData {
    ftData* data;
    ftParam** params;
    ftParamEtc* ParamEtc;
};

struct ftDataCommon {
    soCommonParam* soCommonParams[2];
    ftCommonParam* ftCommonParams[2];
    void* commonActionEntryScripts;
    void* commonActionExitScripts;
    void* flashOverlayScripts;
    ftShakeData* shakeData;
    u8 unk20[0x8];
    ftJostleData* jostleData;
    u8 unk2C[4];
    // HYPOTHESIS: size-customize scaling exponents read by ftUtil::getReactionMul for param customizes 0 and 1.
    float* sizeCustomizeScale[2];
    union {
        u8 unk38[0x10];
        struct {
            float* rabbitCapScale; // HYPOTHESIS: per-attribute multipliers of the rabbit cap customize
            float* metalScale; // HYPOTHESIS: per-attribute multipliers of the metal customize
            u8 unk40[8];
        };
    };
    GXColor subColors;
    ftEffectCommonData* effectCommonData;
    ftEffectScreenData* effectScreenData;
    ftIkData* ikData;
};
static_assert(sizeof(ftDataCommon) == 0x58, "Class is wrong size!");

struct ftCommonData {
    ftDataCommon* dataCommon;
    ftCommonParam** commonParams;
    ftCommonParamFloat commonFloatParams[2];
    ftCommonParamInt commonIntParams[2];
    ftCommonParamIndefinite commonIndefiniteParams[2];
};

struct wnSimpleData;
struct wnGameWatchNormalWeaponData;
struct wnDummyFighterData;
struct wnKirbyPikminData;
struct wnMarioPumpWaterData;
struct wnPokeFushigisouSeedData;
struct wnPokeZenigameWaterData;
struct ftLucarioAuraBallParam;
struct wnLinkBowData;
struct wnLinkBowArrowData;
struct wnToonLinkBowData;
struct wnToonLinkBowArrowData;
struct wnYoshiTamagoData;
struct wnSheikNeedleData;
struct wnFoxLandMasterShellData;
struct wnSnakeMineData;
struct wnSnakeTrenchMortarData;
struct wnSnakeRpg7Data;
struct wnSnakeCypherData;
struct wnSnakeC4Data;
struct wnSnakeC4SwitchData;
struct wnSnakeNikitaData;
struct wnSnakeLadderData;
struct wnSnakeRgb6Data;
struct wnSnakeRgb6SightData;
struct wnSnakeRgb6ClipData;
struct wnSnakeRgb6BulletData;
struct wnPikachuCloudData;
struct wnPikachuVortexData;
struct wnPokeLizardonRockData;
struct wnPokeLizardonRockStoneData;
struct wnZeldaDeinData;
struct wnZeldaDeinSData;
struct wnZeldaLightingBowData;
struct wnZeldaLightingBowArrowData;
struct wnFoxIllusionData;
struct wnFalcoIllusionData;
struct wnWolfIllusionData;
struct wnDededeStarData;
struct wnDededeJethammerData;
struct wnDededeWaddledooBeamData;
struct wnDededeFollowerData;
struct wnDiddyBarrelData;
struct wnDiddyPeanutsData;
struct wnDiddyExplosionData;
struct wnIkeSwordData;
struct wnPikminPikminData;
struct wnPikminDolfinData;
struct wnPikminChainData;
struct wnSZerosuitParalyzerData;
struct wnSZerosuitWhipData;
struct wnSZerosuitArmorData;
struct wnSamusBombData;
struct wnSamusMissileData;
struct wnSamusSuperMissileData;
struct wnSamusLaserData;
struct wnSamusLaser2Data;
struct wnSamusGunData;
struct wnLucarioQigongData;
struct wnDonkeyBarrelCongaData;
struct wnDonkeySoundEffectData;
struct wnGameWatchRescueData;
struct wnGameWatchParachuteData;
struct wnGameWatchPanelData;
struct wnGameWatchOilData;
struct wnGameWatchOctopusData;
struct wnCaptainBlueFalconData;
struct wnCaptainFinalBgData;

// HYPOTHESIS: leading flag word of a common fighter status's data (the bit with mask 0x8 marks statuses that cannot be trodden on).
struct ftStatusFlagData {
    u32 : 5;
    u32 unk0_5 : 1; // HYPOTHESIS: in these statuses Fighter::processUpdate sets the ground cloud-through from the stick
    u32 : 22;
    u32 noTread : 1;
    u32 : 3;
    u32 : 8;
    u32 unk4_8 : 1; // HYPOTHESIS: in these statuses Fighter::processUpdate skips the shield regeneration
    u32 : 9;
    u32 unk4_18 : 1; // HYPOTHESIS: in these statuses Fighter::checkTransitionStatus consults the stop module
    u32 : 13;
};

class ftCommonDataAccesser {
public:
    const wnDummyFighterData* getDummyFighterData() const;
    const ftLucarioAuraBallParam* getLucarioAuraBallParam(int kind) const;
    float* getLinkBowParam(int kind) const;
    float* getToonLinkBowParam(int kind) const;
    // HYPOTHESIS: the getWeaponData overload family specializes on the article data type.
    template <class Data> const Data* getWeaponData(int kind, bool* resourceGroup) const;
    // HYPOTHESIS: parameter types of the methods below follow the register use and callers (the module map has none).
    static void setupCommon(ftDataCommon* dataCommon);
    static bool isValidDat(ftKind kind);
    static void reset(ftKind kind);
    static void reset();
    ~ftCommonDataAccesser();
    ftParam* getParam(ftKind kind, int variation) const;
    float getParamCommonFloat(u32 variation, u32 id) const;
    s32 getParamCommonInt(u32 variation, u32 id) const;
    s32* getParamCommonIndefinite(u32 variation, u32 id) const;
    u32 getResId(ftKind kind) const;
    u32 getMotionResId(int slotNo, ftKind kind) const;
    u32 getModelResId(int slotNo, ftKind kind, bool flag) const;
    u32 getEtcResId(int slotNo, ftKind kind) const;
    u32 getMotionEtcResId(int slotNo, ftKind kind) const;
    u32 getEntryResId(ftKind kind, u8 colorNo) const;
    u32 getResultResId(ftKind kind, u8 colorNo) const;
    u32 getKirbyResId(int slotNo, ftKind kind, int copyKind) const;
    void* getKirbyBinData(ftKind kind) const;
    void* getKirbyStoneData() const;
    void* getPikachuThunderData() const;
    void* getPokeLizardonBreathParam(ftKind kind) const;
    void* getSZerosuitArmorData() const;
    void* getKoopaBreathParam(ftKind kind) const;
    void* getGKoopaBreathParam(ftKind kind) const;
    void* getGanonBeastData() const;
    const ftStatusFlagData* getFtStatusData(ftKind kind, int status) const;
    wnSimpleData* getSimpleData(ftKind kind, bool* resourceGroup) const;
    u32 getFinalResId(ftKind kind) const;
    ftData* getData(ftKind kind) const;
    ftParam* getParam(ftKind kind) const;
    ftParamEtc* getParamEtc(ftKind kind) const;
    ftParam* getParamCommon() const;
    // The special move parameter groups of Samus (module map names); the kind is the one of the asking fighter.
    void* getParamSamusChargeShot(ftKind kind) const;
    void* getParamSamusSpecialLw(ftKind kind) const;
    void* getParamSamusSpecialS(ftKind kind) const;
    void* getParamSamusFinal(ftKind kind) const;
    // HYPOTHESIS: const qualification; native pointer ABI and getter identity are verified.
    const wnGameWatchNormalWeaponData* getGameWatchNormalWeaponData() const;
    // The static data block of a weapon article: indexed by the weapon number (123, 124, ... in the module maps).
    void* getWeaponData0(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData1(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData2(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData3(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData4(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData5(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData14(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData15(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData18(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData24(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData32(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData38(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData40(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData42(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData43(ftKind kind, bool* resourceGroup) const;
    void* getWeaponData91(ftKind kind, bool* resourceGroup) const;  // HYPOTHESIS: Lucas PK Flash
    void* getWeaponData92(ftKind kind, bool* resourceGroup) const;  // HYPOTHESIS: Lucas PK Fire
    void* getWeaponData93(ftKind kind, bool* resourceGroup) const;  // HYPOTHESIS: Lucas PK Thunder
    void* getWeaponData94(ftKind kind, bool* resourceGroup) const;  // HYPOTHESIS: Lucas PK Thunder bolt
    void* getWeaponData95(ftKind kind, bool* resourceGroup) const;  // HYPOTHESIS: Lucas PK Starstorm
    void* getWeaponData96(ftKind kind, bool* resourceGroup) const;  // HYPOTHESIS: Lucas Rope Snake
    void* getWeaponData121(ftKind kind, bool* resourceGroup) const; // HYPOTHESIS: Ness PK Flash
    void* getWeaponData122(ftKind kind, bool* resourceGroup) const; // HYPOTHESIS: Ness PK Fire
    void* getWeaponData123(ftKind kind, bool* resourceGroup) const; // HYPOTHESIS: Ness PK Thunder
    void* getWeaponData124(ftKind kind, bool* resourceGroup) const; // HYPOTHESIS: Ness PK Thunder bolt
    void* getWeaponData125(ftKind kind, bool* resourceGroup) const; // HYPOTHESIS: Ness PK Starstorm
    void* getWeaponData126(ftKind kind, bool* resourceGroup) const; // HYPOTHESIS: Ness Yo-Yo
    void* getWeaponData127(ftKind kind, bool* resourceGroup) const; // HYPOTHESIS: Ness Yo-Yo head
    void* getWeaponData128(ftKind kind, bool* resourceGroup) const; // Meta Knight Mantle
    void* getWeaponData129(ftKind kind, bool* resourceGroup) const; // HYPOTHESIS: Sonic spring (gimmick jump)
    void* getWeaponData130(ftKind kind, bool* resourceGroup) const; // HYPOTHESIS: Super Sonic
};

template <>
const wnDummyFighterData* ftCommonDataAccesser::getWeaponData<wnDummyFighterData>(int kind, bool* resourceGroup) const;
template <>
const wnKirbyPikminData* ftCommonDataAccesser::getWeaponData<wnKirbyPikminData>(int kind, bool* resourceGroup) const;
template <>
const wnMarioPumpWaterData* ftCommonDataAccesser::getWeaponData<wnMarioPumpWaterData>(int kind, bool* resourceGroup) const;
template <>
const wnPokeFushigisouSeedData* ftCommonDataAccesser::getWeaponData<wnPokeFushigisouSeedData>(int kind, bool* resourceGroup) const;
template <>
const wnPokeZenigameWaterData* ftCommonDataAccesser::getWeaponData<wnPokeZenigameWaterData>(int kind, bool* resourceGroup) const;

template <>
const wnLinkBowData* ftCommonDataAccesser::getWeaponData<wnLinkBowData>(int kind, bool* resourceGroup) const;
template <>
const wnLinkBowArrowData* ftCommonDataAccesser::getWeaponData<wnLinkBowArrowData>(int kind, bool* resourceGroup) const;
template <>
const wnToonLinkBowData* ftCommonDataAccesser::getWeaponData<wnToonLinkBowData>(int kind, bool* resourceGroup) const;
template <>
const wnToonLinkBowArrowData* ftCommonDataAccesser::getWeaponData<wnToonLinkBowArrowData>(int kind, bool* resourceGroup) const;
template <>
const wnYoshiTamagoData* ftCommonDataAccesser::getWeaponData<wnYoshiTamagoData>(int kind, bool* resourceGroup) const;
template <>
const wnSheikNeedleData* ftCommonDataAccesser::getWeaponData<wnSheikNeedleData>(int kind, bool* resourceGroup) const;
template <>
const wnFoxLandMasterShellData* ftCommonDataAccesser::getWeaponData<wnFoxLandMasterShellData>(int kind, bool* resourceGroup) const;

template <>
const wnSnakeMineData* ftCommonDataAccesser::getWeaponData<wnSnakeMineData>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeTrenchMortarData* ftCommonDataAccesser::getWeaponData<wnSnakeTrenchMortarData>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeRpg7Data* ftCommonDataAccesser::getWeaponData<wnSnakeRpg7Data>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeCypherData* ftCommonDataAccesser::getWeaponData<wnSnakeCypherData>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeC4Data* ftCommonDataAccesser::getWeaponData<wnSnakeC4Data>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeC4SwitchData* ftCommonDataAccesser::getWeaponData<wnSnakeC4SwitchData>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeNikitaData* ftCommonDataAccesser::getWeaponData<wnSnakeNikitaData>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeLadderData* ftCommonDataAccesser::getWeaponData<wnSnakeLadderData>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeRgb6Data* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6Data>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeRgb6SightData* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6SightData>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeRgb6ClipData* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6ClipData>(int kind, bool* resourceGroup) const;
template <>
const wnSnakeRgb6BulletData* ftCommonDataAccesser::getWeaponData<wnSnakeRgb6BulletData>(int kind, bool* resourceGroup) const;
template <>
const wnPikachuCloudData* ftCommonDataAccesser::getWeaponData<wnPikachuCloudData>(int kind, bool* resourceGroup) const;
template <>
const wnPikachuVortexData* ftCommonDataAccesser::getWeaponData<wnPikachuVortexData>(int kind, bool* resourceGroup) const;
template <>
const wnPokeLizardonRockData* ftCommonDataAccesser::getWeaponData<wnPokeLizardonRockData>(int kind, bool* resourceGroup) const;
template <>
const wnPokeLizardonRockStoneData* ftCommonDataAccesser::getWeaponData<wnPokeLizardonRockStoneData>(int kind, bool* resourceGroup) const;
template <>
const wnZeldaDeinData* ftCommonDataAccesser::getWeaponData<wnZeldaDeinData>(int kind, bool* resourceGroup) const;
template <>
const wnZeldaDeinSData* ftCommonDataAccesser::getWeaponData<wnZeldaDeinSData>(int kind, bool* resourceGroup) const;
template <>
const wnZeldaLightingBowData* ftCommonDataAccesser::getWeaponData<wnZeldaLightingBowData>(int kind, bool* resourceGroup) const;
template <>
const wnZeldaLightingBowArrowData* ftCommonDataAccesser::getWeaponData<wnZeldaLightingBowArrowData>(int kind, bool* resourceGroup) const;
template <>
const wnFoxIllusionData* ftCommonDataAccesser::getWeaponData<wnFoxIllusionData>(int kind, bool* resourceGroup) const;
template <>
const wnFalcoIllusionData* ftCommonDataAccesser::getWeaponData<wnFalcoIllusionData>(int kind, bool* resourceGroup) const;
template <>
const wnWolfIllusionData* ftCommonDataAccesser::getWeaponData<wnWolfIllusionData>(int kind, bool* resourceGroup) const;
template <>
const wnDededeStarData* ftCommonDataAccesser::getWeaponData<wnDededeStarData>(int kind, bool* resourceGroup) const;
template <>
const wnDededeJethammerData* ftCommonDataAccesser::getWeaponData<wnDededeJethammerData>(int kind, bool* resourceGroup) const;
template <>
const wnDededeWaddledooBeamData* ftCommonDataAccesser::getWeaponData<wnDededeWaddledooBeamData>(int kind, bool* resourceGroup) const;
template <>
const wnDededeFollowerData* ftCommonDataAccesser::getWeaponData<wnDededeFollowerData>(int kind, bool* resourceGroup) const;
template <>
const wnDiddyBarrelData* ftCommonDataAccesser::getWeaponData<wnDiddyBarrelData>(int kind, bool* resourceGroup) const;
template <>
const wnDiddyPeanutsData* ftCommonDataAccesser::getWeaponData<wnDiddyPeanutsData>(int kind, bool* resourceGroup) const;
template <>
const wnDiddyExplosionData* ftCommonDataAccesser::getWeaponData<wnDiddyExplosionData>(int kind, bool* resourceGroup) const;
template <>
const wnIkeSwordData* ftCommonDataAccesser::getWeaponData<wnIkeSwordData>(int kind, bool* resourceGroup) const;
template <>
const wnPikminPikminData* ftCommonDataAccesser::getWeaponData<wnPikminPikminData>(int kind, bool* resourceGroup) const;
template <>
const wnPikminDolfinData* ftCommonDataAccesser::getWeaponData<wnPikminDolfinData>(int kind, bool* resourceGroup) const;
template <>
const wnPikminChainData* ftCommonDataAccesser::getWeaponData<wnPikminChainData>(int kind, bool* resourceGroup) const;
template <>
const wnSZerosuitParalyzerData* ftCommonDataAccesser::getWeaponData<wnSZerosuitParalyzerData>(int kind, bool* resourceGroup) const;
template <>
const wnSZerosuitWhipData* ftCommonDataAccesser::getWeaponData<wnSZerosuitWhipData>(int kind, bool* resourceGroup) const;
template <>
const wnSZerosuitArmorData* ftCommonDataAccesser::getWeaponData<wnSZerosuitArmorData>(int kind, bool* resourceGroup) const;
template <>
const wnSamusBombData* ftCommonDataAccesser::getWeaponData<wnSamusBombData>(int kind, bool* resourceGroup) const;
template <>
const wnSamusMissileData* ftCommonDataAccesser::getWeaponData<wnSamusMissileData>(int kind, bool* resourceGroup) const;
template <>
const wnSamusSuperMissileData* ftCommonDataAccesser::getWeaponData<wnSamusSuperMissileData>(int kind, bool* resourceGroup) const;
template <>
const wnSamusLaserData* ftCommonDataAccesser::getWeaponData<wnSamusLaserData>(int kind, bool* resourceGroup) const;
template <>
const wnSamusLaser2Data* ftCommonDataAccesser::getWeaponData<wnSamusLaser2Data>(int kind, bool* resourceGroup) const;
template <>
const wnSamusGunData* ftCommonDataAccesser::getWeaponData<wnSamusGunData>(int kind, bool* resourceGroup) const;
template <>
const wnLucarioQigongData* ftCommonDataAccesser::getWeaponData<wnLucarioQigongData>(int kind, bool* resourceGroup) const;
template <>
const wnDonkeyBarrelCongaData* ftCommonDataAccesser::getWeaponData<wnDonkeyBarrelCongaData>(int kind, bool* resourceGroup) const;
template <>
const wnDonkeySoundEffectData* ftCommonDataAccesser::getWeaponData<wnDonkeySoundEffectData>(int kind, bool* resourceGroup) const;
template <>
const wnGameWatchRescueData* ftCommonDataAccesser::getWeaponData<wnGameWatchRescueData>(int kind, bool* resourceGroup) const;
template <>
const wnGameWatchParachuteData* ftCommonDataAccesser::getWeaponData<wnGameWatchParachuteData>(int kind, bool* resourceGroup) const;
template <>
const wnGameWatchPanelData* ftCommonDataAccesser::getWeaponData<wnGameWatchPanelData>(int kind, bool* resourceGroup) const;
template <>
const wnGameWatchOilData* ftCommonDataAccesser::getWeaponData<wnGameWatchOilData>(int kind, bool* resourceGroup) const;
template <>
const wnGameWatchOctopusData* ftCommonDataAccesser::getWeaponData<wnGameWatchOctopusData>(int kind, bool* resourceGroup) const;
template <>
const wnGameWatchNormalWeaponData* ftCommonDataAccesser::getWeaponData<wnGameWatchNormalWeaponData>(int kind, bool* resourceGroup) const;
template <>
const wnCaptainBlueFalconData* ftCommonDataAccesser::getWeaponData<wnCaptainBlueFalconData>(int kind, bool* resourceGroup) const;
template <>
const wnCaptainFinalBgData* ftCommonDataAccesser::getWeaponData<wnCaptainFinalBgData>(int kind, bool* resourceGroup) const;

extern ftCommonData g_ftCommonData;
extern ftCommonDataAccesser g_ftCommonDataAccesser;
