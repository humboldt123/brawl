#pragma once
// Local header (BrawlHeaders has no grCollisionManager): the stage collision task, with the
// layout verified from its constructor, the ground-collision/status update passes and its users.

#include <ut/ut_bit_array.h>
#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <types.h>

class grCollision;
class grCollStatus;

class grCollisionManager : public gfTask {
public:
    // HYPOTHESIS: type name from addCliffHangInfo/removeCliffHangInfo; one ledge grab per slot.
    struct CliffHangInfo {
        grCollStatus* m_status;
        u32 m_lineId;
        u8 m_team;

        CliffHangInfo() : m_lineId(0xFFFF) { }
    };


    // HYPOTHESIS: the task flags are a member object; the constructor clears them before the members after it.
    struct Flags {
        bool m_isActive : 1; // HYPOTHESIS: set by every processUpdate; gates the map-correction passes
        bool m_isGameFrame : 1; // HYPOTHESIS: GameGlobal::isPrevJustGameFrame of the last processUpdate
        bool unk_b5 : 1;
        bool unk_b4 : 1;
        bool m_useClipRange : 1; // HYPOTHESIS: clip the dynamic collisions against m_clipRange
        bool unk_b2 : 1;
        bool unk_b1 : 1;

        Flags() {
            m_isActive = false;
            m_isGameFrame = false;
            unk_b5 = false;
            unk_b4 = false;
            m_useClipRange = false;
            unk_b2 = false;
            unk_b1 = false;
        }
    };

    // 0x40
    Flags m_flags;
    // 0x41
    u8 unk41;
    // 0x42
    u8 m_nextIndex;
    // 0x44
    u16 unk44;
    // 0x48
    Rect2D m_clipRange;
    // 0x58
    Rect2D m_floorRange;
    // 0x68
    CliffHangInfo m_cliffHangInfos[8];
    // 0xC8
    utBitArrayStatic<256> m_indexBits;
    // 0xF0
    grCollision* m_collisions[256];
    // 0x4F0
    u32 unk4F0;
    u32 unk4F4;

    grCollisionManager();
    virtual void processUpdate();
    virtual void processPreMapCorrection();
    virtual void processMapCorrection();
    virtual void processFixPosition();
    virtual void processDebug();
    virtual void renderDebug();
    virtual ~grCollisionManager();

    void collisionUpdate(bool dynamic); // HYPOTHESIS: parameter type
    void calcFloorRange(); // HYPOTHESIS: fills m_floorRange from the floor lines of the enabled static collisions
    void regist(grCollision* collision);
    grCollision* getCollision(int index); // HYPOTHESIS: index type
    bool checkCliffOverlap(grCollStatus* status, const u32& lineId); // HYPOTHESIS: lineId type
    void addCliffHangInfo(grCollStatus* status, const u32& lineId); // HYPOTHESIS: lineId type
    void removeCliffHangInfo(grCollStatus* status);
};
static_assert(sizeof(grCollisionManager) == 0x4F8, "Class is wrong size!");
