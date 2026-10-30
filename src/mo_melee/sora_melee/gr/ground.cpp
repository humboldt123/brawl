#include <gr/ground.h>
#include <st/stage.h>
#include <types.h>

// HYPOTHESIS: grCollision::setJointStatus (main.dol, unnamed); the ground passes itself as the owner.
extern "C" void fn_80110B98(grCollision* collision, bool enable, Ground* ground);
// HYPOTHESIS: stShadow::removeGroundInfo (unnamed in st_shadow.cpp).
extern "C" void fn_27_228FD8(stShadow* shadow, Ground* ground);

Ground::~Ground() {
    unloadData();
}

void Ground::setupMelee() {
    m_isUseMdlNameForStartAnim = false;
    m_isSetup = true;
}

void Ground::setMotionFrame(float frame, u32 animIndex) {
    gfModelAnimation* anim = m_modelAnims[animIndex];
    if (anim) {
        anim->setFrame(frame);
    }
}

float Ground::getMotionFrame(u32 animIndex) {
    gfModelAnimation* anim = m_modelAnims[animIndex];
    if (anim == NULL) {
        return 0.0f;
    } else {
        return anim->getFrame();
    }
}

void Ground::setMotionLoop(bool shouldLoop, u32 animIndex) {
    gfModelAnimation* anim = m_modelAnims[animIndex];
    if (anim) {
        anim->setLoop(shouldLoop);
    }
}

void Ground::invalidatedByCameraClipping() { }

void Ground::update(float deltaFrame) { }

void Ground::processGameProc() { }

void Ground::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) { }

void Ground::receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) { }

void Ground::receiveCollMsg_Wall(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) { }

void Ground::receiveCollMsg_Attack(grCollStatus* collStatus, grCollisionJoint* collisionJoint) { }

void Ground::renderDebug() { }

bool Ground::setNode() {
    return false;
}

void Ground::setEnableCollisionStatus(bool enableColl) {
    m_isEnableCollisionStatus = enableColl;
    grCollision* collision = m_collision;
    if (collision) {
        if (m_isEnableCollisionStatus && m_isEnableCollisionStatusByClipping) {
            fn_80110B98(collision, true, this);
        } else {
            fn_80110B98(collision, false, this);
        }
    }
}

void Ground::setEnableCollisionStatusByClipping(bool enableColl) {
    m_isEnableCollisionStatusByClipping = enableColl;
    grCollision* collision = m_collision;
    if (collision) {
        if (m_isEnableCollisionStatusByClipping && m_isEnableCollisionStatus) {
            fn_80110B98(collision, true, this);
        } else {
            fn_80110B98(collision, false, this);
        }
    }
}

void Ground::preExit() {
    fn_27_228FD8(g_Stage->m_shadow, this);
}

void Ground::setDontMoveGround() {
    if (m_resFile.HasResAnmChr() != true) {
        m_isUpdateG3dCalcWorld = false;
    }
}

void Ground::setTransparencyFlag(char flag) { }

void Ground::setMotionRatio(float ratio) {
    m_motionRatio = ratio;
}
