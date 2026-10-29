#pragma once

// SHADOW of BrawlHeaders em/masterhand/em_masterhand.h (-I include comes first): the full layout of Master Hand
// (enemy kind Enemy_Boss_Masterhand).

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

// The arrays below are classes of sora_melee: Master Hand only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 36);

// ---- shared by Master Hand and Crazy Hand (em/crazyhand/em_crazyhand.h includes this header) ------------------------

// HYPOTHESIS: the link events the two hands handle themselves, named from what each handler does. A fighter touching
// the grab sends Catch (the hand answers in the event and holds it); a held fighter sends Escape when it breaks free
// and Release when the hold ends (the event says whether it is still held). A hand sends Squeeze to the fighter it holds
// when its squeeze status starts, and PartnerAttackHit to its link parent when one of its attacks connects (Master
// Hand sends it; Crazy Hand takes the hit stop of the sender). PartnerChangeStatus and PartnerFlag are only handled by
// Crazy Hand: the first copies the sender's next-status work int into a status change, the second sets a work flag of
// the sender.
enum {
    Hand_LinkEvent_Catch = 0x8,
    Hand_LinkEvent_Escape = 0x9,
    Hand_LinkEvent_Release = 0xe,
    Hand_LinkEvent_Squeeze = 0x35,
    Hand_LinkEvent_PartnerAttackHit = 0x1005,
    Hand_LinkEvent_PartnerChangeStatus = 0x1006,
    Hand_LinkEvent_PartnerFlag = 0x1007,
};

// HYPOTHESIS: the arguments of the catch and release events, laid out as Enemy's emLinkEventArgs (enemy.cpp): the
// hand answers a catch with 5 in the parameter and the flag at 0x1C set. The release event's byte at 0x5 (in the tail
// padding of soLinkEventArgs) says the fighter is still held, which keeps the hand in its status.
struct emHandLinkEventArgs : public soLinkEventArgs {
    bool m_isHeld; // 0x5
    int m_param;   // 0x8
    int unkC;
    int unk10;
    int unk14;
    float unk18;
    bool unk1C;
};

// The squeeze event: the hand's two squeeze parameters around a constant 1 (Master Hand 0xFAC/0xFAD, Crazy Hand
// 0xFAB/0xFAC). HYPOTHESIS: a vector, from the three consecutive floats cleared together.
struct emHandSqueezeEventArgs : public soLinkEventArgs {
    Vec3f m_param;
    emHandSqueezeEventArgs() : soLinkEventArgs(Hand_LinkEvent_Squeeze), m_param(0.0f, 0.0f, 0.0f) { }
};

// An event without arguments.
struct emHandEventArgs : public soLinkEventArgs {
    emHandEventArgs(int eventKind) : soLinkEventArgs(eventKind) { }
};

// The catch module slots after catchCut (0x2C), which so/catch/so_catch_module.h leaves to its users (as enemy.cpp's
// emCatchModuleView does).
class emHandCatchModuleView : public soCatchModule {
public:
    virtual void unk30();
    virtual void cutConstraint(); // 0x34
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40(int); // 0x40, called with 0 before the catchCut that ends a hold
};

// The collision catch module (void* in the enumeration); only the slot the hands call.
class emHandCollisionCatchModuleView {
public:
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18(); // 0x18, called when a fighter is caught
};

inline emHandCatchModuleView& getHandCatchModule(const soModuleAccesser& acc) {
    return *static_cast<emHandCatchModuleView*>(static_cast<soCatchModule*>(acc.m_enumerationStart->m_catchModule));
}
inline emHandCollisionCatchModuleView& getHandCollisionCatchModule(const soModuleAccesser& acc) {
    return *static_cast<emHandCollisionCatchModuleView*>(acc.m_enumerationStart->m_collisionCatchModule);
}

// ---- Master Hand ----------------------------------------------------------------------------------------------------

class emMasterhand;

// The articles of Master Hand: six wnemSwHolder (article id 0), of weapon kind 59 (its enemy kind).
typedef soKindInfoGeneric<1, Enemy_Boss_Masterhand> emMasterhandArticleKind;
typedef soInstancePoolInfo<wnemSwHolder, 6, emWeaponInstanceHolder, soIntToType<0> > emMasterhandSwHolderPoolInfo;
typedef soArticleMediatorAssignmentInfo<emMasterhand, soArticleMediatorAssignmentNull> emMasterhandArticleAssignment;
typedef soArticleMediatorImpl<soTypeList<emMasterhandSwHolderPoolInfo, soTypeListNullType>,
                              soTypeList<emMasterhandArticleAssignment, soTypeListNullType>, emMasterhandArticleKind,
                              soTypeListNullType>
    emMasterhandArticleMediator;
static_assert(sizeof(emMasterhandArticleMediator) == 0x134, "Article mediator is wrong size!");

class emMasterhand : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 36> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 8> m_areaInstances;           // 0x59DC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x100];                                        // 0x5B08
    emTargetSearchUnit m_targetSearchUnits[2];                   // 0x5C08
    emAIUnit m_aiUnits[24];                                      // 0x5C38, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5CF8, camera module
    emMasterhandArticleMediator m_articleMediator;               // 0x5D48, generate-article-manage module

public:
    emMasterhand(u32 p1, emCreate* create);
    virtual ~emMasterhand();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                         soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
static_assert(sizeof(emMasterhand) == 0x5e7c, "Class is wrong size!");

// Master Hand has no activator of its own: the shared one of the Enemy unit (Enemy::activeArticle) runs and the
// mediator hands out the null article (the original ignores the result of the call).
template <>
struct soArticleMediatorActivator<emMasterhand, 0> {
    static bool activate(wnemSwHolder* article, soModuleAccesser* acc) {
        Enemy::activeArticle(article, acc);
        return false;
    }
};
