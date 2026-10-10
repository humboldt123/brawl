#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba12::grJungleAshiba12(const char* taskName) : grJungleAshiba(taskName) {}

grJungleAshiba12* grJungleAshiba12::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba12* ground = new (Heaps::StageInstance) grJungleAshiba12(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba12::~grJungleAshiba12() {
}

void grJungleAshiba12::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    if (m_posWork != NULL) {
        getNodePosition(m_posGimmickWork, 0, "L12LstartN");
        getNodePosition(&m_posGimmickWork[1], 0, "L12LendN");
        getNodePosition(&m_posGimmickWork[2], 0, "L12RstartN");
        getNodePosition(&m_posGimmickWork[3], 0, "L12RendN");
    }
}
