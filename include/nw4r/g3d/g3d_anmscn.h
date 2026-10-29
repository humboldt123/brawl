#pragma once

// Local BrawlHeaders shadow: adds AnmScn (the base that ScnRoot::SetAnmScn takes), ResAnmScn::Bind and the u32 size
// of AnmScnRes::Construct, as the nw4r library build declares them (src/nw4r/g3d/g3d_anmscn.cpp). Keep in sync.

#include <StaticAssert.h>
#include <MEM.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <types.h>

namespace nw4r {
    namespace g3d {

        struct ResAnmScnData {
            char _spacer2[0x30];
            unsigned short m_animLength;
        };
        class ResAnmScn : public ResCommon<ResAnmScnData> {
        public:
            inline ResAnmScn() : ResCommon() {}
            inline ResAnmScn(void* data) : ResCommon(data) {}

            bool Bind(ResAnmScn scene);
            bool Bind() { return Bind(*this); }
        };

        class AnmScn : public G3dObj { };

        class AnmScnRes : public AnmScn {
        public:
            virtual u32 IsDerivedFrom(int* unk1);

            virtual void G3dProc(int unk1, int unk2, int unk3);

            virtual ~AnmScnRes();

            virtual int* GetTypeObj();

            virtual int GetTypeName();

            virtual void SetFrame(float frame);

            virtual float GetFrame();

            virtual void UpdateFrame();

            virtual void SetUpdateRate(float updateRate);

            virtual float GetUpdateRate();

            virtual int Bind(int* unk1);

            virtual void Release();

            virtual int* GetResult(int* unk1, int unk2);

            virtual int Attach(int unk1, int* unk2); // TODO
            virtual int* Detach(int unk1);           // TODO
            virtual void DetachAll();

            static AnmScnRes* Construct(MEMAllocator* allocator, u32* instanceSize, ResAnmScn scn, bool hasCache);

            char _spacer[52];
        };
    }
}
