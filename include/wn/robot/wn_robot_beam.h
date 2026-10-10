#pragma once

#include <ft/robot/ft_robot_article_info.h>
#include <wn/weapon.h>

// Tuning record of R.O.B.'s Robo Beam (HYPOTHESIS names, from how the beam uses it). The weak beam is fired with a low
// charge, the strong one with a full charge.
struct wnRobotBeamParam {
    float ricochetAngle; // 0x00: allowed incidence beyond 90 degrees (HYPOTHESIS: tuning name)
    float unk04;
    int weakLife;        // 0x08
    float weakSpeed;     // 0x0C
    int ricochetLife;    // 0x10: minimum remaining life required to permit another bounce
    int strongLife;      // 0x14
    float strongSpeed;   // 0x18
};

// R.O.B.'s Robo Beam projectile. Weapon implementation storage is still opaque; the size follows adjacent holder
// offsets in the article builder.
class wnRobotBeam : public Weapon {
    u8 m_unreconstructed[0x201C - sizeof(Weapon)];
    wnRobotBeamParam* m_param; // 0x201C
public:
    wnRobotBeam(s32 articleId, const ftRobotArticleConstructionInfo& info, void* data);
    virtual ~wnRobotBeam();
    virtual bool notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    // HYPOTHESIS: argument meanings, from the transactor's call site. fullCharge selects the strong beam.
    void activate(float lr, float angle, s32 founderTaskId, u32 resourceId, s32 team, const Vec3f* position,
                  bool fullCharge, s32 variant);
};
static_assert(sizeof(wnRobotBeam) == 0x2020, "Beam layout is wrong!");
