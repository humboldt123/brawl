#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_scene.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <string.h>
#include <types.h>

#include <mu/mu_proc_menu.h>

// muMenuController::init(pad) and muMenuController::main()
extern "C" void fn_800B96E8(muMenuController* controller, int pad);
extern "C" void fn_800B9984(muMenuController* controller);
// muMenuController::initRepeatCounter(buttons): the buttons of the mask repeat from the start again
extern "C" void fn_800BA364(muMenuController* controller, int buttons);
// muMenuController::setAButtonPushedControllerData(): remembers the controller that pressed A
extern "C" void fn_800BA2E8(muProcMenu* menu);
// muMenuMain: closes the menu with a code (in the module of the scene)
extern "C" void fn_1_135E0(muMenuMain* scene, int code);

// The priority of the draw of a model of the scene (the same for the opaque and the transparent parts).
static inline int getDrawPriority(MuObject* object) {
    return object->m_scnMdl->m_priorityDrawOpa;
}

static inline muMenuMain* getMenuMain() {
    return dynamic_cast<muMenuMain*>(gfSceneManager::getInstance()->m_currentScene);
}

// The animations of the items of a screen (the entry used by each case of the animations of a proc).
static MuAnimIdxData sDefaultAnims[7] = {
    { 0.0f, 9.0f, 0, 2, 0x1B },
    { 0.0f, 9.0f, 0, 0, 0x1B },
    { 0.0f, 59.0f, 1, 5, 0x1B },
    { 0.0f, 179.0f, 0, 4, 0x1B },
    { 0.0f, 29.0f, 0, 3, 0x1B },
    { 0.0f, 19.0f, 0, 1, 0x1B },
    { 0.0f, 0.0f, 1, 1, 0x1B },
};

muProcMenu::muProcMenu(MuObject** objs, nw4r::g3d::G3dObj** mdls) {
    m_itemCount = 0;
    m_anims = sDefaultAnims;
    m_objs = objs;
    m_mdls = mdls;
    m_unk630 = 1;
    m_cursor = 0;
    setCtrlFlg(0xFF);
    for (u8 i = 0; i < 8; i++) {
        fn_800B96E8(&m_ctrl[i], i);
    }
    fn_800B96E8(&m_ctrl[8], 0xF0);
}

void muProcMenu::setCtrlFlg(u8 mask) {
    m_padMask = mask;
}

u32 muProcMenu::padUpdate() {
    u32 flags = 0;
    int count = 0;
    if (strcmp(gfSceneManager::getInstance()->m_currentScene->m_sceneName, "muMenuMain") == 0) {
        count = getMenuMain()->m_padLock;
    }
    if (m_padMask == 0 || count > 0) {
        m_pad.init();
        return 0;
    }

    if (m_padMask == 0xFF) {
        // The pad of the system menu
        g_gfPadSystem->getSysPadStatus(0xF0, &m_pad);
        fn_800B9984(&m_ctrl[8]);
        m_pad.m_buttonsPressedThisFrame2.bits = *reinterpret_cast<u32*>(reinterpret_cast<char*>(&m_ctrl[8]) + 0x10);
        m_pad.m_buttonsCurrentFrame2.bits = *reinterpret_cast<u32*>(reinterpret_cast<char*>(&m_ctrl[8]) + 8);
    } else {
        gfPadStatus pads[8];
        count = 0;
        for (u8 i = 0; i < 8; i++) {
            if (m_padMask & (1 << i)) {
                g_gfPadSystem->getSysPadStatus(i, &pads[(u8)count]);
                fn_800B9984(&m_ctrl[i]);
                count++;
            }
        }
        fn_800B9984(&m_ctrl[8]);
        gfPadSystem::merge(pads, (u8)count, &m_pad);
        for (u8 i = 0; i < 8; i++) {
            if (m_padMask & (1 << i)) {
                m_pad.m_buttonsPressedThisFrame2.bits |= *reinterpret_cast<u32*>(reinterpret_cast<char*>(&m_ctrl[i]) + 0x10);
                m_pad.m_buttonsCurrentFrame2.bits |= *reinterpret_cast<u32*>(reinterpret_cast<char*>(&m_ctrl[i]) + 8);
            }
        }
    }

    u32 pressed = m_pad.m_buttonsPressedThisFrame.bits;
    if (pressed & GF_PAD_BUTTON_BIT_A) {
        flags |= Pad_A;
    }
    if (pressed & GF_PAD_BUTTON_BIT_L) {
        flags |= Pad_L;
    }
    if (pressed & GF_PAD_BUTTON_BIT_R) {
        flags |= Pad_R;
    }
    if (pressed & GF_PAD_BUTTON_BIT_X) {
        flags |= Pad_X;
    }
    if (pressed & GF_PAD_BUTTON_BIT_Y) {
        flags |= Pad_Y;
    }
    if (pressed & GF_PAD_BUTTON_BIT_START) {
        flags |= Pad_Start;
    }
    if ((pressed & GF_PAD_BUTTON_BIT_A) || (pressed & GF_PAD_BUTTON_BIT_START)) {
        flags |= Pad_Ok;
        // Find the pad that pressed it
        for (int i = 0; i < 4; i++) {
            gfPadStatus pad;
            g_gfPadSystem->getSysPadStatus(i, &pad);
            if ((pad.m_buttonsPressedThisFrame2.bits & GF_PAD_BUTTON_BIT_A) || (pad.m_buttonsPressedThisFrame2.bits & GF_PAD_BUTTON_BIT_START)) {
                m_padNo = i;
                break;
            }
        }
    }
    if (m_pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_B) {
        flags |= Pad_Cancel;
    }
    u32 repeat = m_pad.m_buttonsPressedThisFrame2.bits;
    if (repeat & GF_PAD_BUTTON_BIT_DPAD_UP) {
        flags |= Pad_Up;
    }
    if (repeat & GF_PAD_BUTTON_BIT_DPAD_DOWN) {
        flags |= Pad_Down;
    }
    if (repeat & GF_PAD_BUTTON_BIT_DPAD_LEFT) {
        flags |= Pad_Left;
    }
    if (repeat & GF_PAD_BUTTON_BIT_DPAD_RIGHT) {
        flags |= Pad_Right;
    }
    pressed = m_pad.m_buttonsPressedThisFrame.bits;
    if ((pressed & GF_PAD_BUTTON_BIT_A) && (pressed & GF_PAD_BUTTON_BIT_START) && (pressed & GF_PAD_BUTTON_BIT_L) && (pressed & GF_PAD_BUTTON_BIT_R)) {
        flags |= Pad_All;
    }
    return flags;
}

bool muProcMenu::selectProc(int dir) {
    bool moved = false;
    u16 target;
    if (dir & Pad_Up) {
        moved = true;
        target = m_nodes[m_cursor].m_up.m_next;
    } else if (dir & Pad_Down) {
        moved = true;
        target = m_nodes[m_cursor].m_down.m_next;
    } else if (dir & Pad_Left) {
        moved = true;
        target = m_nodes[m_cursor].m_left.m_next;
    } else if (dir & Pad_Right) {
        moved = true;
        target = m_nodes[m_cursor].m_right.m_next;
    }
    if (moved) {
        if (target != m_cursor) {
            playSE_CursorMove();
            m_cursor = target;
            u32 repeat = 0;
            if (m_nodes[target].m_up.m_repeat) {
                repeat |= GF_PAD_BUTTON_BIT_DPAD_UP;
            }
            if (m_nodes[target].m_down.m_repeat) {
                repeat |= GF_PAD_BUTTON_BIT_DPAD_DOWN;
            }
            if (m_nodes[target].m_left.m_repeat) {
                repeat |= GF_PAD_BUTTON_BIT_DPAD_LEFT;
            }
            if (m_nodes[target].m_right.m_repeat) {
                repeat |= GF_PAD_BUTTON_BIT_DPAD_RIGHT;
            }
            for (u32 i = 0; i < 9; i++) {
                fn_800BA364(&m_ctrl[i], repeat);
            }
        } else {
            moved = false;
        }
    }
    return moved;
}

void muProcMenu::padRepeatClear(int buttons) {
    for (u32 i = 0; i < 9; i++) {
        fn_800BA364(&m_ctrl[i], buttons);
    }
}

void muProcMenu::setAnimFrame(int obj, float frame, u32 flags) {
    if (flags == 0x1F) {
        m_objs[obj]->setFrame(frame);
    } else {
        gfModelAnimation* anim = m_objs[obj]->m_modelAnim;
        if ((flags & 1) && anim->m_anmObjChrRes != NULL) {
            m_objs[obj]->setFrameNode(frame);
        }
        if ((flags & 2) && anim->m_anmObjVisRes != NULL) {
            m_objs[obj]->setFrameVisible(frame);
        }
        if ((flags & 4) && anim->m_anmObjTexPatRes != NULL) {
            m_objs[obj]->setFrameTex(frame);
        }
        if ((flags & 8) && anim->m_anmObjTexSrtRes != NULL) {
            m_objs[obj]->setFrameTexSrt(frame);
        }
        if ((flags & 0x10) && anim->m_anmObjMatClrRes != NULL) {
            m_objs[obj]->setFrameMatCol(frame);
        }
    }
}

void muProcMenu::setAnimUpdateRate(int obj, float rate, u32 flags) {
    if (flags == 0x1F) {
        m_objs[obj]->m_modelAnim->setUpdateRate(rate);
    } else {
        gfModelAnimation* anim = m_objs[obj]->m_modelAnim;
        if ((flags & 1) && anim->m_anmObjChrRes != NULL) {
            anim->m_anmObjChrRes->SetUpdateRate(rate);
        }
        if ((flags & 2) && anim->m_anmObjVisRes != NULL) {
            m_objs[obj]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(rate);
        }
        if ((flags & 4) && anim->m_anmObjTexPatRes != NULL) {
            m_objs[obj]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(rate);
        }
        if ((flags & 8) && anim->m_anmObjTexSrtRes != NULL) {
            m_objs[obj]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(rate);
        }
        if ((flags & 0x10) && anim->m_anmObjMatClrRes != NULL) {
            m_objs[obj]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(rate);
        }
    }
}

int muProcMenu::getFixedControllerID() {
    return getMenuMain()->m_padNo;
}

int muProcMenu::getSceneStartCode() {
    return getMenuMain()->m_startCode;
}

void muProcMenu::exitScene(int code, bool force) {
    fn_800BA2E8(this);
    if (force) {
        fn_1_135E0(getMenuMain(), code);
    } else {
        for (int i = 0; i < m_itemCount; i++) {
            if (i == m_cursor) {
                m_objs[m_objBase + i]->setAnimIdx(&m_anims[2], true);
                MuObject* object = m_objs[m_objBase + i];
                int priority = getDrawPriority(object) + 1;
                object->m_scnMdl->SetPriorityDrawOpa(priority);
                object->m_scnMdl->SetPriorityDrawXlu(priority);
            } else {
                m_objs[m_objBase + i]->setAnimIdx(&m_anims[6], true);
            }
            m_objs[m_objBase + i]->m_modelAnim->setUpdateRate(0.0f);
        }
        muMenuMain* scene = getMenuMain();
        scene->m_exitCode = code;
        scene->m_exitState = 2;
    }
}

void muProcMenu::setComeAnim(bool keepFrame) {
    for (int i = 0; i < m_itemCount; i++) {
        if (i == m_cursor) {
            m_objs[m_objBase + i]->setAnimIdx(&m_anims[0], true);
            MuObject* object = m_objs[m_objBase + i];
            int priority = getDrawPriority(object) + 1;
            object->m_scnMdl->SetPriorityDrawOpa(priority);
            object->m_scnMdl->SetPriorityDrawXlu(priority);
        } else {
            m_objs[m_objBase + i]->setAnimIdx(&m_anims[1], true);
        }
        if (!keepFrame) {
            m_objs[m_objBase + i]->setFrame(9.0f);
        }
    }
}

void muProcMenu::setSelectAnim(int item, bool selected) {
    m_objs[m_objBase + item]->setAnimIdx(&m_anims[1], true);
    m_objs[m_objBase + item]->m_animIdxData = NULL;
    setAnimFrame(m_objBase + item, 9.0f, m_anims[1].m_flags);
    MuObject* object = m_objs[m_objBase + item];
    int priority = getDrawPriority(object);
    if (selected) {
        for (int i = 0; i < m_itemCount; i++) {
            if (i == m_cursor) {
                m_objs[m_objBase + i]->setAnimIdx(&m_anims[3], true);
                object = m_objs[m_objBase + i];
                object->m_scnMdl->SetPriorityDrawOpa(priority);
                object->m_scnMdl->SetPriorityDrawXlu(priority);
            } else {
                m_objs[m_objBase + i]->setFrame(9.0f);
                object = m_objs[m_objBase + i];
                object->m_scnMdl->SetPriorityDrawOpa(priority - 1);
                object->m_scnMdl->SetPriorityDrawXlu(priority - 1);
            }
        }
    } else {
        object->m_scnMdl->SetPriorityDrawOpa(priority - 1);
        object->m_scnMdl->SetPriorityDrawXlu(priority - 1);
        m_objs[m_objBase + m_cursor]->setAnimIdx(&m_anims[3], true);
        MuObject* cursorObject = m_objs[m_objBase + m_cursor];
        cursorObject->m_scnMdl->SetPriorityDrawOpa(priority);
        cursorObject->m_scnMdl->SetPriorityDrawXlu(priority);
    }
}

void muProcMenu::setOutAnim(bool selected) {
    int priority = getDrawPriority(m_objs[m_objBase + m_cursor]) - 1;
    for (int i = 0; i < m_itemCount; i++) {
        if (i == m_cursor && selected) {
            m_objs[m_objBase + i]->setAnimIdx(&m_anims[4], true);
        } else {
            m_objs[m_objBase + i]->setAnimIdx(&m_anims[5], true);
        }
        MuObject* object = m_objs[m_objBase + i];
        object->m_scnMdl->SetPriorityDrawOpa(priority);
        object->m_scnMdl->SetPriorityDrawXlu(priority);
    }
}
