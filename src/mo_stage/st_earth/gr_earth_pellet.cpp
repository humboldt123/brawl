#include <gr/gr_calc_world_callback.h>
#include <it/item.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_earth/gr_earth.h>

// Matrix functions of the main binary without names
// Matrix::setTrans (sets the translation of a matrix to the three numbers)
extern "C" void fn_8003F03C(Matrix* mtx, float x, float y, float z);
// Matrix::getScalingFactor
extern "C" void fn_8003E6DC(const Matrix* mtx, Vec3f* scale);
// the matrix of a translation
extern "C" void fn_8003F074(Matrix* mtx, float x, float y, float z);
// the matrix of a rotation of the three angles
extern "C" void fn_8003EA50(Matrix* mtx, const Vec3f* angles);
// the matrix scaled by the three factors
extern "C" void fn_8003E828(const Matrix* mtx, const Vec3f* scale, Matrix* out);

// HYPOTHESIS: two small constant objects (an 0xFF marker with an index) that the unit's headers instantiate.
struct grEarthPelletMarker {
    int m_a;
    int m_b;
    grEarthPelletMarker(int a, int b) : m_a(a), m_b(b) { }
};
static grEarthPelletMarker sMarkerA(0xFF, 0);
static grEarthPelletMarker sMarkerB(0xFF, 1);

grEarthPellet* grEarthPellet::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthPellet* ground = new (Heaps::StageInstance) grEarthPellet(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthPellet::~grEarthPellet() {
}

void grEarthPellet::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateState(deltaFrame);
        updatePos();
        updateMotion();
        updateCallBack();
        m_hasUpdatedG3dCalcWorld = false;
        updateG3dProcCalcWorld();
    }
}

// The place of the pellet follows the data of the stage (it is lowered by one) and keeps the size of it only while it is
// ripe (the state 0x13); then it is turned around.
void grEarthPellet::updatePos() {
    stEarthPelletData* pellet = m_pelletData;
    if (pellet != NULL) {
        Matrix mtx = pellet->m_mtx;
        Matrix offset;
        fn_8003F03C(&offset, 0.0f, -1.0f, 0.0f);
        mtx.mul(&offset, &mtx);
        Vec3f pos;
        pos.m_x = mtx.m[0][3];
        pos.m_y = mtx.m[1][3];
        pos.m_z = mtx.m[2][3];
        Vec3f rot;
        mtx.getRotate(&rot);
        Vec3f scale;
        fn_8003E6DC(&mtx, &scale);
        mtx.setIdentity();
        fn_8003F074(&mtx, pos.m_x, pos.m_y, pos.m_z);
        fn_8003EA50(&mtx, &rot);
        if (m_state != 0x13) {
            scale.m_x = 1.0f;
            scale.m_y = 1.0f;
            scale.m_z = 1.0f;
        }
        fn_8003E828(&mtx, &scale, &mtx);
        mtx.rotY(3.1415927f);
        m_mtx = mtx;
    }
}

// The steps of the pellet: 0 is hidden, 1 waits for the stage to make it (the state 2), 0x13 shows it and grows it (the ripe
// state 0 - 3), 0x19 makes the item of the color and the ripeness.
void grEarthPellet::updateState(float deltaFrame) {
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    if (m_pelletData != NULL && data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0x13:
            if (m_timer == 0.0f) {
                stEarthPelletData* pellet = m_pelletData;
                if (pellet->unk0B == 1) {
                    m_timer = data->unk2C;
                    pellet->unk0B = 2;
                    m_pelletData->unk08 = 2;
                } else if (pellet->unk0B == 0) {
                    m_timer = data->unk28;
                    pellet->unk0B = 1;
                    m_pelletData->unk08 = 2;
                }
            }
            if (m_pelletData->unk08 == 4) {
                m_state = 0x19;
            }
            break;
        case 1:
            if (m_pelletData->m_state == 2) {
                changeColor();
                setVisibility(1);
                m_timer = data->unk24;
                m_state = 0x13;
            }
            break;
        case 0:
            setVisibility(0);
            m_state = 1;
            break;
        case 0x19: {
            Vec3f pos;
            pos.m_z = m_mtx.m[2][3];
            m_state = 0;
            u32 variation = 0;
            pos.m_y = m_mtx.m[1][3];
            pos.m_x = m_mtx.m[0][3];
            stEarthPelletData* pellet = m_pelletData;
            switch (pellet->m_color) {
            case 1:
                switch (pellet->unk0B) {
                case 2:
                    variation = 7;
                    break;
                case 0:
                    variation = 1;
                    break;
                case 1:
                    variation = 4;
                    break;
                case 3:
                    variation = 10;
                    break;
                }
                break;
            case 0:
                switch (pellet->unk0B) {
                case 2:
                    variation = 6;
                    break;
                case 0:
                    variation = 0;
                    break;
                case 1:
                    variation = 3;
                    break;
                case 3:
                    variation = 9;
                    break;
                }
                break;
            case 2:
                switch (pellet->unk0B) {
                case 2:
                    variation = 8;
                    break;
                case 0:
                    variation = 2;
                    break;
                case 1:
                    variation = 5;
                    break;
                case 3:
                    variation = 0xB;
                    break;
                }
                break;
            }
            itManager* items = itManager::getInstance();
            if (items != NULL) {
                BaseItem* item = items->createItem(Item_Stage_Pellet, variation, -1, NULL, 0, 0xFFFF, 0, 0xFFFF);
                if (item != NULL) {
                    item->warp(&pos);
                }
            }
            break;
        }
        }
    }
}

// The pellet shows the frame of the animation that is its ripeness (the animation does not run).
void grEarthPellet::updateMotion() {
    if (m_sceneModels[0] != NULL) {
        gfModelAnimation* modelAnim = *m_modelAnims;
        if (modelAnim != NULL && m_sceneModels[0]->m_resMdl.IsValid()) {
            float frame;
            switch (m_pelletData->unk0B) {
            case 2:
                frame = 2.0f;
                break;
            case 0:
                frame = 0.0f;
                break;
            case 1:
                frame = 1.0f;
                break;
            case 3:
                frame = 3.0f;
                break;
            default:
                frame = 0.0f;
                break;
            }
            modelAnim->setFrame(frame);
            modelAnim->setUpdateRate(0.0);
        }
    }
}

void grEarthPellet::updateCallBack() {
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
            calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = m_mtx;
        }
    }
}

// The color of the pellet (red, blue or yellow) is set on the material of the model.
void grEarthPellet::changeColor() {
    nw4r::g3d::ResMat mat;
    nw4r::g3d::ResMatTevColor tevColor;
    GXColor color = { 0xFF, 0xFF, 0xFF, 0xFF };
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
    if (scnMdl != NULL) {
        nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
        if (model.IsValid()) {
            mat = model.GetResMat("Pellet00_pellet");
            if (mat.IsValid()) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
                tevColor = access.GetResMatTevColor(false);
                if (tevColor.IsValid()) {
                    u8 kind = m_pelletData->m_color;
                    if (kind == 1) {
                        color.r = 0x00;
                        color.g = 0x00;
                        color.b = 0x80;
                        color.a = 0xFF;
                    } else if (kind == 0) {
                        color.r = 0x80;
                        color.g = 0x00;
                        color.b = 0x00;
                        color.a = 0xFF;
                    } else if (kind < 3) {
                        color.r = 0x80;
                        color.g = 0x64;
                        color.b = 0x00;
                        color.a = 0xFF;
                    }
                    tevColor.GXSetTevColor(GX_TEVREG1, color);
                    tevColor.GXSetTevColor(GX_TEVREG2, color);
                    tevColor.DCStore(false);
                    mat.DCStore(false);
                }
            }
        }
    }
}

