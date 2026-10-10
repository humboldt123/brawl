#include <gf/gf_model.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_village/gr_village.h>

grVillageSky* grVillageSky::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageSky* ground = new (Heaps::StageInstance) grVillageSky(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageSky::~grVillageSky() {
}

void grVillageSky::update(float deltaFrame) {
    grVillage::update(deltaFrame);
    if (m_isUpdate) {
    }
}

// The sky and the clouds get the colors of the time of day on their two tev colors each.
void grVillageSky::changeColor() {
    if (m_scene != *m_sceneWork) {
        nw4r::g3d::ResMatTevColor tevA;
        nw4r::g3d::ResMatTevColor tevB;
        nw4r::g3d::ResMat matA;
        nw4r::g3d::ResMat matB;
        nw4r::g3d::ScnMdl* scnMdl = *m_sceneModels;
        if (scnMdl != NULL) {
            nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
            if (model.IsValid()) {
                matA = model.GetResMat("enkeiSky");
                if (matA.IsValid()) {
                    matB = model.GetResMat("enkeiCloud");
                    if (matB.IsValid()) {
                        nw4r::g3d::ScnMdl::CopiedMatAccess accessA(scnMdl, matA.ptr()->m_id);
                        tevA = accessA.GetResMatTevColor(false);
                        if (tevA.IsValid()) {
                            nw4r::g3d::ScnMdl::CopiedMatAccess accessB(scnMdl, matB.ptr()->m_id);
                            tevB = accessB.GetResMatTevColor(false);
                            if (tevB.IsValid()) {
                                m_scene = *m_sceneWork;
                                switch (m_scene) {
                                case 0: {
                                    GXColor color0 = { 0x30, 0x36, 0x66, 0xFF };
                                    tevA.GXSetTevColor(GX_TEVREG0, color0);
                                    GXColor color1 = { 0x00, 0x00, 0x32, 0xFF };
                                    tevA.GXSetTevColor(GX_TEVREG1, color1);
                                    GXColor color2 = { 0x00, 0x3C, 0x73, 0xFF };
                                    tevB.GXSetTevColor(GX_TEVREG0, color2);
                                    GXColor color3 = { 0x80, 0x80, 0x80, 0xFF };
                                    tevB.GXSetTevColor(GX_TEVREG1, color3);
                                    break;
                                }
                                case 1: {
                                    GXColor color0 = { 0x00, 0x00, 0x78, 0xFF };
                                    tevA.GXSetTevColor(GX_TEVREG0, color0);
                                    GXColor color1 = { 0x00, 0x00, 0x1E, 0xFF };
                                    tevA.GXSetTevColor(GX_TEVREG1, color1);
                                    GXColor color2 = { 0x00, 0x32, 0x80, 0xFF };
                                    tevB.GXSetTevColor(GX_TEVREG0, color2);
                                    GXColor color3 = { 0x80, 0x80, 0x80, 0xFF };
                                    tevB.GXSetTevColor(GX_TEVREG1, color3);
                                    break;
                                }
                                case 2: {
                                    GXColor color0 = { 0x7D, 0x44, 0x2D, 0xFF };
                                    tevA.GXSetTevColor(GX_TEVREG0, color0);
                                    GXColor color1 = { 0x14, 0x32, 0x6A, 0xFF };
                                    tevA.GXSetTevColor(GX_TEVREG1, color1);
                                    GXColor color2 = { 0x1A, 0x1D, 0x4F, 0xFF };
                                    tevB.GXSetTevColor(GX_TEVREG0, color2);
                                    GXColor color3 = { 0x74, 0x6E, 0x5A, 0xFF };
                                    tevB.GXSetTevColor(GX_TEVREG1, color3);
                                    break;
                                }
                                case 3: {
                                    GXColor color0 = { 0x00, 0x1E, 0x41, 0xFF };
                                    tevA.GXSetTevColor(GX_TEVREG0, color0);
                                    GXColor color1 = { 0x00, 0x00, 0x0F, 0xFF };
                                    tevA.GXSetTevColor(GX_TEVREG1, color1);
                                    GXColor color2 = { 0x00, 0x1E, 0x55, 0xFF };
                                    tevB.GXSetTevColor(GX_TEVREG0, color2);
                                    GXColor color3 = { 0x46, 0x50, 0x78, 0xFF };
                                    tevB.GXSetTevColor(GX_TEVREG1, color3);
                                    break;
                                }
                                case 4: {
                                    GXColor color0 = { 0x00, 0x17, 0x32, 0xFF };
                                    tevA.GXSetTevColor(GX_TEVREG0, color0);
                                    GXColor color1 = { 0x00, 0x00, 0x0F, 0xFF };
                                    tevA.GXSetTevColor(GX_TEVREG1, color1);
                                    GXColor color2 = { 0x00, 0x1E, 0x55, 0xFF };
                                    tevB.GXSetTevColor(GX_TEVREG0, color2);
                                    GXColor color3 = { 0x46, 0x50, 0x78, 0xFF };
                                    tevB.GXSetTevColor(GX_TEVREG1, color3);
                                    break;
                                }
                                }
                                matA.DCStore(false);
                                matB.DCStore(false);
                            }
                        }
                    }
                }
            }
        }
    }
}
