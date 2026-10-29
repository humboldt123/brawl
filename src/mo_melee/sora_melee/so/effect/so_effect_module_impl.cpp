#include <ec/ec_mgr.h>
#include <so/effect/so_effect_continual.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/effect/so_effect_time.h>
#include <so/so_array.h>
#include <types.h>

void soEffectModuleImpl::activate() {
    removeAllAfterImage(0, 0);
    removeAllContinual();
    removeAllTime();
    unk135_0 = false;
    unk54 = 0;
    unk135_2 = false;
    unk64 = 0;
    m_isSyncVisibility = true;
    unk74 = -1;
    unk134 = true;
}

void soEffectModuleImpl::pushGroup(u32 group) {
    g_ecMgr->pushsetCurrentGroup(group);
}

void soEffectModuleImpl::reqTime(EfID effectID, int frame, Vec3f* pos, Vec3f* rot, float scale, u32 p6) {
    u32 handle = req(effectID, pos, rot, scale, p6, -1);
    if (handle != 0) {
        soEffectTime time;
        time.set(effectID, handle, frame);
        m_times->push(time);
    }
}

void soEffectModuleImpl::reqTimeFollow(EfID effectID, int nodeId, int frame, Vec3f* pos, Vec3f* rot, float scale, u32 p7) {
    u32 handle = reqFollow(effectID, nodeId, pos, rot, scale, false, p7, 0, -1);
    if (handle != 0) {
        soEffectTime time;
        time.set(effectID, handle, frame);
        m_times->push(time);
    }
}

void soEffectModuleImpl::removeContinual(u32 index) {
    soEffectContinual& continual = m_continuals->at(index);
    continual.m_isActive = false;
}

void soEffectModuleImpl::removeAllContinual() {
    int num = m_continuals->size();
    for (int i = 0; i < num; i++) {
        if (m_continuals->at(i).m_isActive == true) {
            m_continuals->at(i).m_isActive = false;
        }
    }
}

void soEffectModuleImpl::removeTime(int effectID) {
    for (int i = m_times->size() - 1; i >= 0; i--) {
        if (effectID == m_times->at(i).m_effectId) {
            m_times->at(i).end();
            m_times->erase(i);
        }
    }
}

void soEffectModuleImpl::removeAllTime() {
    int num = m_times->size();
    for (int i = 0; i < num; i++) {
        if (m_times->at(i).m_frame > 0) {
            m_times->at(i).end();
        }
    }
    m_times->clear();
}

void soEffectModuleImpl::remove(int handle) {
    g_ecMgr->endEffect(handle);
}

void soEffectModuleImpl::kill(int handle, bool p2, bool p3) {
    g_ecMgr->killEffect(handle, p2, p3);
}

void soEffectModuleImpl::setPos(int handle, Vec3f* pos) {
    g_ecMgr->setPos(handle, pos);
}

void soEffectModuleImpl::setRot(int handle, Vec3f* rot) {
    g_ecMgr->setRot(handle, rot);
}

void soEffectModuleImpl::setVisible(int handle, bool isVisible) {
    g_ecMgr->setVisible(handle, isVisible);
}

bool soEffectModuleImpl::isObserv(char unk1) {
    return unk1 == 0x11;
}

void soEffectModuleImpl::notifyEventDestructInstance(soModuleAccesser* moduleAccesser) {
    killAll(4, true, true);
}

void soEffectModuleImpl::setShieldEffect(int shieldEffect) {
    m_shieldEffect = shieldEffect;
}

bool soEffectModuleImpl::isSyncScale() {
    return m_isSyncScale;
}

void soEffectModuleImpl::setSyncScale(bool sync) {
    m_isSyncScale = sync;
}

bool soEffectModuleImpl::isSyncVisibility() {
    return m_isSyncVisibility;
}

void soEffectModuleImpl::setSyncVisibility(bool sync) {
    m_isSyncVisibility = sync;
}
