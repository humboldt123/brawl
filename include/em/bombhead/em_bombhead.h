#pragma once

// SHADOW of BrawlHeaders em/bombhead/em_bombhead.h (-I include comes first): the full layout of the Bombhead
// (Bombed).

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_simple.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: the Bombhead only calls their constructors and destructors.
// (soArrayVector<soAreaInstance, 9> is shared with the Kokkon and the Popperam: em/em_module_arrays.h declares it.)
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 29);

class emBombhead;

// The articles of the Bombhead: one wnemSimple (article id 0) and one wnemSwHolder (article id 1), of weapon kind 27
// (its enemy kind). The class is large because of the wnemSimple pool (the mediator takes 0x23C4 of its 0x8020 bytes).
typedef soKindInfoGeneric<1, Enemy_Bombhead> emBombheadArticleKind;
typedef soInstancePoolInfo<wnemSimple, 1, emWeaponInstanceHolder, soIntToType<0> > emBombheadSimplePoolInfo;
typedef soInstancePoolInfo<wnemSwHolder, 1, emWeaponInstanceHolder, soIntToType<1> > emBombheadSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emBombhead, soArticleMediatorAssignmentNull> emBombheadArticleAssignment;
typedef soArticleMediatorImpl<
    soTypeList<emBombheadSimplePoolInfo, soTypeList<emBombheadSwHolderPoolInfo, soTypeListNullType> >,
    soTypeList<emBombheadArticleAssignment, soTypeList<emBombheadArticleAssignment, soTypeListNullType> >,
    emBombheadArticleKind, soTypeListNullType>
    emBombheadArticleMediator;
static_assert(sizeof(emBombheadArticleMediator) == 0x23c4, "Article mediator is wrong size!");

class emBombhead : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 29> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x588C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                        // 0x59DC
    emTargetSearchUnit m_targetSearchUnits[3];                   // 0x5AFC
    emAIUnit m_aiUnits[25];                                      // 0x5B44, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5C0C, camera module
    emBombheadArticleMediator m_articleMediator;                 // 0x5C5C, generate-article-manage module

public:
    emBombhead(u32 p1, emCreate* create);
    virtual ~emBombhead();

    // Set up a freshly generated article for a shot (called by the article mediator).
    // HYPOTHESIS: names follow the Primid's activeArticle (wnemSwHolder) / activeArticle1 (wnemSimple) convention.
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
    static bool activeArticle1(wnemSimple* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emBombhead) == 0x8020, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emBombhead, 0> {
    static bool activate(wnemSimple* article, soModuleAccesser* acc) { return emBombhead::activeArticle1(article, acc); }
};
template <>
struct soArticleMediatorActivator<emBombhead, 1> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emBombhead::activeArticle(article, acc); }
};
