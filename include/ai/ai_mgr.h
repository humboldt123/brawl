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

class aiMgr : public gfTask, public ftOutsideEventObserver {
    // HYPOTHESIS: layout recovered from the accessors only; everything not listed is still opaque.
    aiInput* m_aiInput[14];    // 0x4C: the CPU input of each character slot (see aiChrIdx; NULL for humans)
    char _132[772];

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

    static bool isReady();
    static bool isReadyUpdate();
    static bool isSmallMap();
    static char* getMadeinAiData(bool force);
    static aiStat* get_aiStat(aiChrIdx* idx);

    int getCpuType(int player);
    int getCpuRank(int player);
    int getAiCpuTarget(int player);
    bool chkNanaForceMove(int player);
    bool chkIpHumanUpdate(aiChrIdx* idx);
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