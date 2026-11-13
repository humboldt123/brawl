// Havok translation unit hkRigidBodyCinfo.o (main.dol 0x802E3604-0x802E3724).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E3604   288  __ct   [map: hkRigidBodyCinfo____ct]

// fn_802E3604: body from https://github.com/jj-link/brawl src/havok/fn_802E3604.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802E3604 {
extern "C" const float lbl_805A4370;
extern "C" const float lbl_805A4374;
extern "C" const float lbl_805A4378;
extern "C" const float lbl_805A437C;
extern "C" const float lbl_805A4380;
extern "C" const float lbl_805A4384;
extern "C" const float lbl_805A4388;

struct Obj {
    unsigned int a;
    unsigned int b;
    unsigned char c;
    unsigned char _9;
    unsigned short d;
    char _c[4];
    float f[40];
    unsigned char e0;
    unsigned char e1;
    unsigned char e2;
    unsigned char e3;
    unsigned char e4;
};

// TODO: cleanup (shape from jj-link/brawl fn_802E3604)
extern "C" void fn_802E3604(Obj* o)
{
    float t[4] __attribute__((aligned(16)));

    o->f[3] = lbl_805A4370;
    o->f[2] = lbl_805A4370;
    o->f[1] = lbl_805A4370;
    o->f[0] = lbl_805A4370;
    o->f[4] = lbl_805A4370;
    o->f[5] = lbl_805A4370;
    o->f[6] = lbl_805A4370;
    o->f[7] = lbl_805A4374;
    o->f[11] = lbl_805A4370;
    o->f[10] = lbl_805A4370;
    o->f[9] = lbl_805A4370;
    o->f[8] = lbl_805A4370;
    o->f[15] = lbl_805A4370;
    o->f[14] = lbl_805A4370;
    o->f[13] = lbl_805A4370;
    o->f[12] = lbl_805A4370;
    o->f[17] = lbl_805A4370;
    o->f[18] = lbl_805A4370;
    o->f[19] = lbl_805A4370;
    o->f[20] = lbl_805A4370;
    o->f[22] = lbl_805A4370;
    o->f[23] = lbl_805A4370;
    o->f[24] = lbl_805A4370;
    o->f[25] = lbl_805A4370;
    o->f[27] = lbl_805A4370;
    o->f[16] = lbl_805A4374;
    o->f[21] = lbl_805A4374;
    o->f[26] = lbl_805A4374;
    o->f[31] = lbl_805A4370;
    o->f[30] = lbl_805A4370;
    o->f[29] = lbl_805A4370;
    o->f[28] = lbl_805A4370;
    o->f[32] = lbl_805A4374;
    o->f[33] = lbl_805A4370;
    o->f[34] = lbl_805A4378;
    o->f[35] = lbl_805A437C;
    o->f[36] = lbl_805A4380;
    o->f[37] = lbl_805A4384;
    o->f[38] = lbl_805A4384;
    o->f[39] = lbl_805A4388;
    o->e0 = 1;
    o->e1 = 2;
    o->e2 = 2;
    o->e3 = 0;
    o->c = 1;
    o->d = 0xFFFF;
    o->a = 0;
    o->b = 0;
    o->e4 = 0;

    t[3] = lbl_805A4370;
    t[2] = lbl_805A4370;
    t[1] = lbl_805A4370;
    t[0] = lbl_805A4370;
}
} // namespace jj_fn_802E3604
