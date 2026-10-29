#include <em/em_external_value_accesser.h>
#include <em/em_manager.h>
#include <em/enemy.h>
#include <sora_adv_stage/st_adventure_info_basis.h>

extern emManager* g_emManager;

stAdventureInfoIconParam stAdventureInfoBasis::getIconParamForEnemy(int createId, u32 taskId, int index,
                                                                    stAdventureInfoIconParam* prev) {
    Enemy* enemy = NULL;
    if (g_emManager != NULL) {
        enemy = emManager::getInstance()->getEnemyPtrFromId(createId);
    }
    stAdventureInfoIconParam param;
    if (enemy != NULL) {
        param.m_disp.m_index = index;
        param.m_disp.m_hp = emExternalValueAccesser::getHp(enemy, taskId);
        param.m_disp.m_maxHp = emExternalValueAccesser::getMaxHp(enemy, taskId);
        param.m_disp.m_nameMsgId = emExternalValueAccesser::getNameMsgId(enemy, taskId);
        nw4r::g3d::ResFileData* faceTex = emExternalValueAccesser::getFaceTexPtr(enemy, taskId);
        nw4r::g3d::ResFileData* prevTexData = prev->m_faceTexData;
        nw4r::g3d::ResFileData* prevTex = prev->m_disp.m_faceTex;
        param.m_faceTexData = faceTex;
        param.m_disp.m_faceTex = prevTex;
        param.m_isValid = true;
        if (faceTex != prevTexData) {
            nw4r::g3d::ResFile resFile(faceTex);
            resFile.HasResTex();
            param.m_disp.m_faceTex = resFile.ptr();
        }
    }
    return param;
}

Enemy* stAdventureInfoBasis::getEnemy(int createId) {
    Enemy* enemy = NULL;
    if (g_emManager != NULL) {
        enemy = emManager::getInstance()->getEnemyPtrFromId(createId);
    }
    return enemy;
}
