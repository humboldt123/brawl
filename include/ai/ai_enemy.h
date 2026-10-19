#pragma once

#include <mt/mt_vector.h>
#include <types.h>

// What the CPU AI knows about one enemy object (Subspace Emissary / stage enemy), refreshed each frame by aiEnemy::update.
// HYPOTHESIS: layout recovered from aiEnemyMgr's accessors and aiEnemy::update only.
class aiEnemy {
public:
    int m_objId;      // 0x00: object handed to the external value accessor
    u8 m_index;       // 0x04: slot number in aiEnemyMgr
    u8 unk5;
    u16 m_flags;      // 0x06: 1 = active, 2 = on ground, 4 = excluded from targeting
    int m_status;     // 0x08: status kind
    Vec3f m_pos;      // 0x0C
    Vec2f m_move;     // 0x18: position change since the last update
    float m_lr;       // 0x20

    aiEnemy() {
        m_flags = 0;
    }
    void update();
};
static_assert(sizeof(aiEnemy) == 0x24, "Class is wrong size!");

class aiEnemyMgr {
public:
    aiEnemy m_enemies[32];  // 0x000
    int m_count;            // 0x480: number of active enemies

    aiEnemyMgr();
    ~aiEnemyMgr();
    void reset();
    void update();
    aiEnemy* getEnemy(int index);
    aiEnemy* getEnemyNear(Vec3f* pos, float* distanceSq);
};
static_assert(sizeof(aiEnemyMgr) == 0x484, "Class is wrong size!");
