#pragma once

#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/ft_extend_param_accesser.h>
#include <ft/ft_common_data_accesser.h>
#include <types.h>

// TODO: identify the special moves these ExtendParam classes are for

struct ftMarioExtendParamClass1 {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
};

struct ftMarioExtendParamClass2 {
    float unk0;
    float unk4;
    float unk8;
};

struct ftMarioExtendParamClass3 {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
};

struct ftMarioExtendParamClass4 {
    int unk0;
    float unk4;
    int unk8;
    float unkC;
    int unk10;
};

// ftMario and ftMarioD (Dr. Mario) have the same parameter layout; they are separate classes with the same code.
// The setup argument is the fighter's ftData; its extendParam[] slots hold the four parameter groups.
// MATCH-ONLY: the first read of each group goes through the non-const ftData (the *Mutable accessors). A const read
// lets MWCC reuse one pointer load for the whole group and the setup() code no longer matches (same as ftMarth).
// TODO: UBFIX endianness
#define FT_MARIO_EXTEND_PARAM_ACCESSER(Name, Kind) class Name : public ftExtendParamAccesserEx<3999, 15, 23999, 3> {     const ftData* data(const u8* base) { return reinterpret_cast<const ftData*>(base); }     ftData* dataMutable(const u8* base) { return const_cast<ftData*>(reinterpret_cast<const ftData*>(base)); }         const ftMarioExtendParamClass1* group1(const u8* base) { return static_cast<const ftMarioExtendParamClass1*>(data(base)->extendParam[0]); }     const ftMarioExtendParamClass1* group1Mutable(const u8* base) { return static_cast<const ftMarioExtendParamClass1*>(dataMutable(base)->extendParam[0]); }     const ftMarioExtendParamClass2* group2(const u8* base) { return static_cast<const ftMarioExtendParamClass2*>(data(base)->extendParam[1]); }     const ftMarioExtendParamClass2* group2Mutable(const u8* base) { return static_cast<const ftMarioExtendParamClass2*>(dataMutable(base)->extendParam[1]); }     const ftMarioExtendParamClass3* group3(const u8* base) { return static_cast<const ftMarioExtendParamClass3*>(data(base)->extendParam[2]); }     const ftMarioExtendParamClass3* group3Mutable(const u8* base) { return static_cast<const ftMarioExtendParamClass3*>(dataMutable(base)->extendParam[2]); }     const ftMarioExtendParamClass4* group4(const u8* base) { return static_cast<const ftMarioExtendParamClass4*>(data(base)->extendParam[3]); }     const ftMarioExtendParamClass4* group4Mutable(const u8* base) { return static_cast<const ftMarioExtendParamClass4*>(dataMutable(base)->extendParam[3]); } public:     Name() : ftExtendParamAccesserEx(Kind) { }     virtual ~Name() { }     virtual void setup(const u8* extData) {         for (s32 i = 0; i < NumVariations; i++) {             m_floats[i][0] = &group1Mutable(extData)->unk0;             m_floats[i][1] = &group1(extData)->unk4;             m_floats[i][2] = &group1(extData)->unk8;             m_floats[i][3] = &group1(extData)->unkC;             m_floats[i][4] = &group1(extData)->unk10;             m_floats[i][5] = &group2Mutable(extData)->unk0;             m_floats[i][6] = &group2(extData)->unk4;             m_floats[i][7] = &group2(extData)->unk8;             m_floats[i][8] = &group3Mutable(extData)->unk0;             m_floats[i][9] = &group3(extData)->unk4;             m_floats[i][10] = &group3(extData)->unk8;             m_floats[i][11] = &group3(extData)->unkC;             m_floats[i][12] = &group3(extData)->unk10;             m_floats[i][13] = &group4(extData)->unk4;             m_floats[i][14] = &group4(extData)->unkC;             m_ints[i][0] = &group4Mutable(extData)->unk0;             m_ints[i][1] = &group4(extData)->unk8;             m_ints[i][2] = &group4(extData)->unk10;         }     } }

FT_MARIO_EXTEND_PARAM_ACCESSER(ftMarioExtendParamAccesser, Fighter_Mario);
FT_MARIO_EXTEND_PARAM_ACCESSER(ftMarioDExtendParamAccesser, Fighter_MarioD);

extern ftMarioExtendParamAccesser g_ftMarioExtendParamAccesser;
extern ftMarioDExtendParamAccesser g_ftMarioDExtendParamAccesser;
