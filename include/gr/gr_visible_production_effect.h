#pragma once

// Local BrawlHeaders shadow: the members, constructor and create of grVisibleProductionEffect (sora_melee 0x270EF4).
#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <mt/mt_vector.h>
#include <types.h>

class grVisibleProductionEffect : public grVisibleProduction {
public:
    grGimmick* m_ground;  // 0x10
    u32 m_effectId;       // 0x14
    u16 m_startFrame;     // 0x18
    Vec3f m_pos;          // 0x1C
    Vec3f m_rot;          // 0x28
    Vec3f m_scale;        // 0x34
    float m_frame;        // 0x40
    bool m_started;       // 0x44

    grVisibleProductionEffect(grGimmick* ground, grGimmick::SimpleEffectData* data) : m_ground(ground), m_effectId(data->m_id), m_startFrame(data->m_repeatFrame), m_pos(Vec3f(0.0f, 0.0f, 0.0f)), m_frame(0.0f), m_started(false) {
        initialize();
    }
    static grVisibleProductionEffect* create(grGimmick* ground, grGimmick::SimpleEffectData* data);
    virtual void initialize();
    virtual void remove();
    virtual void productFlowStandby();
    virtual void productFlowStart();
    virtual void productFlowProducing(float deltaFrame);
    virtual void productFlowEnd();
    virtual void setForcedEnding();
};
static_assert(sizeof(grVisibleProductionEffect) == 0x48, "Class is wrong size!");
