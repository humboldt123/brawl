// Brawl physics wrapper translation unit ph_shape.o (main.dol 0x80095FC4-0x800962E0).
// Not yet decompiled. Functions in address order with their map names:
//   0x80095FC4   132  __ct   [map: phShape____ct]
//   0x80096048   128  __dt   [map: phShape____dt]
//   0x800960C8   164  getMatrixPosture   [map: phShape__getMatrixPosture]
//   0x8009616C   372  draw   [map: phShape__draw]
#include <ph/ph_shape.h>

#include <havok/hkEntity.h>
#include <havok/hkMemory.h>
#include <havok/hkWorld.h>
#include <revolution/GX.h>
#include <havok/hkRotation.h>
#include <havok/hkQuaternion.h>
#include <havok/hkVector4.h>

// Local declaration (MARKED): the havok header defines the hkRigidBody flag constructor inline, which would
// inline the base constructor into phShape. The target calls the out-of-line constructor (hkRigidBody____ct).
struct hkRigidBody : hkEntity {
    hkRigidBody(int flag);
};

// HYPOTHESIS: source object of getMatrixPosture; float blocks at 0xE0 and 0x120.
struct phPostureSource {
    u8 unk00[0xE0];
    hkReal m_e0[4];
    u8 unkF0[0x30];
    hkReal m_120[4];
};

// HYPOTHESIS: the target writes the composed matrix through the shape's own address (fn_80283E28 = hkMatrix4::set).
struct hkMatrix4 {
    void set(const hkRotation& r);
};

// Same allocation as HK_DECLARE_REF_ALLOCATOR(0x2B) for hkRigidBody (size and class id from the target).
static hkRigidBody* allocRigidBody() {
    hkReferencedObject* p = (hkReferencedObject*)hkMemory::getInstance().allocateChunk(0x1F0, 0x2B);
    p->m_memSizeAndFlags = 0x1F0;
    return (hkRigidBody*)p;
}

phShape::phShape(int flag) {
    m_rigidBody = NULL;
    hkRigidBody* rb = allocRigidBody();
    if (rb != NULL) {
        rb = new (rb) hkRigidBody(flag);
    }
    m_rigidBody = rb;
}

phShape::~phShape() {
    if (m_rigidBody != NULL) {
        hkWorld* world = m_rigidBody->m_world;
        if (world != NULL) {
            world->removeEntity(m_rigidBody);
        }
        m_rigidBody = NULL;
    }
}

void phShape::getMatrixPosture(phPostureSource* const* src) {
    phPostureSource* p = *src;
    hkQuaternion q(p->m_120[0], p->m_120[1], p->m_120[2], p->m_120[3]);
    hkQuaternion unused(p->m_e0[0], p->m_e0[1], p->m_e0[2], p->m_e0[3]);
    hkRotation rot;
    rot.set(q);
    ((hkMatrix4*)this)->set(rot);
}

// Unnamed helpers called from draw (names not recovered yet).
extern "C" void fn_801F471C(int);
extern "C" void fn_801F4748(int);
// Fog colour and parameters (data not recovered yet).
extern u8 lbl_805A1D08;
extern u8 lbl_805A1D09;
extern u8 lbl_805A1D0A;
extern u8 lbl_805A1D0B;
extern float lbl_805A1D0C;
extern float lbl_805A1D10;
extern float lbl_805A1D14;

void G3DState_Invalidate(unsigned long flags);

void phShape::draw() {
    G3DState_Invalidate(0x7FF);
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    fn_801F471C(1);
    fn_801F4748(0);
    GXSetBlendMode(GX_BM_BLEND, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)5);
    GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_GREATER, 0);
    GXSetZMode(1, GX_LEQUAL, 1);
    GXSetZCompLoc(0);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, (GXTevMode)4);
    GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumChans(1);
    GXSetCullMode(GX_CULL_NONE);
    GXColor fogColor;
    fogColor.r = lbl_805A1D08;
    fogColor.g = lbl_805A1D09;
    fogColor.b = lbl_805A1D0A;
    fogColor.a = lbl_805A1D0B;
    GXSetFog(GX_FOG_NONE, fogColor, lbl_805A1D0C, lbl_805A1D10, lbl_805A1D14, lbl_805A1D10);
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetLineWidth(6, 5);
}
