#pragma once

#include <StaticAssert.h>
#include <ef/ef_id.h>
#include <mt/mt_vector.h>
#include <types.h>

class soModuleAccesser;

// One continual effect slot of soEffectModuleImpl: an effect that follows a model node and keeps its previous position.
// The array units use the plain layout of so/templates/so_array_value_soEffectContinual.h instead.
class soEffectContinual {
public:
    EfID m_effectId;
    u32 m_nodeId; // HYPOTHESIS: the resolved node id from soEffectModuleImpl::getNode
    float unk8;
    float unkC;
    Vec3f m_pos;
    Vec3f m_prevPos;
    bool m_isActive;
    bool m_isFirst;
    bool unk2A;

    soEffectContinual();
    ~soEffectContinual();
    // HYPOTHESIS: parameter types from the caller soEffectModuleImpl::reqContinual
    void active(EfID effectId, u32 nodeId, float, bool);
    bool update(soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(soEffectContinual) == 0x2C, "Class is wrong size!");
