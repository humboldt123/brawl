#include <ec/ec_mgr.h>
#include <so/effect/so_effect_time.h>
#include <types.h>

soEffectTime::soEffectTime() : m_effectId((EfID)-1), m_frame(0), m_handle(0) { }

soEffectTime::~soEffectTime() { }

void soEffectTime::set(EfID effectId, u32 handle, int frame) {
    m_effectId = effectId;
    m_handle = handle;
    m_frame = frame;
}

void soEffectTime::end() {
    g_ecMgr->endEffect(m_handle);
    m_handle = 0;
    m_frame = 0;
    m_effectId = (EfID)-1;
}

bool soEffectTime::update(soModuleAccesser* moduleAccesser) {
    if (--m_frame <= 0) {
        end();
        return true;
    }
    return false;
}
