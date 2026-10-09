#pragma once

// Supersedes include/lib/BrawlHeaders/nw4r/include/nw4r/g3d/g3d_resfile.h (shadows it via -I include).
// Declares the ResFile accessors with the const qualifiers and the by-name/Bind members of the nw4r library
// build (src/nw4r/g3d/res/g3d_resfile.cpp), so game code links against the same symbols. Keep in sync.

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_anmshp.h>
#include <nw4r/g3d/g3d_anmtexpat.h>
#include <nw4r/g3d/g3d_anmtexsrt.h>
#include <nw4r/g3d/g3d_anmvis.h>
#include <nw4r/g3d/g3d_rescommon.h>
#include <nw4r/g3d/g3d_resdict.h>
#include <nw4r/g3d/g3d_restex.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <types.h>

namespace nw4r {
    namespace g3d {
        struct ResFileHeaderData {
            char magic[4];
            u16 endian;
            u16 version;
            u32 fileSize;
            u16 headerSize;
            u16 dataBlocks;
        };

        struct ResTopLevelDictData {
            ResBlockHeaderData header;
            ResDicData data;
        };

        struct ResFileData {
            ResFileHeaderData fileHeader;
            ResTopLevelDictData dict;
        };

        class ResFile : public ResCommon<ResFileData> {
        public:
            inline ResFile() : ResCommon() {}
            inline ResFile(ResFileData* data) : ResCommon(data) {}
            inline ResFile(void* data) : ResCommon(data) {}

            static void Init(void* arg);
            void Bind(ResFile file);

            ResMdl GetResMdl(const char* name) const;
            ResMdl GetResMdl(u32 index) const;

            u32 GetResMdlNumEntries() const;
            u32 GetResAnmChrNumEntries() const;
            u32 GetResAnmClrNumEntries() const;
            u32 GetResAnmVisNumEntries() const;
            u32 GetResAnmTexPatNumEntries() const;
            u32 GetResAnmTexSrtNumEntries() const;
            u32 GetResAnmShpNumEntries() const;
            u32 GetResAnmScnNumEntries() const;

            ResAnmChr GetResAnmChr(const char* name) const;
            ResAnmChr GetResAnmChr(u32 index) const;
            ResAnmClr GetResAnmClr(const char* name) const;
            ResAnmClr GetResAnmClr(u32 index) const;
            ResAnmVis GetResAnmVis(const char* name) const;
            ResAnmVis GetResAnmVis(u32 index) const;
            ResAnmTexPat GetResAnmTexPat(const char* name) const;
            ResAnmTexPat GetResAnmTexPat(u32 index) const;
            ResAnmTexSrt GetResAnmTexSrt(const char* name) const;
            ResAnmTexSrt GetResAnmTexSrt(u32 index) const;
            ResAnmShp GetResAnmShp(u32 index) const;
            ResAnmScn GetResAnmScn(const char* name) const;
            ResAnmScn GetResAnmScn(int index) const;

            ResTex GetResTex(int index) const;
            ResTex GetResTex(const char* name) const;

            bool HasResTex() const;
            bool HasResAnmChr() const;
            bool HasResAnmClr() const;
            bool HasResAnmVis() const;
            bool HasResAnmTexPat() const;
            bool HasResAnmTexSrt() const;
            bool HasResAnmShp() const;
            bool HasResAnmScn() const;
        };
    } // namespace g3d
} // namespace nw4r
