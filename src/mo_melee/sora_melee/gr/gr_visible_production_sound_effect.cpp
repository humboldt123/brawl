#include <gr/gr_visible_production_sound_effect.h>
#include <types.h>

grVisibleProductionSoundEffect* grVisibleProductionSoundEffect::create(grGimmick* ground, grGimmick::SimpleEffectData* data) {
    return new (Heaps::StageInstance) grVisibleProductionSoundEffect(ground, data);
}

void grVisibleProductionSoundEffect::initialize() { }

void grVisibleProductionSoundEffect::remove() { }

void grVisibleProductionSoundEffect::productFlowStandby() { }

void grVisibleProductionSoundEffect::productFlowStart() {
    m_started = false;
    m_frame = 0.0f;
    m_soundHandle = -1;
    setState(VisibleProduction_Executing);
}

void grVisibleProductionSoundEffect::productFlowProducing(float deltaFrame) { }

void grVisibleProductionSoundEffect::productFlowEnd() { }

void grVisibleProductionSoundEffect::setForcedEnding() { }
