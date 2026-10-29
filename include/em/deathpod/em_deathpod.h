#pragma once

// SHADOW of BrawlHeaders em/deathpod/em_deathpod.h (-I include comes first): the full layout of the Deathpod.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: the Deathpod only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 16);

class emDeathpod;

// The articles of the Deathpod: six wnemSwHolder (article id 0), of weapon kind 6 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Deathpod> emDeathpodArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 6, emWeaponInstanceHolder, soIntToType<0> > emDeathpodSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emDeathpod, soArticleMediatorAssignmentNull> emDeathpodArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emDeathpodSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emDeathpodArticleAssignment, soTypeListNullType>, emDeathpodArticleKind,
                              soTypeListNullType>
    emDeathpodArticleMediator;
static_assert(sizeof(emDeathpodArticleMediator) == 0x134, "Article mediator is wrong size!");

class emDeathpod : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 16> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 10> m_areaInstances;          // 0x561C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x140];                                        // 0x5790
    emTargetSearchUnit m_targetSearchUnits[2];                   // 0x58D0
    emAIUnit m_aiUnits[15];                                      // 0x5900, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5978, camera module
    emDeathpodArticleMediator m_articleMediator;                 // 0x59C8, generate-article-manage module

public:
    emDeathpod(u32 p1, emCreate* create);
    virtual ~emDeathpod();

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emDeathpod) == 0x5afc, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emDeathpod, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emDeathpod::activeArticle(article, acc); }
};
