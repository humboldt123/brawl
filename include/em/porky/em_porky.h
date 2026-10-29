#pragma once

// SHADOW of BrawlHeaders em/porky/em_porky.h (-I include comes first): the full layout of the Porky.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: the Porky only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 61);

class emPorky;

// The articles of the Porky: seven wnemSwHolder (article id 0), of weapon kind 55 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Boss_Porky> emPorkyArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 7, emWeaponInstanceHolder, soIntToType<0> > emPorkySwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emPorky, soArticleMediatorAssignmentNull> emPorkyArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emPorkySwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emPorkyArticleAssignment, soTypeListNullType>, emPorkyArticleKind,
                              soTypeListNullType>
    emPorkyArticleMediator;
static_assert(sizeof(emPorkyArticleMediator) == 0x164, "Article mediator is wrong size!");

class emPorky : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 61> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x5E8C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                        // 0x5FDC
    emTargetSearchUnit m_targetSearchUnits[3];                   // 0x60FC
    emAIUnit m_aiUnits[33];                                      // 0x6144, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x624C, camera module
    emPorkyArticleMediator m_articleMediator;                    // 0x629C, generate-article-manage module

public:
    emPorky(u32 p1, emCreate* create);
    virtual ~emPorky();
    virtual void updateNodeSRT();

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emPorky) == 0x6400, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emPorky, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emPorky::activeArticle(article, acc); }
};
