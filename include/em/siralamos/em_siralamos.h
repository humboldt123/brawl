#pragma once

// SHADOW of BrawlHeaders em/siralamos/em_siralamos.h (-I include comes first): the full layout of the Sir Alamos.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_simple.h>
#include <wn/wn_em_sw_holder.h>

// Its module arrays (soInterpolation<Vec3f> 33, soAreaInstance 15) are declared in em/em_module_arrays.h.

class emSiralamos;

// The articles of the Sir Alamos: two wnemSimple shots (article id 0) and two wnemSwHolder (article id 1), of weapon
// kind 12 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_SirAlamos> emSiralamosArticleKind;
typedef soInstancePoolInfo<wnemSimple, 2, emWeaponInstanceHolder, soIntToType<0> > emSiralamosSimplePoolInfo;
typedef soInstancePoolInfo<wnemSwHolder, 2, emWeaponInstanceHolder, soIntToType<1> > emSiralamosSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emSiralamos, soArticleMediatorAssignmentNull> emSiralamosArticleAssignment;
typedef soArticleMediatorImpl<
    soTypeList<emSiralamosSimplePoolInfo, soTypeList<emSiralamosSwHolderPoolInfo, soTypeListNullType> >,
    soTypeList<emSiralamosArticleAssignment, soTypeList<emSiralamosArticleAssignment, soTypeListNullType> >,
    emSiralamosArticleKind, soTypeListNullType>
    emSiralamosArticleMediator;
static_assert(sizeof(emSiralamosArticleMediator) == 0x4770, "Article mediator is wrong size!");

class emSiralamos : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 33> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 15> m_areaInstances;          // 0x594C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x1e0];                                        // 0x5B74
    emTargetSearchUnit m_targetSearchUnits[6];                   // 0x5D54
    emAIUnit m_aiUnits[26];                                      // 0x5DE4, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5EB4, camera module
    emSiralamosArticleMediator m_articleMediator;                // 0x5F04, generate-article-manage module

public:
    emSiralamos(u32 p1, emCreate* create);
    virtual ~emSiralamos();
};
static_assert(sizeof(emSiralamos) == 0xa674, "Class is wrong size!");

// The Sir Alamos has no activators of its own: the shared ones of the Enemy unit (Enemy::activeArticle1 for the
// wnemSimple shots, Enemy::activeArticle for the wnemSwHolder) run and the mediator hands out the null article (the
// original ignores the result of the calls, so the activation result is a constant false here, as for the Hammer Bro).
template <>
struct soArticleMediatorActivator<emSiralamos, 0> {
    static bool activate(wnemSimple* article, soModuleAccesser* acc) {
        Enemy::activeArticle1(article, acc);
        return false;
    }
};
template <>
struct soArticleMediatorActivator<emSiralamos, 1> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) {
        Enemy::activeArticle(article, acc);
        return false;
    }
};
