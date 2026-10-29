// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/botron/em_botron.h>
#include <em/em_shot.h>
#include <mt/mt_prng.h>
#include <so/so_value_accesser.h>

// The Botron: an Enemy with three wnemSwHolder articles. The class overrides only its destructor; its own code is the
// activation of the articles. The rest of the unit is the module storage, the article mediator and its pools (their code
// is the template instances of em/em_article_mediator.h).

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

// ---- construction ---------------------------------------------------------------------------------------------------

emBotron::emBotron(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(22, 0),
    m_areaInstances(11, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emBotron::~emBotron() { }

// ---- the activator --------------------------------------------------------------------------------------------------

bool emBotron::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    lbl_41_bss_504.m_rotation.m_y = 90.0f * randf();
    s32 constantId = moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotConstantId);
    if (constantId > ConstantId_CommonLast) {
        lbl_41_bss_504.m_constantParam = soValueAccesser::getConstantIndefinite(moduleAccesser, constantId, 0);
    }
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    emWeaponUtil::activeSimpleWeaponHolder(&pos, false, article, param, moduleAccesser);
    return false;
}
