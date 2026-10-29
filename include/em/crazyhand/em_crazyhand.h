#pragma once

// SHADOW of BrawlHeaders em/crazyhand/em_crazyhand.h (-I include comes first): the full layout of Crazy Hand
// (enemy kind Enemy_Boss_Crazyhand).

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>
// The link events, event arguments and module views both hands share.
#include <em/masterhand/em_masterhand.h>

// The arrays below are classes of sora_melee: Crazy Hand only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 38);
// soArrayVector<soAreaInstance, 7> is shared with other enemies: em/em_module_arrays.h declares it.

class emCrazyhand;

// The articles of Crazy Hand: twenty wnemSwHolder (article id 0), of weapon kind 60 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Boss_Crazyhand> emCrazyhandArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 20, emWeaponInstanceHolder, soIntToType<0> > emCrazyhandSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emCrazyhand, soArticleMediatorAssignmentNull> emCrazyhandArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emCrazyhandSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emCrazyhandArticleAssignment, soTypeListNullType>, emCrazyhandArticleKind,
                              soTypeListNullType>
    emCrazyhandArticleMediator;
static_assert(sizeof(emCrazyhandArticleMediator) == 0x3d4, "Article mediator is wrong size!");

class emCrazyhand : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 38> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;           // 0x5A3C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xe0];                                         // 0x5B44
    emTargetSearchUnit m_targetSearchUnits[1];                   // 0x5C24
    emAIUnit m_aiUnits[24];                                      // 0x5C3C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5CFC, camera module
    emCrazyhandArticleMediator m_articleMediator;                // 0x5D4C, generate-article-manage module

public:
    emCrazyhand(u32 p1, emCreate* create);
    virtual ~emCrazyhand();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                         soModuleAccesser* moduleAccesser);

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emCrazyhand) == 0x6120, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emCrazyhand, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emCrazyhand::activeArticle(article, acc); }
};
