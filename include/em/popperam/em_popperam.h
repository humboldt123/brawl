#pragma once

// SHADOW of BrawlHeaders em/popperam/em_popperam.h (-I include comes first): the full layout of the Popperam (Poppant).

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_scaled_value.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <mt/mt_vector.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The Popperam only calls the constructors and destructors of its arrays, classes of sora_melee that
// em/em_module_arrays.h declares because other enemies share them.

class emPopperam;

// The articles of the Popperam: five wnemSwHolder (article id 0), of weapon kind 1 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Popperam> emPopperamArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 5, emWeaponInstanceHolder, soIntToType<0> > emPopperamSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emPopperam, soArticleMediatorAssignmentNull> emPopperamArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emPopperamSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emPopperamArticleAssignment, soTypeListNullType>, emPopperamArticleKind,
                              soTypeListNullType>
    emPopperamArticleMediator;
static_assert(sizeof(emPopperamArticleMediator) == 0x104, "Article mediator is wrong size!");

// The parameter block of an item the Popperam throws (anim cmd 26 hands its address to emShootItem).
// HYPOTHESIS: field roles from emPopperam::emShootItem, the only reader.
struct emPopperamItemShootParam {
    emScaledValue m_speed;         // 0x0, the launch speed
    emScaledValue m_angleRange[2]; // 0x8, the lower and upper bound of the launch angle, in degrees
    int m_nodeId;                  // 0x18, the model node the item is created at
    Vec3f m_offset;                // 0x1C, HYPOTHESIS: an offset from that node (the original only tests its address)
};

class emPopperam : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 23> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x576C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                        // 0x58BC
    emTargetSearchUnit m_targetSearchUnits[3];                   // 0x59DC
    emAIUnit m_aiUnits[26];                                      // 0x5A24, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5AF4, camera module
    emPopperamArticleMediator m_articleMediator;                 // 0x5B44, generate-article-manage module

public:
    emPopperam(u32 p1, emCreate* create);
    virtual ~emPopperam();
    virtual bool emShootItem(itKind kind, int variation, void* param, float, float);

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emPopperam) == 0x5c48, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emPopperam, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) {
        return emPopperam::activeArticle(article, acc);
    }
};
