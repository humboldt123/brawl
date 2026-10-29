#pragma once

// SHADOW of BrawlHeaders em/robodistance/em_robodistance.h (-I include comes first): the full layout of the
// Robodistance (enemy kind Enemy_Robo_Distance).

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The Robodistance only calls the constructors and destructors of its arrays, classes of sora_melee that
// em/em_module_arrays.h declares because other enemies share them.

class emRobodistance;

// The article of the Robodistance: one wnemSwHolder (article id 0), of weapon kind 38 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Robo_Distance> emRobodistanceArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 1, emWeaponInstanceHolder, soIntToType<0> > emRobodistanceSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emRobodistance, soArticleMediatorAssignmentNull> emRobodistanceArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emRobodistanceSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emRobodistanceArticleAssignment, soTypeListNullType>,
                              emRobodistanceArticleKind, soTypeListNullType>
    emRobodistanceArticleMediator;
static_assert(sizeof(emRobodistanceArticleMediator) == 0x44, "Article mediator is wrong size!");

class emRobodistance : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 54> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 12> m_areaInstances;          // 0x5D3C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x180];                                        // 0x5EF8
    emTargetSearchUnit m_targetSearchUnits[5];                   // 0x6078
    emAIUnit m_aiUnits[25];                                      // 0x60F0, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x61B8, camera module
    emRobodistanceArticleMediator m_articleMediator;             // 0x6208, generate-article-manage module

public:
    emRobodistance(u32 p1, emCreate* create);
    virtual ~emRobodistance();
};
static_assert(sizeof(emRobodistance) == 0x624c, "Class is wrong size!");

// The Robodistance has no activator of its own: the shared one of the Enemy unit (Enemy::activeArticle) runs and the
// mediator hands out the null article (the original ignores the result of the call, as for the Robobeam).
template <>
struct soArticleMediatorActivator<emRobodistance, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) {
        Enemy::activeArticle(article, acc);
        return false;
    }
};
