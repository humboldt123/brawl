#include <st_greenhill/gr_greenhill.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>

// HYPOTHESIS: an unnamed sora_melee function that takes a Yakumono (the original calls it right after a hit).
extern "C" void fn_27_26399C(Yakumono* yakumono);

grGreenhillCheck* grGreenhillCheck::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grGreenhillCheck* ground = new (Heaps::StageInstance) grGreenhillCheck(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grGreenhillCheck::grGreenhillCheck(const char* taskName) : grGreenhill(taskName) {
    m_timer2 = 0.0f;
    unk15C = NULL;
    unk160 = NULL;
    unk164 = NULL;
    m_order[0] = 4;
    m_order[1] = 4;
    m_order[2] = 4;
    m_order[3] = 4;
    m_orderDone = 0;
    m_hitTeam = 0;
    m_animId = 5;
    unk17C = 0.0f;
    m_motionEndFrame = 0.0f;
    unk184 = 0;
    m_attackEnabled = 0;
    unk188[0] = NULL;
    unk188[1] = NULL;
    unk188[2] = NULL;
    unk188[3] = NULL;
    unk188[4] = NULL;
    m_dangerZoneId = -1;
    m_effectId = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grGreenhillCheck::~grGreenhillCheck() {
    delete[] static_cast<u8*>(unk188[0]);
    unk188[0] = NULL;
    delete[] static_cast<u8*>(unk188[1]);
    unk188[1] = NULL;
    delete[] static_cast<u8*>(unk188[2]);
    unk188[2] = NULL;
    delete[] static_cast<u8*>(unk188[3]);
    unk188[3] = NULL;
    delete[] static_cast<u8*>(unk188[4]);
    unk188[4] = NULL;
}

// Runs the three Check updates once the gimmick is running.
void grGreenhillCheck::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit model/yakumono is built the first time the ball needs it.
void grGreenhillCheck::updateYakumono(float deltaFrame) {
    if (unk184 != 1) {
        setHit();
        if (m_yakumono != NULL) {
            unk184 = 1;
        }
    }
}

// A fighter hit the ball: remember the attacker's team and let the ball react (state work 3).
void grGreenhillCheck::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    fn_27_26399C(m_yakumono);
    unk15C[0] = 3;
    m_sndGenerator.playSE(static_cast<SndID>(0x1d16), 0, 0, -1);
    m_sndGenerator.setPos(&unk164[m_order[0]]);
    m_hitTeam = damage->m_collisionLog.m_teamNo;
    if (m_yakumono != NULL) {
        m_yakumono->setTeam(damage->m_collisionLog.m_teamNo);
    }
}
