#pragma once

// Local shadow: expose callback timing values without changing the scene-object layout.

#include <StaticAssert.h>
#include <types.h>

namespace nw4r {
    namespace math {
        struct MTX34;
    }
    namespace g3d {

        class ScnObj {
        public:
            enum Timing { CALLBACK_TIMING_A = 1, CALLBACK_TIMING_B = 2, CALLBACK_TIMING_C = 4, CALLBACK_TIMING_ALL = 7 };
            enum ScnObjMtxType { MTX_LOCAL, MTX_WORLD, MTX_VIEW, MTX_TYPE_MAX };
            // TODO
            virtual u32 IsDerivedFrom(int* unk1);

            virtual void G3dProc(int unk1, int unk2, int unk3);

            virtual ~ScnObj();

            char _spacer[216];

            bool GetMtx(ScnObjMtxType type, math::MTX34* pMtx) const;

            void SetPriorityDrawOpa(int prio);

            void SetPriorityDrawXlu(int prio);

        };
    }
}
//// Size: 220
