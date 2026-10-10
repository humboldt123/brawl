#include <memory.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <types.h>

#include <st_pictchat/gr_pictchat.h>

// The ladder of the picture: it has the area of a ladder only while the picture is there (the stage says so with a flag).
grPictchatLadder::grPictchatLadder(const char* taskName) : grGimmickLadder(taskName) {
    m_state = 0;
    m_posWork = NULL;
    m_pictIDWork = NULL;
    m_pictID = 0;
    m_flgWork = NULL;
    m_triggerMade = 0;
    m_startFlag = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        setupMelee();
    }
}

grPictchatLadder* grPictchatLadder::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatLadder* ground = new (Heaps::StageInstance) grPictchatLadder(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatLadder::~grPictchatLadder() {
}

void grPictchatLadder::processGameProc() {
    Ground::processGameProc();
    if (m_startFlag == 1) {
        disableArea();
        m_startFlag = 0;
    }
}

void grPictchatLadder::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    setNode();
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
    }
}

// Once the stage hands over the data of the ladder, the area of it is added to the Yakumono and a ladder trigger observes
// the Yakumono. After that the ladder only follows the position the stage gives.
void grPictchatLadder::updateYakumono(float deltaFrame) {
    if (m_posWork != NULL) {
        if (m_triggerMade == 1) {
            setPos(m_posWork);
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

// The ladder is turned on when the picture is drawn (the stage says the flag) and off when the flag goes.
void grPictchatLadder::updateActive(float deltaFrame) {
    switch (m_state) {
    case 0:
        m_startFlag = 1;
        m_state = 1;
        break;
    case 1:
        if (*m_pictIDWork == m_pictID && *m_flgWork == 1) {
            enableArea();
            m_state = 2;
        }
        break;
    case 2:
        if (*m_flgWork == 0) {
            m_state = 0;
        }
        break;
    }
}

void grPictchatLadder::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
}
