#include <st_greenhill/gr_greenhill.h>
#include <ai/ai_mgr.h>
#include <ec/ec_mgr.h>
#include <mt/mt_prng.h>

// MATCH-ONLY: keep the stage's original constant pool and string pool.
extern const float g_greenhillCheckConstants[]; // [0] = 0.0f
extern const char g_greenhillCheckBallNode[];         // "ball"
extern const char g_greenhillCheckBallPositionNode[]; // "ballPosition"
extern const float g_greenhillCheckDangerHalfExtent;  // 20.0f

// HYPOTHESIS: reports whether an effect handle is still alive (unnamed function in the main binary).
extern "C" u32 fn_80060F3C(ecMgr* mgr, u32 handle);

// The four values the stage's Check parameter block holds (read through getStageData()).
struct GreenhillCheckParams {
    float unk0;     // timer set when the ball starts rolling
    float unk4;     // minimum wait between two drops
    float unk8;     // maximum wait between two drops
    float firstWait; // wait before the first drop
};

// State machine of the checkered ball. A marker is picked from a shuffled order of four positions, the ball waits,
// drops there, stays until its timer runs out and then rolls off; a fighter hit (state work 3) sends it rolling.
// State numbers are HYPOTHESIS; the transitions are read from the original jump table.
void grGreenhillCheck::updateActive(float deltaFrame) {
    GreenhillCheckParams* params = static_cast<GreenhillCheckParams*>(getStageData());
    if (params == NULL) {
        return;
    }
    const float* constants = g_greenhillCheckConstants;

    m_timer -= deltaFrame;
    if (m_timer < constants[0]) {
        m_timer = constants[0];
    }
    m_timer2 -= deltaFrame;
    if (m_timer2 < constants[0]) {
        m_timer2 = constants[0];
    }

    switch (m_state) {
    case 0: {
        // Reset: hide the ball and pick the next marker.
        setMotion(4, false, true, NULL);
        setVisibility(0);
        disableHit(0, 0);
        disableAttack(0);
        m_attackEnabled = 0;
        if (m_order[1] == 4) {
            // The order is used up (or never built): shuffle the four markers, avoiding the one used last.
            u8 previousFirst = m_order[0];
            m_order[0] = 0;
            m_order[1] = 1;
            m_order[2] = 2;
            m_order[3] = 3;
            do {
                int count = 4;
                for (u8 i = 0; i < 4; i++) {
                    u8 j = (u8)(count * randf());
                    if (j >= 3) {
                        j = 3;
                    }
                    u8 swap = m_order[i];
                    m_order[i] = m_order[j];
                    m_order[j] = swap;
                }
            } while (m_order[0] == previousFirst);
        } else {
            m_order[0] = m_order[1];
            m_order[1] = m_order[2];
            m_order[2] = m_order[3];
            m_order[3] = 4;
        }
        m_orderDone = 0;
        if (m_yakumono != NULL) {
            m_yakumono->setTeam(16);
        }
        unk15C[0] = 5;
        m_timer = params->firstWait;
        m_state = 1;
        break;
    }
    case 1:
        // Wait for the drop; skip a marker whose breakable piece is already gone.
        if (constants[0] == m_timer) {
            switch (m_order[0]) {
            case 1:
                if (unk160[1] == 2) {
                    return;
                }
                break;
            case 2:
                if (unk160[2] == 2) {
                    return;
                }
                break;
            }
            setMotion(0, false, true, &m_motionEndFrame);
            unk15C[0] = 0;
            m_sndGenerator.playSE(static_cast<SndID>(0x1d17), 0, 0, -1);
            m_sndGenerator.setPos(&unk164[m_order[0]]);
            m_state = 5;
        }
        break;
    case 5:
        if (getMotionFrame(0) >= m_motionEndFrame) {
            setMotion(4, false, true, &m_motionEndFrame);
            enableHit(0, 0);
            float random = randf();
            m_timer = params->unk4 + (params->unk8 - params->unk4) * random;
            unk15C[0] = 4;
            m_state = 9;
        } else if (!m_isVisible) {
            setVisibility(1);
        }
        break;
    case 6:
        if (getMotionFrame(0) >= m_motionEndFrame) {
            m_state = 0;
        }
        break;
    case 7:
        if (getMotionFrame(0) >= m_motionEndFrame) {
            m_state = 0;
        }
        break;
    case 8: {
        Vec3f ballPos;
        getNodePosition(&ballPos, 0, g_greenhillCheckBallNode);
        // Danger zone for the AI: a box around the ball.
        const float half = g_greenhillCheckDangerHalfExtent;
        Vec2f corners[2];
        corners[0].m_x = ballPos.m_x - half;
        corners[0].m_y = half + ballPos.m_y;
        corners[1].m_x = half + ballPos.m_x;
        corners[1].m_y = ballPos.m_y - half;
        m_dangerZoneId = g_aiMgr->setDangerZone(&corners[0], &corners[1], m_dangerZoneId, false, false);
        if (constants[0] == m_timer2) {
            if (getMotionFrame(0) < unk17C) {
                // The roll animation wrapped around: the ball is done.
                setMotion(4, false, true, &m_motionEndFrame);
                enableHit(0, 0);
                disableAttack(0);
                m_attackEnabled = 0;
                if (m_yakumono != NULL) {
                    m_yakumono->setTeam(16);
                }
                if (m_dangerZoneId != -1) {
                    g_aiMgr->delDangerZone(m_dangerZoneId);
                    m_dangerZoneId = -1;
                }
                changeColor(0);
                unk15C[0] = 4;
                m_state = 9;
            } else {
                unk17C = getMotionFrame(0);
            }
        } else {
            unk17C = getMotionFrame(0);
        }
        break;
    }
    case 9:
        if (constants[0] == m_timer) {
            setMotion(1, false, true, &m_motionEndFrame);
            disableHit(0, 0);
            m_sndGenerator.playSE(static_cast<SndID>(0x1d18), 0, 0, -1);
            m_sndGenerator.setPos(&unk164[m_order[0]]);
            m_state = 6;
        } else if (unk15C[0] == 3) {
            // A fighter hit the ball: roll.
            setMotion(3, true, true, &m_motionEndFrame);
            unk17C = constants[0];
            disableHit(0, 0);
            setAttack();
            m_effectId = g_ecMgr->setEffect(ef_ptc_stg_greenhill_marker);
            g_ecMgr->setParent(m_effectId, m_sceneModels[0], g_greenhillCheckBallPositionNode, false);
            changeColor(1);
            m_timer2 = params->unk0;
            m_state = 8;
        }
        break;
    }

    // Common tail: when the marker's breakable piece has been broken the ball is removed.
    if (m_orderDone == 0) {
        u8 first = m_order[0];
        if ((first == 1 && unk160[1] == 2) || (first == 2 && unk160[2] == 2)) {
            setMotion(2, false, true, &m_motionEndFrame);
            disableHit(0, 0);
            if (m_attackEnabled == 1) {
                disableAttack(0);
                m_attackEnabled = 0;
            }
            if (fn_80060F3C(g_ecMgr, m_effectId) == 1) {
                g_ecMgr->endEffect(m_effectId);
                m_effectId = 0;
            }
            unk15C[0] = 2;
            m_orderDone = 1;
            if (m_dangerZoneId != -1) {
                g_aiMgr->delDangerZone(m_dangerZoneId);
                m_dangerZoneId = -1;
            }
            m_state = 7;
        }
    }
}
