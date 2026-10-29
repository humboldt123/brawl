#pragma once

// SHADOW of BrawlHeaders em/bosspackun/em_bosspackun.h (-I include comes first): the full layout of Petey Piranha
// (Boss Packun).

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <so/link/so_link_event_presenter.h>
#include <types.h>
#include <wn/wn_em_simple.h>

// The arrays are classes of sora_melee, declared in em/em_module_arrays.h (soInterpolation<Vec3f> x31 and
// soAreaInstance x7 are shared with other enemies).

class emBosspackun;

// The articles of Petey Piranha: two wnemSimple (article id 0), of weapon kind 46 (its enemy kind). Their procs are
// wnemBosspackunProc::ArmEnter / ArmIdol / ArmDead (map names), so they are called its arms here.
typedef soKindInfoGeneric<1, Enemy_Boss_BossPackun> emBosspackunArticleKind;
typedef soInstancePoolInfo<wnemSimple, 2, emWeaponInstanceHolder, soIntToType<0> > emBosspackunSimplePoolInfo;
typedef soArticleMediatorAssignmentInfo<emBosspackun, soArticleMediatorAssignmentNull> emBosspackunArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emBosspackunSimplePoolInfo, soTypeListNullType>,
                              soTypeList<emBosspackunArticleAssignment, soTypeListNullType>, emBosspackunArticleKind,
                              soTypeListNullType>
    emBosspackunArticleMediator;
static_assert(sizeof(emBosspackunArticleMediator) == 0x470c, "Article mediator is wrong size!");

// The link events Petey Piranha handles carry which of the two arms they concern (the layout of so/stageobject.h's
// UnkLinkEvent, which the Arman's ArmDead proc sends).
// HYPOTHESIS: the field is the arm index (0 or 1); every reaction of notifyEventLink comes in a pair selected by it.
struct emBosspackunLinkEventArgs : public soLinkEventArgs {
    int m_armId; // 0x8
};

class emBosspackun : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 31> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;           // 0x58EC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xe0];                                         // 0x59F4
    emTargetSearchUnit m_targetSearchUnits[1];                   // 0x5AD4
    emAIUnit m_aiUnits[31];                                      // 0x5AEC, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5BE4, camera module
    emBosspackunArticleMediator m_articleMediator;               // 0x5C34, generate-article-manage module

public:
    emBosspackun(u32 p1, emCreate* create);
    virtual ~emBosspackun();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);

    // Set up a freshly generated arm (called by the article mediator).
    static bool activeArticle(wnemSimple* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emBosspackun) == 0xa340, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emBosspackun, 0> {
    static bool activate(wnemSimple* article, soModuleAccesser* acc) { return emBosspackun::activeArticle(article, acc); }
};
