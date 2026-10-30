#include <types.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <it/it_create.h>
#include <mt/mt_prng.h>
#include <stdio.h>

// gm_common_b: gmlib melee item switch lookup, trophy coin counters, collection rarity bits and the trophy, sticker and
// CD lotteries (0x80050BB0-0x80051F28 of main.dol). The tables that are read here are in the data of gm_common.cpp:
// lbl_8042B858 is a larger data block of which the song ids start at word 0x15A, the hidden-song stage keys at 0x1C0 and
// the stage list at 0x212 (the offsets are what the original code adds to that base).

extern const u32 lbl_8042B858[];
extern const int lbl_8042BF58[];
extern const int lbl_8042C0A0[];

// Stand-ins for main.dol functions of the neighbouring gm_common clusters.
// gmdatacollection::gmLotFigureToCollection(rare bit set): picks a figure id (HYPOTHESIS: parameter and return type).
extern "C" u16 fn_8005630C(u8 rareBits);
// gmdatacollection::gmLotSealToCollection(rare bit set, rarity): picks a sticker id (HYPOTHESIS: parameter and return type).
extern "C" u16 fn_80056518(u8 rareBits, u8 rarity);
// GameGlobal::getStageAppFlag(): the bit set of the unlocked stages (the map names it getStageAppFlag).
extern "C" u32* fn_8004E594(GameGlobal* global);

// HYPOTHESIS: the item switch of a match is three bit tables. 0 is the item bit set (64 bits, the high word comes
// first in memory), 1 the Pokemon word and 2 the assist trophy word. A switch bit of -1 means that the item has no switch.
enum ItemSwitchTable {
    ItemSwitchTable_Item = 0,
    ItemSwitchTable_Pokemon = 1,
    ItemSwitchTable_Assist = 2,
};

// MATCH-ONLY: gmItSwitch has byte alignment in the shared header, so passing it by value copies it byte by byte. The
// original copies four words; this word view of it (same 0x10 bytes) reproduces that.
struct gmItSwitchWords {
    u64 m_item;
    u32 m_pokemon;
    u32 m_assist;
    gmItSwitchWords(const gmItSwitchWords& other) : m_item(other.m_item), m_pokemon(other.m_pokemon), m_assist(other.m_assist) {
    }
};

// Whether the item may appear in a match with the given item switch (gmItSwitch). Items that have no switch bit are always allowed.
bool gmCheckItemSwitch(gmItSwitchWords itemSwitch, itKind kind) {
    int bit = -1;
    int table;
    switch (kind) {
    case Item_AssistTrophy: bit = 1; table = ItemSwitchTable_Item; break; // m_assistTrophy
    case Item_FranklinBadge: bit = 47; table = ItemSwitchTable_Item; break; // m_franklinBadge
    case Item_BananaPeel: bit = 41; table = ItemSwitchTable_Item; break; // m_bananaPeel
    case Item_Barrel: bit = 3; table = ItemSwitchTable_Item; break; // m_containers
    case Item_BeamSword: bit = 19; table = ItemSwitchTable_Item; break; // m_beamSword
    case Item_Bill: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_BobOmb: bit = 30; table = ItemSwitchTable_Item; break; // m_bobOmb
    case Item_Crate: bit = 3; table = ItemSwitchTable_Item; break; // m_containers
    case Item_Bumper: bit = 42; table = ItemSwitchTable_Item; break; // m_bumper
    case Item_Capsule: bit = 3; table = ItemSwitchTable_Item; break; // m_containers
    case Item_RollingCrate: bit = 3; table = ItemSwitchTable_Item; break; // m_containers
    case Item_CD: bit = 51; table = ItemSwitchTable_Item; break; // m_cd
    case Item_GooeyBomb: bit = 32; table = ItemSwitchTable_Item; break; // m_gooeyBomb
    case Item_CrackerLauncher: bit = 29; table = ItemSwitchTable_Item; break; // m_crackerLauncher
    case Item_CrackerLauncher_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Coin: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_SuperspicyCurry: bit = 16; table = ItemSwitchTable_Item; break; // m_superspicyCurry
    case Item_SuperspicyCurry_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_DekuNut: bit = 34; table = ItemSwitchTable_Item; break; // m_dekuNut
    case Item_MrSaturn: bit = 39; table = ItemSwitchTable_Item; break; // m_mrSaturn
    case Item_Dragoon_Part: bit = 9; table = ItemSwitchTable_Item; break; // m_dragoonParts
    case Item_Dragoon_Set: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Dragoon_Sight: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Trophy: bit = 50; table = ItemSwitchTable_Item; break; // m_trophy
    case Item_FireFlower: bit = 28; table = ItemSwitchTable_Item; break; // m_fireFlower
    case Item_FireFlower_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Freezie: bit = 35; table = ItemSwitchTable_Item; break; // m_freezie
    case Item_GoldenHammer: bit = 25; table = ItemSwitchTable_Item; break; // m_goldenHammer
    case Item_GreenShell: bit = 40; table = ItemSwitchTable_Item; break; // m_greenShell
    case Item_Hammer: bit = 24; table = ItemSwitchTable_Item; break; // m_hammer
    case Item_Hammer_Head: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Fan: bit = 21; table = ItemSwitchTable_Item; break; // m_fan
    case Item_HeartContainer: bit = 8; table = ItemSwitchTable_Item; break; // m_heartContainer
    case Item_HomeRunBat: bit = 20; table = ItemSwitchTable_Item; break; // m_homeRunBat
    case Item_PartyBall: bit = 3; table = ItemSwitchTable_Item; break; // m_containers
    case Item_Manaphy_Heart: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_MaximTomato: bit = 7; table = ItemSwitchTable_Item; break; // m_maximTomato
    case Item_PoisonMushroom: bit = 11; table = ItemSwitchTable_Item; break; // m_poisonMushroom
    case Item_SuperMushroom: bit = 10; table = ItemSwitchTable_Item; break; // m_superMushroom
    case Item_MetalBox: bit = 14; table = ItemSwitchTable_Item; break; // m_metalBox
    case Item_Hothead: bit = 38; table = ItemSwitchTable_Item; break; // m_hothead
    case Item_Pitfall: bit = 37; table = ItemSwitchTable_Item; break; // m_pitfall
    case Item_PokeBall: bit = 2; table = ItemSwitchTable_Item; break; // m_pokeBall
    case Item_BlastBox: bit = 4; table = ItemSwitchTable_Item; break; // m_blastBox
    case Item_RayGun: bit = 27; table = ItemSwitchTable_Item; break; // m_rayGun
    case Item_RayGun_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_LipStick: bit = 22; table = ItemSwitchTable_Item; break; // m_lipStick
    case Item_LipStick_Flower: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_LipStick_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Sandbag: bit = 5; table = ItemSwitchTable_Item; break; // m_sandBag
    case Item_ScrewAttack: bit = 48; table = ItemSwitchTable_Item; break; // m_screwAttack
    case Item_Sticker: bit = 49; table = ItemSwitchTable_Item; break; // m_sticker
    case Item_MotionSensorBomb: bit = 31; table = ItemSwitchTable_Item; break; // m_motionSensorBomb
    case Item_Timer: bit = 17; table = ItemSwitchTable_Item; break; // m_timer
    case Item_SmartBomb: bit = 33; table = ItemSwitchTable_Item; break; // m_smartBomb
    case Item_SmashBall: bit = 0; table = ItemSwitchTable_Item; break; // m_smashBall
    case Item_SmokeBall: bit = 36; table = ItemSwitchTable_Item; break; // m_smokeBall
    case Item_Spring: bit = 43; table = ItemSwitchTable_Item; break; // m_spring
    case Item_StarRod: bit = 23; table = ItemSwitchTable_Item; break; // m_starRod
    case Item_StarRod_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_SoccerBall: bit = 45; table = ItemSwitchTable_Item; break; // m_soccerBall
    case Item_SuperScope: bit = 26; table = ItemSwitchTable_Item; break; // m_superScope
    case Item_SuperScope_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Star: bit = 13; table = ItemSwitchTable_Item; break; // m_starman
    case Item_Food: bit = 6; table = ItemSwitchTable_Item; break; // m_food
    case Item_TeamHealer: bit = 46; table = ItemSwitchTable_Item; break; // m_teamHealer
    case Item_Lightning: bit = 18; table = ItemSwitchTable_Item; break; // m_lightning
    case Item_Unira: bit = 44; table = ItemSwitchTable_Item; break; // m_unira
    case Item_BunnyHood: bit = 15; table = ItemSwitchTable_Item; break; // m_bunnyHood
    case Item_WarpStar: bit = 12; table = ItemSwitchTable_Item; break; // m_warpStar
    case Item_Stage_Vegetable: bit = 6; table = ItemSwitchTable_Item; break; // m_food
    case Item_DiddyKong_Peanut_Seed: bit = 6; table = ItemSwitchTable_Item; break; // m_food
    case Item_Pokemon_Torchic: bit = 0; table = ItemSwitchTable_Pokemon; break; // m_torchic
    case Item_Pokemon_Celebi: bit = 1; table = ItemSwitchTable_Pokemon; break; // m_celebi
    case Item_Pokemon_Chikorita: bit = 2; table = ItemSwitchTable_Pokemon; break; // m_chikorita
    case Item_Pokemon_Chikorita_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_Entei: bit = 3; table = ItemSwitchTable_Pokemon; break; // m_entei
    case Item_Pokemon_Moltres: bit = 4; table = ItemSwitchTable_Pokemon; break; // m_moltres
    case Item_Pokemon_Munchlax: bit = 5; table = ItemSwitchTable_Pokemon; break; // m_munchlax
    case Item_Pokemon_Deoxys: bit = 6; table = ItemSwitchTable_Pokemon; break; // m_deoxys
    case Item_Pokemon_Groudon: bit = 7; table = ItemSwitchTable_Pokemon; break; // m_groudon
    case Item_Pokemon_Gulpin: bit = 8; table = ItemSwitchTable_Pokemon; break; // m_gulpin
    case Item_Pokemon_Staryu: bit = 9; table = ItemSwitchTable_Pokemon; break; // m_staryu
    case Item_Pokemon_Staryu_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_HoOh: bit = 10; table = ItemSwitchTable_Pokemon; break; // m_hoOh
    case Item_Pokemon_HoOh_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_Jirachi: bit = 11; table = ItemSwitchTable_Pokemon; break; // m_jirachi
    case Item_Pokemon_Snorlax: bit = 12; table = ItemSwitchTable_Pokemon; break; // m_snorlax
    case Item_Pokemon_Bellossom: bit = 13; table = ItemSwitchTable_Pokemon; break; // m_bellosom
    case Item_Pokemon_Kyogre: bit = 14; table = ItemSwitchTable_Pokemon; break; // m_kyogre
    case Item_Pokemon_Kyogre_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_LatiasLatios: bit = 15; table = ItemSwitchTable_Pokemon; break; // m_latiasLatios
    case Item_Pokemon_Lugia: bit = 16; table = ItemSwitchTable_Pokemon; break; // m_lugia
    case Item_Pokemon_Lugia_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_Manaphy: bit = 17; table = ItemSwitchTable_Pokemon; break; // m_manaphy
    case Item_Pokemon_Weavile: bit = 18; table = ItemSwitchTable_Pokemon; break; // m_weavile
    case Item_Pokemon_Electrode: bit = 19; table = ItemSwitchTable_Pokemon; break; // m_electrode
    case Item_Pokemon_Metagross: bit = 20; table = ItemSwitchTable_Pokemon; break; // m_metagross
    case Item_Pokemon_Mew: bit = 21; table = ItemSwitchTable_Pokemon; break; // m_mew
    case Item_Pokemon_Meowth: bit = 22; table = ItemSwitchTable_Pokemon; break; // m_meowth
    case Item_Pokemon_Meowth_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_Piplup: bit = 23; table = ItemSwitchTable_Pokemon; break; // m_piplup
    case Item_Pokemon_Togepi: bit = 24; table = ItemSwitchTable_Pokemon; break; // m_togepi
    case Item_Pokemon_Goldeen: bit = 25; table = ItemSwitchTable_Pokemon; break; // m_goldeen
    case Item_Pokemon_Gardevoir: bit = 26; table = ItemSwitchTable_Pokemon; break; // m_gardevoir
    case Item_Pokemon_Wobbuffet: bit = 27; table = ItemSwitchTable_Pokemon; break; // m_wobuffet
    case Item_Pokemon_Suicune: bit = 28; table = ItemSwitchTable_Pokemon; break; // m_suicune
    case Item_Pokemon_Bonsly: bit = 29; table = ItemSwitchTable_Pokemon; break; // m_bonsly
    case Item_Assist_Andross: bit = 0; table = ItemSwitchTable_Assist; break; // m_andross
    case Item_Assist_Andross_Shot: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Barbara: bit = 1; table = ItemSwitchTable_Assist; break; // m_barbara
    case Item_Assist_GrayFox: bit = 2; table = ItemSwitchTable_Assist; break; // m_grayFox
    case Item_Assist_RayMKII: bit = 3; table = ItemSwitchTable_Assist; break; // m_rayMKII
    case Item_Assist_RayMKII_Bomb: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_RayMKII_Gun: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_SamuraiGoroh: bit = 4; table = ItemSwitchTable_Assist; break; // m_samuraiGoroh
    case Item_Assist_Devil: bit = 5; table = ItemSwitchTable_Assist; break; // m_devil
    case Item_Assist_Excitebike: bit = 6; table = ItemSwitchTable_Assist; break; // m_excitebike
    case Item_Assist_Jeff: bit = 7; table = ItemSwitchTable_Assist; break; // m_jeff
    case Item_Assist_Jeff_PencilBullet: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Jeff_PencilRocket: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Lakitu: bit = 8; table = ItemSwitchTable_Assist; break; // m_lakitu
    case Item_Assist_KnuckleJoe: bit = 9; table = ItemSwitchTable_Assist; break; // m_knuckleJoe
    case Item_Assist_KnuckleJoe_Shot: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_HammerBro: bit = 10; table = ItemSwitchTable_Assist; break; // m_hammerBro
    case Item_Assist_HammerBro_Hammer: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Helirin: bit = 11; table = ItemSwitchTable_Assist; break; // m_helirin
    case Item_Assist_KatAna: bit = 12; table = ItemSwitchTable_Assist; break; // m_katAna
    case Item_Assist_KatAna_Ana: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_JillDozer: bit = 13; table = ItemSwitchTable_Assist; break; // m_jillDozer
    case Item_Assist_Lyn: bit = 14; table = ItemSwitchTable_Assist; break; // m_lyn
    case Item_Assist_LittleMac: bit = 15; table = ItemSwitchTable_Assist; break; // m_littleMac
    case Item_Assist_Metroid: bit = 16; table = ItemSwitchTable_Assist; break; // m_metroid
    case Item_Assist_Nintendog: bit = 17; table = ItemSwitchTable_Assist; break; // m_nintendog
    case Item_Assist_Nintendog_Full: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_MrResetti: bit = 18; table = ItemSwitchTable_Assist; break; // m_mrResetti
    case Item_Assist_Isaac: bit = 19; table = ItemSwitchTable_Assist; break; // m_isaac
    case Item_Assist_Isaac_Shot: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Saki: bit = 20; table = ItemSwitchTable_Assist; break; // m_saki
    case Item_Assist_Saki_Shot1: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Saki_Shot2: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Shadow: bit = 21; table = ItemSwitchTable_Assist; break; // m_shadow
    case Item_Assist_War_Infantry: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_War_Infantry_Shot: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Starfy: bit = 22; table = ItemSwitchTable_Assist; break; // m_starfy
    case Item_Assist_War_Tank: bit = 23; table = ItemSwitchTable_Assist; break; // m_infantryTank
    case Item_Assist_War_Tank_Shot: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Tingle: bit = 24; table = ItemSwitchTable_Assist; break; // m_tingle
    case Item_Assist_Lakitu_Spiny: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Waluigi: bit = 25; table = ItemSwitchTable_Assist; break; // m_waluigi
    case Item_Assist_DrWright: bit = 26; table = ItemSwitchTable_Assist; break; // m_drWright
    case Item_Assist_DrWright_Building: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    }
    bool enabled = true;
    if (bit != -1) {
        u32 value = 0;
        switch (table) {
        case ItemSwitchTable_Item:
            if (bit < 32) {
                value = static_cast<u32>(itemSwitch.m_item);
            } else {
                value = static_cast<u32>(itemSwitch.m_item >> 32);
                bit -= 32;
            }
            break;
        case ItemSwitchTable_Pokemon:
            value = itemSwitch.m_pokemon;
            break;
        case ItemSwitchTable_Assist:
            value = itemSwitch.m_assist;
            break;
        }
        enabled = (value & (1 << bit)) != 0;
    }
    return enabled;
}

// gmCheckGlobalItemSwitch: whether the item may appear in the match that is being set up (the item switch of the melee
// settings).
extern "C" bool fn_8005136C(itKind kind) {
    return gmCheckItemSwitch(*reinterpret_cast<gmItSwitchWords*>(&g_GameGlobal->m_modeMelee->m_meleeInitData.m_itSwitch), kind);
}

// The figure coin balance of the record (HYPOTHESIS: the coins of the coin shooter).
u32 gmGetCoinNum() {
    return *g_GameGlobal->getGlobalRecordFigCoinDatap();
}

// Sets the coin balance. HYPOTHESIS: the second counter is the highest balance reached, so it is raised when the new
// balance is above it.
u32 gmSetCoinNum(u32 num) {
    u32* coin = g_GameGlobal->getGlobalRecordFigCoinDatap();
    u32* gotCoin = g_GameGlobal->getGlobalRecordGetFigCoinDatap();
    if (num > *gotCoin) {
        *gotCoin = num;
    }
    *coin = num;
    return num;
}

// gmAddCoin: adds coins to the balance (it stops at 99999) and to the second counter (it stops at the u32 maximum).
// HYPOTHESIS: the second counter is the total of all coins gained. Callers declare this as fn_80051440.
extern "C" u32 fn_80051440(u32 num) {
    u32* coin = g_GameGlobal->getGlobalRecordFigCoinDatap();
    u32* gotCoin = g_GameGlobal->getGlobalRecordGetFigCoinDatap();
    *coin = 99999 - *coin > num ? *coin + num : 99999;
    *gotCoin = 0xFFFFFFFF - *gotCoin > num ? *gotCoin + num : 0xFFFFFFFF;
    return *coin;
}

// Takes coins off the balance; it does not go below zero.
u32 gmSubCoin(u32 num) {
    u32* coin = g_GameGlobal->getGlobalRecordFigCoinDatap();
    u32 balance = *coin;
    u32 result = balance >= num ? balance - num : 0;
    *coin = result;
    return result;
}

// How many coins a match gives (the coin shooter pays them out): a rate per match kind times the player count.
u32 gmCountMeleeCoin(int kind, int playerCount) {
    // HYPOTHESIS: one coin rate per match kind.
    float rate[8] = { 1.0f, 0.8f, 0.8f, 0.6f, 1.0f, 0.9f, 0.8f, 0.7f };
    if (playerCount < 1) {
        playerCount = 1;
    }
    float r = rate[kind];
    return 10.0f * (r * (0.11f * playerCount));
}

// HYPOTHESIS: the bytes 0x2E4 and 0x2E5 of the record are rarity bit sets for the trophy and for the sticker lottery;
// the melee settings keep a copy of each (bytes 0x60 and 0x61 of the melee data). They are not declared in the headers.
static inline u8& gmRecordFigureRareBit(gmGlobalRecord* record) {
    return reinterpret_cast<u8*>(record)[0x2E4];
}

static inline u8& gmRecordSealRareBit(gmGlobalRecord* record) {
    return reinterpret_cast<u8*>(record)[0x2E5];
}

static inline u8& gmMeleeFigureRareBit(gmGlobalModeMelee* melee) {
    return reinterpret_cast<u8*>(melee)[0x60];
}

static inline u8& gmMeleeSealRareBit(gmGlobalModeMelee* melee) {
    return reinterpret_cast<u8*>(melee)[0x61];
}

u8 gmGetRecFigureRareBit() {
    return gmRecordFigureRareBit(g_GameGlobal->getGlobalRecordGameDatap());
}

// gmSetupMeleeFigureRareBit: copies the trophy rarity bits of the record to the melee settings.
extern "C" void fn_800515E8() {
    u8 bits = gmRecordFigureRareBit(g_GameGlobal->getGlobalRecordGameDatap());
    gmMeleeFigureRareBit(g_GameGlobal->m_modeMelee) = bits;
}

void gmSetRecFigureRareBit(u8 bit) {
    gmRecordFigureRareBit(g_GameGlobal->getGlobalRecordGameDatap()) |= bit;
}

u8 gmGetRecSealRareBit() {
    return gmRecordSealRareBit(g_GameGlobal->getGlobalRecordGameDatap());
}

// gmSetupMeleeSealRareBit: the same for the sticker rarity bits.
extern "C" void fn_80051680() {
    u8 bits = gmRecordSealRareBit(g_GameGlobal->getGlobalRecordGameDatap());
    gmMeleeSealRareBit(g_GameGlobal->m_modeMelee) = bits;
}

void gmSetRecSealRareBit(u8 bit) {
    gmRecordSealRareBit(g_GameGlobal->getGlobalRecordGameDatap()) |= bit;
}

// gmLotFigure: lotteries a trophy with the rarity bits of the match.
extern "C" u16 fn_800516F0() {
    return fn_8005630C(gmMeleeFigureRareBit(g_GameGlobal->m_modeMelee));
}

u16 gmLotFigureForCoinshooter() {
    return fn_8005630C(gmMeleeFigureRareBit(g_GameGlobal->m_modeMelee) | 1);
}

u16 gmCheckFigureRarity(u32 id) {
    return gmCheckFigureRarityToCollection(id);
}

// HYPOTHESIS: rows are the sticker rarities 1-5, columns the lottery kind (the callers pass 0-2, the sticker album
// reset passes 2); the entries are chances per hundred.
u16 gmLotSealToBitset(u8 rareBits, u8 kind) {
    int chance[5][5] = {
        { 70, 43, 30, 23, 10 },
        { 30, 35, 30, 25, 15 },
        { 0, 15, 20, 25, 30 },
        { 0, 5, 15, 20, 30 },
        { 0, 2, 5, 7, 10 },
    };
    int total = 0;
    for (int i = 0; i < 5; i++) {
        total += chance[i][kind];
    }
    int roll = randi(total);
    int rarity = 0;
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += chance[i][kind];
        if (roll < sum) {
            rarity = i;
            break;
        }
    }
    return fn_80056518(rareBits, rarity + 1);
}

// gmLotSeal: lotteries a sticker with the rarity bits of the match.
extern "C" u16 fn_80051844(u8 kind) {
    return gmLotSealToBitset(gmMeleeSealRareBit(g_GameGlobal->m_modeMelee), kind);
}

// A bit of the CD (song) acquired flags or stage flags: 32 bits per word.
static inline bool gmTestFlagBit(const u32* flags, int bit) {
    return (flags[bit / 32] & (1 << (bit % 32))) != 0;
}

// HYPOTHESIS: the song table has 0x40 entries, two flag bits each in the BGM app flags (acquired: the even bit); the
// last 16 are hidden songs that belong to stages. Entry stageKeys[i - 0x30] names the stage that unlocks hidden song i
// and the stage list gives the bit of that stage in the stage app flags.
// Picks a random CD for the CD item: one that the player does not have yet; when all are there, any one.
u32 gmLotCD() {
    int allIds[102];
    int freeIds[102];
    int freeCount = 0;
    int allCount = 0;
    int* freeNext = freeIds;
    int* allNext = allIds;
    int i;
    int j;
    for (i = 0; i <= 0x2F; i++) {
        if (!gmTestFlagBit(reinterpret_cast<u32*>(g_GameGlobal->getBGMAppFlag()), i * 2)) {
            *freeNext++ = lbl_8042B858[0x15A + i];
            freeCount++;
        }
        *allNext++ = lbl_8042B858[0x15A + i];
        allCount++;
    }
    for (i = 0x30; i <= 0x3F; i++) {
        u32 available = 1;
        for (j = 0; j < 12; j++) {
            if (static_cast<int>(lbl_8042B858[0x1C0 + i - 0x30]) == static_cast<int>(lbl_8042B858[0x212 + j])) {
                available = gmTestFlagBit(fn_8004E594(g_GameGlobal), j);
                break;
            }
        }
        if (available == 1) {
            if (!gmTestFlagBit(reinterpret_cast<u32*>(g_GameGlobal->getBGMAppFlag()), i * 2)) {
                freeIds[freeCount++] = lbl_8042B858[0x15A + i];
            }
            allIds[allCount++] = lbl_8042B858[0x15A + i];
        }
    }
    if (freeCount != 0) {
        return freeIds[randi(freeCount)];
    }
    return allIds[randi(allCount)];
}

// Whether every CD that can be found has been acquired (hidden songs of locked stages do not count).
// gmCheckRandomCDAllAcquired
extern "C" bool fn_80051A6C() {
    int i;
    int j;
    int bit = 0;
    u32 available;
    for (i = 0; i <= 0x2F; i++, bit += 2) {
        if (!gmTestFlagBit(reinterpret_cast<u32*>(g_GameGlobal->getBGMAppFlag()), bit)) {
            return false;
        }
    }
    bit = 0x60;
    for (i = 0x30; i <= 0x3F; i++, bit += 2) {
        available = 1;
        const int* stage = lbl_8042C0A0;
        for (j = 0; j < 12; j++, stage++) {
            if (lbl_8042BF58[i - 0x30] == *stage) {
                available = gmTestFlagBit(fn_8004E594(g_GameGlobal), j);
                break;
            }
        }
        if (available == 1) {
            if (!gmTestFlagBit(reinterpret_cast<u32*>(g_GameGlobal->getBGMAppFlag()), bit)) {
                return false;
            }
        }
    }
    return true;
}

// HYPOTHESIS: one halfword per figure or sticker in the record (figures from 0x842, stickers from 0xD58): bit 15
// 'owned', bits 12-14 the viewed/info flags and bits 0-9 the number owned (stops at 999). This is a view of the part of
// the record that is not declared in the header yet.
struct gmRecordCollectionView {
    char _0[0x842];
    u16 m_figureFlags[0x28B];
    u16 m_sealFlags[1]; // 0xD58, more entries follow
};

static inline gmRecordCollectionView* gmRecordCollection(gmGlobalRecord* record) {
    return reinterpret_cast<gmRecordCollectionView*>(record);
}

static inline void gmSetRecordFigureRareBit(u8 bits) {
    gmRecordFigureRareBit(g_GameGlobal->getGlobalRecordGameDatap()) = bits;
}

static inline void gmSetRecordSealRareBit(u8 bits) {
    gmRecordSealRareBit(g_GameGlobal->getGlobalRecordGameDatap()) = bits;
}

// Resets the trophy collection and gives the player one random trophy.
void gmRecFigureAllInit() {
    int count = gmGetFigMaxCount();
    for (int i = 0; i < count; i++) {
        gmRecordCollection(g_GameGlobal->m_record)->m_figureFlags[i] = 0;
    }
    reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordGameDatap())[0x2E4] = 2;
    int index = gmFig_Id2Index(fn_8005630C(2));
    u16 entry = gmRecordCollection(g_GameGlobal->m_record)->m_figureFlags[index];
    int number = (entry & 0x3FF) + 1;
    if (number >= 1000) {
        number = 999;
    }
    entry = (entry & 0xF000) | 0x8000;
    entry |= number;
    gmRecordCollection(g_GameGlobal->m_record)->m_figureFlags[index] = entry;
}

// Resets the sticker collection and gives the player one random sticker.
void gmRecSealAllInit() {
    int count = gmGetSealMaxCount();
    for (int i = 0; i < count; i++) {
        gmRecordCollection(g_GameGlobal->m_record)->m_sealFlags[i] = 0;
    }
    reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordGameDatap())[0x2E5] = 4;
    int index = gmSeal_Id2Index(gmLotSealToBitset(4, 2));
    u16 entry = gmRecordCollection(g_GameGlobal->m_record)->m_sealFlags[index];
    int number = (entry & 0x3FF) + 1;
    if (number >= 1000) {
        number = 999;
    }
    entry = (entry & 0xF000) | 0x8000;
    entry |= number;
    gmRecordCollection(g_GameGlobal->m_record)->m_sealFlags[index] = entry;
}

u8 gmGetMeleeCPULevel(u8 level) {
    return lbl_8042B858[level];
}
