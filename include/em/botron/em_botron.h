#pragma once

// SHADOW of BrawlHeaders em/botron/em_botron.h (-I include comes first): the full layout of the Botron.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The Botron only calls the constructors and destructors of its arrays, classes of sora_melee that
// em/em_module_arrays.h declares because other enemies share them.

class emBotron;

// The articles of the Botron: three wnemSwHolder (article id 0), of weapon kind 9 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Botron> emBotronArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 3, emWeaponInstanceHolder, soIntToType<0> > emBotronSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emBotron, soArticleMediatorAssignmentNull> emBotronArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emBotronSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emBotronArticleAssignment, soTypeListNullType>, emBotronArticleKind,
                              soTypeListNullType>
    emBotronArticleMediator;
static_assert(sizeof(emBotronArticleMediator) == 0xa4, "Article mediator is wrong size!");

class emBotron : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 22> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 11> m_areaInstances;          // 0x573C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x160];                                        // 0x58D4
    emTargetSearchUnit m_targetSearchUnits[4];                   // 0x5A34
    emAIUnit m_aiUnits[20];                                      // 0x5A94, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5B34, camera module
    emBotronArticleMediator m_articleMediator;                   // 0x5B84, generate-article-manage module

public:
    emBotron(u32 p1, emCreate* create);
    virtual ~emBotron();

    // Set up a freshly generated article for a shot (called by the article mediator).
    // HYPOTHESIS: name follows the fighters' activeArticle convention, as for the Primid.
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emBotron) == 0x5c28, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emBotron, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emBotron::activeArticle(article, acc); }
};
