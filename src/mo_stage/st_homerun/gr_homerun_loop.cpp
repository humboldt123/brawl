#include <st_homerun/gr_homerun.h>

grHomerunLoop::grHomerunLoop(const char* taskName) : grHomerun(taskName) {
    m_posGroundWork = NULL;
    m_posFloorWork = NULL;
    m_posSkyWork = NULL;
    m_posScoreWork = NULL;
    m_posLimitWork = NULL;
    m_scoreWork = NULL;
    m_scrollWork = NULL;
    m_scrollRateWork = NULL;
    m_type = 0;
    m_frameSceneWork = NULL;
}

grHomerunLoop::~grHomerunLoop() { }

void grHomerunLoop::processFixPosition() {
    fixpos();
}

void grHomerunLoop::fixpos() { }
