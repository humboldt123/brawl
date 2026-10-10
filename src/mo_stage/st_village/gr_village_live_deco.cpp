#include <memory.h>
#include <types.h>

#include <st_village/gr_village.h>

grVillageLiveDeco* grVillageLiveDeco::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageLiveDeco* ground = new (Heaps::StageInstance) grVillageLiveDeco(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageLiveDeco::~grVillageLiveDeco() {
}

void grVillageLiveDeco::update(float deltaFrame) {
    grVillage::update(deltaFrame);
    if (m_isUpdate) {
        updateLight(deltaFrame);
    }
}

// The lamps are lit when the stage says that the bar has its live night.
void grVillageLiveDeco::updateLight(float deltaFrame) {
    switch (m_state) {
    case 0:
        setVisibility(0);
        m_state = 4;
        break;
    case 4: {
        u8 light = *m_stateWork;
        if (m_light != light) {
            switch (light) {
            case 0:
                setVisibility(1);
                setNodeVisibility(false, 0, m_node[0], true, false);
                setNodeVisibility(false, 0, m_node[1], true, false);
                setNodeVisibility(false, 0, m_node[2], true, false);
                setNodeVisibility(false, 0, m_node[3], true, false);
                break;
            case 1:
                setVisibility(1);
                setNodeVisibility(true, 0, m_node[0], true, false);
                setNodeVisibility(true, 0, m_node[1], true, false);
                setNodeVisibility(true, 0, m_node[2], true, false);
                setNodeVisibility(true, 0, m_node[3], true, false);
                break;
            default:
                setVisibility(1);
                break;
            }
            m_light = *m_stateWork;
        }
        break;
    }
    }
}

bool grVillageLiveDeco::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "rampHikari1");
    getNodeIndex(&m_node[1], 0, "rampHikari2");
    getNodeIndex(&m_node[2], 0, "rampHikari3");
    getNodeIndex(&m_node[3], 0, "rampHikari4");
    return result;
}
