#pragma force_active on

#include <ft/ft_log_data_accesser.h>

ftLogDataAccesser::ftLogDataAccesser() {
    for (u32 i = 0; i < 22; i++)
        m_floatTable[i] = nullptr;
    for (u32 i = 0; i < 75; i++)
        m_intTable[i] = nullptr;
    for (u32 i = 0; i < 20; i++)
        m_flagTable[i] = nullptr;
}

ftLogDataAccesser::~ftLogDataAccesser() { }

// Binds every table slot to its section of the log data. HYPOTHESIS: the offsets are the layout of ftLogData.
void ftLogDataAccesser::setup(ftLogData* logData) {
    m_floatTable[0] = &logData->unk0;
    m_count[0] = 1;
    m_floatTable[1] = &logData->unk4;
    m_count[1] = 1;
    m_floatTable[2] = &logData->unkC;
    m_count[2] = 1;
    m_floatTable[3] = &logData->unk10;
    m_count[3] = 1;
    m_floatTable[4] = &logData->unk14;
    m_count[4] = 1;
    m_floatTable[5] = logData->unk18;
    m_count[5] = 7;
    m_floatTable[6] = &logData->unk3C;
    m_count[6] = 1;
    m_floatTable[7] = &logData->unk40;
    m_count[7] = 1;
    m_floatTable[8] = &logData->unk44;
    m_count[8] = 1;
    m_floatTable[9] = &logData->unk7C;
    m_count[9] = 1;
    m_floatTable[10] = &logData->unk80;
    m_count[10] = 1;
    m_floatTable[11] = &logData->unk98;
    m_count[11] = 1;
    m_floatTable[12] = &logData->unk13C;
    m_count[12] = 1;
    m_floatTable[13] = &logData->unk140;
    m_count[13] = 1;
    m_floatTable[14] = &logData->unk144;
    m_count[14] = 1;
    m_floatTable[15] = &logData->unk148;
    m_count[15] = 1;
    m_floatTable[16] = &logData->unk14C;
    m_count[16] = 1;
    m_floatTable[17] = &logData->unk150;
    m_count[17] = 1;
    m_floatTable[18] = &logData->unk154;
    m_count[18] = 1;
    m_floatTable[19] = &logData->unk15C;
    m_count[19] = 1;
    m_floatTable[20] = &logData->unk164;
    m_count[20] = 1;
    m_floatTable[21] = &logData->unk18C;
    m_count[21] = 1;
    m_intTable[0] = &logData->unk8;
    m_count[22] = 1;
    m_intTable[1] = &logData->unk34;
    m_count[23] = 1;
    m_intTable[2] = &logData->unk38;
    m_count[24] = 1;
    m_intTable[3] = &logData->unk48;
    m_count[25] = 1;
    m_intTable[4] = &logData->unk4C;
    m_count[26] = 1;
    m_intTable[5] = &logData->unk50;
    m_count[27] = 1;
    m_intTable[6] = &logData->unk54;
    m_count[28] = 1;
    m_intTable[7] = &logData->unk58;
    m_count[29] = 1;
    m_intTable[8] = &logData->unk5C;
    m_count[30] = 1;
    m_intTable[9] = &logData->unk60;
    m_count[31] = 1;
    m_intTable[10] = &logData->unk64;
    m_count[32] = 1;
    m_intTable[11] = &logData->unk68;
    m_count[33] = 1;
    m_intTable[12] = &logData->unk6C;
    m_count[34] = 1;
    m_intTable[13] = &logData->unk70;
    m_count[35] = 1;
    m_intTable[14] = &logData->unk74;
    m_count[36] = 1;
    m_intTable[15] = &logData->unk78;
    m_count[37] = 1;
    m_intTable[16] = &logData->unk84;
    m_count[38] = 1;
    m_intTable[17] = &logData->unk88;
    m_count[39] = 1;
    m_intTable[18] = &logData->unk8C;
    m_count[40] = 1;
    m_intTable[19] = &logData->unk90;
    m_count[41] = 1;
    m_intTable[20] = &logData->unk94;
    m_count[42] = 1;
    m_intTable[21] = &logData->unk9C;
    m_count[43] = 1;
    m_intTable[22] = &logData->unkA0;
    m_count[44] = 1;
    m_intTable[23] = &logData->unkA4;
    m_count[45] = 1;
    m_intTable[24] = &logData->unkA8;
    m_count[46] = 1;
    m_intTable[25] = &logData->unkAC;
    m_count[47] = 1;
    m_intTable[26] = &logData->unkB0;
    m_count[48] = 1;
    m_intTable[27] = &logData->unkB4;
    m_count[49] = 1;
    m_intTable[28] = &logData->unkB8;
    m_count[50] = 1;
    m_intTable[29] = &logData->unkBC;
    m_count[51] = 1;
    m_intTable[30] = &logData->unkC0;
    m_count[52] = 1;
    m_intTable[31] = &logData->unkC4;
    m_count[53] = 1;
    m_intTable[32] = &logData->unkC8;
    m_count[54] = 1;
    m_intTable[33] = &logData->unkCC;
    m_count[55] = 1;
    m_intTable[34] = &logData->unkD0;
    m_count[56] = 1;
    m_intTable[35] = &logData->unkD4;
    m_count[57] = 1;
    m_intTable[36] = &logData->unkD8;
    m_count[58] = 1;
    m_intTable[37] = &logData->unkDC;
    m_count[59] = 1;
    m_intTable[38] = &logData->unkE0;
    m_count[60] = 1;
    m_intTable[39] = &logData->unkE4;
    m_count[61] = 1;
    m_intTable[40] = &logData->unkE8;
    m_count[62] = 1;
    m_intTable[41] = &logData->unkEC;
    m_count[63] = 1;
    m_intTable[42] = &logData->unkF0;
    m_count[64] = 1;
    m_intTable[43] = &logData->unkF4;
    m_count[65] = 1;
    m_intTable[44] = &logData->unkF8;
    m_count[66] = 1;
    m_intTable[45] = &logData->unkFC;
    m_count[67] = 1;
    m_intTable[46] = &logData->unk100;
    m_count[68] = 1;
    m_intTable[47] = &logData->unk104;
    m_count[69] = 1;
    m_intTable[48] = &logData->unk108;
    m_count[70] = 1;
    m_intTable[49] = &logData->unk10C;
    m_count[71] = 1;
    m_intTable[50] = &logData->unk110;
    m_count[72] = 1;
    m_intTable[51] = &logData->unk114;
    m_count[73] = 1;
    m_intTable[52] = &logData->unk118;
    m_count[74] = 1;
    m_intTable[53] = &logData->unk11C;
    m_count[75] = 1;
    m_intTable[54] = &logData->unk120;
    m_count[76] = 1;
    m_intTable[55] = &logData->unk124;
    m_count[77] = 1;
    m_intTable[56] = &logData->unk128;
    m_count[78] = 1;
    m_intTable[57] = &logData->unk12C;
    m_count[79] = 1;
    m_intTable[58] = &logData->unk130;
    m_count[80] = 1;
    m_intTable[59] = &logData->unk134;
    m_count[81] = 1;
    m_intTable[60] = &logData->unk138;
    m_count[82] = 1;
    m_intTable[61] = &logData->unk158;
    m_count[83] = 1;
    m_intTable[62] = &logData->unk160;
    m_count[84] = 1;
    m_intTable[63] = &logData->unk168;
    m_count[85] = 1;
    m_intTable[64] = &logData->unk16C;
    m_count[86] = 1;
    m_intTable[65] = &logData->unk170;
    m_count[87] = 1;
    m_intTable[66] = &logData->unk174;
    m_count[88] = 1;
    m_intTable[67] = &logData->unk178;
    m_count[89] = 1;
    m_intTable[68] = &logData->unk17C;
    m_count[90] = 1;
    m_intTable[69] = &logData->unk180;
    m_count[91] = 1;
    m_intTable[70] = &logData->unk184;
    m_count[92] = 1;
    m_intTable[71] = &logData->unk188;
    m_count[93] = 1;
    m_intTable[72] = &logData->unk190;
    m_count[94] = 1;
    m_intTable[73] = &logData->unk194;
    m_count[95] = 1;
    m_intTable[74] = &logData->unk198;
    m_count[96] = 1;
    m_flagTable[0] = &logData->unk19C;
    m_count[97] = 1;
    m_flagTable[1] = &logData->unk19D;
    m_count[98] = 1;
    m_flagTable[2] = &logData->unk19E;
    m_count[99] = 1;
    m_flagTable[3] = &logData->unk19F;
    m_count[100] = 1;
    m_flagTable[4] = &logData->unk1A0;
    m_count[101] = 1;
    m_flagTable[5] = &logData->unk1A1;
    m_count[102] = 1;
    m_flagTable[6] = &logData->unk1A2;
    m_count[103] = 1;
    m_flagTable[7] = &logData->unk1A3;
    m_count[104] = 1;
    m_flagTable[8] = &logData->unk1A4;
    m_count[105] = 1;
    m_flagTable[9] = &logData->unk1A5;
    m_count[106] = 1;
    m_flagTable[10] = &logData->unk1A6;
    m_count[107] = 1;
    m_flagTable[11] = &logData->unk1A7;
    m_count[108] = 1;
    m_flagTable[12] = &logData->unk1A8;
    m_count[109] = 1;
    m_flagTable[13] = &logData->unk1A9;
    m_count[110] = 1;
    m_flagTable[14] = &logData->unk1AA;
    m_count[111] = 1;
    m_flagTable[15] = &logData->unk1AB;
    m_count[112] = 1;
    m_flagTable[16] = &logData->unk1AC;
    m_count[113] = 1;
    m_flagTable[17] = &logData->unk1AD;
    m_count[114] = 1;
    m_flagTable[18] = &logData->unk1AE;
    m_count[115] = 1;
    m_flagTable[19] = &logData->unk1AF;
    m_count[116] = 1;
}

// Clears the logged values behind every table slot.
void ftLogDataAccesser::reset() {
    for (int i = 0; i < 22; i++)
        for (int j = 0; j < m_count[i]; j++)
            m_floatTable[i][j] = 0.0f;
    for (u32 i = 0; i < 75; i++)
        for (int j = 0; j < m_count[i + 22]; j++)
            m_intTable[i][j] = 0;
    for (s32 i = 0; i < 20; i++)
        for (int j = 0; j < m_count[97 + i]; j++)
            m_flagTable[i][j] = false;
}

float ftLogDataAccesser::getFloat(u32 category, u32 slot) {
    return m_floatTable[category][slot];
}

void ftLogDataAccesser::setFloat(float val, u32 category, u32 slot) {
    m_floatTable[category][slot] = val;
}

void ftLogDataAccesser::addFloat(float val, u32 category, u32 slot) {
    m_floatTable[category][slot] += val;
}

int ftLogDataAccesser::getInt(u32 category, u32 slot) {
    return m_intTable[category][slot];
}

void ftLogDataAccesser::setInt(int val, u32 category, u32 slot) {
    m_intTable[category][slot] = val;
}

void ftLogDataAccesser::addInt(int val, u32 category, u32 slot) {
    m_intTable[category][slot] += val;
}

bool ftLogDataAccesser::isFlag(u32 category, u32 slot) {
    return m_flagTable[category][slot];
}

void ftLogDataAccesser::setFlag(bool val, u32 category, u32 slot) {
    m_flagTable[category][slot] = val;
}

void ftLogDataAccesser::onFlag(u32 category, u32 slot) {
    m_flagTable[category][slot] = true;
}

void ftLogDataAccesser::offFlag(u32 category, u32 slot) {
    m_flagTable[category][slot] = false;
}
