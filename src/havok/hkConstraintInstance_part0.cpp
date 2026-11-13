// Havok translation unit hkConstraintInstance_part0.o (main.dol 0x802DE6F4-0x802DE99C), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802DE6F4    32  fn_802DE6F4
//   0x802DE714    20  fn_802DE714
//   0x802DE728    60  fn_802DE728
//   0x802DE764    56  __ct__20hkConstraintInstanceFQ220hkConstraintInstance18ConstraintPriority
//   0x802DE79C    24  setPriority__20hkConstraintInstanceFQ220hkConstraintInstance18ConstraintPriority
//   0x802DE7B4     4  fn_802DE7B4
//   0x802DE7B8     4  fn_802DE7B8
//   0x802DE7BC    36  fn_802DE7BC
//   0x802DE7E0   220  __dt__20hkConstraintInstanceFv
//   0x802DE8BC   136  fn_802DE8BC
//   0x802DE944     8  fn_802DE944
//   0x802DE94C    80  fn_802DE94C

#include <types.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkWorldObject.h>

// fn_802DE6F4: body from https://github.com/jj-link/brawl src/havok/fn_802DE6F4.cpp (namespaced).
namespace jj_fn_802DE6F4 {
extern "C" {
extern unsigned char lbl_80487BB8[];

// TODO: cleanup (shape from jj-link/brawl fn_802DE6F4)
extern "C" void fn_802DE6F4(void* arg0) {
    if (arg0 != 0) {
        *(unsigned short*)((char*)arg0 + 6) = 1;
        *(void**)arg0 = lbl_80487BB8;
    }
}
}
} // namespace jj_fn_802DE6F4

// fn_802DE714: body from https://github.com/jj-link/brawl src/havok/fn_802DE714.cpp (namespaced).
namespace jj_fn_802DE714 {
class soVirtBase {
public:
    virtual void vf0(int p);
};

extern "C" {
// TODO: cleanup (shape from jj-link/brawl fn_802DE714)
extern "C" void fn_802DE714(soVirtBase* p) {
    p->vf0(-1);
}
}
} // namespace jj_fn_802DE714

// fn_802DE728: body from https://github.com/jj-link/brawl src/havok/fn_802DE728.cpp (namespaced).
namespace jj_fn_802DE728 {
extern "C" unsigned char lbl_80487BB8[];

struct Frame {
    void* m_p;
    short _a;
    short m_s;
    char _0[0x1C];
};

// TODO: cleanup (shape from jj-link/brawl fn_802DE728)
extern "C" void* fn_802DE728() {
    __attribute__((aligned(16))) Frame f;
    void** pp = &f.m_p;
    if (pp != 0) {
        f.m_s = 1;
        f.m_p = lbl_80487BB8;
    }
    return f.m_p;
}
} // namespace jj_fn_802DE728

// fn_802DE7B4: body from https://github.com/jj-link/brawl src/havok/fn_802DE7B4.cpp (namespaced).
namespace jj_fn_802DE7B4 {
// TODO: cleanup (shape from jj-link/brawl fn_802DE7B4)
extern "C" void fn_802DE7B4(void) {}
} // namespace jj_fn_802DE7B4

// fn_802DE7B8: body from https://github.com/jj-link/brawl src/havok/fn_802DE7B8.cpp (namespaced).
namespace jj_fn_802DE7B8 {
// TODO: cleanup (shape from jj-link/brawl fn_802DE7B8)
extern "C" void fn_802DE7B8(void) {}
} // namespace jj_fn_802DE7B8

// fn_802DE7BC: body from https://github.com/jj-link/brawl src/havok/fn_802DE7BC.cpp (namespaced).
namespace jj_fn_802DE7BC {
struct Obj {
    int x0;
    int x4;
    void* ptr;
};

extern "C" {
extern void fn_802F8F98(void*, void*, int);

// TODO: cleanup (shape from jj-link/brawl fn_802DE7BC)
extern "C" void fn_802DE7BC(Obj* a, Obj* b) {
    if (a->ptr == 0) return;
    hkWorldOperationUtil::removeConstraintImmediately((hkWorld*)b->ptr, (hkConstraintInstance*)a, 1);
}
}
} // namespace jj_fn_802DE7BC

// fn_802DE8BC: body from https://github.com/jj-link/brawl src/havok/fn_802DE8BC.cpp (namespaced).
namespace jj_fn_802DE8BC {
extern "C" void fn_802F064C(void* p);

// TODO: cleanup (shape from jj-link/brawl fn_802DE8BC)
extern "C" void fn_802DE8BC(u32* arg0) {
    u32* cur = arg0;
    int i = 0;

    while (i < 2) {
        int val = cur[5];
        if (val == 0) {
            int t = val ^ arg0[5];
            int target = arg0[6] ^ t;
            if (target != 0) {
                int ptr = *(int*)(target + 8);
                if (ptr != 0) {
                    int result = *(int*)(ptr + 0x24);
                    cur[5] = result;
                    ((hkWorldObject*)result)->addReference();
                }
            }
        }
        i++;
        cur++;
    }
}
} // namespace jj_fn_802DE8BC

// fn_802DE944: body from https://github.com/jj-link/brawl src/havok/fn_802DE944.cpp (namespaced).
namespace jj_fn_802DE944 {
// TODO: cleanup (shape from jj-link/brawl fn_802DE944)
extern "C" s32 fn_802DE944(void) {
    return 0x0;
}
} // namespace jj_fn_802DE944

// fn_802DE94C: body from https://github.com/jj-link/brawl src/havok/fn_802DE94C.cpp (namespaced).
namespace jj_fn_802DE94C {
typedef unsigned int u32;

extern "C" {
u32 fn_802DE728(void);
void fn_802DE6F4(void* arg0);
void fn_802DE714(void* p);
extern char lbl_804126F0[];
extern u32 lbl_80532F10[4];
}

// TODO: cleanup (shape from jj-link/brawl fn_802DE94C)
extern "C" void fn_802DE94C(void) {
    u32 temp_r3 = fn_802DE728();
    lbl_80532F10[0] = (u32)lbl_804126F0;
    lbl_80532F10[1] = (u32)fn_802DE6F4;
    lbl_80532F10[2] = (u32)fn_802DE714;
    lbl_80532F10[3] = temp_r3;
}

__declspec(section ".ctors") extern void* const fn_802DE94C_ctor = (void*)fn_802DE94C;
} // namespace jj_fn_802DE94C
