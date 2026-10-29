#pragma once

// Local BrawlHeaders shadow: by default identical to BrawlHeaders so/link/so_link_connection_server.h. With
// SO_LINK_MODULE_IMPL_FULL (the sora_melee link units) it has the node and connection layout verified against
// soLinkConnection::reset, soLinkConnectionServer::link/disconnect and the soLinkModuleImpl constructor, and the
// connection server class.

#include <StaticAssert.h>
#include <types.h>

class soLinkModule;
class soLinkConnection;

#ifdef SO_LINK_MODULE_IMPL_FULL
#include <ut/ut_list.h>

class soLinkModuleImpl;

class soLinkNode : public utListNode {
public:
    u32 m_taskId;                    // 0x8 (the task id of the node's own end)
    soLinkConnection* m_connection;  // 0xC
    soLinkModuleImpl* m_linkModule;  // 0x10

    soLinkModuleImpl* getLinkModule();
};
static_assert(0x14 == sizeof(soLinkNode), "Class is wrong size!");

class soLinkConnection {
public:
    u8 unk0[0x8];
    soLinkNode m_node;   // 0x8: the child end, in the parent module's node list
    soLinkNode m_parent; // 0x1C: the parent end, in the child module's parent list
    u32 m_attribute : 24; // 0x30
    u32 m_id : 7;
    u32 m_isLinked : 1;

    enum Attribute {
        Attribute_Reference_Parent_Stop = 0x0,
        Attribute_Reference_Parent_Shake = 0x1,
        Attribute_Reference_Parent_Scale = 0x2,
        // HYPOTHESIS: 0x3, 0x5, 0x8, 0xB, 0xC, 0xE, 0xF and 0x10 are named after the only soLinkModuleImpl query that
        // tests each (getLinkColorBlend, getLinkSlopeRotation, getConstraintModelScale, sendEnvMapNodes,
        // adjustLinkGroundCollision, getLinkLightSet, isNodeGroundCrush, setNodeScalePowerMul).
        Attribute_Reference_Parent_Color_Blend = 0x3,
        Attribute_Reference_Parent_Unknown_4 = 0x4, // tested by getLinkController
        Attribute_Reference_Parent_Slope = 0x5,

        Attribute_Reference_Parent_Visibility = 0x7,
        Attribute_Reference_Parent_Model_Scale = 0x8,

        Attribute_Reference_Parent_Power_Up = 0xA, // glow
        Attribute_Reference_Parent_Env_Map = 0xB,
        Attribute_Reference_Parent_Ground_Collision = 0xC,
        Attribute_Reference_Parent_Light_Set = 0xE,
        Attribute_Reference_Parent_Ground_Crush = 0xF,
        Attribute_Reference_Parent_Scale_Power_Mul = 0x10,
    };

    void reset();
};
static_assert(0x34 == sizeof(soLinkConnection), "Class is wrong size!");

class soLinkConnectionServer : public utList {
public:
    soLinkConnectionServer() {
        m_tail = NULL;
        m_head = NULL;
        m_length = 0;
    }
    virtual ~soLinkConnectionServer();
    void entry(utListNode* node);
    void exit(utListNode* node);
    bool link(soLinkModuleImpl* child, int taskId, soLinkConnection* connection);
    void disconnect(soLinkConnection* connection);
    void disconnectAll(soLinkModuleImpl* linkModule);
    void disconnectNodeAll(soLinkModuleImpl* linkModule);
};
static_assert(0x10 == sizeof(soLinkConnectionServer), "Class is wrong size!");

extern soLinkConnectionServer lbl_27_bss_110;
#else
class soLinkNode {
    char _0x0[0xC];
    soLinkConnection* m_connection;
    soLinkModule* m_linkModule;
};
static_assert(0x14 == sizeof(soLinkNode), "Class is wrong size!");

class soLinkConnection {
    char _0x0[0x4];
    soLinkNode m_node1;
    soLinkNode m_node2;
    union {
        struct {
            u32 : 21;
            bool m_attributeReferenceParentScale : 1;
            bool m_attributeReferenceParentShake : 1;
            bool m_attributeReferenceParentStop : 1;
            u8 m_id : 7;
            bool : 1;
        };
        struct {
            u32 m_attribute : 24;
            u8 : 7;
            bool : 1;
        };
    };

    char _0x30[0x4];

public:
    enum Attribute {
        Attribute_Reference_Parent_Stop = 0x0,
        Attribute_Reference_Parent_Shake = 0x1,
        Attribute_Reference_Parent_Scale = 0x2,
        // HYPOTHESIS: 0x3, 0x5, 0x8, 0xB, 0xC, 0xE, 0xF and 0x10 are named after the only soLinkModuleImpl query that
        // tests each (getLinkColorBlend, getLinkSlopeRotation, getConstraintModelScale, sendEnvMapNodes,
        // adjustLinkGroundCollision, getLinkLightSet, isNodeGroundCrush, setNodeScalePowerMul).
        Attribute_Reference_Parent_Color_Blend = 0x3,
        Attribute_Reference_Parent_Unknown_4 = 0x4, // tested by getLinkController
        Attribute_Reference_Parent_Slope = 0x5,

        Attribute_Reference_Parent_Visibility = 0x7,
        Attribute_Reference_Parent_Model_Scale = 0x8,

        Attribute_Reference_Parent_Power_Up = 0xA, // glow
        Attribute_Reference_Parent_Env_Map = 0xB,
        Attribute_Reference_Parent_Ground_Collision = 0xC,
        Attribute_Reference_Parent_Light_Set = 0xE,
        Attribute_Reference_Parent_Ground_Crush = 0xF,
        Attribute_Reference_Parent_Scale_Power_Mul = 0x10,
    };
};
static_assert(0x34 == sizeof(soLinkConnection), "Class is wrong size!");
#endif
