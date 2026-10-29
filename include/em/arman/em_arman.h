#pragma once

// SHADOW of BrawlHeaders em/arman/em_arman.h (-I include comes first): the full layout of the Arman (Armank).

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_simple.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: the Arman only calls their constructors and destructors.
// (soArrayVector<soAreaInstance, 15> is shared with the Sir Alamos: em/em_module_arrays.h declares it.)
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 43);

class emArman;

// The articles of the Arman: one wnemSimple (article id 0, its arm: wnemArmanProc runs it) and six wnemSwHolder
// (article id 1), of weapon kind 42 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Arman> emArmanArticleKind;
typedef soInstancePoolInfo<wnemSimple, 1, emWeaponInstanceHolder, soIntToType<0> > emArmanSimplePoolInfo;
typedef soInstancePoolInfo<wnemSwHolder, 6, emWeaponInstanceHolder, soIntToType<1> > emArmanSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emArman, soArticleMediatorAssignmentNull> emArmanArticleAssignment;
typedef soArticleMediatorImpl<
    soTypeList<emArmanSimplePoolInfo, soTypeList<emArmanSwHolderPoolInfo, soTypeListNullType> >,
    soTypeList<emArmanArticleAssignment, soTypeList<emArmanArticleAssignment, soTypeListNullType> >,
    emArmanArticleKind, soTypeListNullType>
    emArmanArticleMediator;
static_assert(sizeof(emArmanArticleMediator) == 0x24b4, "Article mediator is wrong size!");

class emArman : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 43> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 15> m_areaInstances;          // 0x5B2C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x1e0];                                        // 0x5D54
    emTargetSearchUnit m_targetSearchUnits[9];                   // 0x5F34
    emAIUnit m_aiUnits[51];                                      // 0x600C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x61A4, camera module
    emArmanArticleMediator m_articleMediator;                    // 0x61F4, generate-article-manage module

public:
    emArman(u32 p1, emCreate* create);
    virtual ~emArman();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender,
                                 int unk4);

    // Set up a freshly generated article for a shot (called by the article mediator).
    // The names are the map's: activeArticle for the wnemSwHolder, activeArticle1 for the wnemSimple.
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
    static bool activeArticle1(wnemSimple* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emArman) == 0x86a8, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emArman, 0> {
    static bool activate(wnemSimple* article, soModuleAccesser* acc) { return emArman::activeArticle1(article, acc); }
};
template <>
struct soArticleMediatorActivator<emArman, 1> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emArman::activeArticle(article, acc); }
};
