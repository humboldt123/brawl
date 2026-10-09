#pragma once

// Local shadow of BrawlHeaders' so/collision/so_collision_group.h. The SDK copy is a plain 120-byte blob (alignment 1);
// the original class has 4-byte alignment, which matters for arrays that embed it (soArrayVector<soCollisionGroup, N>
// places its elements 4-aligned). Translation units that need the aligned layout define SO_COLLISION_GROUP_ALIGNED.

#include <StaticAssert.h>
#include <types.h>

class soCollisionGroup {
#if defined(SO_COLLISION_GROUP_ALIGNED) && defined(SO_COLLISION_GROUP_MEMBERWISE)
    // MATCH-ONLY: the array instantiations of the stage RELs copy the group member by member (words, floats, a half and
    // three bytes), so the layout is spelled out; field meanings are unknown.
    u32 unk0, unk4, unk8, unkc, unk10, unk14, unk18, unk1c, unk20, unk24, unk28, unk2c, unk30, unk34, unk38, unk3c, unk40, unk44, unk48;
    float unk4c, unk50, unk54;
    u32 unk58, unk5c, unk60, unk64, unk68, unk6c;
    s16 unk70;
    u8 unk72, unk73, unk74;
#elif defined(SO_COLLISION_GROUP_ALIGNED)
    u32 _spacer[30];
#else
    char _spacer[120];
#endif

public:
    soCollisionGroup();
    ~soCollisionGroup();

};
static_assert(sizeof(soCollisionGroup) == 120, "Class is wrong size!");
