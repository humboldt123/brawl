#pragma once

// SHADOW of BrawlHeaders em/em_archive.h (-I include comes first): the members are public, the Enemy reads them (its
// constructor, getFaceTexPtr, getExtendBrresResourceID), and the methods of em_manager.cpp are declared.

#include <StaticAssert.h>
#include <em/em_create.h>
#include <types.h>

class gfArchive;

// The resource ids of one enemy kind's archives, registered with soArchiveDb manager 1 (0xFFFF: not registered).
class emArchive {
public:
    static const u32 InvalidResourceId = 0xFFFF;

    u32 enmCommonResourceId;    // 0x0
    u32 brresResourceId;        // 0x4
    u32 paramResourceId;        // 0x8
    u32 weaponResourceId;       // 0xC, the brres archive
    u32 faceTexBrresResourceId; // 0x10, the brres archive
    u32 extendBrresResourceId;  // 0x14, the Primid face brres (Primid kinds only)

    // The Enemy reads the ids through these: as call arguments the original evaluates them before the trivial
    // arguments (an accessor result is a temporary; a plain member read is deferred in parameter order).
    u32 getEnmCommonResourceId() const { return enmCommonResourceId; }
    u32 getBrresResourceId() const { return brresResourceId; }
    u32 getParamResourceId() const { return paramResourceId; }
    u32 getFaceTexBrresResourceId() const { return faceTexBrresResourceId; }

    emArchive(gfArchive* param, gfArchive* brres, gfArchive* enmCommon, gfArchive* primFaceBrres, EnemyKind kind);
    ~emArchive();
    // Whether every registered archive has finished loading.
    bool checkLoadFiles();
    void unloadFiles();
    // HYPOTHESIS: the id selects a weapon of the archive; every id gives the brres archive.
    int getWeaponResourceID(u32 id);
};
static_assert(sizeof(emArchive) == 24, "Class is wrong size!");
