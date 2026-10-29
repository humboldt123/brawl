#pragma once

// SHADOW of BrawlHeaders em/taboo/em_taboo.h (-I include comes first): the full layout of Tabuu (enemy kind
// Enemy_Boss_Taboo), the final boss of the Subspace Emissary.

#include <StaticAssert.h>
#include <em/em_article_mediator.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <so/catch/so_catch_module.h>
#include <so/link/so_link_event_presenter.h>
#include <types.h>
#include <wn/wn_em_sw_holder.h>

// The array below is a class of sora_melee: Tabuu only calls its constructor and destructor.
// (soArrayVector<soAreaInstance, 7> is shared with other enemies: em/em_module_arrays.h declares it.)
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 146);

class emTaboo;

// The articles of Tabuu: twenty wnemSwHolder (article id 0), of weapon kind 58 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Boss_Taboo> emTabooArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 20, emWeaponInstanceHolder, soIntToType<0> > emTabooSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emTaboo, soArticleMediatorAssignmentNull> emTabooArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emTabooSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emTabooArticleAssignment, soTypeListNullType>, emTabooArticleKind,
                              soTypeListNullType>
    emTabooArticleMediator;
static_assert(sizeof(emTabooArticleMediator) == 0x3d4, "Article mediator is wrong size!");

// HYPOTHESIS: the link events Tabuu handles himself, as Master Hand and Crazy Hand handle the same three
// (em/masterhand/em_masterhand.h): a fighter touching the grab sends Catch (Tabuu answers in the event and holds it), a
// held fighter sends Escape when it breaks free and Release when the hold ends.
enum {
    Taboo_LinkEvent_Catch = 0x8,
    Taboo_LinkEvent_Escape = 0x9,
    Taboo_LinkEvent_Release = 0xe,
};

// HYPOTHESIS: the arguments of the catch event, laid out as Enemy's emLinkEventArgs (enemy.cpp) and the hands'
// emHandLinkEventArgs: Tabuu answers a catch with 0x8E in the parameter and the flag at 0x1C set.
struct emTabooLinkEventArgs : public soLinkEventArgs {
    int m_param; // 0x8
    int unkC;
    int unk10;
    int unk14;
    float unk18;
    bool unk1C;
};

// The catch module slots after catchCut (0x2C), which so/catch/so_catch_module.h leaves to its users (as enemy.cpp's
// emCatchModuleView and the hands' emHandCatchModuleView do).
class emTabooCatchModuleView : public soCatchModule {
public:
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40(int); // 0x40, called with 0 before the catchCut that ends a hold (the hands do the same)
};

// The collision catch module (void* in the enumeration); only the slot Tabuu calls.
class emTabooCollisionCatchModuleView {
public:
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18(); // 0x18, called when a fighter is caught (the hands do the same)
};

inline emTabooCatchModuleView& getTabooCatchModule(const soModuleAccesser& acc) {
    return *static_cast<emTabooCatchModuleView*>(static_cast<soCatchModule*>(acc.m_enumerationStart->m_catchModule));
}
inline emTabooCollisionCatchModuleView& getTabooCollisionCatchModule(const soModuleAccesser& acc) {
    return *static_cast<emTabooCollisionCatchModuleView*>(acc.m_enumerationStart->m_collisionCatchModule);
}

class emTaboo : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 146> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;            // 0x6E7C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xe0];                                          // 0x6F84
    emTargetSearchUnit m_targetSearchUnits[1];                    // 0x7064
    emAIUnit m_aiUnits[23];                                       // 0x707C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;           // 0x7134, camera module
    emTabooArticleMediator m_articleMediator;                     // 0x7184, generate-article-manage module

public:
    emTaboo(u32 p1, emCreate* create);
    virtual ~emTaboo();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender,
                                 int unk4);

    // Set up a freshly generated article for a shot (called by the article mediator).
    static bool activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emTaboo) == 0x7558, "Class is wrong size!");

template <>
struct soArticleMediatorActivator<emTaboo, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) { return emTaboo::activeArticle(article, acc); }
};
