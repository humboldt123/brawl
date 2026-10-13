#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#include <cm/cm_quake.h>
#include <ef/ef_id.h>
#include <gr/gr_calc_world_callback.h>
#include <mt/mt_prng.h>
#include <nw4r/math/math_triangular.h>
#include <st_pirates/gr_pirates_bomb.h>
#include <st_pirates/gr_pirates_util.h>
#include <st_pirates/st_pirates.h>
#include <yk/yk_no_hit_normal.h>

grPiratesBomb::grPiratesBomb(const char* taskName) : grPirates(taskName) {
    m_mtxWork = NULL;
    unk160 = 0.0f;
    m_mtxGimmickWork = NULL;
    m_matrixIndex = NULL;
    m_eventWork = NULL;
    m_fireWork = NULL;
    m_posTgtWork = NULL;
    unk17C.m_x = 0.0f;
    unk17C.m_y = 0.0f;
    unk17C.m_z = 0.0f;
    unk188.m_x = 0.0f;
    unk188.m_y = 0.0f;
    unk188.m_z = 0.0f;
    unk194.m_x = 0.0f;
    unk194.m_y = 0.0f;
    unk194.m_z = 0.0f;
    unk1A0 = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    unk1A1 = 0;
    unk1A2 = 0;
    m_work = NULL;
    unk1A8 = 0;
}

grPiratesBomb* grPiratesBomb::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesBomb* ground = new (Heaps::StageInstance) grPiratesBomb(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesBomb::~grPiratesBomb() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grPiratesBomb::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grPiratesBomb::updateYakumono(float deltaFrame) {
    if (unk1A1 != 1) {
        setHit();
        if (m_yakumono != NULL) {
            unk1A1 = 1;
        }
    }
}

// The flight of the bomb. Step 0 hides it, step 1 waits for the shot, then it flies to the target in an arc; when it lands it
// explodes (on the sea it splashes) and shakes the camera.
void grPiratesBomb::updateMove(float deltaFrame) {
    Vec3f flash;
    Vec3f muzzle;
    Vec3f dir;
    Vec3f seaPos;
    Vec3f path;
    stPiratesData* data = static_cast<stPiratesData*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
        case 0:
            setVisibility(0);
            disableAttack(0);
            unk1A2 = 0;
            m_state = 1;
            break;
        case 1: {
            if (*m_fireWork != 1) {
                break;
            }
            setVisibility(1);
            setAttack();
            float* target = m_posTgtWork;
            Matrix* cannon = m_mtxWork + (*m_matrixIndex + 8);
            float startZ = cannon->m[2][3];
            float startX = cannon->m[0][3] + 25.0f;
            float startY = cannon->m[1][3];
            unk188.m_y = startY;
            unk188.m_z = startZ;
            unk188.m_x = startX;
            unk17C.m_x = startX;
            unk17C.m_y = startY;
            unk17C.m_z = startZ;
            unk194.m_x = target[0];
            unk194.m_y = target[1];
            unk194.m_z = target[2];
            float rnd = randf();
            muzzle = unk188;
            muzzle.m_x -= 25.0f;
            float time = data->unk78 + (data->unk7C - data->unk78) * rnd;
            Vec3f toTarget = unk194 - muzzle;
            m_timer = time;
            unk160 = time;
            dir = toTarget;
            dir.normalize();
            dir.m_x *= 10.0f;
            dir.m_y *= 10.0f;
            dir.m_z *= 10.0f;
            Vec3f flashPos = muzzle + dir;
            flash = flashPos;
            g_ecMgr->setDrawPrio(1);
            g_ecMgr->setEffect(ef_ptc_stg_pirates_hougeki, &flash);
            g_ecMgr->setDrawPrio(-1);
            g_ecMgr->setDrawPrio(1);
            unk1A8 = g_ecMgr->setEffect(ef_ptc_stg_pirates_tamakiseki);
            g_ecMgr->setDrawPrio(-1);
            g_ecMgr->setParent(unk1A8, m_sceneModels[0], "DoukasenN", false);
            if (randf() < 0.5f) {
                m_sndGenerator.playSE(snd_se_stage_Pirates_09, 0, 0, -1);
            } else {
                m_sndGenerator.playSE(snd_se_stage_Pirates_10, 0, 0, -1);
            }
            Vec3f quake(0.0f, 0.0f, 0.0f);
            cmReqQuake(cmQuake::Amplitude_S, &quake);
            m_state = 2;
        }
        case 2:
            if (0.0f == m_timer) {
                if (unk1A8 != 0) {
                    g_ecMgr->endEffect(unk1A8);
                    unk1A8 = 0;
                }
                disableAttack(0);
                unk1A2 = 0;
                setAttackBreak();
                Matrix* sea = m_mtxGimmickWork + 6;
                seaPos.m_x = sea->m[0][3];
                seaPos.m_y = sea->m[1][3];
                seaPos.m_z = sea->m[2][3];
                if (m_posTgtWork[1] == seaPos.m_y) {
                    g_ecMgr->setEffect(ef_ptc_stg_pirates_mizu_shibuki, &unk17C);
                    if (randf() < 0.5f) {
                        m_sndGenerator.playSE(snd_se_stage_Pirates_11, 0, 0, -1);
                    } else {
                        m_sndGenerator.playSE(snd_se_stage_Pirates_12, 0, 0, -1);
                    }
                } else {
                    g_ecMgr->setEffect(ef_ptc_stg_pirates_bakuhatu, &unk17C);
                    if (randf() < 0.5f) {
                        m_sndGenerator.playSE(snd_se_stage_Pirates_13, 0, 0, -1);
                    } else {
                        m_sndGenerator.playSE(snd_se_stage_Pirates_14, 0, 0, -1);
                    }
                }
                Vec3f quake(0.0f, 0.0f, 0.0f);
                cmReqQuake(cmQuake::Amplitude_L, &quake);
                *m_fireWork = 2;
                m_state = 0;
            } else {
                float rate = 1.0f - m_timer / unk160;
                if (rate < 0.0f) {
                    rate = 0.0f;
                }
                if (rate > 1.0f) {
                    rate = 1.0f;
                }
                Vec3f diff = unk194 - unk188;
                float length = piratesLength(diff);
                path = diff;
                path.normalize();
                float distance = length * rate;
                path.m_x *= distance;
                path.m_y *= distance;
                path.m_z *= distance;
                int angle = (int)(rate * 32768.0f);
                Vec3f pos = unk188 + path;
                unk17C = pos;
                float height = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)angle)));
                unk17C.m_y += height * ((unk160 * 60.0f) / data->unk78);
            }
            break;
    }
}
// The model sits where the bomb is.
void grPiratesBomb::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos = unk17C;
            m_sndGenerator.setPos(&unk17C);
        }
    }
}

void grPiratesBomb::setHit() {
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

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

// The ball of the bomb in the air.
void grPiratesBomb::setAttack() {
    if (unk1A2 == 1) {
        return;
    }
    soCollisionAttackData attack(1.0f);
    Vec3f offset(0.0f, 0.0f, 0.0f);
    setAttackGimmickDetails(&attack, 15.0f, 1.0f, 1.0f, 1.0f,
        20, &offset, 361, 100, 0, 70,
        m_nodeIndex, 0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Small,
        soCollisionAttackData::Sound_Attribute_None,
        false, false, false, true, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Pos,
        false, false, false, false, false, soCollisionAttackData::Region_None, true);
    m_yakumono->setAttack(0, 0, &attack);
    unk1A2 = 1;
}

// The explosion where the bomb lands.
void grPiratesBomb::setAttackBreak() {
    if (unk1A2 == 1) {
        return;
    }
    soCollisionAttackData attack(1.0f);
    Vec3f offset(0.0f, 0.0f, 0.0f);
    setAttackGimmickDetails(&attack, 35.0f, 1.0f, 1.0f, 1.0f,
        35, &offset, 361, 100, 30, 80,
        m_nodeIndex, 0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Fire, soCollisionAttackData::Sound_Level_Large,
        soCollisionAttackData::Sound_Attribute_Fire,
        false, false, false, true, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Pos,
        false, false, false, false, false, soCollisionAttackData::Region_None, true);
    m_yakumono->setAttack(0, 0, &attack);
    unk1A2 = 1;
}
