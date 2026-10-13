#include <gf/gf_shutdown_manager.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_strap.h>

muStrapTask* muStrapTask::create() {
    g_gfShutdownManager.unkC = true;
    return new (Heaps::MenuInstance) muStrapTask();
}

// The names of the files and models of the strap for each language.
muStrapTask::muStrapTask()
    : gfTask("Strap", Category_Menu, 0xF, 8, true), m_resFile(), m_scene(NULL), m_strap(NULL), m_boot(NULL), m_state(0) {
    memset(m_strapFile, 0, sizeof(m_strapFile));
    sprintf(m_strapFile[0], "/menu/common/strap_jp.brres");
    sprintf(m_strapFile[1], "/menu/common/strap_en.brres");
    sprintf(m_strapFile[2], "/menu/common/strap_fr.brres");
    sprintf(m_strapFile[3], "/menu/common/strap_sp.brres");
    memset(m_modelName, 0, sizeof(m_modelName));
    sprintf(m_modelName[0], "MenBoot0001Ja_TopN");
    sprintf(m_modelName[1], "MenBoot0001En_TopN");
    sprintf(m_modelName[2], "MenBoot0001Fr_TopN");
    sprintf(m_modelName[3], "MenBoot0001Sp_TopN");
    memset(m_animName, 0, sizeof(m_animName));
    sprintf(m_animName[0], "MenBoot0001Ja_TopN__0");
    sprintf(m_animName[1], "MenBoot0001En_TopN__0");
    sprintf(m_animName[2], "MenBoot0001Fr_TopN__0");
    sprintf(m_animName[3], "MenBoot0001Sp_TopN__0");
}

muStrapTask::~muStrapTask() { }

// The strap comes in, stays about 20 seconds (or until it is skipped) and goes out.
void muStrapTask::processDefault() {
    switch (m_state) {
    case 0:
        break;
    case 1:
        m_state = 2;
        break;
    case 2:
        m_strap->setFrame(0.0f);
        m_strap->m_modelAnim->setUpdateRate(1.0f);
        m_boot->changeAnimN("MenBoot0000_TopN__0");
        m_boot->setFrame(0.0f);
        m_boot->m_modelAnim->setUpdateRate(1.0f);
        m_state = 3;
        break;
    case 3:
        if (static_cast<u32>(m_boot->isAnimFinished()) == 1) {
            m_state = 4;
        }
        break;
    case 4:
        m_boot->m_modelAnim->setUpdateRate(0.0f);
        m_state = 5;
        m_frame = 0;
        break;
    case 5:
        m_state = 6;
        break;
    case 6:
        if (m_frame >= 0x4B0) {
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

// The language is the one of the game (0: japanese, 3: french, 4: spanish; the others show the english strap).
void muStrapTask::initialize(int language) {
    unk2C_b1 = false;
    switch (language) {
    case 0:
        m_language = 0;
        break;
    case 2:
    default:
        m_language = 1;
        break;
    case 3:
        m_language = 2;
        break;
    case 4:
        m_language = 3;
        break;
    }
    m_frame = 0;
}

void muStrapTask::release() {
    m_resFile.Release();
    m_scene->exit();
    m_scene = NULL;
    delete m_strap;
    m_strap = NULL;
    delete m_boot;
    m_boot = NULL;
}

void muStrapTask::createData(gfArchive* archive) {
    m_resFile = nw4r::g3d::ResFile(archive->getData(static_cast<ARCNodeType>(2), 0, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_scene = fn_800AFBFC("Strap", 0x10, archive, 0xA);
    m_strap = MuObject::create(&m_resFile, m_modelName[m_language], 1, NULL, Heaps::MenuInstance);
    m_strap->changeAnimN(m_animName[m_language]);
    m_strap->m_modelAnim->setUpdateRate(0.0f);
    m_boot = MuObject::create(&m_resFile, "MenBoot0000_TopN", 1, NULL, Heaps::MenuInstance);
    m_boot->changeAnimN("MenBoot0000_TopN__0");
    m_boot->setFrame(0.0f);
    m_boot->m_modelAnim->setUpdateRate(0.0f);
}

void muStrapTask::addModel() {
    m_scene->m_group->Insert(m_scene->m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_strap->m_scnMdl));
    m_scene->m_group->Insert(m_scene->m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_boot->m_scnMdl));
}

void muStrapTask::removeModel() {
    m_scene->m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_strap->m_scnMdl));
    m_scene->m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_boot->m_scnMdl));
}

// Whether the strap has stayed long enough to go on.
bool muStrapTask::isNextMode() {
    if (m_frame >= 0x3C && m_state == 6) {
        return true;
    }
    return false;
}

void muStrapTask::setNextMode() {
    m_state = 7;
}
