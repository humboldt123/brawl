#include <gf/gf_heap_manager.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <revolution/OS.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_boot_network.h>
#include <mu/mu_simple_ending.h>

// main.dol functions with names that the shared headers do not declare.
// Creates the camera task that draws the menu models (the task keeps the scene group at +0x48)
extern "C" muEndCameraTask* fn_800AFBFC(const char* name, int heap, gfArchive* archive, int unk);
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// nw4r::g3d::ResFile::Release()
extern "C" void Release__Q34nw4r3g3d7ResFileFv(nw4r::g3d::ResFile* resFile);

muBootNetworkTask* muBootNetworkTask::create() {
    return new (Heaps::MenuInstance) muBootNetworkTask("BootNetwork");
}

muBootNetworkTask::~muBootNetworkTask() { }

// Takes the open window away.
inline void muBootNetworkTask::closeWnd() {
    if (m_wnd != NULL) {
        fn_800BF830(m_wnd);
        m_wnd->exit();
        m_wnd = NULL;
    }
}

// Whether the server sent anything.
inline bool muBootNetworkTask::hasCount() const {
    if (m_countA > 0) {
        return true;
    }
    if (m_countB > 0) {
        return true;
    }
    return m_countC > 0;
}

// The steps of the network check: the check is started, the "please wait" panel is shown while it runs and the player
// is told what it found.
void muBootNetworkTask::processDefault() {
    switch (m_step) {
    case 0x01:
        m_step = 2;
        break;
    case 0x02: {
        // starts the check on the network
        nteBootTask* task = static_cast<nteBootTask*>(operator new(sizeof(nteBootTask), Heaps::MenuInstance));
        if (task != NULL) {
            task = fn_8015820C(task, Heaps::MenuInstance);
        }
        m_task = task;
        fn_80158334(task);
        m_step = 3;
        m_frame = 0;
        break;
    }
    case 0x03:
        // waits for the check, but not longer than three seconds before the panel is shown
        if (m_task->m_finished == 1) {
            if (m_task->m_errorState != 0) {
                OSReport("NetEtcBoot ErrorState[%d]\n", m_task->m_errorState);
            }
            m_step = 8;
        } else if (m_frame >= 0xB4) {
            m_step = 4;
        }
        m_frame++;
        break;
    case 0x04: {
        // shows the "please wait" panel
        void* msgData = m_msgData;
        closeWnd();
        m_wnd = fn_800BEEB8(3, msgData, 0x1A, &m_resFileMsg, m_cameraTask->m_scnRoot, 0x14, -1, Heaps::MenuInstance,
                            Heaps::MenuResource, 0, 0, 0xF0);
        nw4r::g3d::ScnGroup* root = m_cameraTask->m_scnRoot;
        root->Insert(root->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
        m_object->setFrame(0.0f);
        m_frame = 0;
        m_step = 5;
        break;
    }
    case 0x05:
        // the panel stays at least a second
        if (m_task->m_finished == 1 && m_frame >= 0x3C) {
            if (m_task->m_errorState != 0) {
                OSReport("NetEtcBoot ErrorState[%d]\n", m_task->m_errorState);
            }
            fn_800BF828(m_wnd, 2);
            nw4r::g3d::ScnGroup* root = m_cameraTask->m_scnRoot;
            root->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
            m_step = 6;
        }
        m_frame++;
        break;
    case 0x06:
        if (m_wnd->m_step == 3) {
            m_step = 7;
        }
        break;
    case 0x07:
        closeWnd();
        m_step = 8;
        break;
    case 0x08:
        // takes the counts that the server sent
        m_countA = m_task->m_countA;
        m_countB = m_task->m_countB;
        m_countC = m_task->m_countC;
        m_task->exit();
        m_task = NULL;
        m_step = (hasCount() == 1) ? 9 : 0xF;
        break;
    case 0x09: {
        // tells the player what was received
        createMessageFromReceiveCount();
        int size = strlen(m_message) + 1;
        closeWnd();
        m_wnd = fn_800BF048(0, m_message, size, &m_resFileMsg, m_cameraTask->m_scnRoot, 0x14, 0, Heaps::MenuInstance,
                            Heaps::MenuResource, 0, 0, 0xF0);
        m_step = 0xA;
        break;
    }
    case 0x0A:
        if (m_wnd->m_step == 3) {
            m_step = 0xB;
        }
        break;
    case 0x0B:
        closeWnd();
        m_step = 0xC;
        break;
    case 0x0C: {
        void* msgData = m_msgData;
        closeWnd();
        m_wnd = fn_800BEEB8(0, msgData, 0x20, &m_resFileMsg, m_cameraTask->m_scnRoot, 0x14, 0, Heaps::MenuInstance,
                            Heaps::MenuResource, 0, 0, 0xF0);
        m_step = 0xD;
        break;
    }
    case 0x0D:
        if (m_wnd->m_step == 3) {
            m_step = 0xE;
        }
        break;
    case 0x0E:
        closeWnd();
        m_step = 0xF;
        break;
    case 0x0F:
    case 0x10:
        // the check is over
        break;
    }
}

void muBootNetworkTask::initialize() {
    m_countA = 0;
    m_countB = 0;
    m_countC = 0;
    memset(m_message, 0, sizeof(m_message));
    m_frame = 0;
}

// Takes the open window away (the part of the release of the task that the owner can call).
void muBootNetworkTask::releaseWnd() {
    if (m_wnd != NULL) {
        fn_800BF830(m_wnd);
        m_wnd->exit();
        m_wnd = NULL;
    }
}

// Creates the camera, the "please wait" panel and the texts.
void muBootNetworkTask::createData(gfArchive* archive) {
    m_msgData = archive->getData(Data_Type_Misc, 0, 0xFFFE);
    m_resFileMsg = nw4r::g3d::ResFile(archive->getData(Data_Type_Model, 0, 0xFFFE));
    m_resFileWait = nw4r::g3d::ResFile(archive->getData(Data_Type_Model, 1, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resFileMsg);
    nw4r::g3d::ResFile::Init(&m_resFileWait);
    m_cameraTask = fn_800AFBFC("BootNetworkTask", 0x10, archive, 0xA);
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x200, 1);
    m_msg->setMsgData(m_msgData);
    MuObject* object = MuObject::create(&m_resFileWait, "InfCmnWait_TopN", 1, NULL, Heaps::MenuInstance);
    m_object = object;
    object->m_scnMdl->SetPriorityDrawOpa(0xFF);
    object->m_scnMdl->SetPriorityDrawXlu(0xFF);
    Vec3f pos(0.0f, -7.0f, 0.0f);
    m_object->setTrans(&pos);
}

void muBootNetworkTask::destroyData() {
    m_msgData = NULL;
    Release__Q34nw4r3g3d7ResFileFv(&m_resFileMsg);
    Release__Q34nw4r3g3d7ResFileFv(&m_resFileWait);
    m_cameraTask->exit();
    m_cameraTask = NULL;
    delete m_object;
    m_object = NULL;
    delete m_msg;
    m_msg = NULL;
}

// Writes the text that tells the player how many of the three things were received: for every count that is not zero
// the name of the thing, the count and the unit, one line each.
void muBootNetworkTask::createMessageFromReceiveCount() {
    if (m_countA > 0) {
        char name[0x100];
        int nameSize;
        memset(name, 0, sizeof(name));
        const char* nameData = m_msg->getMsgData(0x1B, &nameSize);
        memcpy(name, nameData, nameSize);
        strcat(m_message, name);
        int count = m_countA;
        char number[0x100];
        memset(number, 0, sizeof(number));
        sprintf(number, "%2d", count);
        strcat(m_message, number);
        char unit[0x100];
        int unitSize;
        memset(unit, 0, sizeof(unit));
        const char* unitData = m_msg->getMsgData(0x1E, &unitSize);
        memcpy(unit, unitData, unitSize);
        strcat(m_message, unit);
        strcat(m_message, "\n");
    }
    if (m_countB > 0) {
        char name[0x100];
        int nameSize;
        memset(name, 0, sizeof(name));
        const char* nameData = m_msg->getMsgData(0x1C, &nameSize);
        memcpy(name, nameData, nameSize);
        strcat(m_message, name);
        int count = m_countB;
        char number[0x100];
        memset(number, 0, sizeof(number));
        sprintf(number, "%2d", count);
        strcat(m_message, number);
        char unit[0x100];
        int unitSize;
        memset(unit, 0, sizeof(unit));
        const char* unitData = m_msg->getMsgData(0x1E, &unitSize);
        memcpy(unit, unitData, unitSize);
        strcat(m_message, unit);
        strcat(m_message, "\n");
    }
    if (m_countC > 0) {
        char name[0x100];
        int nameSize;
        memset(name, 0, sizeof(name));
        const char* nameData = m_msg->getMsgData(0x1D, &nameSize);
        memcpy(name, nameData, nameSize);
        strcat(m_message, name);
        int count = m_countC;
        char number[0x100];
        memset(number, 0, sizeof(number));
        sprintf(number, "%2d", count);
        strcat(m_message, number);
        char unit[0x100];
        int unitSize;
        memset(unit, 0, sizeof(unit));
        const char* unitData = m_msg->getMsgData(0x1E, &unitSize);
        memcpy(unit, unitData, unitSize);
        strcat(m_message, unit);
        strcat(m_message, "\n");
    }
    char tail[0x100];
    int tailSize;
    memset(tail, 0, sizeof(tail));
    const char* tailData = m_msg->getMsgData(0x1F, &tailSize);
    memcpy(tail, tailData, tailSize);
    strcat(m_message, tail);
}
