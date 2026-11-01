#include <mu/mu_adv_selchr_c.h>
#include <stdio.h>
#include <types.h>

// muMenuController::getControllerID(): the pad of the controller (below 0: there is none).
extern "C" int fn_800BA3D8(muMenuController* controller);

// The trainer and the three Pokemon, Zelda and Sheik, Samus and Zero Suit Samus each take one entry: the first of a
// group keeps the group's first kind, the next ones of the same group give CharKind_None.
int muAdvSelchrCTask::normalizeCharKind(int kind, bool* pokeTrainerSeen, bool* zeldaSeen, bool* samusSeen) {
    // BYTES: each range test is one unsigned compare of the distance to the group's first kind (subi/cmplwi).
    if (static_cast<u32>(kind - CharKind_PokeTrainer) <= 3) {
        if (*pokeTrainerSeen) {
            return CharKind_None;
        }
        *pokeTrainerSeen = true;
        return CharKind_PokeTrainer;
    }
    if (static_cast<u32>(kind - CharKind_Zelda) <= 1) {
        if (*zeldaSeen) {
            return CharKind_None;
        }
        *zeldaSeen = true;
        return CharKind_Zelda;
    }
    if (static_cast<u32>(kind - CharKind_Samus) <= 1) {
        if (*samusSeen) {
            return CharKind_None;
        }
        *samusSeen = true;
        return CharKind_Samus;
    }
    return kind;
}

// The team that has the character (the characters that change form are found by the group's first kind), and where
// in the team it is.
int muAdvSelchrCTask::getCharTeam(int* index, int kind) {
    // BYTES: the same unsigned range tests as normalizeCharKind.
    if (static_cast<u32>(kind - CharKind_PokeTrainer) <= 3) {
        kind = CharKind_PokeTrainer;
    }
    if (static_cast<u32>(kind - CharKind_Zelda) <= 1) {
        kind = CharKind_Zelda;
    }
    if (static_cast<u32>(kind - CharKind_Samus) <= 1) {
        kind = CharKind_Samus;
    }
    for (int team = 0; team < m_numTeams; team++) {
        for (int i = 0; i < m_teams[team].m_count; i++) {
            if (kind == m_teams[team].m_members[i]) {
                *index = i;
                return m_teams[team].m_id;
            }
        }
    }
    return 10;
}

int muAdvSelchrCTask::getTeamMember(int team, int index) {
    return m_teams[team].m_members[index];
}

// The kind of the member with the form that is shown for the characters that change form.
int muAdvSelchrCTask::getTeamMemberDetail(int team, int index) {
    int kind = m_teams[team].m_members[index];
    // BUG: the trainer is tested twice (the second test can never be true); the code without it does not match.
    // HYPOTHESIS: the original tested two names of the same kind.
    if (kind == CharKind_PokeTrainer) {
        return m_pokeTrainerKind;
    } else if (kind == CharKind_PokeTrainer) {
        return m_pokeTrainerKind;
    } else if (kind == CharKind_Zelda) {
        return m_zeldaKind;
    } else if (kind == CharKind_Samus) {
        return m_samusKind;
    }
    return kind;
}

int muAdvSelchrCTask::getNumTeamMember(int team) {
    return m_teams[team].m_count;
}

MuObject* muAdvSelchrCTask::getFaceObj(int team, int index) {
    return m_objects[m_teams[team].m_top + index + Obj_Face];
}

bool muAdvSelchrCTask::isPlayerJoined(int player) {
    return fn_800BA3D8(&m_players[player].m_controller) >= 0;
}

int muAdvSelchrCTask::getTeamCurPos() {
    return m_teamCurPos;
}

// How many lines of seven faces the team takes.
int muAdvSelchrCTask::getNumTeamLine(int team) {
    return (m_teams[team].m_count + 6) / 7;
}

int muAdvSelchrCTask::getCharCursorPos(int player) {
    return m_players[player].m_cursor;
}

void muAdvSelchrCTask::setCharCursorPos(int player, int pos) {
    m_players[player].m_cursor = pos;
}

// Where the character at the position is in the picks of the player that picked it (-1: no player picked it).
int muAdvSelchrCTask::getMemberNumber(int pos) {
    // BYTES: the counter is declared before the pointer to the player's data (the register order needs it).
    int player = 0;
    PlayerWork* work = m_players;
    for (; player < 2; player++, work++) {
        if (isPlayerJoined(player)) {
            for (int i = 0; i < work->m_count; i++) {
                if (pos == work->m_members[i]) {
                    return i;
                }
            }
        }
    }
    return -1;
}

// Whether the cursor of the player can be on the position: nobody picked the character, and the cursors of the players
// before it are not on it.
bool muAdvSelchrCTask::canLocateCursor(int player, int pos) {
    if (getMemberNumber(pos) >= 0) {
        return false;
    }
    for (int i = player - 1; i >= 0; i--) {
        if (m_players[i].m_cursor == pos) {
            return false;
        }
    }
    return true;
}

// How many characters of the current team nobody picked.
int muAdvSelchrCTask::getNumFreeChar() {
    int count = 0;
    for (int pos = 0; pos < m_teams[m_teamCurPos].m_count; pos++) {
        if (getMemberNumber(pos) == -1) {
            count++;
        }
    }
    return count;
}

// Puts the cursor of the player on the face at the position.
void muAdvSelchrCTask::moveCharCursor(int player, int pos) {
    nw4r::g3d::ScnMdlExpand* expand = m_expands[m_teamCurPos + 2];
    int index = Obj_Cursor + 1;
    if (player == 0) {
        index = Obj_Cursor;
    }
    MuObject* cursor = m_objects[index];
    expand->Remove(cursor->m_sceneModel);
    char nodeName[8];
    sprintf(nodeName, "pos%02d", pos);
    expand->PushBack(cursor->m_sceneModel, nodeName);
}

// Shows the number of the pick of the player on the face at the position.
void muAdvSelchrCTask::dispNumber(int player, int pos, int number) {
    int base = player == 0 ? Obj_Number1 : Obj_Number2;
    MuObject* numberObj = m_objects[base + number - 1];
    numberObj->setFrameTex(number);
    nw4r::g3d::ScnMdlExpand* expand = m_expands[m_teamCurPos + 2];
    char nodeName[8];
    sprintf(nodeName, "pos%02d", pos);
    expand->PushBack(numberObj->m_sceneModel, nodeName);
}

void muAdvSelchrCTask::removeNumber(int player, int number) {
    int base = Obj_Number2;
    if (player == 0) {
        base = Obj_Number1;
    }
    m_expands[m_teamCurPos + 2]->Remove(m_objects[base + number - 1]->m_sceneModel);
}

int muAdvSelchrCTask::mainStepSelectedMain(muAdvSelchrCTask* task) {
    if (++task->m_timer >= 2) {
        task->m_state = 2;
        return Step_Zombie;
    }
    return task->m_step;
}

void muAdvSelchrCTask::mainStepZombieInit(muAdvSelchrCTask* task) { }

int muAdvSelchrCTask::mainStepZombieMain(muAdvSelchrCTask* task) {
    return task->m_step;
}

bool muAdvSelchrCTask::isSelected() {
    if (m_step >= Step_Selected && m_stepPrev >= Step_Selected) {
        return true;
    }
    return false;
}
