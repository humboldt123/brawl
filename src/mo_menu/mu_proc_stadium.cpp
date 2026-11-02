#include <types.h>

#include <gm/gm_global.h>
#include <mu/mu_scn_mdl_expand.h>
#include <snd/snd_system.h>
#include <stdio.h>

#include <mu/mu_proc_stadium.h>

// The panel: comes in, goes out, and its texture scrolls (texture srt).
static MuAnimIdxData sPanelAnims[3] = {
    { 0.0f, 9.0f, 0, 0, 0x1 },
    { 10.0f, 19.0f, 0, 0, 0x1 },
    { 0.0f, 30.0f, 1, 0, 0x8 },
};
// The cursor blinks (colour).
static MuAnimIdxData sCursorAnim = { 0.0f, 30.0f, 1, 0, 0x10 };

muProcStadium::muProcStadium(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData) : muProcMenu(objs, mdls) {
    m_msg = MuMsg::create(1, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(Msg_Buf, 2);
    m_msg->setMsgData(msgData);
    for (int i = 0; i < 2; i++) {
        m_msg->setFontWidthModeAuto(i);
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Panel]->m_sceneModel), i,
                                  1.0f / 16.0f);
        m_msg->setAlignMode(i, MuMsg::Align_Center);
        m_msg->setEdgeEnable(i, false);
    }
    m_msg->setFontColor(Msg_Title, 0x00, 0x00, 0x00, 0xFF);
    m_msg->setFontColor(Msg_Help, 0xFA, 0xFA, 0xFA, 0xFF);
}

void muProcStadium::init(nw4r::g3d::ScnGroup* group, int from) {
    u16 kind;   // BYTES: declared here, which gives it the register of the original
    char node[16];
    // Where the frame of the cursor stands over the place of each game, for a row of one ... four games
    float rowOffsets[GameCount] = { 0.0f, 1.0f, 3.0f, 6.0f };
    m_group = group;
    // HYPOTHESIS: the name of the function in the map says the opposite of what its result does here.
    if (g_GameGlobal->checkHideMenuBossBattle() == true) {
        m_gameCount = GameCount;
    } else {
        m_gameCount = GameCount - 1;
    }
    for (u32 i = 0; i < m_gameCount; i++) {
        m_games[i] = i;
    }

    reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Row])
        ->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Icons]), "pos00");
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Row]));
    for (u32 i = 0; i < m_gameCount; i++) {
        sprintf(node, "pos%02d", i);
        reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Icons])
            ->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Game0 + m_games[i]]->m_sceneModel), node);
    }

    m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[2], false);
    m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[0], false);
    m_objs[Obj_Cursor]->setAnimIdx(&sCursorAnim, false);

    if (static_cast<u32>(from) == 2) {
        m_unk664 = Step_Come;
        m_cursor = 0;
        m_code = 2;
    } else {
        m_unk664 = Step_Select;
        // Back from a game: the cursor goes to it
        u32 game;
        switch (getSceneStartCode()) {
        case 9:
            game = Game_TargetSmash;
            break;
        case 10:
            game = Game_HomeRun;
            break;
        case 11:
            game = Game_Kumite;
            break;
        case 7:
            game = Game_BossBattle;
            break;
        default:
            game = Game_Kumite;
            break;
        }
        for (u16 i = 0; i < m_gameCount; i++) {
            if (game == m_games[i]) {
                m_cursor = i;
                break;
            }
        }
        m_code = 0x22;
        setAnimFrame(Obj_Panel, 9.0f, 1);
    }

    setAnimFrame(Obj_Cursor, m_cursor + rowOffsets[m_gameCount - 1], 3);
    for (int i = 0; i < GameCount; i++) {
        if (i == m_cursor) {
            setAnimFrame(Obj_Game0 + i, 1.0f, 2);
        } else {
            setAnimFrame(Obj_Game0 + i, 0.0f, 2);
        }
    }
    m_panel->changeProcess1(0x21, 0);
    m_msg->printIndex(Msg_Title, m_games[m_cursor]);
    m_msg->printIndex(Msg_Help, m_games[m_cursor] + Msg_HelpGame);

    if (static_cast<u32>(from) != 2 && getSceneStartCode() >= 0x29 && getSceneStartCode() <= 0x2E) {
        // Back from a multi-man brawl: its page is open, with the cursor on the kind that was played
        kind = getSceneStartCode() - 0x29;
        m_cursor = kind;
        reinterpret_cast<nw4r::g3d::ScnGroup*>(m_mdls[Mdl_Row])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Icons]));
        m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Kumite]));
        m_msg->printIndex(Msg_Help, m_cursor + Msg_HelpKumite);
        m_objs[Obj_KumitePanel]->setAnimIdx(&sPanelAnims[0], false);
        m_objs[Obj_KumiteCursor]->setAnimIdx(&sCursorAnim, false);
        setAnimFrame(Obj_KumiteCursor, kind, 1);
        m_unk664 = Step_KumiteSelect;
        m_objs[Obj_KumitePanel]->setFrameNode(9.0f);
    }
}

int muProcStadium::proc() {
    u32 pad;    // BYTES: one for both pages; a block-scoped one per page puts the second in another register
    switch (m_unk664) {
    case Step_Come:
        if (m_objs[Obj_Panel]->m_modelAnim->getFrame() == 9.0f) {
            m_unk664 = Step_Select;
        }
        // The cursor already moves while the panel comes in
    case Step_Select:
        pad = padUpdate();
        selectProc(pad);
        if (pad & Pad_Ok) {
            switch (m_games[m_cursor]) {
            case Game_TargetSmash:
                exitScene(9, false);
                g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
                break;
            case Game_HomeRun:
                exitScene(10, false);
                g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
                break;
            case Game_Kumite:
                // The page of the multi-man brawl comes in
                m_cursor = 0;
                reinterpret_cast<nw4r::g3d::ScnGroup*>(m_mdls[Mdl_Row])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Icons]));
                m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Kumite]));
                m_msg->printIndex(Msg_Help, m_cursor + Msg_HelpKumite);
                m_objs[Obj_KumitePanel]->setAnimIdx(&sPanelAnims[0], false);
                m_objs[Obj_KumiteCursor]->setAnimIdx(&sCursorAnim, false);
                setAnimFrame(Obj_KumiteCursor, 0.0f, 1);
                m_unk664 = Step_KumiteCome;
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                break;
            case Game_BossBattle:
                exitScene(7, false);
                g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
                break;
            }
        } else if (pad & Pad_Cancel) {
            // Back to the main menu
            m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[1], false);
            m_result = 2;
            m_unk664 = Step_Leave;
            return 1;
        }
        break;
    case Step_KumiteCome:
        if (m_objs[Obj_KumitePanel]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            // The announcer calls the multi-man brawl
            if (!g_sndSystem->isPlaySEID(static_cast<SndID>(0x2032))) {
                g_sndSystem->playSEwithFixEffect(static_cast<SndID>(0x2032), 1.0f, 1.0f);
            }
            m_unk664 = Step_KumiteSelect;
        }
        break;
    case Step_KumiteSelect:
        pad = padUpdate();
        selectKumite(pad);
        if (pad & Pad_Ok) {
            switch (m_cursor) {
            case 0:
                exitScene(0xB, false);
                break;
            case 1:
                exitScene(0x25, false);
                break;
            case 2:
                exitScene(0x26, false);
                break;
            case 3:
                exitScene(0x27, false);
                break;
            case 4:
                exitScene(0x28, false);
                break;
            case 5:
                exitScene(0x29, false);
                break;
            }
            g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
        } else if (pad & Pad_Cancel) {
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            m_objs[Obj_KumitePanel]->setAnimIdx(&sPanelAnims[1], false);
            m_unk664 = Step_KumiteLeave;
            m_msg->printIndex(Msg_Help, Game_Kumite + Msg_HelpGame);
        }
        break;
    case Step_KumiteLeave:
        if (m_objs[Obj_KumitePanel]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            // The page has gone: the row comes back with the cursor on the multi-man brawl
            for (u16 i = 0; i < m_gameCount; i++) {
                if (m_games[i] == Game_Kumite) {
                    m_cursor = i;
                    break;
                }
            }
            m_unk664 = Step_Select;
            m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Kumite]));
            reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Row])
        ->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Icons]), "pos00");
        }
        break;
    case Step_Leave:
        if (m_objs[Obj_Panel]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcStadium::exit() {
    reinterpret_cast<nw4r::g3d::ScnGroup*>(m_mdls[Mdl_Row])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Icons]));
    for (u32 i = 0; i < GameCount; i++) {
        reinterpret_cast<nw4r::g3d::ScnGroup*>(m_mdls[Mdl_Icons])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Game0 + i]->m_sceneModel));
    }
    // HYPOTHESIS: the original inserts the row into the group here (Insert, not Remove).
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Row]));
}

bool muProcStadium::selectProc(int dir) {
    float rowOffsets[GameCount] = { 0.0f, 1.0f, 3.0f, 6.0f };
    bool changed = false;
    if (dir & Pad_Left) {
        if (m_cursor != 0) {
            m_cursor--;
            if (m_cursor == 0) {
                padRepeatClear(GF_PAD_BUTTON_BIT_DPAD_LEFT);
            }
        } else {
            m_cursor = m_gameCount - 1;
        }
        changed = true;
    } else if (dir & Pad_Right) {
        if (m_cursor < m_gameCount - 1) {
            m_cursor++;
            if (m_cursor == m_gameCount - 1) {
                padRepeatClear(GF_PAD_BUTTON_BIT_DPAD_RIGHT);
            }
        } else {
            m_cursor = 0;
        }
        changed = true;
    }

    if (changed) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        m_msg->printIndex(Msg_Title, m_games[m_cursor]);
        m_msg->printIndex(Msg_Help, m_games[m_cursor] + Msg_HelpGame);
        setAnimFrame(Obj_Cursor, m_cursor + rowOffsets[m_gameCount - 1], 3);
        for (int i = 0; i < GameCount; i++) {
            if (i == m_cursor) {
                setAnimFrame(Obj_Game0 + i, 1.0f, 2);
            } else {
                setAnimFrame(Obj_Game0 + i, 0.0f, 2);
            }
        }
    }
    return changed;
}

bool muProcStadium::selectKumite(int dir) {
    bool changed = false;
    if (dir & Pad_Up) {
        if (m_cursor == 0) {
            m_cursor = KumiteCount - 1;
        } else {
            m_cursor--;
            if (m_cursor == 0) {
                padRepeatClear(GF_PAD_BUTTON_BIT_DPAD_UP);
            }
        }
        changed = true;
    } else if (dir & Pad_Down) {
        if (m_cursor == KumiteCount - 1) {
            m_cursor = 0;
        } else {
            m_cursor++;
            if (m_cursor == KumiteCount - 1) {
                padRepeatClear(GF_PAD_BUTTON_BIT_DPAD_DOWN);
            }
        }
        changed = true;
    }

    if (changed) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        m_msg->printIndex(Msg_Help, m_cursor + Msg_HelpKumite);
        setAnimFrame(Obj_KumiteCursor, m_cursor, 1);
    }
    return changed;
}

void muProcStadium::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
