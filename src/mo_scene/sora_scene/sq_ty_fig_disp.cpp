#include <gf/gf_scene.h>
#include <sq/sq_ty_fig_disp.h>
#include <sr/sr_common.h>
#include <types.h>

sqTyFigDisp* sqTyFigDisp::create() {
    return new (Heaps::GlobalMode) sqTyFigDisp("sqTyFigDisp");
}

void sqTyFigDisp::start() {
    m_state = 0;
}

void sqTyFigDisp::setNext() {
    gfSceneManager* mgr = gfSceneManager::getInstance();
    bool loop = true;
    while (loop) {
        loop = false;
        switch (m_state) {
        default:
            m_state = 0;
        case 0:
            m_state = 1;
            loop = true;
            m_stateAfterMemoryChange = 3;
            m_memoryLayout = 0xF;
            break;
        case 1:
            mgr->setNextScene("scMemoryChange", m_memoryLayout);
            m_state = 2;
            break;
        case 2:
            m_state = m_stateAfterMemoryChange;
            loop = true;
            break;
        case 3:
            mgr->setNextScene("scTyFigDisp", 0);
            m_state = 4;
            break;
        case 4:
            // HYPOTHESIS: unk1 is the result the trophy viewer leaves behind (1: open the second viewer).
            switch (mgr->unk1) {
            case 0:
                m_state = 7;
                break;
            case 1:
                m_state = 5;
                break;
            }
            loop = true;
            break;
        case 5:
            mgr->setNextScene("scTyFigDisp2", 0);
            m_state = 6;
            break;
        case 6:
            m_state = 7;
            loop = true;
            break;
        case 7:
            mgr->setNextSequence("sqMenuMain", 0x12);
            m_state = 8;
            break;
        }
    }
}

void sqTyFigDisp::exit() { }
