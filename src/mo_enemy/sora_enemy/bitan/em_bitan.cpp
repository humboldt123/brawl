// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/bitan/em_bitan.h>
#include <so/so_value_accesser.h>

// The Bitan: an Enemy with no articles. Its one override scales the score the Enemy code awards for it.

// HYPOTHESIS: a work float of the enemy (Enemy::init clears it) that the Bitan's anim cmds or status code set; the Bitan
// is the only enemy that multiplies its score by it.
static const u32 WorkFloat_ScoreMul = 0x11000008;

emBitan::emBitan(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(12, 0),
    m_areaInstances(8, 0),
    m_cameraSubjects(1, 0) { }

emBitan::~emBitan() { }

float emBitan::getScore() {
    float score = Enemy::getScore();
    return score * soValueAccesser::getValueFloat(&moduleAccesser, WorkFloat_ScoreMul, 0);
}
