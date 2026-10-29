#pragma once

// SHADOW of BrawlHeaders em/metaridley/em_metaridley.h (-I include comes first): the full layout of Meta Ridley.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The arrays below are classes of sora_melee: Meta Ridley only calls their constructors and destructors.
// (soArrayVector<soAreaInstance, 8> is shared with the Hammer Bro and the Robobeam: em/em_module_arrays.h declares it.)
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 106);

class emMetaridley;

// The articles of Meta Ridley: twenty wnemSwHolder (article id 0), of weapon kind 56 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Boss_Metaridley> emMetaridleyArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 20, emWeaponInstanceHolder, soIntToType<0> > emMetaridleySwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emMetaridley, soArticleMediatorAssignmentNull> emMetaridleyArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emMetaridleySwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emMetaridleyArticleAssignment, soTypeListNullType>, emMetaridleyArticleKind,
                              soTypeListNullType>
    emMetaridleyArticleMediator;
static_assert(sizeof(emMetaridleyArticleMediator) == 0x3d4, "Article mediator is wrong size!");

class emMetaridley : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 106> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 8> m_areaInstances;            // 0x66FC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x100];                                         // 0x6828
    emTargetSearchUnit m_targetSearchUnits[2];                    // 0x6928
    emAIUnit m_aiUnits[22];                                       // 0x6958, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;           // 0x6A08, camera module
    emMetaridleyArticleMediator m_articleMediator;                // 0x6A58, generate-article-manage module

public:
    emMetaridley(u32 p1, emCreate* create);
    virtual ~emMetaridley();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                         soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject,
                                 int unk4);

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emMetaridley) == 0x6e2c, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emMetaridley, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) {
        return emMetaridley::activeArticle(article, acc);
    }
};
