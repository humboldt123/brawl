#pragma once

#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/ft_extend_param_accesser.h>
#include <types.h>

// The template arguments are the RTTI string of the base in the data of the unit.
// TODO: identify the special moves the ExtendParam groups are for.
class ftPokeFushigisouExtendParamAccesser : public ftExtendParamAccesserEx<3999, 10, 23999, 4> {
    const u8* group1(const u8* base) { return *(const u8* const*)(base + 0x80); }
    const u8* group1Cast(const u8* base) { return *(const u8**)(base + 0x80); }
    const u8* group2(const u8* base) { return *(const u8* const*)(base + 0x84); }
    const u8* group2Cast(const u8* base) { return *(const u8**)(base + 0x84); }
    const u8* group3Cast(const u8* base) { return *(const u8**)(base + 0x88); }

public:
    ftPokeFushigisouExtendParamAccesser() : ftExtendParamAccesserEx(Fighter_PokeFushigisou) { }
    virtual ~ftPokeFushigisouExtendParamAccesser() { }

    virtual void setup(const u8* extData) {
        for (s32 i = 0; i < NumVariations; i++) {
            m_floats[i][0] = (const float*)(group1(extData) + 0x4);
            m_floats[i][1] = (const float*)(group1(extData) + 0x8);
            m_floats[i][2] = (const float*)(group2Cast(extData) + 0x0);
            m_floats[i][3] = (const float*)(group2(extData) + 0x8);
            m_floats[i][4] = (const float*)(group2(extData) + 0xC);
            m_floats[i][5] = (const float*)(group2(extData) + 0x10);
            m_floats[i][6] = (const float*)(group2(extData) + 0x14);
            m_floats[i][7] = (const float*)(group2(extData) + 0x18);
            m_floats[i][8] = (const float*)(group2(extData) + 0x1C);
            m_floats[i][9] = (const float*)(group3Cast(extData) + 0x0);
            m_ints[i][0] = (const int*)(group1Cast(extData) + 0x0);
            m_ints[i][1] = (const int*)(group1(extData) + 0xC);
            m_ints[i][2] = (const int*)(group1(extData) + 0x10);
            m_ints[i][3] = (const int*)(group2(extData) + 0x4);
        }
    }
};

extern ftPokeFushigisouExtendParamAccesser g_ftPokeFushigisouExtendParamAccesser;
