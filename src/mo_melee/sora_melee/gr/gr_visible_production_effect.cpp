#include <gr/gr_visible_production_effect.h>
#include <ec/ec_mgr.h>
#include <mt/mt_matrix.h>
#include <types.h>

void getScalingFactor(Matrix* matrix, Vec3f* scale);

grVisibleProductionEffect* grVisibleProductionEffect::create(grGimmick* ground, grGimmick::SimpleEffectData* data) {
    return new (Heaps::StageInstance) grVisibleProductionEffect(ground, data);
}

void grVisibleProductionEffect::initialize() { }

void grVisibleProductionEffect::remove() { }

void grVisibleProductionEffect::productFlowStandby() { }

void grVisibleProductionEffect::productFlowStart() {
    m_started = false;
    m_frame = 0.0f;
    setState(VisibleProduction_Executing);
}

void grVisibleProductionEffect::productFlowProducing(float deltaFrame) {
    if (!m_started) {
        if (m_frame >= m_startFrame) {
            Matrix matrix(true);
            if (!m_ground->getNodeMatrix(&matrix, 0, "effect_locator")) {
                m_ground->getNodeMatrix(&matrix, 0, u32(0));
            }
            m_pos = matrix.getPosition();
            matrix.getRotate(&m_rot);
            getScalingFactor(&matrix, &m_scale);
            g_ecMgr->setEffect((EfID)m_effectId, &m_pos, &m_rot, &m_scale);
            m_visiblePermission = true;
            m_started = true;
        } else {
            m_frame += deltaFrame;
        }
    } else {
        setState(VisibleProduction_Finished);
    }
}

void grVisibleProductionEffect::productFlowEnd() { }

void grVisibleProductionEffect::setForcedEnding() { }
