#pragma once

// SHADOW of BrawlHeaders em/robohoming/em_robohoming.h (-I include comes first): the full layout of the Robohoming
// (enemy kind Enemy_Robo_Homing).

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// (soArrayVector<soAreaInstance, 8> is shared with the Hammer Bro and the Robobeam: em/em_module_arrays.h declares it.)

class emRobohoming;

// The articles of the Robohoming: five wnemSwHolder (article id 0), of weapon kind 39 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Robo_Homing> emRobohomingArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 5, emWeaponInstanceHolder, soIntToType<0> > emRobohomingSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emRobohoming, soArticleMediatorAssignmentNull> emRobohomingArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emRobohomingSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emRobohomingArticleAssignment, soTypeListNullType>, emRobohomingArticleKind,
                              soTypeListNullType>
    emRobohomingArticleMediator;
static_assert(sizeof(emRobohomingArticleMediator) == 0x104, "Article mediator is wrong size!");

class emRobohoming : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 54> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 8> m_areaInstances;           // 0x5D3C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x100];                                        // 0x5E68
    emTargetSearchUnit m_targetSearchUnits[1];                   // 0x5F68
    emAIUnit m_aiUnits[18];                                      // 0x5F80, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x6010, camera module
    emRobohomingArticleMediator m_articleMediator;               // 0x6060, generate-article-manage module

public:
    emRobohoming(u32 p1, emCreate* create);
    virtual ~emRobohoming();

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emRobohoming) == 0x6164, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emRobohoming, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emRobohoming::activeArticle(article, acc); }
};
