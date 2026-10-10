#include <ec/ec_mgr.h>
#include <ef/ef_id.h>
#include <st_dxgarden/gr_dxgarden.h>

grDxGardenLamp* grDxGardenLamp::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxGardenLamp* ground = new (Heaps::StageInstance) grDxGardenLamp(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxGardenLamp::~grDxGardenLamp() { }

// The flame is an effect that sits on the model: the first lamp burns with the effect "a", the second with "b".
void grDxGardenLamp::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    switch (m_state) {
    case 0:
        switch (m_type) {
        case 0: {
            g_ecMgr->setDrawPrio(1);
            u32 handle = g_ecMgr->setEffect(ef_ptc_stg_dx_garden_rosoku_a);
            g_ecMgr->setDrawPrio(-1);
            g_ecMgr->setParent(handle, m_sceneModels[0], "null341", false);
            break;
        }
        case 1: {
            g_ecMgr->setDrawPrio(1);
            u32 handle = g_ecMgr->setEffect(ef_ptc_stg_dx_garden_rosoku_b);
            g_ecMgr->setDrawPrio(-1);
            g_ecMgr->setParent(handle, m_sceneModels[0], "null342", false);
            break;
        }
        }
        m_state = 1;
        break;
    case 1:
        break;
    }
}
