#include <types.h>

#include <gf/gf_scene.h>
#include <mu/mu_menu_main.h>
#include <mu/mu_wifi_cnct_wnd_task.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_top.h>

// Where the cursor goes from each icon (up, down, left, right).
static muProcMenuNode sTopNodes[muProcTop::Entry_Count] = {
    { { 2, 1 }, { 2, 0 }, { 1, 1 }, { 1, 0 } },
    { { 4, 1 }, { 4, 0 }, { 0, 0 }, { 0, 1 } },
    { { 0, 0 }, { 0, 1 }, { 5, 1 }, { 3, 0 } },
    { { 0, 0 }, { 0, 1 }, { 2, 0 }, { 4, 0 } },
    { { 1, 0 }, { 1, 1 }, { 3, 0 }, { 5, 0 } },
    { { 1, 0 }, { 1, 1 }, { 4, 0 }, { 2, 1 } },
};

void muProcTop::init(nw4r::g3d::ScnGroup* group, int from) {
    int cursor;
    int panelProc;
    int panelMsg;
    u32 i;
    m_wifiWnd = NULL;
    m_nodes = sTopNodes;
    m_group = group;
    m_objBase = 0;
    m_waitCount = 0;
    m_code = from;
    for (i = 0; i < Entry_Count; i++) {
        m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[i]->m_sceneModel));
    }

    switch (m_code) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        // Back from one of the screens: the cursor is on its icon
        m_unk664 = 2;
        m_cursor = from - 1;
        m_panel->changeProcess1(m_cursor + 1, m_cursor);
        break;
    default:
        // The menu has just opened (code 0x28: coming back from Wi-Fi)
        m_unk664 = 0;
        switch (getSceneStartCode()) {
        case 0:
            cursor = 0;
            panelProc = 1;
            panelMsg = 0;
            break;
        case 0x28:
            cursor = Entry_Wifi;
            panelProc = 3;
            panelMsg = 2;
            break;
        }
        m_cursor = cursor;
        m_code = 0;
        setComeAnim(true);
        m_panel->changeProcess1(panelProc, panelMsg);
        break;
    }
    setComeAnim(true);
}

int muProcTop::proc() {
    u32 pad = 0;
    if (m_wifiWnd != NULL) {
        int ret = 0;
        if (m_wifiWnd->m_step == 12) {
            // The connection window has closed: go on to the Wi-Fi screens if it connected
            switch (m_wifiWnd->m_result) {
            case 2:
                break;
            case 0:
                setOutAnim(true);
                m_result = 3;
                ret = 2;
                m_unk664 = 4;
                break;
            }
            m_wifiWnd->exit();
            m_wifiWnd = NULL;
            if (ret != 0) {
                return ret;
            }
        }
    } else {
        pad = padUpdate();
    }

    switch (m_unk664) {
    case 0:
        // The icons come in
        if (m_objs[m_cursor]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[m_cursor]->setAnimIdx(&m_anims[3], true);
            m_unk664 = 1;
        }
        break;
    case 1:
        // Wait a little before the cursor can move
        if (++m_waitCount >= 25.0f) {
            m_unk664 = 3;
        }
        break;
    case 2: {
        // Back from a screen: the icon of the cursor starts to blink when it has arrived
        MuObject* object = m_objs[m_cursor];
        if (muGetAnimIdx(object) == 2 && object->m_modelAnim->getFrame() == 9.0f) {
            m_objs[m_cursor]->setAnimIdx(&m_anims[2], true);
            m_unk664 = 3;
        }
        u16 oldCursor = m_cursor;
        if (selectProc(pad)) {
            setSelectAnim(oldCursor, true);
            m_panel->changeProcess1(m_cursor + 1, m_cursor);
            m_unk664 = 3;
            return 4;
        }
        if (pad & Pad_Ok) {
            if (m_cursor == Entry_Wifi) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                muMenuMain* menu = dynamic_cast<muMenuMain*>(gfSceneManager::getInstance()->m_currentScene);
                m_wifiWnd = muWifiCnctWndTask::create(0, &menu->m_unkA94, menu->getBuffer(9), m_group, 0x29,
                                                      &menu->m_unkA98, &menu->m_unkA9C);
            } else {
                m_panel->frameCancel();
                setOutAnim(true);
                m_unk664 = 4;
                m_result = m_cursor + 1;
                return 2;
            }
        } else if (pad & Pad_Cancel) {
            exitScene(0, false);
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        }
        break;
    }
    case 3: {
        // The cursor moves; the icon of the cursor keeps blinking
        MuObject* object = m_objs[m_cursor];
        if (muGetAnimIdx(object) == 4 && object->m_modelAnim->getFrame() >= 179.0f) {
            m_objs[m_cursor]->setAnimIdx(&m_anims[2], true);
        }
        u16 oldCursor = m_cursor;
        if (selectProc(pad)) {
            setSelectAnim(oldCursor, false);
            m_panel->changeProcess1(m_cursor + 1, m_cursor);
            return 0;
        }
        if (pad & Pad_Ok) {
            if (m_cursor == Entry_Wifi) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                muMenuMain* menu = dynamic_cast<muMenuMain*>(gfSceneManager::getInstance()->m_currentScene);
                m_wifiWnd = muWifiCnctWndTask::create(0, &menu->m_unkA94, menu->getBuffer(9), m_group, 0x29,
                                                      &menu->m_unkA98, &menu->m_unkA9C);
            } else {
                setOutAnim(true);
                m_unk664 = 4;
                m_result = m_cursor + 1;
                return 2;
            }
        } else if (pad & Pad_Cancel) {
            exitScene(0, false);
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        }
        break;
    }
    case 4:
        // The icons leave; the screen is done when the one of the cursor has stopped
        if (m_objs[m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcTop::exit() {
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[i]->m_sceneModel));
    }
    if (m_wifiWnd != NULL) {
        m_wifiWnd->exit();
        m_wifiWnd = NULL;
    }
}

void muProcTop::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0x14), -1, 0, 0, -1);
}
