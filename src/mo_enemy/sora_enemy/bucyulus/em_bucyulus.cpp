// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/bucyulus/em_bucyulus.h>
#include <so/catch/so_catch_module.h>
#include <so/so_value_accesser.h>

// The Bucyulus: an Enemy without articles of its own (the shared null mediator) that latches onto a fighter. Besides
// its destructor the class overrides notifyEventLink (catching a fighter, being shaken off) and notifyEventChangeStatus
// (on latching, it tells the fighter how to carry it).

// The catch module slots after catchCut (0x2C), which so/catch/so_catch_module.h leaves to its users (enemy.cpp keeps
// its own copy of this view).
// HYPOTHESIS: names; cutConstraint as in enemy.cpp (Enemy::cutCatch calls 0x34 before catchCut).
class emBucyulusCatchModuleView : public soCatchModule {
public:
    virtual void unk30();
    virtual void cutConstraint(); // 0x34
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40(int); // 0x40, called with 0 before the constraint is cut
};
static inline emBucyulusCatchModuleView& getCatchModule(const soModuleAccesser& acc) {
    return *static_cast<emBucyulusCatchModuleView*>(static_cast<soCatchModule*>(acc.m_enumerationStart->m_catchModule));
}

// The collision catch module (void* in the enumeration); only the slot the Bucyulus calls.
// HYPOTHESIS: slot 0x18 clears the catch collisions of the frame (called when the Bucyulus takes the fighter).
class emBucyulusCollisionCatchModuleView {
public:
    virtual ~emBucyulusCollisionCatchModuleView();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18(); // 0x18
};
static inline emBucyulusCollisionCatchModuleView& getCollisionCatchModule(const soModuleAccesser& acc) {
    return *static_cast<emBucyulusCollisionCatchModuleView*>(acc.m_enumerationStart->m_collisionCatchModule);
}

// HYPOTHESIS: the link events below, from what the handler does: 8 a fighter touches the catch collision (the
// Bucyulus latches on), 9 and 0xE the fighter shook it off or it was cut loose.
// The catch event's flag at 0x1C is cleared when the Bucyulus takes the fighter (HYPOTHESIS: the sender may still
// catch it).
struct emBucyulusCatchLinkEventArgs : public soLinkEventArgs {
    u8 unk5[0x1C - 0x5];
    bool unk1C;
};

emBucyulus::emBucyulus(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(23, 0),
    m_areaInstances(7, 0),
    m_cameraSubjects(1, 0) { }

emBucyulus::~emBucyulus() { }

void emBucyulus::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* acc, StageObject* sender, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 8:
        // A fighter the Bucyulus is not holding yet: it latches on (status 0xC).
        if (getCatchModule(moduleAccesser).isCatch() == false && sender->soGetKind() == StageObject_Fighter) {
            getCollisionCatchModule(moduleAccesser).unk18();
            eventInfo->_0x4 = true;
            static_cast<emBucyulusCatchLinkEventArgs*>(eventInfo)->unk1C = false;
            getCatchModule(*m_moduleAccesser).setCatch(sender->m_taskId);
            m_moduleAccesser->getStatusModule().changeStatusRequest(0xc, m_moduleAccesser);
            return;
        }
        break;
    case 0xe:
        // Released (0xE, or 9 below): the Bucyulus lets go, faces front again and enters status 0xE.
        getCatchModule(*m_moduleAccesser).unk40(0);
        getCatchModule(*m_moduleAccesser).cutConstraint();
        getCatchModule(*m_moduleAccesser).catchCut(true, false);
        initRot(0);
        m_moduleAccesser->getStatusModule().changeStatusRequest(0xe, m_moduleAccesser);
        return;
    case 9:
        getCatchModule(*m_moduleAccesser).unk40(0);
        getCatchModule(*m_moduleAccesser).cutConstraint();
        getCatchModule(*m_moduleAccesser).catchCut(true, false);
        initRot(0);
        m_moduleAccesser->getStatusModule().changeStatusRequest(0xe, m_moduleAccesser);
        return;
    }
    Enemy::notifyEventLink(eventInfo, acc, sender, unk4);
}

// On entering status 0xC with a fighter linked, the Bucyulus sends it link event 0x35 with parameters 0xFA6 and 0xFA7.
void emBucyulus::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* acc) {
    if (statusKind == 0xc) {
        if (acc->getLinkModule().isLinked(0) == true) {
            emBucyulusHoldLinkEventArgs args(0x35);
            args.m_param.m_x = soValueAccesser::getValueFloat(acc, 0xfa6, 0);
            args.m_param.m_y = 1.0f;
            args.m_param.m_z = soValueAccesser::getValueFloat(acc, 0xfa7, 0);
            acc->getLinkModule().sendEventNodes(0, args, 0);
        }
        return;
    }
    Enemy::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, acc);
}
