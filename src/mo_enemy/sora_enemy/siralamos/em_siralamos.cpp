// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <so/article/so_article.h>

// The weak copy of the active-article thunk of every article mediator's getActiveNum (em/em_article_mediator.h
// declares it) sits in this unit, the first enemy unit in the REL that counts active articles; the other units call it.
// Defined inline before that declaration, so that this unit emits it.
inline bool wnArticleIsActive(soArticle* article) { return article->isActiveArticle(); }

#include <em/siralamos/em_siralamos.h>

// MATCH-ONLY: names the wnemSwHolder pool's getInstanceAt first, so its weak copy comes out before the wnemSimple
// pool's as in the original (see SO_ARTICLE_MEDIATOR_POOL_1_FIRST in em/em_article_mediator.h).
// TODO(link): the original also has wnArticleIsActive before the two getInstanceAt copies; ours emits it after them
// (naming it first in an unused function does not move it), so the unit is exact but not linked yet. Declaring the
// soArticleMediatorSlot primary template before the mediator class moves wnArticleIsActive to the front, but then
// the copies come out in pool order whatever this probe names first (see em/em_article_mediator.h).
SO_ARTICLE_MEDIATOR_POOL_1_FIRST(emSiralamosArticleMediator)

// The Sir Alamos: an Enemy with two wnemSimple shots and two wnemSwHolder articles. The class overrides only its
// destructor, and its articles use the shared activation of the Enemy unit, so this unit holds no behavior of its own:
// the module storage, the article mediator and its pools (their code is the template instances of
// em/em_article_mediator.h).
// HYPOTHESIS: its behavior is driven by the shared Enemy code from the Sir Alamos's parameters and AI data.

emSiralamos::emSiralamos(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(33, 0),
    m_areaInstances(15, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emSiralamos::~emSiralamos() { }
