// Local BrawlHeaders shadow: adds scene event, camera and resource animation declarations.
#pragma once

// Shadows include/Brawl/Include/gf/gf_3d_scene.h (adds setInfoCamResAnm / removeInfoCamResAnm).

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_light.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>
#include <ut/ut_list.h>

class gfSceneRootEvent;

namespace nw4r {
namespace g3d {
class ScnRoot;
class ScnGroup;
} // namespace g3d
} // namespace nw4r

class gfSceneRoot : public gfTask {
public:
    gfSceneRoot();
    static void create();
    static void drawFrameBufferReverse(int direction);

    enum LayerType {
        Layer_Ground = 0x0,
        Layer_Ground_And_Chara = 0x1,
        Layer_Chara = 0x2,
        Layer_All = 0x3,
        Layer_Fighter = 0x3,
        Layer_Stage_Object = 0x3,
        Layer_Ground2 = 0x3,
        Layer_Effect = 0x4,
        Layer_Effect_Fighter = 0x5,
        Layer_Special_Stage_Object = 0x6,
        Layer_Effect2D = 0x7,
        Layer_Info = 0x8,
    };

    union {
        char _64[20];
        struct {
            nw4r::g3d::ScnRoot* unk40;
            nw4r::g3d::ScnRoot* unk44;
            nw4r::g3d::ScnRoot* unk48;
            nw4r::g3d::ResAnmScnData* unk4C;
            u32 unk50;
        };
    };
    nw4r::g3d::AnmScnRes* m_anmScnRes;
    union {
        char _88[12];
        struct {
            u32 unk58;
            nw4r::g3d::AnmScnRes* unk5C;
            nw4r::g3d::AnmScnRes* unk60;
        };
    };
    union TransformFlag {
        u32 m_mask;
        struct {
            bool m_flag31 : 1;      // 0x80000000
            bool m_flag30 : 1;      // 0x40000000
            bool m_flag29 : 1;      // 0x20000000
            bool m_flag28 : 1;      // 0x10000000
            bool m_flag27 : 1;      // 0x8000000
            bool m_flag26 : 1;      // 0x4000000
            bool m_flag25 : 1;       // 0x2000000
            bool m_reverseLr : 1;       // 0x1000000
            bool m_flag23 : 1;       // 0x800000
            bool m_flag22 : 1;       // 0x400000
            bool m_flag21 : 1;       // 0x200000
            bool m_flag20 : 1;       // 0x100000
            bool m_flag19 : 1;       // 0x80000
            bool m_flag18 : 1;       // 0x40000
            bool m_flag17 : 1;       // 0x20000
            bool m_flag16 : 1;       // 0x10000
            bool m_flag15 : 1;      // 0x8000
            bool m_flag14 : 1;      // 0x4000
            bool m_flag13 : 1;      // 0x2000
            bool m_flag12 : 1;      // 0x1000
            bool m_flag11 : 1;      // 0x800
            bool m_flag10 : 1;      // 0x400
            bool m_flag9 : 1;       // 0x200
            bool m_flag8 : 1;       // 0x100
            bool m_flag7 : 1;       // 0x80
            bool m_flag6 : 1;       // 0x40
            bool m_flag5 : 1;       // 0x20
            bool m_flag4 : 1;       // 0x10
            bool m_flag3 : 1;       // 0x8
            bool m_flag2 : 1;       // 0x4
            bool m_flag1 : 1;       // 0x2
            bool m_flag0 : 1;       // 0x1
        };
    } m_transformFlag;

    // HYPOTHESIS: per-layer record (layout from the layer functions; the name is not from the map).
    struct Layer {
        Layer();
        ~Layer();
        u32 unk0;
        nw4r::g3d::ScnGroup* unk4;
        nw4r::g3d::AnmScnRes* unk8;
        u8 unkC;
        u8 unkD_7 : 1;
        u8 unkD_6 : 1;
        u8 unkD_4 : 2;
        u8 unkD_0 : 4;
        u16 unkE;
        u32 unk10;
        u32 unk14;
    };
    // HYPOTHESIS: light handle returned by allocLightHandle (slot index and a byte copied from the slot).
    struct LightHandle {
        u8 m_index;
        u8 m_unk1;
        // HYPOTHESIS: inline accessor
        bool isValid() const { return m_index != 0xFF; }
    };
    // HYPOTHESIS: per-light slot at 0x68 + i * 0xC (layout from allocLightHandle/releaseLightHandle).
    struct LightSlot {
        nw4r::g3d::LightObj* m_light;
        u8 m_used : 1;
        u8 m_released : 1;
        u8 m_flags : 6;
        u8 m_unk5;
        u8 m_unk6;
        u8 m_unk7;
        u8 m_unk8;
        u8 m_unk9;
        u8 m_unkA;
        u8 m_unkB;
    };
    union {
        char _104[1044];
        struct {
            union {
                char unk68[0x324];
                LightSlot m_lightSlots[10];
            };
            Layer unk38C[9];
            union {
                char unk464[0x18];
                struct {
                    u8 unk464_7 : 7;
                    u8 unk464_0 : 1;
                    u8 unk465_7 : 1;
                    u8 unk465_6 : 1;
                    u8 unk465_4 : 2;
                    u8 unk465_3 : 1; // HYPOTHESIS: cleared while a Final Smash camera animation plays (ftCaptainFinalSubRoutine)
                    u8 unk465_0 : 3;
                    u8 unk466[2];
                    utList unk468;
                    u8 unk474[8];
                };
            };
        };
    };
    float m_shadowPitch;
    float m_shadowYaw;
    float m_1156;
    char _1160[132];

    virtual void processAnim();
    virtual void processFixCamera();
    virtual void renderPre();
    virtual void renderOpa();
    virtual void renderXlu();
    virtual void renderDebug();
    virtual ~gfSceneRoot();

    void setCurrentFrame(float);
    void addEventNotify(gfSceneRootEvent* event);
    void removeEventNotify(gfSceneRootEvent* event);
    void setUseCameraAnim(bool useCameraAnim);
    void setClearDepthBufferState(LayerType layer, bool clear);
    void setResAnmScn(nw4r::g3d::ResAnmScn* anmScn, int heap);
    void removeResAnmScn();
    void setLayerDispStatus(LayerType layer, bool status);
    bool clear(LayerType layer);
    void setAnmScnResToScnRoot3D(nw4r::g3d::AnmScnRes* anmScn);
    void setFinalSceneAnm(nw4r::g3d::ResAnmScn* anmScn);
    void removeFinalSceneAnm();
    void setInfoCamResAnm(nw4r::g3d::ResAnmScn* anmScn);   // the camera of the Info layer (map: gfSceneRoot::setInfoCamResAnm)
    void removeInfoCamResAnm();
    nw4r::g3d::LightObj* getLightObj(u8 p1);
    void updateZoneLightSet(u8 p1, u8 p2);
    void setUseCopyFB(u8 p1);
    LightHandle allocLightHandle(nw4r::g3d::LightObj* light);
    void releaseLightHandle(LightHandle* handle);

    bool add(gfSceneRoot::LayerType lt, nw4r::g3d::ScnMdl* scnMdl);
    bool remove(nw4r::g3d::ScnMdl* scnMdl);
    void layerUpdateFrame(int layer);
    void resetScene();
    void blendLightColor(f32 ratio, nw4r::g3d::LightObj* from, nw4r::g3d::LightObj* to, nw4r::g3d::LightObj* out);
};
static_assert(sizeof(gfSceneRoot) == 1292, "Class is wrong size!");

extern gfSceneRoot* g_gfSceneRoot;
