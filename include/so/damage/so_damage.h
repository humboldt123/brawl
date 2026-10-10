#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <so/collision/so_collision_attack_part.h>
#include <so/collision/so_collision_log.h>

class soDamage {
public:
    enum Level {
        Level_1 = 0x0,
        Level_2 = 0x1,
        Level_3 = 0x2,
        Level_Fly = 0x3,
        Level_FlyRoll = 0x4, // HYPOTHESIS: set by setupDamageFlyRollStatus
    };

#ifdef YK_STAGE_FULL
    // MATCH-ONLY: the stage RELs copy soDamage with the word/half/byte layout of so/templates/so_damage.h (their array
    // instantiations are compiled against it), and emit an empty destructor that the soArrayVector destructor calls per element.
    float unk0, unk4, unk8, unkc;
    struct { u32 unk0, unk4, unk8; } unk10;
    u32 unk1c;
    struct { u32 unk0, unk4, unk8; } unk20;
    u16 unk2c, unk2e;
    u8 unk30, unk31, unk32, unk33, unk34, unk35, unk36, unk37, unk38, unk39, unk3a;
    u32 unk3c;
    struct { u32 unk0, unk4, unk8; } unk40;
    float unk4c;
    u32 unk50, unk54, unk58, unk5c;
    float unk60, unk64, unk68;
    u32 unk6c, unk70, unk74, unk78;
    float unk7c;
    struct { u32 unk0, unk4, unk8; } unk80;
    struct { u32 unk0, unk4; } unk8c;
    float unk94;
    u32 unk98;
    u8 unk9c;
    soDamage() {
        unk60 = 0.0f;
        unk64 = 0.0f;
        unk68 = 0.0f;
        unk74 = unk74 & ~0x1F; // HYPOTHESIS: five flag bits of the word at 0x74 are cleared
    }
    ~soDamage() { }
#else
    float m_damage;
    float m_damageAdd;
    float m_powerMax;
    float m_reaction;
    soCollisionLog m_collisionLog;
    soCollisionAttackData m_attackData;
    float m_lr;
    Vec3f m_pos;
    Vec2f m_speed; // was m_140/m_144
    float m_damageAdd_;
    int m_attackerTeamOwnerId;
    bool m_isFlinchFlag; // +0x9c, HYPOTHESIS name: onDamage returns early when 0
    char _157[3];
#endif
};
static_assert(sizeof(soDamage) == 160, "Class is wrong size!"); // size 160

struct soDamageLog {
    float m_reaction;
    soDamage::Level m_level;
    int m_height;
    Vec2f m_speed;
    float m_angle;
    float m_lr;
    float m_frame;
    u32 m_hitStopFrame;
    soCollisionAttackData::Attribute m_attribute;
    float m_damageAdd;
    int m_attackerTeamNo;
    float m_hitStopDelay;
    Vec2f m_groundTouchNormal;
    int m_attackerTaskId;
    int m_attackerTeamOwnerId;
    gfTask::Category m_attackerTaskCategory : 8;
    bool m_isDamageAir : 1;
    bool m_isSituationGround : 1;
    bool m_unk26 : 1;
    bool m_isCollisionAbsolute : 1;
    bool m_isMeteor : 1;
    bool m_isAttackDirect : 1;
    bool m_isVector365 : 1;
    bool m_unk31 : 1;
    char _0x46[2];
};
static_assert(sizeof(soDamageLog) == 0x48, "Class is wrong size!");