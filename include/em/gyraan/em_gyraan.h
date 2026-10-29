#pragma once

// SHADOW of BrawlHeaders em/gyraan/em_gyraan.h (-I include comes first): the full layout of the Gyraan.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>
#include <wn/wn_em_simple.h>

// The arrays are classes of sora_melee, declared in em/em_module_arrays.h (soInterpolation<Vec3f> x22 is shared with
// the Botron, soAreaInstance x9 with the Kokkon and the Popperam).

class emGyraan;

// The articles of the Gyraan: two wnemSimple (article id 0), of weapon kind 11 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Gyraan> emGyraanArticleKind;
typedef soInstancePoolInfo<wnemSimple, 2, emWeaponInstanceHolder, soIntToType<0> > emGyraanSimplePoolInfo;
typedef soArticleMediatorAssignmentInfo<emGyraan, soArticleMediatorAssignmentNull> emGyraanArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emGyraanSimplePoolInfo, soTypeListNullType>,
                              soTypeList<emGyraanArticleAssignment, soTypeListNullType>, emGyraanArticleKind,
                              soTypeListNullType>
    emGyraanArticleMediator;
static_assert(sizeof(emGyraanArticleMediator) == 0x470c, "Article mediator is wrong size!");

class emGyraan : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 22> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x573C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                        // 0x588C
    emTargetSearchUnit m_targetSearchUnits[3];                   // 0x59AC
    emAIUnit m_aiUnits[31];                                      // 0x59F4, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5AEC, camera module
    // Set when one of the Gyraan's axes sticks in a floor or wall: wnemGyraanProc::AxeStick
    // (gyraan/wnem_proc_gyraan.cpp) sends link event 0x1403 to its parents, and notifyEventLink sets this. Cleared by
    // the constructor.
    // HYPOTHESIS: read by the Gyraan's AI or status code (not in this unit).
    bool m_isAxeStuck;                                           // 0x5B3C
    emGyraanArticleMediator m_articleMediator;                   // 0x5B40, generate-article-manage module

public:
    emGyraan(u32 p1, emCreate* create);
    virtual ~emGyraan();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
};
static_assert(sizeof(emGyraan) == 0xa24c, "Class is wrong size!");

// The Gyraan has no activator of its own: the shared one of the Enemy unit runs and the mediator hands out the null
// article (the original ignores the result of the call, as for the Sir Alamos).
template <>
struct soArticleMediatorActivator<emGyraan, 0> {
    static bool activate(wnemSimple* article, soModuleAccesser* acc) {
        Enemy::activeArticle1(article, acc);
        return false;
    }
};
