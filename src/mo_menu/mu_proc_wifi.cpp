#include <types.h>

#include <gf/gf_scene.h>
#include <mu/mu_menu_main.h>
#include <mu/mu_wifi_cnct_wnd_task.h>
#include <nt/nt_etc_global.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_wifi.h>

// Where the cursor goes from each icon (up, down, left, right); init cuts the third icon out when it is hidden.
static muProcMenuNode sWifiNodes[muProcWifi::Entry_Count] = {
    { { 2, 1 }, { 2, 0 }, { 1, 1 }, { 1, 0 } },
    { { 2, 1 }, { 2, 0 }, { 0, 0 }, { 0, 1 } },
    { { 0, 0 }, { 0, 1 }, { 0, 1 }, { 1, 0 } },
};

void muProcWifi::init(nw4r::g3d::ScnGroup* group, int from) {
    int item;
    m_nodes = sWifiNodes;
    m_group = group;
    m_objBase = Entry_ObjBase;
    m_wifiWnd = NULL;
    nteGlobal::updateButtonCondition();
    m_showExtra = nteGlobal::isEnableButton(0x20) == true ? true : nteGlobal::isEnableButton(0x40) == true;
    for (u32 i = 0; i < Entry_Count; i++) {
        if (m_showExtra || i != Entry_Extra) {
            group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
        }
    }
    if (m_showExtra == true) {
        sWifiNodes[Entry_Friend].m_up.m_next = Entry_Extra;
        sWifiNodes[Entry_Friend].m_down.m_next = Entry_Extra;
        sWifiNodes[Entry_Anybody].m_up.m_next = Entry_Extra;
        sWifiNodes[Entry_Anybody].m_down.m_next = Entry_Extra;
    }
    if (m_showExtra == false) {
        sWifiNodes[Entry_Friend].m_up.m_next = Entry_Friend;
        sWifiNodes[Entry_Friend].m_down.m_next = Entry_Friend;
        sWifiNodes[Entry_Anybody].m_up.m_next = Entry_Anybody;
        sWifiNodes[Entry_Anybody].m_down.m_next = Entry_Anybody;
    }
    // Without the third icon the first two move down to the middle of the screen
    float y = m_showExtra ? 0.0f : -4.0f;
    Vec3f pos;
    pos = m_objs[Entry_ObjBase + Entry_Friend]->m_trans;
    pos.m_y = y;
    m_objs[Entry_ObjBase + Entry_Friend]->setTrans(&pos);
    pos = m_objs[Entry_ObjBase + Entry_Anybody]->m_trans;
    pos.m_y = y;
    m_objs[Entry_ObjBase + Entry_Anybody]->setTrans(&pos);

    switch (from) {
    case 0:
        // From the top screen
        m_unk664 = 0;
        m_cursor = Entry_Friend;
        m_code = 0;
        setComeAnim(true);
        break;
    case 0x1A:
        // Back from one of the screens: the cursor is on its icon
        m_unk664 = 1;
        m_cursor = Entry_Friend;
        m_code = 0x1A;
        setComeAnim(true);
        break;
    case 0x1B:
        m_unk664 = 1;
        m_cursor = Entry_Anybody;
        m_code = 0x1B;
        setComeAnim(true);
        break;
    case 0x1C:
        m_unk664 = 1;
        m_cursor = Entry_Extra;
        m_code = 0x1C;
        setComeAnim(true);
        break;
    default:
        // Back from an online scene: the cursor is on the icon of the mode that was played
        m_unk664 = 2;
        switch (getSceneStartCode()) {
        case 0x22:
            item = Entry_Friend;
            break;
        case 0x23:
            item = Entry_Anybody;
            break;
        }
        m_cursor = item;
        setSelectAnim(item, true);
        m_code = 0x22;
        break;
    }
    m_panel->changeProcess1(9, m_cursor + Entry_ObjBase);
}

int muProcWifi::proc() {
    u32 pad = 0;
    if (m_wifiWnd != NULL) {
        int ret = 0;
        if (m_wifiWnd->m_step == 12) {
            if (m_wifiWnd->m_mode == 1) {
                // The disconnection window has closed: back to the top screen unless it was cancelled
                if (m_wifiWnd->m_result != 3) {
                    setOutAnim(false);
                    m_result = 0;
                    ret = 1;
                    m_unk664 = 3;
                }
            } else {
                switch (m_wifiWnd->m_result) {
                case 2:
                    break;
                case 0:
                    setOutAnim(true);
                    if (m_cursor == Entry_Friend) {
                        m_result = 0x1A;
                    } else {
                        m_result = 0x1B;
                    }
                    ret = 2;
                    m_unk664 = 3;
                    break;
                }
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
    case 0: {
        // The icons come in; the one of the cursor starts to blink when it has arrived
        MuObject* object = m_objs[Entry_ObjBase + m_cursor];
        if (muGetAnimIdx(object) == 2 && object->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&m_anims[3], true);
            m_unk664 = 2;
        }
        u16 oldCursor = m_cursor;
        if (selectProc(pad)) {
            m_panel->setMsg(m_cursor + Entry_ObjBase, 1, 0);
            setSelectAnim(oldCursor, true);
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_Ok) {
            switch (m_cursor) {
            case Entry_Friend:
                m_result = 0x1A;
                break;
            case Entry_Anybody:
                m_result = 0x1B;
                break;
            case Entry_Extra:
                m_result = 0x1C;
                break;
            }
            setOutAnim(true);
            m_unk664 = 3;
            return 2;
        } else if (pad & Pad_Cancel) {
            // B disconnects first
            muMenuMain* menu = dynamic_cast<muMenuMain*>(gfSceneManager::getInstance()->m_currentScene);
            m_wifiWnd = muWifiCnctWndTask::create(1, &menu->m_unkA94, menu->getBuffer(9), m_group, 0x29, NULL, NULL);
        }
        break;
    }
    case 1:
        // Back from a screen: the icon of the cursor starts to blink when it has arrived
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&m_anims[2], true);
            m_unk664 = 2;
        }
        break;
    case 2: {
        // The cursor moves; the icon of the cursor keeps blinking
        MuObject* object = m_objs[Entry_ObjBase + m_cursor];
        if (muGetAnimIdx(object) == 4 && object->m_modelAnim->getFrame() >= 179.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&m_anims[2], true);
        }
        u16 oldCursor = m_cursor;
        if (selectProc(pad)) {
            m_panel->setMsg(m_cursor + Entry_ObjBase, 1, 0);
            setSelectAnim(oldCursor, false);
        }
        if (pad & Pad_Ok) {
            switch (m_cursor) {
            case Entry_Friend:
                m_result = 0x1A;
                break;
            case Entry_Anybody:
                m_result = 0x1B;
                break;
            case Entry_Extra:
                m_result = 0x1C;
                break;
            }
            setOutAnim(true);
            m_unk664 = 3;
            return 2;
        } else if (pad & Pad_Cancel) {
            muMenuMain* menu = dynamic_cast<muMenuMain*>(gfSceneManager::getInstance()->m_currentScene);
            m_wifiWnd = muWifiCnctWndTask::create(1, &menu->m_unkA94, menu->getBuffer(9), m_group, 0x29, NULL, NULL);
        }
        break;
    }
    case 3:
        // The icons leave; the screen is done when the one of the cursor has stopped
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcWifi::exit() {
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
    if (m_wifiWnd != NULL) {
        m_wifiWnd->exit();
        m_wifiWnd = NULL;
    }
}

void muProcWifi::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0x14), -1, 0, 0, -1);
}
