// Local BrawlHeaders shadow: soAreaInstance fields and accessors recovered from so_area_instance.o (sora_melee)
#pragma once

#include <StaticAssert.h>
#include <types.h>

class gfArea;
struct soAreaData;

class soAreaInstance {
public:
    enum ShapeType {
        Shape_Rectangle = 0x0,
        Shape_Circle = 0x1,
        Shape_Rectangle_Ref_Ground_Rhombus = 0x2,
        Shape_Rectangle_Ref_Ground_Rhombus_Upper = 0x3,
        Shape_Rectangle_Ref_Ground_Rhombus_Lower = 0x4,
    };

    union {
        char _0[32];
        struct {
            soAreaData* m_data;
            gfArea* m_area;
            int m_enableFrame; // HYPOTHESIS: the second argument of enable()
            char _0xc[0x10];
            u8 m_shapeType;
            u8 m_isEnable : 1;
            u8 m_isCreated : 1; // HYPOTHESIS: getId only reads the area id while this is set
            u8 m_isWindSet : 1; // HYPOTHESIS: the destructor erases the stage wind area while this is set
            u8 m_isCenterX0 : 1;
            u8 m_unk1D_lo : 4;
            char _0x1e[2];
        };
    };

    virtual ~soAreaInstance();

    void setOwnerId(int ownerId);
    void deactivate();
    // HYPOTHESIS: parameter types of the accessors come from the callee's register use.
    bool isFlags(u16 mask);
    u32 getId();
    void enable(bool isEnable, int frame);
    void setWhole(bool isWhole);
    void setCheckTargetGroup(int group);
    void setCheckTargetLocalGroup(int group);
    void setTeamId(u32 teamId);
    u32 getTeamId();
    void setShapeType(ShapeType shapeType);
};
static_assert(sizeof(soAreaInstance) == 36, "Class is wrong size!");
