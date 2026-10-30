#pragma once

// Supersedes include/lib/BrawlHeaders/nw4r/include/nw4r/g3d/g3d_resmdl.h (shadows it via -I include).
// Adds GetResMatNumEntries and the by-index GetResMat, which the BrawlHeaders submodule lacks. Keep in sync.

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_rescommon.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_resnode.h>
#include <types.h>

namespace nw4r {
    namespace g3d {

        // MDL0 header and the ResMdlInfo block that follows it (offset 0x40). Only the fields soModelModuleImpl
        // touches are named; HYPOTHESIS: the names, from the file format.
        struct ResMdlData {
            u32 m_magic;
            u32 m_size;
            u32 m_version;
            s32 m_toResFile;
            s32 m_toDic[11]; // 0x10
            s32 m_toName; // 0x3C
            u32 m_infoSize; // 0x40
            s32 m_infoToMdl;
            u32 m_scalingRule;
            u32 m_texMtxMode;
            u32 m_numVtx; // 0x50
            u32 m_numTri;
            s32 m_toSrcMdl;
            u32 m_numNode;
            u8 m_needNrmMtxArray; // 0x60
            u8 m_needTexMtxArray; // 0x61
            u8 m_isValidBBox;
            u8 m_pad;
        };

        class ResMdl : public ResCommon<ResMdlData> {

            /* data */
        public:
            inline ResMdl() : ResCommon() {}
            inline ResMdl(void* data) : ResCommon(data) {}

            ResMat GetResMat(const char* name) const;
            ResMat GetResMat(u32 index) const;
            u32 GetResMatNumEntries() const;
            u32 GetResNodeNumEntries();
            u8* GetResByteCode(const char* name) const;
            ResNode GetResNode(const int nodeIndex) const;
            ResNode GetResNode(const u32 nodeIndex) const;
            ResNode GetResNode(const char* nodeName) const;
        };

    } // namespace g3d
} // namespace nw4r
