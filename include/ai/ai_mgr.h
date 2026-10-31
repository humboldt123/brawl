#pragma once

// Local shadow of BrawlHeaders/ai/ai_mgr.h: expose native danger-zone cleanup.

#include <StaticAssert.h>
#include <ft/ft_manager.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <types.h>

class aiInput;
class aiStat;
struct AiClBlock;
class aiWeaponMgr;
class aiEnemyMgr;
class IpHuman;

// One row of the AI "refer" parameter table (0x36 rows of 0x20 bytes).
// Field names follow the aiParam getters that read them.
struct aiReferPar {
    float m_downAtkRate;   // 0x00: aiParam::get_down_atk_r
    float m_cliffAtkRate;  // 0x04: aiParam::get_cliff_atk_r
    float m_width;         // 0x08: scaled by aiStat::m_scale.m_x in aiParam::get_width
    float m_height;        // 0x0C: scaled by aiStat::m_scale.m_y in aiParam::get_height
    u8 m_downAtkFlag;      // 0x10: aiParam::get_down_atk_f
    u8 m_cliffAtkFlag;     // 0x11: aiParam::get_cliff_atk_f
    char unk12[6];
    float m_airHigh;       // 0x18
    float m_airLength;     // 0x1C
};
static_assert(sizeof(aiReferPar) == 0x20, "Class is wrong size!");

// Identifies a CPU character slot: 0..6 are the players' fighters, 7..13 their sub-characters (Nana).
struct aiChrIdx {
    s8 m_idx;

    static int GetPlayerNo(const aiChrIdx* idx);
    static int GetFighterNo(const aiChrIdx* idx);
    static bool IsValid(const aiChrIdx* idx);
    static bool IsPlayer(const aiChrIdx* idx);
};

// One danger zone: an axis-aligned box that the CPUs avoid (see aiMgr::setDangerZone).
struct aiDangerZone {
    float m_minX;
    float m_minY;
    float m_maxX;
    float m_maxY;
    int m_team;            // -1 = no team (set when the zone is registered, see setDangerTeam)
};
static_assert(sizeof(aiDangerZone) == 0x14, "Class is wrong size!");

class aiMgr : public gfTask, public ftOutsideEventObserver {
    // HYPOTHESIS: layout recovered from ai_mgr.cpp, ai_stat.cpp and ai_input.cpp; the names follow how the members are used.
public:
    enum {
        NumPlayers = 7,
        NumChrs = 14,           // the fighters (0..6) and their sub-characters (7..13)
        NumDangerZones = 24,    // 0..7 the stage's zones, 8..23 the dynamic ones
    };

    aiInput* m_aiInput[NumChrs];        // 0x4C: the CPU input of each character slot (see aiChrIdx; NULL for humans)
    s8 m_chrState[NumChrs];             // 0x84: 1 = registered, -1 = removed (regist_ai_child, removePlayer)
    s8 m_cpuTypeLock[NumPlayers];       // 0x92: >= 0 = the CPU type of the player was chosen by the rules (setCpuType is ignored)
    s8 m_cpuRankLock[NumPlayers];       // 0x99: >= 0 = the CPU rank of the player was chosen by the rules (setCpuRank is ignored)
    u8 m_enable;                        // 0xA0: aiMgr::setEnable
    u8 m_adventureMd;                   // 0xA1: aiMgr::setAdventureMd, copied into aiInput::m_unk112
    u16 m_flags;                        // 0xA2: 0x100 = the stage's danger zones are not used
    s8 m_unkA4;                         // 0xA4: HYPOTHESIS: the sub-enemy (aiEnemyMgr slot) that the CPUs look at last, -1 = none
    char unkA5[3];
    int m_unkA8;                        // 0xA8
    u8 m_work[0x4C];                    // 0xAC: HYPOTHESIS: per-match work, cleared by CrearWork
    aiStat* m_stat;                     // 0xF8: the aiStat of each character slot (NumChrs, plus one for the enemies)
    aiWeaponMgr* m_weaponMgr;           // 0xFC
    aiEnemyMgr* m_enemyMgr;             // 0x100
    u8 m_unk104;                        // 0x104: the four records of 0xC bytes whose first byte CrearWork clears
    char unk105[0xB];
    u8 m_unk110;                        // 0x110
    char unk111[0xB];
    u8 m_unk11C;                        // 0x11C
    char unk11D[0xB];
    u8 m_unk128;                        // 0x128
    char unk129[0xF];
    aiReferPar* m_referPar;             // 0x138: the 0x36 rows of the refer parameter table
    void* m_systemPar;                  // 0x13C: aiSystemPar
    char unk140[9];
    u8 m_numDangerZones;                // 0x149: the zones in use
    char unk14A[2];
    aiDangerZone m_dangerZone[NumDangerZones]; // 0x14C
    u8 m_dangerAttr[NumDangerZones];    // 0x32C: per zone, bit 0 = in use, the rest an attribute mask
    int m_unk344;                       // 0x344
    char unk348[4];
    AiClBlock* m_blocks;                // 0x34C
    u16 m_numBlocks;                    // 0x350
    char unk352[2];
    u16 m_unk354;                       // 0x354
    char unk356[2];
    u16 m_unk358;                       // 0x358: HYPOTHESIS: floor block count (aiStat::chkEnableAway needs more than one)
    u16 m_unk35A;                       // 0x35A
    u16 m_unk35C;                       // 0x35C
    char unk35E[2];
    float m_areaLeft;                   // 0x360: HYPOTHESIS: the edges of the stage that the CPUs stay inside
    float m_areaRight;                  // 0x364
    float m_areaTop;                    // 0x368
    float m_areaBottom;                 // 0x36C
    int m_stageKind;                    // 0x370: HYPOTHESIS: the stage that is loaded (compared with Stages::Madein and 0x1C/0x1F)
    int m_unk374;                       // 0x374
    int m_unk378;                       // 0x378
    float m_unk37C;                     // 0x37C
    int m_unk380;                       // 0x380
    u8 m_unk384;                        // 0x384
    u8 m_unk385;                        // 0x385
    u8 m_unk386;                        // 0x386
    char unk387;

public:
    virtual void processBegin();
    virtual void renderDebug();
    virtual ~aiMgr();

    virtual void notifyEventOnDamage(int entryId, u32 hp, soDamage* damage);
    virtual void notifyEventDead(int entryId, int deadCount, Fighter::Dead::Reason deadReason, int respawnFrames);
    virtual void notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind);
    virtual void notifyEventChangeEnd(int entryId, int playerNo, int activeInstanceIndex, ftKind);

    static void setEnable(int enable);
    void CrearWork();
    void setCpuType(int player, int type);
    void setCpuRank(int player, s8 rank);
    void regist_ai_child(int player, aiInput* input, int chrKind);
    static void setEnablePlayer(int player, int enable, int unk);
    void removeTgtRefer(int player, int unk);

    void* searchAtkData(int ftKind, u32 id);

    AiClBlock* get_top_block(int team);
    AiClBlock* get_right_block(int team);
    AiClBlock* get_center_block(int team);
    bool chkUnderFloor(float slope, Vec2f* pos);
    AiClBlock* search_blk1(u32* key);
    // HYPOTHESIS: parameter meaning; the types follow the register use of the original and of its callers.
    // HYPOTHESIS: the parameters follow the call sites (position, found block, unused, team, then three limits).
    u32 get_under_floor(Vec3f* pos, AiClBlock** block, void* unk1, int team, float f1, float f2, float f3);
    AiClBlock* get_via_blk(AiClBlock* from, int unk1, float x, Vec2f* pos, int unk2, int unk3, float f1, float f2, float f3);
    AiClBlock* get_via_blk_team(AiClBlock* from, int unk1, float x, int team, Vec2f* pos, int unk2, int unk3, float f1, float f2, float f3);

    static bool isReady();
    static bool isReadyUpdate();
    static bool isSmallMap();
    static char* getMadeinAiData(bool includePreparation);
    static aiStat* get_aiStat(aiChrIdx* idx);
    aiStat* get_aiStatFinal();
    void removePlayer(aiChrIdx* idx);
    static void setAdventureMd(u32 md);

    int getCpuType(int player);
    int getCpuRank(int player);
    int getAiCpuTarget(int player);
    bool chkNanaForceMove(int player);
    bool chkIpHumanUpdate(aiChrIdx* idx);
    static aiWeaponMgr* getWeaponMgr();
    static void* getAiSystemPar();
    static aiReferPar* getAiReferPar(int index);

    int setDangerZone(Vec2f* p1, Vec2f* p2, int index, bool unk, bool attr);
    void delDangerZone(int);
    void setDangerAttr(int index, u8 attr);
    void setDangerTeam(int index, int team);
    static bool chkDangerZone(Vec2f* pos, int team, u8 attr);
    // HYPOTHESIS: return value is unused by the stage caller.
    void clearDangerZone(int index);
};
static_assert(sizeof(aiMgr) == 0x388, "Class is wrong size!");

extern aiMgr* g_aiMgr;
