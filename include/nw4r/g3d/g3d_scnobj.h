#pragma once

// Local shadow: expose callback timing values, the G3dObj base and the scene callback member without changing
// the scene-object layout.

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_obj.h>
#include <types.h>

namespace nw4r {
    namespace math {
        struct MTX34;
    }
    namespace g3d {

        class IScnObjCallback;

        class ScnObj : public G3dObj {
        public:
            enum Timing { CALLBACK_TIMING_A = 1, CALLBACK_TIMING_B = 2, CALLBACK_TIMING_C = 4, CALLBACK_TIMING_ALL = 7 };
            enum ScnObjMtxType { MTX_LOCAL, MTX_WORLD, MTX_VIEW, MTX_TYPE_MAX };
            // TODO
            virtual u32 IsDerivedFrom(int* unk1);

            virtual void G3dProc(int unk1, int unk2, int unk3);

            virtual ~ScnObj();

            char _spacer[208];
            IScnObjCallback* m_callback; // +0xD4: set by createModel callers (see IfMarthFinalTask)
            char _spacer2[4];

            bool GetMtx(ScnObjMtxType type, math::MTX34* pMtx) const;

            void SetPriorityDrawOpa(int prio);

            void SetPriorityDrawXlu(int prio);

        };
    }
}
//// Size: 220
