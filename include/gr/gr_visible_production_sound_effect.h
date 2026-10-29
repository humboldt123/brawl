#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <types.h>

class grVisibleProductionSoundEffect : public grVisibleProduction {
public:
    grGimmick* m_ground;        // 0x10
    SndID m_soundId;            // 0x14
    u16 m_startFrame;           // 0x18
    Vec3f m_pos;                // 0x1C
    float m_frame;              // 0x28
    bool m_started;             // 0x2C
    snd3DGenerator m_generator; // 0x30
    s32 m_soundHandle;          // 0x38

    // HYPOTHESIS: the sound id and start frame come from the same record as the effect productions.
    grVisibleProductionSoundEffect(grGimmick* ground, grGimmick::SimpleEffectData* data) : m_ground(ground), m_soundId((SndID)data->m_id), m_startFrame(data->m_repeatFrame), m_pos(Vec3f(0.0f, 0.0f, 0.0f)), m_frame(0.0f), m_started(false), m_soundHandle(-1) {
        initialize();
    }
    static grVisibleProductionSoundEffect* create(grGimmick* ground, grGimmick::SimpleEffectData* data);
    virtual void initialize();
    virtual void remove();
    virtual void productFlowStandby();
    virtual void productFlowStart();
    virtual void productFlowProducing(float deltaFrame);
    virtual void productFlowEnd();
    virtual void setForcedEnding();
};
static_assert(sizeof(grVisibleProductionSoundEffect) == 0x3C, "Class is wrong size!");
