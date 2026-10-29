#pragma once

// SHADOW of BrawlHeaders em/ghamgha/em_ghamgha.h (-I include comes first): the full layout of the Ghamgha.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The Ghamgha only calls the constructors and destructors of its arrays, classes of sora_melee that
// em/em_module_arrays.h declares because other enemies share them.

class emGhamgha;

// The articles of the Ghamgha: five wnemSwHolder (article id 0), of weapon kind 36 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Ghamgha> emGhamghaArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 5, emWeaponInstanceHolder, soIntToType<0> > emGhamghaSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emGhamgha, soArticleMediatorAssignmentNull> emGhamghaArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emGhamghaSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emGhamghaArticleAssignment, soTypeListNullType>, emGhamghaArticleKind,
                              soTypeListNullType>
    emGhamghaArticleMediator;
static_assert(sizeof(emGhamghaArticleMediator) == 0x104, "Article mediator is wrong size!");

class emGhamgha : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 24> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;           // 0x579C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xE0];                                         // 0x58A4
    emTargetSearchUnit m_targetSearchUnits[1];                   // 0x5984
    emAIUnit m_aiUnits[15];                                      // 0x599C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5A14, camera module
    emGhamghaArticleMediator m_articleMediator;                  // 0x5A64, generate-article-manage module

public:
    emGhamgha(u32 p1, emCreate* create);
    virtual ~emGhamgha();

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emGhamgha) == 0x5b68, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emGhamgha, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emGhamgha::activeArticle(article, acc); }
};
