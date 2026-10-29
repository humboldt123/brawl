#pragma once

// SHADOW of BrawlHeaders em/spar/em_spar.h (-I include comes first): the full layout of the Spar.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: the Spar only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 39);

class emSpar;

// The articles of the Spar: three wnemSwHolder (article id 0), of weapon kind 15 (its enemy kind).
// HYPOTHESIS: these carry the Spar's shots; three is the number that can be in flight at once.
typedef soKindInfoGeneric<1, Enemy_Spar> emSparArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 3, emWeaponInstanceHolder, soIntToType<0> > emSparSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emSpar, soArticleMediatorAssignmentNull> emSparArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emSparSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emSparArticleAssignment, soTypeListNullType>, emSparArticleKind,
                              soTypeListNullType>
    emSparArticleMediator;
static_assert(sizeof(emSparArticleMediator) == 0xa4, "Article mediator is wrong size!");

class emSpar : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 39> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 10> m_areaInstances;          // 0x5A6C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x140];                                        // 0x5BE0
    emTargetSearchUnit m_targetSearchUnits[3];                   // 0x5D20
    emAIUnit m_aiUnits[24];                                      // 0x5D68, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5E28, camera module
    emSparArticleMediator m_articleMediator;                     // 0x5E78, generate-article-manage module

public:
    emSpar(u32 p1, emCreate* create);
    virtual ~emSpar();

    // Sets up a freshly generated article for a shot (called by the article mediator).
    // HYPOTHESIS: named after the Primid's activeArticle, which this one follows step by step.
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emSpar) == 0x5f1c, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emSpar, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emSpar::activeArticle(article, acc); }
};
