#pragma once

// SHADOW of BrawlHeaders em/duon/em_duon.h (-I include comes first): the full layout of the Duon.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: the Duon only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 65);

class emDuon;

// The articles of the Duon: fourteen wnemSwHolder (article id 0), of weapon kind 54 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Boss_Duon> emDuonArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 14, emWeaponInstanceHolder, soIntToType<0> > emDuonSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emDuon, soArticleMediatorAssignmentNull> emDuonArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emDuonSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emDuonArticleAssignment, soTypeListNullType>, emDuonArticleKind,
                              soTypeListNullType>
    emDuonArticleMediator;
static_assert(sizeof(emDuonArticleMediator) == 0x2b4, "Article mediator is wrong size!");

class emDuon : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 65> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 10> m_areaInstances;          // 0x5F4C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x140];                                        // 0x60C0
    emTargetSearchUnit m_targetSearchUnits[3];                   // 0x6200
    emAIUnit m_aiUnits[24];                                      // 0x6248, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x6308, camera module
    emDuonArticleMediator m_articleMediator;                     // 0x6358, generate-article-manage module

public:
    emDuon(u32 p1, emCreate* create);
    virtual ~emDuon();

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emDuon) == 0x660c, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emDuon, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emDuon::activeArticle(article, acc); }
};
