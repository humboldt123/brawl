#pragma once

// The hit (hurtbox) module of the enemies: soCollisionHitModuleImpl plus the weak spots and the damage multipliers of
// the enemy kind (mo_enemy/sora_enemy/collision/em_collision_hit_module_impl.cpp).

#include <StaticAssert.h>
#include <so/collision/so_collision_attack_part.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <types.h>

class acAnimCmd;
class soModuleAccesser;

// The hit parts that are the enemy's weak spots (emCollisionHitModuleImpl::isHitWeakness looks the part id up in it).
// HYPOTHESIS: names; the layout is from the lookup loop (a part id array and its length).
struct emCollisionHitWeakness {
    u32* m_partIds;
    int m_num;
};

// The damage multipliers of an enemy kind, one per attack attribute class
// (emCollisionHitModuleImpl::getDurabilityFromAttackAttr), for an ordinary hit part and for a weak spot.
// HYPOTHESIS: names; the indirect multiplier applies to attacks without soCollisionAttackData::m_isDirect.
struct emCollisionHitDurability {
    float m_attrMul[10];          // 0x00
    float m_indirectMul;          // 0x28
    float m_weakAttrMul[10];      // 0x2C
    float m_weakIndirectMul;      // 0x54
};

class emCollisionHitModuleImpl : public soCollisionHitModuleImpl {
public:
    emCollisionHitWeakness* m_weakness;     // 0x68
    emCollisionHitDurability* m_durability; // 0x6C

    // HYPOTHESIS: hitGroupData is the weak spot list, hitData the multipliers (soValueAccesser constants of the enemy).
    emCollisionHitModuleImpl(soModuleAccesser* acc, int taskId, u8 category, soArray<soCollisionHitPart>* parts,
                             soArray<soCollisionGroup>* groups, soArray<soCollisionHitGroup>* hitGroups, void* hitGroupData,
                             void* hitData);
    virtual ~emCollisionHitModuleImpl();
    // The damage multiplier of an attack on a hit part (soCollisionHitModule vtable +0x94).
    virtual float getDurabilityMulFromAttackData(soCollisionAttackData* attackData, u32 partId);
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);

    bool isHitWeakness(u32 partId);
    // Sets the hit status of every weak spot (anim cmd 'm' 0, argument 0).
    void setWeaknessCollisionStatus(int status);
    // The multiplier class of an attack attribute (9: every attribute without a class of its own).
    u32 getDurabilityFromAttackAttr(int attr);
};
static_assert(sizeof(emCollisionHitModuleImpl) == 0x70, "Class is wrong size!");
