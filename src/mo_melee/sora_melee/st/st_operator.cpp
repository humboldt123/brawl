#include <gm/gm_global.h>
#include <st/operator/st_operator.h>
#include <types.h>

stOperator::stOperator(const char* name) :
    gfTask(name, Category_None, 0xD, 6, true), m_state(0), m_isStart(false), m_isExit(false), m_isPause(false),
    m_condition(NULL) {
    m_condition = g_GameGlobal->m_modeMelee;
}

stOperator::~stOperator() { }

void stOperator::processBegin() { }

bool stOperator::isExit() {
    return m_isExit == true;
}

bool stOperator::isPause() {
    return m_isPause == true;
}

void stOperator::requestExit() {
    m_isExit = true;
}

void stOperator::requestStart() {
    m_isStart = true;
}

void stOperator::requestPause(bool pause) {
    m_isPause = pause;
}

void stOperator::setCondition(gmGlobalModeMelee* condition) {
    m_condition = condition;
}
