#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#include <gr/gr_calc_world_callback.h>
#include <st_pirates/gr_pirates_attack.h>
#include <st_pirates/gr_pirates_util.h>
#include <yk/yk_no_hit_normal.h>

extern "C" void fn_8003F074(Matrix* mtx, float x, float y, float z); // HYPOTHESIS: moves the matrix along its own axes

grPiratesAttack::grPiratesAttack(const char* taskName) : grPirates(taskName) {
    m_mtxWork = NULL;
    m_stateWork = NULL;
    mtx()->setIdentity();
    m_type = 3;
    unk199 = 7;
    unk19A = 0;
    unk19B = 0;
    m_work = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grPiratesAttack::~grPiratesAttack() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grPiratesAttack::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made once; the attack is on while the step of the stage is below 2.
void grPiratesAttack::updateYakumono(float deltaFrame) {
    if (unk19A == 1) {
        if (*m_stateWork <= 1) {
            setAttack();
        } else if (unk19B == 1) {
            disableAttack(0);
            unk19B = 0;
        }
        unk199 = *m_stateWork;
    } else {
        setHit();
        if (m_yakumono != NULL) {
            unk19A = 1;
        }
    }
}

void grPiratesAttack::updateActive(float deltaFrame) { }

// The attack follows the matrix it keeps.
void grPiratesAttack::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *mtx();
        }
    }
}

// Builds the hit object of the attack: one attack part, one collision group and no hit module (a ykNoHitNormal of the wall
// category).
void grPiratesAttack::setHit() {
    m_work = new (Heaps::StageInstance) grPiratesAttackWork;
    m_work->unk0 = 0;
    m_work->unk4 = 0;

    ykInitInfo info = {NULL, NULL, 0x10, NULL, NULL};
    info.m_ground = this;
    nw4r::g3d::ScnMdl* model = ykDynamicCastScnMdl(m_sceneModels[0]);
    info.m_node = model;
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_work;

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Wall, 1, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

void grPiratesAttack::setAttack() { }

grPiratesAttackBottom* grPiratesAttackBottom::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesAttackBottom* ground = new (Heaps::StageInstance) grPiratesAttackBottom(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesAttackBottom::~grPiratesAttackBottom() { }

// The attack sits on the bottom of the ship: it follows the matrix, moved by the step of the stage (the bow or the stern).
void grPiratesAttackBottom::updateActive(float deltaFrame) {
    if (m_mtxWork == NULL) {
        return;
    }
    m_mtx = *reinterpret_cast<stPiratesMtx*>(m_mtxWork);
    switch (m_type) {
        case 0:
            if (*m_stateWork == 0) {
                fn_8003F074(mtx(), -25.0f, -50.0f, 0.0f);
            } else if (*m_stateWork == 1) {
                fn_8003F074(mtx(), -5.0f, -50.0f, 0.0f);
            }
            break;
        case 1:
            if (*m_stateWork == 0) {
                fn_8003F074(mtx(), -45.0f, -40.0f, 0.0f);
            } else if (*m_stateWork == 1) {
                fn_8003F074(mtx(), 45.0f, -50.0f, 0.0f);
            }
            break;
    }
}

// The ball of the attack: its place depends on the step and on the part of the ship.
void grPiratesAttackBottom::setAttack() {
    if (m_yakumono != NULL) {
        soCollisionAttackData attack(1.0f);
        if (unk19B == 1) {
            if (*m_stateWork == unk199) {
                return;
            }
            disableAttack(0);
            unk19B = 0;
        }
        Vec3f offset;
        if (*m_stateWork == 0) {
            switch (m_type) {
                case 0:
                    offset.m_x = 60.0f;
                    offset.m_y = 0.0f;
                    offset.m_z = 0.0f;
                    break;
                case 1:
                    offset.m_x = 10.0f;
                    offset.m_y = -10.0f;
                    offset.m_z = 0.0f;
                    break;
            }
        } else if (*m_stateWork == 1) {
            switch (m_type) {
                case 0:
                    offset.m_x = 45.0f;
                    offset.m_y = 0.0f;
                    offset.m_z = 0.0f;
                    break;
                case 1:
                    offset.m_x = 10.0f;
                    offset.m_y = 40.0f;
                    offset.m_z = 0.0f;
                    break;
            }
        }
        setAttackGimmickDetails(&attack, 7.0f, 1.0f, 1.0f, 1.0f,
            20, &offset, 270, 200, 200, 100,
            m_nodeIndex, 0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Large,
            soCollisionAttackData::Sound_Attribute_Punch,
            false, false, false, false, false, false, 10, 60,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, true);
        m_yakumono->setAttack(0, 0, &attack);
        unk19B = 1;
    }
}

grPiratesAttackAground* grPiratesAttackAground::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesAttackAground* ground = new (Heaps::StageInstance) grPiratesAttackAground(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesAttackAground::~grPiratesAttackAground() { }

// The attack sits on the reef: it follows the matrix, moved to the reef.
void grPiratesAttackAground::updateActive(float deltaFrame) {
    if (m_mtxWork == NULL) {
        return;
    }
    m_mtx = *reinterpret_cast<stPiratesMtx*>(m_mtxWork);
    fn_8003F074(mtx(), 86.0f, 10.0f, 0.0f);
}

// The ball of the attack: bigger when the reef is up.
void grPiratesAttackAground::setAttack() {
    if (m_yakumono != NULL) {
        soCollisionAttackData attack(1.0f);
        if (unk19B == 1) {
            if (*m_stateWork == unk199) {
                return;
            }
            disableAttack(0);
            unk19B = 0;
        }
        float size;
        if (*m_stateWork == 0) {
            size = 7.0f;
        } else if (*m_stateWork == 1) {
            size = 15.0f;
        }
        Vec3f offset(0.0f, 0.0f, 0.0f);
        setAttackGimmickDetails(&attack, size, 1.0f, 1.0f, 1.0f,
            10, &offset, 70, 100, 70, 0,
            m_nodeIndex, 0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Small,
            soCollisionAttackData::Sound_Attribute_Punch,
            false, false, false, false, false, false, 0, 30,
            false, true, true, soCollisionAttackData::Lr_Check_Forward,
            false, true, true, true, false, soCollisionAttackData::Region_None, false);
        m_yakumono->setAttack(0, 0, &attack);
        unk19B = 1;
    }
}
