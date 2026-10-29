#pragma once

// SHADOW of BrawlHeaders em/galleom/em_galleom.h (-I include comes first): the full layout of the Galleom boss
// (enemy kind Enemy_Boss_Galleom).

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: the Galleom only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 92);
class emGalleom;

// The articles of the Galleom: five wnemSwHolder (article id 0), of weapon kind 51 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Boss_Galleom> emGalleomArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 5, emWeaponInstanceHolder, soIntToType<0> > emGalleomSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emGalleom, soArticleMediatorAssignmentNull> emGalleomArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emGalleomSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emGalleomArticleAssignment, soTypeListNullType>, emGalleomArticleKind,
                              soTypeListNullType>
    emGalleomArticleMediator;
static_assert(sizeof(emGalleomArticleMediator) == 0x104, "Article mediator is wrong size!");

class emGalleom : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 92> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;           // 0x645C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xE0];                                         // 0x6564
    emTargetSearchUnit m_targetSearchUnits[1];                   // 0x6644
    emAIUnit m_aiUnits[24];                                      // 0x665C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x671C, camera module
    emGalleomArticleMediator m_articleMediator;                  // 0x676C, generate-article-manage module

public:
    emGalleom(u32 p1, emCreate* create);
    virtual ~emGalleom();

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emGalleom) == 0x6870, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emGalleom, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emGalleom::activeArticle(article, acc); }
};
