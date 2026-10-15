// Local BrawlHeaders shadow: expose the Final Smash resource identifier and extend parameters.
#pragma once

#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <so/so_common_data_accesser.h>
#include <types.h>

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
    u8 unk40[0x10]; // HYPOTHESIS: possibly two bone-float tables and two bone-reference tables (0x40..0x4C in the moveset header order)
    soAnimCmdDisguiseList* entryActionOverrides; // 0x50: per-action replacement entry scripts
    soAnimCmdDisguiseList* exitActionOverrides;  // 0x54: per-action replacement exit scripts
    u8 unk58[0x24]; // HYPOTHESIS: possibly unknown-22, bone floats 3, unknown-24, static articles, entry article, two unknowns and two flag words (0x58..0x78 in the moveset header order)
    void* extendParam[6];
};

// The fighter attribute block (an array of 4-byte entries, 0x2E4 bytes in the moveset file). modelScale lines up with the
// attribute list's size entry, and 0x1FC..0x208 are the camera extents read by ftManager::getCameraRange.
struct ftParam {
    u8 unk0[0xB4]; // HYPOTHESIS: possibly the walk, dash, run, jump, air-drift and gravity attributes plus weight at 0xB0
    float modelScale;
    u8 unkB8[0x1E0]; // HYPOTHESIS: possibly shield size at 0xC4, edge jump speed at 0xF8/0xFC, item throw strength at 0x118, landing lag at 0x13C..0x150, wall jump speed at 0x164/0x168
    float slopeAngleLimit;
};

struct ftParamEtc {

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
    u8 unk2C[0x1C];
    GXColor subColors;
    ftEffectCommonData* effectCommonData;
    ftEffectScreenData* effectScreenData;
    ftIkData* ikData;
};

struct ftCommonData {
    ftDataCommon* dataCommon;
    ftCommonParam** commonParams;
    ftCommonParamFloat commonFloatParams[2];
    ftCommonParamInt commonIntParams[2];
    ftCommonParamIndefinite commonIndefiniteParams[2];
};

struct wnSimpleData;
struct wnGameWatchNormalWeaponData;

class ftCommonDataAccesser {
public:
    wnSimpleData* getSimpleData(ftKind kind, bool* resourceGroup) const;
    u32 getFinalResId(ftKind kind) const;
    ftData* getData(ftKind kind) const;
    ftParam* getParam(ftKind kind) const;
    ftParam* getParamCommon() const;
    // HYPOTHESIS: const qualification; native pointer ABI and getter identity are verified.
    const wnGameWatchNormalWeaponData* getGameWatchNormalWeaponData() const;
    // The static data block of a weapon article: indexed by the weapon number (123, 124, ... in the module maps).
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
};

extern ftCommonData g_ftCommonData;
extern ftCommonDataAccesser g_ftCommonDataAccesser;
