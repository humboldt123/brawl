#pragma once

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_rescommon.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_resnode.h>
#include <types.h>

namespace nw4r {
    namespace g3d {

        // TEX0 header. HYPOTHESIS: the names, from the file format; soModelModuleImpl reads the data offset.
        struct ResTexData {
            u32 m_magic;
            u32 m_size;
            u32 m_version;
            s32 m_toResFile;
            s32 m_dataOffset; // 0x10
            s32 m_nameOffset;
        };

        class ResTex : public ResCommon<ResTexData> {

            /* data */
            public:
                inline ResTex() : ResCommon() {}
                inline ResTex(void* data) : ResCommon(data) {}
        };

    } // namespace g3d
} // namespace nw4r
