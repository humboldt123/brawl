#pragma once

// SHADOW of BrawlHeaders em/kokkon/em_kokkon.h (-I include comes first): the full layout of the Kokkon.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The Kokkon only calls the constructors and destructors of its arrays, classes of sora_melee that
// em/em_module_arrays.h declares because other enemies share them.

class emKokkon;

// The articles of the Kokkon: five wnemSwHolder (article id 0), of weapon kind 22 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Kokkon> emKokkonArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 5, emWeaponInstanceHolder, soIntToType<0> > emKokkonSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emKokkon, soArticleMediatorAssignmentNull> emKokkonArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emKokkonSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emKokkonArticleAssignment, soTypeListNullType>, emKokkonArticleKind,
                              soTypeListNullType>
    emKokkonArticleMediator;
static_assert(sizeof(emKokkonArticleMediator) == 0x104, "Article mediator is wrong size!");

class emKokkon : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 28> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x585C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                        // 0x59AC
    emTargetSearchUnit m_targetSearchUnits[2];                   // 0x5ACC
    emAIUnit m_aiUnits[18];                                      // 0x5AFC, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5B8C, camera module
    emKokkonArticleMediator m_articleMediator;                   // 0x5BDC, generate-article-manage module

public:
    emKokkon(u32 p1, emCreate* create);
    virtual ~emKokkon();

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emKokkon) == 0x5ce0, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emKokkon, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emKokkon::activeArticle(article, acc); }
};
