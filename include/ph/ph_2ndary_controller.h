#pragma once

#include <havok/hkArray.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <revolution/MTX.h>
#include <types.h>

namespace nw4r {
namespace g3d {
class ScnMdl;
}
}

class ph2ndaryWorld;
class ph2ndaryLine;


// Weapon / secondary-physics controller (main.dol map class ph2ndaryController).
// Object size 0xF4, created by phInterface::get2ndaryController.
class ph2ndaryController {
public:
    ph2ndaryController(void* owner, int a, int b);
    ~ph2ndaryController();

    void calacCallBackTimingCFor2ndary(int a, int b, int c, int d);
    void ph2ndaryMain(int a, int b, int c, int d, int e, int f);
    int get2ndaryLastNode(u32* nodes, nw4r::g3d::ResMdl resMdl);
    void setUndispNode(nw4r::g3d::ScnMdl* scnMdl);
    int get2ndaryUndispNode(u32* nodes, nw4r::g3d::ResMdl resMdl);
    int getWeaponLineCount();
    u32 getWeaponStartID(int index);
    void setLastVelocity(const Vec3f* velocity, int lineIndex, u8 a); // HYPOTHESIS: parameter types
    void setWeaponType(int type); // HYPOTHESIS: int parameter
    void set2Vector(const Vec3f* first, const Vec3f* second); // HYPOTHESIS: pointer parameters
    void resetWinding();
    void setAwake(bool awake, u32 lineId); // HYPOTHESIS: parameter types
    void setSleep(bool sleep);
    bool isHitCollisionLastNode2Pos(u32 mask);
    void processDefault();
    void draw();

    hkArray<ph2ndaryLine*> m_lines;   // 0x00 (data, count, capacity)
    u8 unk0C;                         // 0x0C
    u8 unk0D;                         // 0x0D, read by setAwake
    u8 unk0E;                         // 0x0E
    u8 unk0F[0x20 - 0x0F];            // 0x0F
    u8 unk20;                         // 0x20, latched from a global flag by setLastVelocity
    u8 unk21[0x30 - 0x21];            // 0x21
    int unk30;                        // 0x30, last line index
    u8 unk34[0x3C - 0x34];            // 0x34
    f32 m_gravity;                    // 0x3C, set by soPhysicsModuleImpl::set2ndGravity
    u8 m_active;                      // 0x40, weapon type (set to 1 by the owner)
    u8 unk41;                         // 0x41
    u8 unk42;                         // 0x42
    u8 unk43;                         // 0x43, line count limit
    Vec3f m_lastVelocity;             // 0x44
    f32 unk50;                        // 0x50
    u8 unk54;                         // 0x54
    u8 unk55;                         // 0x55
    u8 unk56;                         // 0x56
    u8 unk57;                         // 0x57
    u8 unk58;                         // 0x58
    u8 unk59;                         // 0x59
    u8 unk5A;                         // 0x5A
    u8 unk5B;                         // 0x5B
    u8 m_winding;                     // 0x5C, cleared by resetWinding
    u8 unk5D;                         // 0x5D
    u8 unk5E[0x65 - 0x5E];            // 0x5E
    u8 m_waver;                       // 0x65, set by soPhysicsModuleImpl::set2ndBothWaver
    u8 unk66[0x68 - 0x66];            // 0x66
    Mtx m_vectorMtx1;                 // 0x68, set by set2Vector
    Mtx m_vectorMtx2;                 // 0x98, set by set2Vector
    u8 m_sleep;                       // 0xC8
    u8 unkC9;                         // 0xC9
    u8 m_awake;                       // 0xCA
    u8 unkCB[0xD8 - 0xCB];            // 0xCB
    u8* m_sleepFlags;                 // 0xD8, one flag per entry
    int m_sleepFlagCount;             // 0xDC
    u8 unkE0[0xE4 - 0xE0];            // 0xE0
    int m_awakeIndex;                 // 0xE4, index of the line awakened by setAwake
    u8 unkE8;                         // 0xE8
    u8 unkE9;                         // 0xE9
    u8 unkEA[0xEC - 0xEA];            // 0xEA
    u32 m_flags;                      // 0xEC
    f32 m_weaponScale;                // 0xF0, set by soPhysicsModuleImpl::set2ndWeaponScale
};
static_assert(sizeof(ph2ndaryController) == 0xF4, "ph2ndaryController size");

// Pair wrapper (main.dol map class ph2ndary2Controller). Object size 0x0C.
class ph2ndary2Controller {
public:
    ph2ndary2Controller(void* a, void* b);
    ~ph2ndary2Controller();

    bool isHitCollisionLastNode2Pos();

    ph2ndaryController* m_controller[2];   // 0x00, 0x04
    u8 unk08[3];                           // 0x08..0x0A
};
