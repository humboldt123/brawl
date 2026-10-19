#include <ai/ai_act.h>
#include <ai/ai_input.h>

void aiAct::script_reload(void* script) {
    s8 target[16];
    target[0] = -1;
    act_change(0, target, 0, 0);
    m_script = script;
}

aiAct::~aiAct() { }

void aiAct::set_id(u16 id) {
    if (m_id != 0) {
        m_prevId = m_id;
    }
    m_id = id;
    m_input->swingChkReset(0);
}

void aiAct::setReflectDone() {
    m_reflectDone = 180;
    m_reflectRate *= 0.85f;
}

void aiAct::addDefendAirRate(float amount) {
    m_defendAirRate = m_defendAirRate + amount;
}
