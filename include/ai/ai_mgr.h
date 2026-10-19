#pragma once

// Local shadow of BrawlHeaders/ai/ai_mgr.h: expose native danger-zone cleanup.

#include <StaticAssert.h>
#include <ft/ft_manager.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <types.h>

class aiInput;

// One row of the AI "refer" parameter table (0x36 rows of 0x20 bytes).
struct aiReferPar {
    char unk[0x20];
};

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

    static bool isReady();
    static bool isReadyUpdate();
    static bool isSmallMap();
    static char* getMadeinAiData(bool force);

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