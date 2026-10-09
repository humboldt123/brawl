#pragma once

#include <wn/wn_weapon_builder.h>
#include <wn/wn_kinetic_energy_normal.h>

struct wnWolfBlasterBulletModuleAccesserBuildConfig;

// Constructor/destructor offsets establish the builder extent through the
// embedded normal energy at +0x20C0. The remaining module-builder storage is
// opaque until the configuration is reconstructed.
template <>
class wnWeaponBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig> : public Weapon {
    u8 unkD0[0x1FF0];
    wnKineticEnergyNormal m_normalEnergy;
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig>) == 0x20F8,
              "Wolf Bullet builder extent");

class wnWolfBlasterBullet : public wnWeaponBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig> {
    void* unk20F8;
    u8 unk20FC[0x40];
    int* unk213C; // HYPOTHESIS: native activate reads one word through this pointer.
    u8 unk2140[0x38];
public:
    virtual ~wnWolfBlasterBullet();
    // HYPOTHESIS: r5 is unused; remaining arguments follow native caller/register stores.
    void activate(int founderTaskId, int unk5, int team, Vec3f* pos, int unkMode, EfID effectId, float lr, float speed, float angle);
    virtual bool reflect();
    virtual float getCollisionLr(soModuleAccesser* moduleAccesser);
    virtual void processUpdate();
    virtual void updateNodeSRT();
};
static_assert(sizeof(wnWolfBlasterBullet) == 0x2178, "Wolf Bullet article extent");
