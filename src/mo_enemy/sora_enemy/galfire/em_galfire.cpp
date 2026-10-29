// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// MATCH-ONLY: processUpdate copies the returned trace speed with the word copy of the plain Vec2f base.
#define MT_VEC2F_PLAIN_BASE
#include <em/galfire/em_galfire.h>
#include <so/ground/so_ground_module_impl.h>

// The Galfire: an Enemy with no articles. While its ground module traces a line (the Galfire crawls along walls and
// ceilings), every update carries the trace's ground movement into its kinetic module before the Enemy update runs.

emGalfire::emGalfire(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(24, 0),
    m_areaInstances(7, 0),
    m_cameraSubjects(1, 0) { }

emGalfire::~emGalfire() { }

void emGalfire::processUpdate() {
    soGroundModule& ground = moduleAccesser.getGroundModule();
    if (ground.isTraced() == true) {
        Vec2f speed = ground.getTraceGroundMovementSpeed();
        // A reference cast (the original passes 1 as __dynamic_cast's last argument).
        dynamic_cast<emKineticModuleImpl&>(moduleAccesser.getKineticModule()).addSpeedToGroundMovement(&speed);
    }
    Enemy::processUpdate();
}
