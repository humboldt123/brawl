#include <memory.h>
#include <types.h>

#include <mu/mu_now_loading.h>

// HYPOTHESIS: the constructor takes the name of the task (it keeps the string first in the data of the unit).
inline muNowLoadingTask::muNowLoadingTask(const char* name)
    : gfTask(name, Category_Menu, 0xF, 8, true), m_resFile(), m_state(0) { }

muNowLoadingTask* muNowLoadingTask::create() {
    return new (Heaps::MenuInstance) muNowLoadingTask("NowLoading");
}

muNowLoadingTask::~muNowLoadingTask() { }

void muNowLoadingTask::processDefault() {
    switch (m_state) {
    case 0:
        break;
    case 1:
        m_state = 2;
        break;
    case 2:
        m_object->m_modelAnim->setUpdateRate(1.0f);
        m_state = 3;
        break;
    case 3:
        break;
    case 4:
        break;
    }
    m_frame++;
}

void muNowLoadingTask::initialize() {
    unk2C_b1 = false;
    m_frame = 0;
}

void muNowLoadingTask::release() {
    m_resFile.Release();
    m_scene->exit();
    m_scene = NULL;
    delete m_object;
    m_object = NULL;
}

void muNowLoadingTask::createData(gfArchive* archive) {
    m_resFile = nw4r::g3d::ResFile(archive->getData(static_cast<ARCNodeType>(2), 1, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_scene = fn_800AFBFC("NoeLoading", 0x10, archive, 0xA);
    m_object = MuObject::create(&m_resFile, "MenBoot0003_TopN", 1, NULL, Heaps::MenuInstance);
    m_object->changeAnimN("MenBoot0003_TopN__0");
    m_object->m_modelAnim->setUpdateRate(0.0f);
}

void muNowLoadingTask::addModel() {
    m_scene->m_group->Insert(m_scene->m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
}

void muNowLoadingTask::removeModel() {
    m_scene->m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
}
