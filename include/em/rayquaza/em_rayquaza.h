#pragma once

// SHADOW of BrawlHeaders em/rayquaza/em_rayquaza.h (-I include comes first): the full layout of the Rayquaza boss
// (enemy kind Enemy_Boss_Rayquaza). Its weapon processes are wnemRayquazaProc (em/rayquaza/wnem_proc_rayquaza.h).

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: the Rayquaza only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 76);
class emRayquaza;

// The articles of the Rayquaza: five wnemSwHolder (article id 0), of weapon kind 53 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Boss_Rayquaza> emRayquazaArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 5, emWeaponInstanceHolder, soIntToType<0> > emRayquazaSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emRayquaza, soArticleMediatorAssignmentNull> emRayquazaArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emRayquazaSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emRayquazaArticleAssignment, soTypeListNullType>, emRayquazaArticleKind,
                              soTypeListNullType>
    emRayquazaArticleMediator;
static_assert(sizeof(emRayquazaArticleMediator) == 0x104, "Article mediator is wrong size!");

class emRayquaza : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 76> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;           // 0x615C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xE0];                                         // 0x6264
    emTargetSearchUnit m_targetSearchUnits[1];                   // 0x6344
    emAIUnit m_aiUnits[18];                                      // 0x635C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x63EC, camera module
    emRayquazaArticleMediator m_articleMediator;                 // 0x643C, generate-article-manage module

public:
    emRayquaza(u32 p1, emCreate* create);
    virtual ~emRayquaza();

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emRayquaza) == 0x6540, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emRayquaza, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emRayquaza::activeArticle(article, acc); }
};
