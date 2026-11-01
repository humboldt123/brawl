#pragma once

// Local BrawlHeaders shadow (supersedes include/Brawl/Include/st/st_collision_attr_param.h): adds
// stCollisionAttrParam, the stage's collision-attribute table at Stage + 0xF8 (sora_melee 0x227ED8-0x2283F0).

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <types.h>

struct grGimmickDamageFloor {
    grGimmick::AttackData m_attackData;
    char _88[4];
    u32 m_index;
    int m_teamNo;

    inline grGimmickDamageFloor() {};
    inline grGimmickDamageFloor(grGimmick::AttackData* attackData, u32 index, int teamNo = -1)
    : m_attackData(*attackData), m_index(index), m_teamNo(teamNo) {
    }
};
static_assert(sizeof(grGimmickDamageFloor) == 100, "Class is wrong size!");

// HYPOTHESIS: the per-material parameter set handed out by getCollisionAttrParamSet; only the down friction is
// read so far (soGroundShapeImpl).
struct stCollisionAttrParamSet {
    int m_materialType;
    float m_downFriction;
};

class stDataMultiContainer;
struct stDataContainerData;

class stCollisionAttrParam {
public:
    stDataMultiContainer* m_container;  // the GDMD (attribute) block loaded by setCollisionAttrParamSet
    grGimmickDamageFloor* m_attackData; // three damage-floor records, allocated with new[]
    u8 m_attackRate;                    // HYPOTHESIS: scales the damage of the records (loadStageAttackData)

    stCollisionAttrParam();
    ~stCollisionAttrParam();
    stCollisionAttrParamSet* getCollisionAttrParamSet(int materialType);
    void setCollisionAttrParamSet(stDataContainerData* data);
    void setCollisionAttackData(grGimmickDamageFloor* attackData);
};
static_assert(sizeof(stCollisionAttrParam) == 0xC, "Class is wrong size!");
