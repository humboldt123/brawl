// Havok translation unit hk_weak_802FB894.o (main.dol 0x802FB894-0x802FC6AC), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802FB894   180  fn_802FB894
//   0x802FB948    92  fn_802FB948
//   0x802FB9A4  1248  fn_802FB9A4
//   0x802FBE84   116  fn_802FBE84
//   0x802FBEF8    64  fn_802FBEF8
//   0x802FBF38    56  fn_802FBF38
//   0x802FBF70     4  fn_802FBF70
//   0x802FBF74     4  fn_802FBF74
//   0x802FBF78  1844  fn_802FBF78

#include <types.h>
#include <havok/hkCollisionDispatcher.h>

// fn_802FB894: body from https://github.com/jj-link/brawl src/havok/fn_802FB894.cpp (namespaced).
namespace jj_fn_802FB894 {
typedef void (*F)();
extern "C" {
void fn_802FB948();
void fn_802FB9A4();
void fn_802FBE84();
void fn_802FBEF8();
void fn_802FBF38();
void fn_802FBF70();
void fn_802FBF74();
void fn_802FBF78();
void fn_802CC214(void*, F*, int, int);

// TODO: cleanup (shape from jj-link/brawl fn_802FB894)
extern "C" void fn_802FB894(void* p) {
    struct S {
        F f[6];
        int a;
        int b;
        int c;
        F g;
        F h;
        char c1;
        char c2;
    } s;
    s.b = 0;
    s.c = 0;
    s.c2 = 0;
    s.f[0] = fn_802FB948;
    s.h = fn_802FBF78;
    s.g = fn_802FB9A4;
    s.f[2] = fn_802FBE84;
    s.f[3] = fn_802FBEF8;
    s.f[4] = fn_802FBF38;
    s.f[5] = fn_802FBF70;
    s.a = 0;
    s.f[1] = fn_802FBF74;
    s.c1 = 0;
    ((hkCollisionDispatcher*)p)->registerAgent3(*(const hkAgent3Funcs*)s.f, 8, 6);
}
}
} // namespace jj_fn_802FB894

// fn_802FBEF8: body from https://github.com/jj-link/brawl src/havok/fn_802FBEF8.cpp (namespaced).
namespace jj_fn_802FBEF8 {
// TODO: cleanup (shape from jj-link/brawl fn_802FBEF8)
extern "C" void fn_802FBEF8(void *unused, unsigned short *arr, unsigned long key) {
    unsigned short *p = arr;
    int i;
    for (i = 0; i < 3; i++) {
        if (key == *p) {
            arr[i] = 0xFFFF;
            return;
        }
        p++;
    }
}
} // namespace jj_fn_802FBEF8

// fn_802FBF38: body from https://github.com/jj-link/brawl src/havok/fn_802FBF38.cpp (namespaced).
namespace jj_fn_802FBF38 {
// TODO: cleanup (shape from jj-link/brawl fn_802FBF38)
extern "C" void fn_802FBF38(void* arg1, unsigned short* arg2, unsigned short arg3) {
    unsigned short* ptr = arg2;
    for (int i = 0; i < 3; ++i) {
        if (*ptr == 0xFFFF) {
            arg2[i] = arg3;
            return;
        }
        ptr++;
    }
}
} // namespace jj_fn_802FBF38

// fn_802FBF70: body from https://github.com/jj-link/brawl src/havok/fn_802FBF70.cpp (namespaced).
namespace jj_fn_802FBF70 {
// TODO: cleanup (shape from jj-link/brawl fn_802FBF70)
extern "C" void fn_802FBF70(void) {}
} // namespace jj_fn_802FBF70

// fn_802FBF74: body from https://github.com/jj-link/brawl src/havok/fn_802FBF74.cpp (namespaced).
namespace jj_fn_802FBF74 {
extern "C" void fn_802FBE84(void);

// TODO: cleanup (shape from jj-link/brawl fn_802FBF74)
extern "C" void fn_802FBF74(void) {
    fn_802FBE84();
}
} // namespace jj_fn_802FBF74
