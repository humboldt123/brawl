#include <memory.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>

#include <st_donkey/gr_donkey.h>

inline grDonkeyLadder::grDonkeyLadder(const char* taskName) : grGimmickLadder(taskName) {
    m_posWork = NULL;
    m_triggerMade = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        setupMelee();
    }
}

grDonkeyLadder* grDonkeyLadder::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDonkeyLadder* ground = new (Heaps::StageInstance) grDonkeyLadder(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDonkeyLadder::~grDonkeyLadder() {
}

void grDonkeyLadder::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    setNode();
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
    }
}

// Once the stage hands over the data of the ladder, the area of it is added to the Yakumono and a ladder trigger observes
// the Yakumono. After that the ladder only follows the position the stage gives.
void grDonkeyLadder::updateYakumono(float deltaFrame) {
    if (m_posWork != NULL) {
        if (m_triggerMade == 1) {
            setPos(m_posWork);
        } else {
            m_ladderData = static_cast<grGimmickLadderData*>(getGimmickData());
            if (m_ladderData != NULL) {
                m_areaData.m_shapeType = static_cast<soAreaInstance::ShapeType>(0);
                m_areaData.m_group = gfArea::Stage_Group_Gimmick_Ladder;
                m_areaData.m_0x4 = 0;
                m_areaData.m_0x8 = 0;
                m_areaData.m_shapeFlag.m_mask = 0;
                m_areaData.m_nodeIndex = 0;
                m_areaData.m_offsetPos.m_x = m_ladderData->m_areaData.m_offsetPos.m_x;
                m_areaData.m_offsetPos.m_y = m_ladderData->m_areaData.m_offsetPos.m_y;
                m_areaData.m_range.m_x = m_ladderData->m_areaData.m_range.m_x;
                m_areaData.m_range.m_y = m_ladderData->m_areaData.m_range.m_y;
                setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, false);
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

void grDonkeyLadder::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
}
