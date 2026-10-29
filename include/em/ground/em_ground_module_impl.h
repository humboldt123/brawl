#pragma once

// The ground module of the enemies: soGroundModuleImpl with the enemy's own ground tracer and, for the enemy kinds that
// have one, a table that makes the ground collision vertices follow model nodes
// (mo_enemy/sora_enemy/ground/em_ground_module_impl.cpp).

#include <StaticAssert.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/ground/so_ground_tracer_impl.h>
#include <types.h>

class soModuleAccesser;

// One ground collision vertex that follows a model node.
struct emGroundVtxNode {
    u32 m_vtxIndex;
    u32 m_nodeId;
};

// The vertex / node table of an enemy kind (a soValueAccesser constant, 0xA813). HYPOTHESIS: names.
struct emGroundVtxNodeData {
    emGroundVtxNode* m_entries;
    u32 m_num;
};

class emGroundModuleImpl : public soGroundModuleImpl {
public:
    emGroundVtxNodeData* m_vtxNodeData; // 0x58
    soGroundTracerImpl m_tracer;        // 0x5C (the tracer the base module drives)

private:
    u8 unk84[0x8C - 0x84]; // never touched by this unit

public:
    // HYPOTHESIS: groundData is the enemy's vertex / node table (null for most enemy kinds).
    emGroundModuleImpl(soModuleAccesser* acc, soArray<soGroundShapeImpl>* shapes, void* groundData);
    virtual ~emGroundModuleImpl();
    virtual void update(u32 index);
    virtual float getRotateYForGrCollisionMatrix();
    virtual bool isReferenceModelNode();

    // Moves the ground collision vertices onto their model nodes (init: set their initial positions instead). Facing
    // the camera turned away (rotation -90 without the front flag), the node of the mirrored entry is used.
    void updateVtxFromNode(bool init);
    void updateVtxFromNodeSub(u32 nodeId, u32 vtxIndex, bool init);

    // The vertex / node entry at index. (updateVtxFromNode reads an entry's node id and vertex index through two calls
    // of this accessor: the original computes the entry offset before loading the table, as two inline expansions do.)
    const emGroundVtxNode& getVtxNode(u32 index) const {
        return m_vtxNodeData->m_entries[index];
    }
};
static_assert(sizeof(emGroundModuleImpl) == 0x8C, "Class is wrong size!");
