#pragma once

// SHADOW of BrawlHeaders em/prim/em_prim.h (-I include comes first): the full layout of the Primid.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_simple.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: the Primid only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 44);
FT_DOL_ARRAY_VECTOR(soAreaInstance, 17);

class emPrim;

// The articles of the Primid: two wnemSimple shots (article id 0) and six wnemSwHolder (article id 1), of weapon kind 23.
typedef soKindInfoGeneric<1, Enemy_Prim> emPrimArticleKind;
typedef soInstancePoolInfo<wnemSimple, 2, emWeaponInstanceHolder, soIntToType<0> > emPrimSimplePoolInfo;
typedef soInstancePoolInfo<wnemSwHolder, 6, emWeaponInstanceHolder, soIntToType<1> > emPrimSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emPrim, soArticleMediatorAssignmentNull> emPrimArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emPrimSimplePoolInfo, soTypeList<emPrimSwHolderPoolInfo, soTypeListNullType> >,
                              soTypeList<emPrimArticleAssignment, soTypeList<emPrimArticleAssignment, soTypeListNullType> >,
                              emPrimArticleKind, soTypeListNullType>
    emPrimArticleMediator;
static_assert(sizeof(emPrimArticleMediator) == 0x4830, "Article mediator is wrong size!");

class emPrim : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 44> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 17> m_areaInstances;          // 0x5B5C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance; the enemy area module keeps the pointer (its +0x54).
    u8 m_areaWork[0x220];                                        // 0x5DCC
    emTargetSearchUnit m_targetSearchUnits[12];                  // 0x5FEC
    emAIUnit m_aiUnits[92];                                      // 0x610C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x63EC, camera module
    emPrimArticleMediator m_articleMediator;                     // 0x643C, generate-article-manage module

public:
    emPrim(u32 p1, emCreate* create);
    virtual ~emPrim();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);

    // Set up a freshly generated article for a shot (called by the article mediator).
    // HYPOTHESIS: names follow the fighters' activeArticle / activeArticle1 convention, numbered in definition order.
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
    static bool activeArticle1(wnemSimple* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emPrim) == 0xac6c, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emPrim, 0> {
    static bool activate(wnemSimple* article, soModuleAccesser* acc) { return emPrim::activeArticle1(article, acc); }
};
template <>
struct soArticleMediatorActivator<emPrim, 1> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emPrim::activeArticle(article, acc); }
};
