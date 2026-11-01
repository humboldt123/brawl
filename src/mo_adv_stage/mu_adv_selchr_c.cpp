#include <mu/mu_adv_selchr_c.h>
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
    return m_objects[m_teams[team].m_top + index + 13];
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
