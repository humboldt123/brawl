#include <memory.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>

// (inline: the original has no constructor of its own)
inline grJungleLadder::grJungleLadder(const char* taskName) : grGimmickLadder(taskName) {
    m_posWork = NULL;
    m_posHashigoWork = NULL;
    m_triggerMade = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 2;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[1].m_flags |= 1;
    }
}

grJungleLadder* grJungleLadder::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleLadder* ground = new (Heaps::StageInstance) grJungleLadder(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleLadder::~grJungleLadder() {
}

void grJungleLadder::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    setNode();
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Once the stage hands over the data of the ladder, the area of it is added to the Yakumono and a ladder trigger observes
// the Yakumono. After that the ladder follows the place the stage gives.
void grJungleLadder::updateYakumono(float deltaFrame) {
    if (m_posWork != NULL) {
        if (m_triggerMade == 1) {
            Vec3f pos;
            pos.m_x = m_posHashigoWork->m_x;
            pos.m_y = m_posHashigoWork->m_y;
            pos.m_z = 0.0f;
            setPos(&pos);
        } else {
            m_ladderData = static_cast<grGimmickLadderData*>(getGimmickData());
            if (m_ladderData != NULL) {
                // MATCH-ONLY: the shape type and the group of the area are set as two bytes
                reinterpret_cast<u8*>(&m_areaData)[0] = 0;
                reinterpret_cast<u8*>(&m_areaData)[1] = gfArea::Stage_Group_Gimmick_Ladder;
                m_areaData.m_0x4 = 0;
                m_areaData.m_0x8 = 0;
                m_areaData.m_shapeFlag.m_mask = 0;
                m_areaData.m_nodeIndex = 0;
                m_areaData.m_offsetPos.m_x = m_ladderData->m_areaData.m_offsetPos.m_x;
                m_areaData.m_offsetPos.m_y = m_ladderData->m_areaData.m_offsetPos.m_y;
                m_areaData.m_range.m_x = m_ladderData->m_areaData.m_range.m_x;
                m_areaData.m_range.m_y = m_ladderData->m_areaData.m_range.m_y;
                grYakumono::setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, false);
                if (m_yakumono != NULL) {
                    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Ladder, -1);
                    if (trigger != NULL) {
                        trigger->setObserveYakumono(m_yakumono);
                        m_triggerMade = 1;
                    }
                }
            }
        }
    }
}

void grJungleLadder::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = *m_sceneModels;
        if (scnMdl != NULL) {
            calcWorldCallBack->m_index = 1;
            calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex = m_nodeIndex;
            scnMdl->m_calcWorldCallBack = calcWorldCallBack;
            scnMdl->EnableScnMdlCallbackTiming(1);
            scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex;
            if (m_posWork != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data[1].m_pos.m_x = m_posWork->m_x;
                data[1].m_pos.m_y = m_posWork->m_y;
                data[1].m_pos.m_z = m_posWork->m_z;
            }
        }
    }
}

void grJungleLadder::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(archive, unk1, layerType);
}
