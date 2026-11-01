// Verified copy layout for the abstract-template instantiation.
#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/so_array.h>
#include <so/collision/so_collision.h>
#include <types.h>

// HYPOTHESIS: the part of a hit sphere that both of its forms begin with. The stage RELs set its shape type as the top bit
// of the byte at 0x1c and copy it into the simple form with its implicit assignment (the positions as words, the size,
// the whole shape byte).
struct soCollisionHitSphere {
    Vec3f m_startOffsetPos;
    Vec3f m_endOffsetPos;
    float m_size;
    u8 m_shapeType : 1; // soCollision::ShapeType
};
static_assert(sizeof(soCollisionHitSphere) == 32, "Class is wrong size!");

struct soCollisionHitData : soCollisionHitSphere {
    enum Height {
        Height_Low = 0x1,
        Height_Center = 0x2,
        Height_High = 0x3,
    };

    // MATCH-ONLY: aggregate positions and the flag word preserve the original bitwise copy.
    soCollisionHitData& operator=(const soCollisionHitData& other) {
        union DataWords { u32 words[8]; };
        *(DataWords*)this = *(const DataWords*)&other;
        return *this;
    }

    struct Simple : soCollisionHitSphere {
        Height m_height;
        unsigned int m_nodeIndex;

        Simple& operator=(const soCollisionHitSphere& sphere) {
            soCollisionHitSphere::operator=(sphere);
            return *this;
        }
    };
    static_assert(sizeof(Simple) == 40, "Class is wrong size!");

};
static_assert(sizeof(soCollisionHitData) == 32, "Class is wrong size!");

class soCollisionHitPart {
    int m_status;
    u32 unk4;
    soCollisionHitData m_hitData;
    soArrayVector<clTarget, 6> m_clTargetArrayVector;
    u8 unk64;
    u8 unk65;

public:
#ifdef YK_STAGE_FULL
    soCollisionHitPart() { } // the stage RELs emit the default constructor themselves (only the clTarget array is constructed)
#else
    soCollisionHitPart();
#endif
    soCollisionHitPart(soCollision::Category selfCategory, u32 opponentCategory);
    ~soCollisionHitPart();
};
static_assert(sizeof(soCollisionHitPart) == 0x68, "Class is wrong size!");
