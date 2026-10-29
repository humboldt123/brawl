// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/taboo/em_taboo.h>
#include <em/em_shot.h>
#include <math.h>

// Tabuu: an Enemy with twenty wnemSwHolder articles. Besides its destructor the class overrides notifyEventLink, where
// it grabs and lets go of fighters as the hands do; its other own code is the activator of its articles, which can
// place a shot of kind 3 at an offset from the aimed point and clamps its height. Everything else in this unit is the
// module storage and the template instances of em/em_article_mediator.h.

// HYPOTHESIS: the statuses in which Tabuu holds a fighter it grabs. In the second pair a grab also moves him on to
// Taboo_Status_CatchHold.
static const int Taboo_Status_CatchA = 0x24;
static const int Taboo_Status_CatchB = 0x25;
static const int Taboo_Status_CatchC = 0x13;
static const int Taboo_Status_CatchD = 0x15;
static const int Taboo_Status_CatchHold = 0x14;

// HYPOTHESIS: work ids of a placed shot (kind 3), from how the activator uses them: with the flag clear, the shot is
// moved from the aimed point (WorkFloat_ShotTargetX/Y, em/em_shot.h) by the given distance at the given angle (degrees),
// mirrored by the facing; its height is then clamped to [min, max] and the result written back to the target point.
static const u32 WorkFlag_ShotNoOffset = 0x12000006;
static const u32 WorkFloat_ShotOffsetAngle = 0x11000004;
static const u32 WorkFloat_ShotOffsetDistance = 0x1100000d;
static const u32 WorkFloat_ShotMinY = 0x11000007;
static const u32 WorkFloat_ShotMaxY = 0x11000008;

// ---- construction ---------------------------------------------------------------------------------------------------

emTaboo::emTaboo(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(146, 0),
    m_areaInstances(7, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emTaboo::~emTaboo() { }

// ---- the activator --------------------------------------------------------------------------------------------------

bool emTaboo::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    if (param->m_shotKind == 3) {
        if (!moduleAccesser->getWorkManageModule().isFlag(WorkFlag_ShotNoOffset)) {
            pos.m_x = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetX);
            pos.m_y = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetY);
            float angle = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotOffsetAngle);
            float distance = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotOffsetDistance);
            angle = 0.017453292f * angle;
            float offsetX = distance * static_cast<float>(cos(angle));
            float offsetY = distance * static_cast<float>(sin(angle));
            if (1.0f == moduleAccesser->getPostureModule().getLr()) {
                pos.m_x += offsetX;
                pos.m_y += offsetY;
            } else {
                pos.m_x -= offsetX;
                pos.m_y -= offsetY;
            }
        }
        float minY = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotMinY);
        float maxY = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotMaxY);
        if (pos.m_y < minY) {
            pos.m_y = minY;
        }
        if (pos.m_y > maxY) {
            pos.m_y = maxY;
        }
        moduleAccesser->getWorkManageModule().setFloat(pos.m_x, WorkFloat_ShotTargetX);
        moduleAccesser->getWorkManageModule().setFloat(pos.m_y, WorkFloat_ShotTargetY);
    }
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

// ---- events ---------------------------------------------------------------------------------------------------------

void emTaboo::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* acc, StageObject* sender, int unk4) {
    emTabooLinkEventArgs* args = static_cast<emTabooLinkEventArgs*>(eventInfo);
    if (moduleAccesser.getStatusModule().getStatusKind() == Taboo_Status_CatchA ||
        moduleAccesser.getStatusModule().getStatusKind() == Taboo_Status_CatchB) {
        switch (eventInfo->m_eventKind) {
        case Taboo_LinkEvent_Catch:
            // A fighter touched the grab: hold it, unless Tabuu already holds something.
            if (getTabooCatchModule(moduleAccesser).isCatch() == false && sender->soGetKind() == StageObject_Fighter) {
                getTabooCollisionCatchModule(moduleAccesser).unk18();
                args->_0x4 = true;
                args->unk1C = true;
                args->m_param = 0x8e;
                return;
            }
            break;
        case Taboo_LinkEvent_Release:
            getTabooCatchModule(*m_moduleAccesser).unk40(0);
            getTabooCatchModule(*m_moduleAccesser).catchCut(true, false);
            return;
        case Taboo_LinkEvent_Escape:
            getTabooCatchModule(*m_moduleAccesser).unk40(0);
            getTabooCatchModule(*m_moduleAccesser).catchCut(true, false);
            return;
        }
    }
    if (moduleAccesser.getStatusModule().getStatusKind() == Taboo_Status_CatchC ||
        moduleAccesser.getStatusModule().getStatusKind() == Taboo_Status_CatchD) {
        switch (eventInfo->m_eventKind) {
        case Taboo_LinkEvent_Catch:
            if (getTabooCatchModule(moduleAccesser).isCatch() == false && sender->soGetKind() == StageObject_Fighter) {
                getTabooCollisionCatchModule(moduleAccesser).unk18();
                args->_0x4 = true;
                args->unk1C = true;
                args->m_param = 0x8e;
                m_moduleAccesser->getStatusModule().changeStatusRequest(Taboo_Status_CatchHold, m_moduleAccesser);
                return;
            }
            break;
        case Taboo_LinkEvent_Release:
            getTabooCatchModule(*m_moduleAccesser).unk40(0);
            getTabooCatchModule(*m_moduleAccesser).catchCut(true, false);
            return;
        case Taboo_LinkEvent_Escape:
            getTabooCatchModule(*m_moduleAccesser).unk40(0);
            getTabooCatchModule(*m_moduleAccesser).catchCut(true, false);
            return;
        }
    }
    Enemy::notifyEventLink(eventInfo, acc, sender, unk4);
}
