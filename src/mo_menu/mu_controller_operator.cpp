#include <memory.h>
#include <types.h>

#include <mu/mu_controller_operator.h>

// HYPOTHESIS: the constructor takes the name of the task (it keeps the string first in the data of the unit).
inline muControllerOperatorTask::muControllerOperatorTask(const char* name)
    : gfTask(name, Category_Menu, 0xF, 8, true), m_resFile(), m_scene(NULL), m_object(NULL), m_state(0) { }

muControllerOperatorTask* muControllerOperatorTask::create() {
    return new (Heaps::MenuInstance) muControllerOperatorTask("ControllerOperator");
}

muControllerOperatorTask::~muControllerOperatorTask() { }

// The model plays its animation (about 2 seconds after it has ended the task goes on).
void muControllerOperatorTask::processDefault() {
    switch (m_state) {
    case 0:
        break;
    case 1:
        m_state = 2;
        break;
    case 2:
        m_object->setFrame(0.0f);
        m_object->m_modelAnim->setUpdateRate(1.0f);
        m_state = 3;
        m_frame = 0;
        break;
    case 3:
        if (static_cast<u32>(m_object->isAnimFinished()) == 1) {
            m_state = 4;
        }
        break;
    case 4:
        if (m_frame >= 0x78) {
            m_state = 5;
        }
        break;
    case 5:
        m_state = 6;
        break;
    case 6:
        break;
    case 7:
        m_state = m_state + 1;
        break;
    }
    m_frame++;
}

void muControllerOperatorTask::initialize() {
    m_frame = 0;
    unk2C_b1 = false;
}

void muControllerOperatorTask::release() {
    m_resFile.Release();
    m_scene->exit();
    m_scene = NULL;
    delete m_object;
    m_object = NULL;
}

void muControllerOperatorTask::createData(gfArchive* archive) {
    m_resFile = nw4r::g3d::ResFile(archive->getData(static_cast<ARCNodeType>(2), 0, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_scene = fn_800AFBFC("ControllerOperator", 0x10, archive, 0xA);
    m_object = MuObject::create(&m_resFile, "MenController0000_TopN", 1, NULL, Heaps::MenuInstance);
    m_object->changeAnimN("MenController0000_TopN__0");
    m_object->setFrame(0.0f);
    m_object->m_modelAnim->setUpdateRate(0.0f);
}

void muControllerOperatorTask::addModel() {
    m_scene->m_group->Insert(m_scene->m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
}

void muControllerOperatorTask::removeModel() {
    m_scene->m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
}

// Stops the animation at the frame 30 if it is not that far yet.
void muControllerOperatorTask::setLastFrame() {
    float frame = m_object->m_modelAnim->getFrame();
    if (frame < 30.0f) {
        frame = 30.0f;
        m_object->setFrame(frame);
        m_object->m_modelAnim->setUpdateRate(0.0f);
    }
}

// Whether the animation has ended or it is on its last frames (the states 3 and 4).
bool muControllerOperatorTask::isNextMode() {
    return static_cast<u32>(m_state - 3) <= 1;
}

void muControllerOperatorTask::setNextMode() {
    m_state = 6;
}
