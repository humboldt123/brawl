#include <ai/ai_mgr.h>
#include <ec/ec_mgr.h>
#include <ef/ef_id.h>
#include <snd/snd_id.h>
#include <st_dxgarden/gr_dxgarden.h>

grDxGardenSuimen* grDxGardenSuimen::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxGardenSuimen* ground = new (Heaps::StageInstance) grDxGardenSuimen(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxGardenSuimen::grDxGardenSuimen(const char* taskName) : grDxGarden(taskName) {
    m_posLimitWork = NULL;
    m_dangerZone = -1;
}

grDxGardenSuimen::~grDxGardenSuimen() { }

// The splash effects start on the first frame; every frame afterwards the AI is told that the river is dangerous.
void grDxGardenSuimen::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    switch (m_state) {
    case 0: {
        Vec3f pos;
        g_ecMgr->setDrawPrio(1);
        u32 handle = g_ecMgr->setEffect(ef_ptc_stg_dx_garden_sibuki_01);
        g_ecMgr->setParent(handle, m_sceneModels[0], "null364", false);
        handle = g_ecMgr->setEffect(ef_ptc_stg_dx_garden_sibuki_01);
        g_ecMgr->setParent(handle, m_sceneModels[0], "null365", false);
        handle = g_ecMgr->setEffect(ef_ptc_stg_dx_garden_sibuki_01);
        g_ecMgr->setParent(handle, m_sceneModels[0], "ptcp", false);
        handle = g_ecMgr->setEffect(ef_ptc_stg_dx_garden_sibuki_01);
        g_ecMgr->setParent(handle, m_sceneModels[0], "ptcp1", false);
        handle = g_ecMgr->setEffect(ef_ptc_stg_dx_garden_sibuki_01);
        g_ecMgr->setParent(handle, m_sceneModels[0], "ptcp2", false);
        handle = g_ecMgr->setEffect(ef_ptc_stg_dx_garden_sibuki_01);
        g_ecMgr->setParent(handle, m_sceneModels[0], "ptcp3", false);
        g_ecMgr->setDrawPrio(-1);
        getNodePosition(&pos, 0, "ptcp");
        m_snd.playSE(snd_se_stage_Garden_river_ambient, 0, 0, -1);
        m_snd.setPos(&pos);
        m_state = 1;
        break;
    }
    case 1: {
        Vec3f pos;
        getNodePosition(&pos, 0, "ptcp");
        Vec2f from(m_posLimitWork[0], pos.m_y);
        Vec2f to(m_posLimitWork[3], m_posLimitWork[4]);
        m_dangerZone = g_aiMgr->setDangerZone(&from, &to, m_dangerZone, false, false);
        break;
    }
    }
}
