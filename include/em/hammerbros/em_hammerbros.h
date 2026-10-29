#pragma once

// SHADOW of BrawlHeaders em/hammerbros/em_hammerbros.h (-I include comes first): the full layout of the Hammer Bro.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The Hammer Bro only calls the constructors and destructors of its arrays, classes of sora_melee that
// em/em_module_arrays.h declares because other enemies share them.

class emHammerbros;

// The articles of the Hammer Bro: ten wnemSwHolder (article id 0), of weapon kind 19 (its enemy kind).
// HYPOTHESIS: these carry the thrown hammers; ten is the number that can be in flight at once.
typedef soKindInfoGeneric<1, Enemy_HammerBros> emHammerbrosArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 10, emWeaponInstanceHolder, soIntToType<0> > emHammerbrosSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emHammerbros, soArticleMediatorAssignmentNull> emHammerbrosArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emHammerbrosSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emHammerbrosArticleAssignment, soTypeListNullType>, emHammerbrosArticleKind,
                              soTypeListNullType>
    emHammerbrosArticleMediator;
static_assert(sizeof(emHammerbrosArticleMediator) == 0x1f4, "Article mediator is wrong size!");

class emHammerbros : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 32> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 8> m_areaInstances;           // 0x591C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid (8 instances, 0x100 bytes).
    u8 m_areaWork[0x100];                                        // 0x5A48
    emTargetSearchUnit m_targetSearchUnits[2];                   // 0x5B48
    emAIUnit m_aiUnits[29];                                      // 0x5B78, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5C60, camera module
    emHammerbrosArticleMediator m_articleMediator;               // 0x5CB0, generate-article-manage module

public:
    emHammerbros(u32 p1, emCreate* create);
    virtual ~emHammerbros();
};
static_assert(sizeof(emHammerbros) == 0x5ea4, "Class is wrong size!");

// The Hammer Bro has no activator of its own: the shared one of the Enemy unit (Enemy::activeArticle) runs and the
// mediator hands out the null article (the original ignores the result of the call, so the activation result is a
// constant false here).
template <>
struct soArticleMediatorActivator<emHammerbros, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) {
        Enemy::activeArticle(article, acc);
        return false;
    }
};
