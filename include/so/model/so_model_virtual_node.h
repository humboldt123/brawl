#pragma once

// SHADOW of BrawlHeaders so/model/so_model_virtual_node.h (-I include comes first): gives soModelNodeSetUp its members.
// The layout is read from soModelModuleImpl (sora_melee): the node id, three vectors, the flag word and the intrusive
// nw4r::ut::List link the module keeps the entries sorted by node id with. The vectors are plain floats because the
// fighter builders keep these records in soArrayVector arrays whose constructors do not initialize them (Vec3f has a
// constructor).

#include <StaticAssert.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_list.h>
#include <so/model/so_model_module_simple.h>
#include <types.h>

// One node's scale/rotation/translation override, as set by soModelModule::setNodeSRT and friends.
class soModelNodeSetUp {
public:
    // Which components hold a value; soModelModuleImpl reads them in groups.
    enum Flag {
        Flag_ScaleX = 0x1,
        Flag_ScaleY = 0x2,
        Flag_ScaleZ = 0x4,
        Flag_Scale = Flag_ScaleX | Flag_ScaleY | Flag_ScaleZ,
        Flag_RotateX = 0x8,
        Flag_RotateY = 0x10,
        Flag_RotateZ = 0x20,
        Flag_Rotate = Flag_RotateX | Flag_RotateY | Flag_RotateZ,
        Flag_TranslateX = 0x40,
        Flag_TranslateY = 0x80,
        Flag_TranslateZ = 0x100,
        Flag_Translate = Flag_TranslateX | Flag_TranslateY | Flag_TranslateZ,
        Flag_All = Flag_Scale | Flag_Rotate | Flag_Translate
    };

    // HYPOTHESIS: a constructor-free vector so the arrays of these records stay uninitialized storage.
    struct Vec {
        float m_x;
        float m_y;
        float m_z;

        Vec& operator=(const Vec3f& v) {
            m_x = v.m_x;
            m_y = v.m_y;
            m_z = v.m_z;
            return *this;
        }
    };

    u32 m_nodeId; // 0x0
    Vec m_scale; // 0x4
    Vec m_rotate; // 0x10
    Vec m_translate; // 0x1C
    u32 m_flag; // 0x28: Flag bits; 0 marks a free record
    nw4r::ut::Link m_link; // 0x2C
};
static_assert(sizeof(soModelNodeSetUp) == 0x34, "Class is the wrong size!");

// TODO: name
struct UnkVirtualNodeInfo {
    u32 m_parentNodeId;
    Vec3f m_translate;
    Vec3f m_rotate;
};
static_assert(sizeof(UnkVirtualNodeInfo) == 0x1C, "Class is the wrong size!");

// Manages the SRT matrices for a set of model nodes with IDs ranging
// from m_start to m_end. TODO: This class likely has more inline methods
class soModelVirtualNode {
    u32 m_start;
    u32 m_end;
    UnkVirtualNodeInfo* m_nodeInfo;
    Matrix* m_matrices;
    bool m_unused;
public:
    soModelVirtualNode();
    ~soModelVirtualNode();
    void init(u32 start, u32 end, UnkVirtualNodeInfo* info, Matrix* mtxs);
    bool updateMatrix(const soModelModule& mod, u32 id, const Vec3f& scale);
    bool getMatrix(u32 id, Matrix& mtx) const;
    bool setMatrix(u32 id, Matrix& mtx);
    bool isNode(u32 id) const;
    bool getOffset(u32 id, Vec3f& offset) const;
    bool getNodeParent(u32 id, u32& parent) const;
    // HYPOTHESIS: soModelModuleImpl::setVirtualNodeUpdate writes this byte through the array (0x10).
    void setUpdate(bool update) { m_unused = update; }
};
static_assert(sizeof(soModelVirtualNode) == 0x14, "Class is the wrong size!");
