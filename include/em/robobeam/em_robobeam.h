#pragma once

// SHADOW of BrawlHeaders em/robobeam/em_robobeam.h (-I include comes first): the full layout of the Robobeam (enemy
// kind Enemy_Robo_Beam).

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: the Robobeam only calls their constructors and destructors.
// (soArrayVector<soInterpolation<Vec3f>, 54> and <soAreaInstance, 8> are shared with other enemies: em/em_module_arrays.h
// declares them.)

class emRobobeam;

// The articles of the Robobeam: five wnemSwHolder (article id 0), of weapon kind 37 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Robo_Beam> emRobobeamArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 5, emWeaponInstanceHolder, soIntToType<0> > emRobobeamSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emRobobeam, soArticleMediatorAssignmentNull> emRobobeamArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emRobobeamSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emRobobeamArticleAssignment, soTypeListNullType>, emRobobeamArticleKind,
                              soTypeListNullType>
    emRobobeamArticleMediator;
static_assert(sizeof(emRobobeamArticleMediator) == 0x104, "Article mediator is wrong size!");

class emRobobeam : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 54> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 8> m_areaInstances;           // 0x5D3C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x100];                                        // 0x5E68
    emTargetSearchUnit m_targetSearchUnits[1];                   // 0x5F68
    emAIUnit m_aiUnits[23];                                      // 0x5F80, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x6038, camera module
    emRobobeamArticleMediator m_articleMediator;                 // 0x6088, generate-article-manage module

public:
    emRobobeam(u32 p1, emCreate* create);
    virtual ~emRobobeam();
};
static_assert(sizeof(emRobobeam) == 0x618c, "Class is wrong size!");

// The Robobeam has no activator of its own: the shared one of the Enemy unit (Enemy::activeArticle) runs and the
// mediator hands out the null article (the original ignores the result of the call, so the activation result is a
// constant false here).
template <>
struct soArticleMediatorActivator<emRobobeam, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) {
        Enemy::activeArticle(article, acc);
        return false;
    }
};
