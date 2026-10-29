#include <so/shake/so_shake_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

void soShakeModuleImpl::activate() {
    stop();
}

void soShakeModuleImpl::req(int kind, int frame, bool flag, const Vec2f* offset) {
    if (isShake() == true) {
        u8 priority = m_terms->at(kind).m_data->unk8;
        if (m_terms->at(m_kind).m_data->unk8 > priority) {
            return;
        }
        soShakeTerm& current = m_terms->at(m_kind);
        current.m_frame = 0;
        current.m_step = 0;
        current.m_isActive = false;
    }
    soShakeTerm& term = m_terms->at(kind);
    term.m_frame = frame;
    term.m_step = 0;
    term.m_flag = flag;
    term.m_offset = *offset;
    term.m_isActive = true;
    m_kind = kind;
}

bool soShakeModuleImpl::isShake() {
    return m_kind != -1;
}

void soShakeModuleImpl::extend(int kind, int frame) {
    if (m_kind == kind) {
        m_terms->at(kind).m_frame = frame;
    }
}

void soShakeModuleImpl::stop() {
    int num = m_terms->size();
    for (int i = 0; i < num; i++) {
        soShakeTerm& term = m_terms->at(i);
        term.m_frame = 0;
        term.m_step = 0;
        term.m_isActive = false;
    }
    m_kind = -1;
}

void soShakeModuleImpl::stopKind(int kind) {
    if (m_kind == kind) {
        soShakeTerm& term = m_terms->at(kind);
        term.m_frame = 0;
        term.m_step = 0;
        term.m_isActive = false;
    }
}

void soShakeModuleImpl::update(bool) {
    if (m_kind == -1) {
        return;
    }
    soShakeTerm& term = m_terms->at(m_kind);
    if (term.m_data->unk9 != 0 && term.m_frame > 0) {
        term.m_step++;
        if (term.m_step >= term.m_data->unk4) {
            term.m_step = 0;
        }
        if (--term.m_frame == 0) {
            term.m_isActive = false;
        }
    }
    if (!m_terms->at(m_kind).m_isActive) {
        m_kind = -1;
    }
}

bool soShakeModuleImpl::isObserv(char group) {
    return group == 0x1D;
}
