#include <types.h>
#include <cm/cm_subject.h>
#include <gf/gf_camera.h>
#include <gf/gf_loupe.h>
#include <mt/mt_coll2d.h>
#include <nw4r/g3d/g3d_restex.h>
#include <nw4r/ut.h>
#include <revolution/GX.h>
#include <revolution/OS.h>

extern "C" void GXInitTexObjWrapMode(GXTexObj* obj, GXTexWrapMode wrapS, GXTexWrapMode wrapT);
extern "C" void fn_8001AA88(GXTexObj* texObj, nw4r::g3d::ResTex tex);
extern "C" void fn_8002503C(clSegment2D* segment, Vec2f* out, float width, float height);
extern nw4r::ut::LinkList<cmSubject, 0> lbl_8049DEDC;

// HYPOTHESIS: the background texture object, the per-loupe states and the 4x4 RGBA8 colour grabs.
GXTexObj lbl_80494DE0;
// HYPOTHESIS: untyped placeholder; only its first byte is cleared by initialize.
unsigned char lbl_80494E00[0xe8] __attribute__((aligned(8))) = { 0 };
u32 lbl_805A0010;
u32 lbl_805A0014;
float lbl_805A0018;
GXTexObj* lbl_805A001C;
nw4r::g3d::ResTex lbl_805A0020;
gfLoupe::LoupeInfo lbl_80494EE8[7];
u8 lbl_804951C0[7][0x40] __attribute__((aligned(32)));
u8 lbl_805A0024[7];

void gfLoupe::initialize() {
    LoupeInfo* info = lbl_80494EE8;
    for (int i = 0; i < 7; i++, info++) {
        for (int j = 0; j < 8; j++) {
            info->m_objInfo[j].m_layer = gfSceneRoot::Layer_Ground;
            info->m_objInfo[j].m_scnObj = NULL;
        }
        info->m_enable = false;
        info->m_subject = NULL;
        info->m_image = NULL;
        info->m_bgScrollU = 0.0f;
        info->m_bgScrollV = 0.0f;
        info->m_center.m_x = 0.0f;
        info->m_center.m_y = 0.0f;
        info->m_center.m_z = 0.0f;
        info->m_radius = 0.0f;
    }
    lbl_805A0010 = 0x40;
    lbl_805A0014 = 0x40;
    lbl_805A0018 = 0.05f;
    lbl_80494E00[0] = 0;
}

void gfLoupe::setup() {
    LoupeInfo* infos = lbl_80494EE8;
    for (int i = 0; i < 7; i++) {
        infos[i].m_subject = NULL;
    }
    for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = lbl_8049DEDC.GetBeginIter(); it != lbl_8049DEDC.GetEndIter(); ++it) {
        if (it->m_isLoupeTarget) {
            u8 kind = it->m_stateB;
            if (kind == 2 || kind == 3) {
                u32 no = it->m_playerNo;
                if (no < 7) {
                    infos[no].m_subject = &*it;
                }
            }
        }
    }
    for (int i = 0; i < 7; i++) {
        LoupeInfo* info = &lbl_80494EE8[i];
        cmSubject* subject = info->m_subject;
        if (subject == NULL) {
            info->m_bgScrollU = 0.0f;
            info->m_bgScrollV = 0.0f;
        } else {
            float speed = lbl_805A0018;
            // HYPOTHESIS: the subject's screen velocity at +0x60/+0x64, not declared in the cmSubject header.
            info->m_bgScrollU += speed * *(float*)&subject->_92[4];
            info->m_bgScrollV += -speed * *(float*)&subject->_92[8];
            if (info->m_bgScrollU >= 1.0f) {
                info->m_bgScrollU -= 1.0f;
            }
            if (info->m_bgScrollU < 0.0f) {
                info->m_bgScrollU += 1.0f;
            }
            if (info->m_bgScrollV >= 1.0f) {
                info->m_bgScrollV -= 1.0f;
            }
            if (info->m_bgScrollV < 0.0f) {
                info->m_bgScrollV += 1.0f;
            }
        }
    }
}

void gfLoupe::grabLoupeBGColor() {
    Vec3f* pPos;
    clSegment2D* pSegment;
    gfCamera* camera = gfCameraManager::getManager()->getCamera(0);
    float width = camera->m_viewportWidth;
    float height = camera->m_viewportHeight;
    u16 screenW = width;
    u16 screenH = height;
    Vec3f target;
    target = camera->m_targetPos;
    Vec2f center;
    camera->calcProjection3Dto2D(&target, &center);
    Vec3f pos;
    clSegment2D segment;
    // MATCH-ONLY: the original keeps the addresses of the two loop locals in registers.
    pPos = &pos;
    pSegment = &segment;
    for (u8 i = 0; i < 7; i++) {
        LoupeInfo* info = &lbl_80494EE8[i];
        if (info->m_subject != NULL) {
            // MATCH-ONLY: the original copies the vectors as whole words, not as float fields.
            ((u32*)pPos)[0] = ((u32*)&info->m_center)[0];
            ((u32*)pPos)[1] = ((u32*)&info->m_center)[1];
            ((u32*)pPos)[2] = ((u32*)&info->m_center)[2];
            Vec2f pos2D;
            camera->calcProjection3Dto2D(pPos, &pos2D);
            ((u32*)&pSegment->m_p0Pos)[0] = ((u32*)&center)[0];
            ((u32*)&pSegment->m_p0Pos)[1] = ((u32*)&center)[1];
            ((u32*)&pSegment->m_p1Pos)[0] = ((u32*)&pos2D)[0];
            ((u32*)&pSegment->m_p1Pos)[1] = ((u32*)&pos2D)[1];
            Vec2f edge;
            fn_8002503C(pSegment, &edge, width, height);
            s32 x = (u16)edge.m_x;
            s32 y = (u16)edge.m_y;
            if (y >= screenH) {
                y = screenH - 1;
            }
            if (y < 0) {
                y = 0;
            }
            if (x >= screenW) {
                x = screenW - 1;
            }
            if (x < 0) {
                x = 0;
            }
            x &= ~1;
            y &= ~1;
            if (x + 4 >= screenW) {
                x = screenW - 4;
            }
            if (y + 4 >= screenH) {
                y = screenH - 4;
            }
            GXSetTexCopySrc(x, y, 4, 4);
            GXSetTexCopyDst(4, 4, GX_TF_RGBA8, FALSE);
            GXCopyTex(lbl_804951C0[i], FALSE);
            lbl_805A0024[i] = 0;
        }
    }
}

GXColor gfLoupe::getBGColor(u8 index) {
    static const float weights[16] = {
        0.01f, 0.01f, 0.01f, 0.01f,
        0.01f, 0.22f, 0.22f, 0.01f,
        0.01f, 0.22f, 0.22f, 0.01f,
        0.01f, 0.0f, 0.01f, 0.01f,
    };
    u8* tex = lbl_804951C0[index];
    float a = 0.0f;
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    for (int i = 0; i < 16; i++) {
        float w = weights[i];
        a += w * tex[i * 2];
        r += w * tex[i * 2 + 1];
        g += w * tex[i * 2 + 0x20];
        b += w * tex[i * 2 + 0x21];
    }
    GXColor color;
    OSf32tou8(&a, &color.a);
    OSf32tou8(&r, &color.r);
    OSf32tou8(&g, &color.g);
    OSf32tou8(&b, &color.b);
    return color;
}

GXTexObj* gfLoupe::getBGTexture() {
    return lbl_805A001C;
}

u32 gfLoupe::getScnObjNum(u32 index) {
    return lbl_80494EE8[index].m_scnObjNum;
}

gfLoupe::ObjInfo* gfLoupe::getObjInfoArray(u32 index) {
    return lbl_80494EE8[index].m_objInfo;
}

Vec3f* gfLoupe::getLoupeCenter(u32 index) {
    return &lbl_80494EE8[index].m_center;
}

float gfLoupe::getLoupeRadius(u32 index) {
    return lbl_80494EE8[index].m_radius;
}

void* gfLoupe::getImage(u32 index) {
    return lbl_80494EE8[index].m_image;
}

void gfLoupe::setBGTexture(nw4r::g3d::ResTex tex) {
    lbl_805A0020 = tex;
    if (lbl_805A0020.ptr()) {
        lbl_805A001C = &lbl_80494DE0;
        fn_8001AA88(lbl_805A001C, lbl_805A0020);
        GXInitTexObjWrapMode(lbl_805A001C, GX_REPEAT, GX_REPEAT);
    } else {
        lbl_805A001C = NULL;
    }
}

void gfLoupe::setEnable(u8 index, bool enable) {
    lbl_80494EE8[index].m_enable = enable;
}

bool gfLoupe::isEnable(u32 index) {
    return lbl_80494EE8[index].m_enable;
}

void gfLoupe::setLoupeTextureSize(u32 width, u32 height) {
    lbl_805A0010 = width;
    lbl_805A0014 = height;
}

void gfLoupe::setTextureRef(u8 index, void* image) {
    lbl_80494EE8[index].m_image = image;
}

void gfLoupe::addPlayerLoupeScnObj(u32 index, gfSceneRoot::LayerType layer, nw4r::g3d::ScnObj* scnObj) {
    LoupeInfo* info = &lbl_80494EE8[index];
    ObjInfo* objInfo = &info->m_objInfo[info->m_scnObjNum];
    objInfo->m_layer = layer;
    objInfo->m_scnObj = scnObj;
    info->m_scnObjNum++;
}

void gfLoupe::removePlayerLoupeScnObj(u32 index, nw4r::g3d::ScnObj* scnObj) {
    LoupeInfo* info = &lbl_80494EE8[index];
    ObjInfo* objInfo = info->m_objInfo;
    u32 num = info->m_scnObjNum;
    for (u32 i = 0; i < num; i++) {
        if (objInfo[i].m_scnObj == scnObj) {
            for (int j = i; j < num - 1; j++) {
                objInfo[j] = objInfo[j + 1];
            }
            if (i < num - 1) {
                objInfo[num - 1].m_layer = gfSceneRoot::Layer_Ground;
                objInfo[num - 1].m_scnObj = NULL;
            }
            info->m_scnObjNum--;
            return;
        }
    }
}

void gfLoupe::setPlayerLoupeInfo(u32 index, Vec3f* center, float radius) {
    if (index < 7) {
        LoupeInfo* info = &lbl_80494EE8[index];
        info->m_center = *center;
        info->m_radius = radius;
    }
}
