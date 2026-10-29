#pragma once

// SHADOW of BrawlHeaders em/cataguard/em_cataguard.h (-I include comes first): the full layout of the Cataguard.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// (soArrayVector<soInterpolation<Vec3f>, 23> and soArrayVector<soAreaInstance, 12>, classes of sora_melee, are shared
// with other enemies: em/em_module_arrays.h declares them.)

class emCataguard;

// The articles of the Cataguard: three pools of wnemSwHolder (article ids 0, 1 and 2, holding one, three and one
// instance), of weapon kind 41 (its enemy kind). All three are set up by the same activator.
typedef soKindInfoGeneric<1, Enemy_Cataguard> emCataguardArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 1, emWeaponInstanceHolder, soIntToType<0> > emCataguardSwHolderPoolInfo0;
typedef soInstancePoolInfo<wnemSwHolder, 3, emWeaponInstanceHolder, soIntToType<1> > emCataguardSwHolderPoolInfo1;
typedef soInstancePoolInfo<wnemSwHolder, 1, emWeaponInstanceHolder, soIntToType<2> > emCataguardSwHolderPoolInfo2;
typedef soArticleMediatorAssignmentInfo<emCataguard, soArticleMediatorAssignmentNull> emCataguardArticleAssignment;
typedef soArticleMediatorImpl<
    soTypeList<emCataguardSwHolderPoolInfo0,
               soTypeList<emCataguardSwHolderPoolInfo1, soTypeList<emCataguardSwHolderPoolInfo2, soTypeListNullType> > >,
    soTypeList<emCataguardArticleAssignment,
               soTypeList<emCataguardArticleAssignment, soTypeList<emCataguardArticleAssignment, soTypeListNullType> > >,
    emCataguardArticleKind, soTypeListNullType>
    emCataguardArticleMediator;
static_assert(sizeof(emCataguardArticleMediator) == 0x10c, "Article mediator is wrong size!");

class emCataguard : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 23> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 12> m_areaInstances;          // 0x576C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x180];                                        // 0x5928
    emTargetSearchUnit m_targetSearchUnits[5];                   // 0x5AA8
    emAIUnit m_aiUnits[32];                                      // 0x5B20, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5C20, camera module
    emCataguardArticleMediator m_articleMediator;                // 0x5C70, generate-article-manage module

public:
    emCataguard(u32 p1, emCreate* create);
    virtual ~emCataguard();

    // Set up a freshly generated article for a shot (called by the article mediator for all three pools).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emCataguard) == 0x5d7c, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emCataguard, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emCataguard::activeArticle(article, acc); }
};
template <>
struct soArticleMediatorActivator<emCataguard, 1> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emCataguard::activeArticle(article, acc); }
};
template <>
struct soArticleMediatorActivator<emCataguard, 2> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emCataguard::activeArticle(article, acc); }
};
