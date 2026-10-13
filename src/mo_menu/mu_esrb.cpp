#include <memory.h>
#include <types.h>

#include <mu/mu_esrb.h>

// MuMsg::create(): the real third argument is a number (the shared header has it as a heap type).
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);

// HYPOTHESIS: the constructor takes the name of the task (it keeps the string first in the data of the unit).
inline muESRBTask::muESRBTask(const char* name)
    : gfTask(name, Category_Menu, 0xF, 8, true), m_resFile(), m_scene(NULL), m_notice(NULL), m_boot(NULL), m_msg(NULL) {
    m_state = 0;
}

muESRBTask* muESRBTask::create() {
    return new (Heaps::MenuInstance) muESRBTask("BootOnline");
}

muESRBTask::~muESRBTask() { }

// The notice fades in, stays, and fades out.
void muESRBTask::processDefault() {
    switch (m_state) {
    case 0:
        break;
    case 1:
        m_notice->setFrame(0.0f);
        m_notice->m_modelAnim->setUpdateRate(1.0f);
        m_state = 2;
        break;
    case 2:
        m_boot->changeAnimN("MenBoot0000_TopN__0");
        m_boot->setFrame(0.0f);
        m_boot->m_modelAnim->setUpdateRate(1.0f);
        m_state = 3;
        m_frame = 0;
        break;
    case 3:
        if (static_cast<u32>(m_boot->isAnimFinished()) == 1) {
            m_state = 4;
        }
        break;
    case 4:
        m_boot->m_modelAnim->setUpdateRate(0.0f);
        m_state = 5;
        break;
    case 5:
        m_state = 6;
        m_frame = 0;
        break;
    case 6:
        if (static_cast<u32>(m_notice->isAnimFinished()) == 1) {
            m_state = 7;
        }
        break;
    case 7:
        m_state = 8;
        break;
    case 8:
        m_boot->changeAnimN("MenBoot0000_TopN__1");
        m_boot->setFrame(0.0f);
        m_boot->m_modelAnim->setUpdateRate(1.0f);
        m_state = 9;
        break;
    case 9:
        if (static_cast<u32>(m_boot->isAnimFinished()) == 1) {
            m_state = 10;
        }
        break;
    case 10:
        m_boot->m_modelAnim->setUpdateRate(0.0f);
        m_state = 11;
        break;
    case 11:
        break;
    case 12:
        m_state = m_state + 1;
        break;
    }
    m_frame++;
}

void muESRBTask::initialize(u8 language) {
    m_language = language;
    unk2C_b1 = false;
    m_frame = 0;
}

void muESRBTask::release() {
    m_resFile.Release();
    m_scene->exit();
    m_scene = NULL;
    delete m_notice;
    m_notice = NULL;
    delete m_boot;
    m_boot = NULL;
    delete m_msg;
    m_msg = NULL;
}

void muESRBTask::createData(gfArchive* archive) {
    m_resFile = nw4r::g3d::ResFile(archive->getData(static_cast<ARCNodeType>(2), 2, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_scene = fn_800AFBFC("BootOnline", 0x10, archive, 0xA);
    m_notice = MuObject::create(&m_resFile, "MenBoot0002_TopN", 1, NULL, Heaps::MenuInstance);
    m_boot = MuObject::create(&m_resFile, "MenBoot0000_TopN", 1, NULL, Heaps::MenuInstance);
    m_boot->changeAnimN("MenBoot0000_TopN__0");
    m_boot->setFrame(0.0f);
    m_boot->m_modelAnim->setUpdateRate(0.0f);
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(8, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x200, 1);
    m_msg->setMsgData(archive->getData(static_cast<ARCNodeType>(1), 0, 0xFFFE));
    m_msg->attachScnMdlSimple(0, m_notice->m_sceneModel, 0, 0.0625f);
    m_msg->setFontType(0, 4);
    m_msg->setFontColor(0, 0x40, 0x40, 0x40, 0xFF);
    switch (m_language) {
    case 0:
        m_msg->printIndex(0, 0, NULL);
        break;
    case 2:
    default:
        m_msg->printIndex(0, 0, NULL);
        break;
    case 3:
        m_msg->printIndex(0, 1, NULL);
        break;
    case 4:
        m_msg->printIndex(0, 2, NULL);
        break;
    }
}

void muESRBTask::addModel() {
    m_scene->m_group->Insert(m_scene->m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_notice->m_scnMdl));
    m_scene->m_group->Insert(m_scene->m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_boot->m_scnMdl));
    m_notice->changeAnimN("MenBoot0002_TopN__0");
    m_notice->setFrame(0.0f);
    m_notice->m_modelAnim->setUpdateRate(1.0f);
}

void muESRBTask::removeModel() {
    m_scene->m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_notice->m_scnMdl));
    m_scene->m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_boot->m_scnMdl));
}

// Whether the notice has stayed long enough to go on.
bool muESRBTask::isNextMode() {
    if ((m_state == 6 || m_state == 9) && m_frame >= 0x3C) {
        return true;
    }
    return false;
}

void muESRBTask::setNextMode() {
    m_state = 11;
}
