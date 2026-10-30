#pragma once

// Shadows BrawlHeaders so/model/so_model_module_impl.h: restores the scale argument between node setup and extension
// data, and carries the member layout recovered from the sora_melee unit (src/mo_melee/sora_melee/so/model/
// so_model_module_impl.cpp). Offsets were read from the constructor, destructor and every accessor of the unit; names
// without a caller, vtable or string behind them are marked HYPOTHESIS.

#include <StaticAssert.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_list.h>
#include <so/model/so_model_module_simple.h>
#include <so/model/so_model_virtual_node.h>
#include <so/model/so_model_calc_world_callback.h>
#include <so/model/so_model_scn_obj_callback.h>
#include <so/model/so_model_scene_root_event.h>
#include <so/event/so_event_presenter.h>
#include <so/model/so_model_event_presenter.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <types.h>

class soModuleAccesser;
class soResourceModule;

class soModelModuleImpl : public soModelModuleSimple, public soAnimCmdEventObserver, public soEventPresenter<soModelEventObserver> {
public:
    // Node ids below the model's node count are model nodes, ids up to ExtendNodeIdBase are virtual nodes, ids from
    // ExtendNodeIdBase on are looked up in m_extendNodeTbl (soModelModuleImplVariable remaps ids from 400 on).
    enum { ExtendNodeIdBase = 300 };

    // HYPOTHESIS: how far the node matrices are built this frame (m_setUpState).
    enum SetUpState {
        SetUpState_None = 0,
        SetUpState_Begin = 1,
        SetUpState_Final = 2
    };

    // HYPOTHESIS: where the ScnMdl came from (m_instanceKind, the fourth constructInstance argument).
    enum InstanceKind {
        InstanceKind_None = 0,
        InstanceKind_Resource = 1, // built from the resource module's model file (with shadow and loupe models)
        InstanceKind_Default = 2, // built from the built-in default model resource
        InstanceKind_Extern = 3 // setScnMdlExtern: owned by someone else
    };

    // The record detachScnMdl fills and setScnMdl reads back when a model instance moves between modules.
    // HYPOTHESIS: the name; the fields are the instance data the module owns besides the ScnMdl itself.
    struct InstanceData {
        s8* m_nodeSetUpFlags;
        Vec3f* m_nodeScales;
        void* m_shadowNode;
        nw4r::g3d::ScnMdl* m_shadowScnMdl;
        bool m_isAddEnvMapMat;
    };

    soModelModuleCalcWorldCallback m_calcWorldCallback; // 0x2C
    soModelModuleScnObjCallback m_scnObjCallback; // 0x34
    nw4r::ut::List m_nodeSetUpList; // 0x3C: soModelNodeSetUp records sorted by node id, free records at the tail
    soModelNodeSetUp* m_nodeSetUpCursor; // 0x48: HYPOTHESIS: the record the calc-world callback expects next
    float m_scale; // 0x4C
    float m_scaleZ; // 0x50
    nw4r::math::MTX34 m_viewMtx; // 0x54: HYPOTHESIS: the view matrix the ScnObj callback saw this frame
    u32 m_nextSetUpNodeId; // 0x84: HYPOTHESIS: the node the calc-world callback continues with (-1: none)
    u32 m_transNNodeId; // 0x88 (-1: look the node up by name)
    u32* m_extendNodeTbl; // 0x8C
    soArray<soModelVirtualNode>* m_virtualNodes; // 0x90
    u32 unk94; // 0x94
    soModelSceneRootEvent m_sceneRootEvent; // 0x98
    s8* m_nodeSetUpFlags; // 0xA8: HYPOTHESIS: one mark per node, set once its matrix was built this frame
    Vec3f* m_nodeScales; // 0xAC: HYPOTHESIS: one scale per node, applied by the calc-world callback
    void* m_shadowNode; // 0xB0: the shadow node the instance was built with (constructInstance's void* argument)
    nw4r::g3d::ScnMdl* m_shadowScnMdl; // 0xB4
    bool m_isAddEnvMapMat; // 0xB8
    int m_shadowNodeOverride; // 0xBC: HYPOTHESIS: setShadowNode's value, used instead of the argument when set
    s16 unkC0; // 0xC0: set to 1 once the ScnMdl is constructed
    s8 m_setUpState; // 0xC2: SetUpState
    u8 m_instanceKind; // 0xC3: InstanceKind
    u8 m_loupeNo; // 0xC4 (0xFF: none)
    u8 m_isEnvMap : 1; // 0xC5 (0x80)
    u8 m_isLockMtx : 1; // 0x40
    u8 m_isRefFB : 1; // 0x20
    u8 m_isDisposeInstanceImm : 1; // 0x10
    soModuleAccesser* m_moduleAccesser; // 0xC8

    soModelModuleImpl(soModuleAccesser* acc,
                      soArray<soModelNodeSetUp>* nodeSetUps,
                      float modelScale,
                      void* extendNodeTbl,
                      soArray<soModelVirtualNode>* virtualNodes,
                      soEventObserverRegistrationDesc* regDesc);
    // TODO: Verify Params
    virtual void setUpMtx(u32 nodeId);
    virtual void setUpMtx(u32* nodeIds, s32 numNodeIds);
    virtual void setUpMtxFinal();
    virtual void setUpSkinningMtx();
    virtual void clearSetUpMtx();
#ifdef EM_ENEMY_BUILDER
    // The Enemy unit passes the resource file as a plain pointer (0), the fourth argument unextended and mdlResId as
    // 0xFFFF. HYPOTHESIS: these are the original parameter types; the other units keep the shared declaration.
    virtual void constructInstance(gfSceneRoot::LayerType, u32 flags, bool, u8, void* resFile, const char*, const char*, u32 mdlResId, void*, u8 loupeNo,
                                   int priorityDrawXlu);
#else
    virtual void constructInstance(gfSceneRoot::LayerType, u32 flags, bool, s8, nw4r::g3d::ResFile, const char*, const char*, s16 mdlResId, void*, u8 loupeNo, int
                                   priorityDrawXlu);
#endif
    virtual void destructInstance();
    virtual void activate();
    virtual void deactivate();
    virtual void begin(bool);
    virtual nw4r::g3d::ScnMdl* getScnMdl(bool) const;
    virtual void setScnMdl(nw4r::g3d::ScnMdl*, void*, gfSceneRoot::LayerType);
    virtual void setScnMdlExtern(nw4r::g3d::ScnMdl *);
    virtual nw4r::g3d::ScnMdl* detachScnMdl(void*);
    virtual void setLockMtx(bool);
    virtual void clearNodeSetUp();
    virtual void clearNodeSRT(u32 p1);
    virtual void setNodeSRT(u32 , Vec3f* scale, Vec3f* rot, Vec3f* xlate);
    virtual Vec3f getNodeScale(u32 nodeIndex);
    virtual void setNodeScale(u32 , Vec3f* );
    virtual Vec3f getNodeRotate(u32 nodeIndex);
    virtual void setNodeRotateX(u32 , float x);
    virtual void setNodeRotateY(u32 , float y);
    virtual void setNodeRotateZ(u32 , float z);
    virtual void setNodeRotate(u32 , Vec3f* rot);
    virtual Vec3f getNodeTranslate(u32 p1);
    virtual void setNodeTranslate(u32 , Vec3f* xlate);
    virtual int getCorrectNodeId(u32);
    virtual Vec3f getNodeGlobalPosition(u32 correctNodeId, bool );
    virtual Vec3f getNodeGlobalPosition(u32 correctNodeId, const Vec3f* p2, bool, bool );
    virtual Vec3f getNodeLocalPosition(u32 correctNodeId, bool );
    virtual void getNodeLocalMtxFromNode(u32 node1, u32 node2, Matrix* outMtx, bool );
    virtual void getNodeGlobalMtx(u32 index, Matrix* outMtx, bool ) const;
    virtual void setNodeGlobalMtx(u32 index, Matrix* , bool );
    virtual Vec3f getNodeGlobalRotation(u32 nodeIndex, bool );
    virtual bool isNode(u32 id);
    virtual u32 getNodeNum(bool) const;
    virtual void setTransNNodeId(int);
    virtual const char *getTransNNodeName();
    virtual int getTransNNodeId();
    virtual Vec3f getResFileNodeTranslate(u32 nodeIndex);
    virtual Vec3f getResFileNodeRotation(u32 nodeIndex);
    virtual void renderNodeAxis(u32 nodeIndex, float );
    virtual float getScale();
    virtual void setScale(float);
    virtual void setScaleZ(float);
    virtual gfSceneRoot::LayerType getLayerType();
    virtual s8 getLoupeNo();
    virtual void setLoupeNo(s8);
    virtual void setVisibility(bool visibility, bool);
    virtual void setNodeVisibility(u32 nodeId, bool visibility, bool);
    virtual void setNodeVisibility(s32 numIds, const u32* nodeIds, bool visibility, bool);
    virtual void changeMaterialTex(soResourceModule*, const char*resMat, const char*resPlt, nw4r::g3d::ResFile);
    virtual void initVirtualNode(int, u32, u32, void*, Matrix*);
    virtual void setVirtualNodeUpdate(u32 virtualNodeIndex, bool);
    virtual bool isAddEnvMapMat();
    virtual bool isEnvMap();
    virtual void switchEnvMap(bool);
    virtual bool isRefFB();
    virtual void preShadowCalcView();
    virtual Matrix *getWorldMtxArray();
    virtual void setShadowNode(int);
    virtual void setDisposeInstanceImm(bool);
    virtual ~soModelModuleImpl();

    virtual bool isObserv(char unk1);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);

    // Callbacks of the nw4r scene objects (soModelModuleCalcWorldCallback, soModelModuleScnObjCallback) and the scene
    // root event. HYPOTHESIS: the names.
    u32 calcWorldCallbackA(nw4r::g3d::ChrAnmResult* result, u16 nodeId);
    u32 calcWorldCallbackB(nw4r::g3d::WorldMtxManip* manip, u16 nodeId);
    void calcViewCallbackPre(nw4r::math::MTX34* viewMtx);
    void calcViewCallbackPost(nw4r::math::MTX34* viewMtx);

private:
    // HYPOTHESIS: the names of the two helpers constructInstance and the setUpMtx family call out of line.
    void constructScnMdl(soResourceModule* resource, gfSceneRoot::LayerType lt, u32 flags, nw4r::g3d::ResFile resFile,
                         const char* refFBTexName, const char* envMapMatName, s16 mdlResId);
    void setUpConstraintNodeMtx();

    bool isVirtualNodeId(u32 nodeId) const;
    soModelNodeSetUp* findNodeSetUp(u32 nodeId);
    soModelNodeSetUp* getNodeSetUp(u32 nodeId);
};
static_assert(sizeof(soModelModuleImpl) == 204, "Class is wrong size!");

class soModelModuleImplVariable : public soModelModuleImpl {
    u32** unkCC;
public:
    soModelModuleImplVariable(soModuleAccesser* acc,
                              soArray<soModelNodeSetUp>* nodeSetUps,
                              float modelScale,
                              void* extendNodeTbl,
                              soArray<soModelVirtualNode>* virtualNodes,
                              soEventObserverRegistrationDesc* regDesc);
    virtual void setUpMtx(u32* nodeIds, s32 numNodeIds);
    virtual void clearNodeSRT(u32 p1);
    virtual void setNodeSRT(u32 , Vec3f* scale, Vec3f* rot, Vec3f* xlate);
    virtual Vec3f getNodeScale(u32 nodeIndex);
    virtual void setNodeScale(u32 , Vec3f* );
    virtual Vec3f getNodeRotate(u32 nodeIndex);
    virtual void setNodeRotateX(u32 , float x);
    virtual void setNodeRotateY(u32 , float y);
    virtual void setNodeRotateZ(u32 , float z);
    virtual void setNodeRotate(u32 , Vec3f* rot);
    virtual Vec3f getNodeTranslate(u32 p1);
    virtual void setNodeTranslate(u32 , Vec3f* xlate);
    virtual int getNodeId(const char *nodeName);
    virtual int getCorrectNodeId(u32);
    virtual int getRealNodeId(u32) const;
    virtual const char* getNodeName(u32 p1);
    virtual Vec3f getNodeGlobalPosition(u32 correctNodeId, bool );
    virtual Vec3f getNodeGlobalPosition(u32 correctNodeId, const Vec3f* p2, bool, bool );
    virtual Vec3f getNodeLocalPosition(u32 correctNodeId, bool );
    virtual void getNodeLocalMtxFromNode(u32 node1, u32 node2, Matrix* outMtx, bool );
    virtual void getNodeGlobalMtx(u32 index, Matrix* outMtx, bool ) const;
    virtual void setNodeGlobalMtx(u32 index, Matrix* , bool );
    virtual Vec3f getNodeGlobalRotation(u32 nodeIndex, bool );
    virtual bool isNode(u32 id);
    virtual Vec3f getResFileNodeTranslate(u32 nodeIndex);
    virtual Vec3f getResFileNodeRotation(u32 nodeIndex);
    virtual Vec3f getNodeGlobalOffsetFromTop(u32 p1);
    virtual Vec3f getTopNodeGlobalPosFromNode(u32 p1, Vec3f* p2);
    virtual void renderNodeAxis(u32 nodeIndex, float );
    virtual void setNodeVisibility(u32 nodeId, bool visibility, bool);
    virtual void setNodeVisibility(s32 numIds, const u32* nodeIds, bool visibility, bool);
    virtual void setNodeConvertData(u32** convertData);
    virtual ~soModelModuleImplVariable();
};
static_assert(sizeof(soModelModuleImplVariable) == 0xD0, "Class is wrong size!");
