// Havok translation unit hk_weak_802DBA6C.o (main.dol 0x802DBA6C-0x802DBC80), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802DBA6C   104  fn_802DBA6C
//   0x802DBAD4    16  fn_802DBAD4
//   0x802DBAE4    32  fn_802DBAE4
//   0x802DBB04     8  fn_802DBB04
//   0x802DBB0C    28  fn_802DBB0C
//   0x802DBB28    12  fn_802DBB28
//   0x802DBB34    12  fn_802DBB34
//   0x802DBB40    12  fn_802DBB40
//   0x802DBB4C   108  fn_802DBB4C
//   0x802DBBB8     8  fn_802DBBB8
//   0x802DBBC0     8  fn_802DBBC0
//   0x802DBBC8     8  fn_802DBBC8
//   0x802DBBD0    16  fn_802DBBD0
//   0x802DBBE0    20  fn_802DBBE0
//   0x802DBBF4    16  fn_802DBBF4
//   0x802DBC04    12  fn_802DBC04
//   0x802DBC10     8  fn_802DBC10
//   0x802DBC18     8  fn_802DBC18
//   0x802DBC20     8  fn_802DBC20
//   0x802DBC28     8  fn_802DBC28
//   0x802DBC30    80  fn_802DBC30

#include <types.h>
#include <havok/hkArray.h>

// fn_802DBA6C: body from https://github.com/jj-link/brawl src/havok/fn_802DBA6C.cpp (namespaced).
namespace jj_fn_802DBA6C {
extern "C" void fn_8027CD94(void* a, s32 b, s32 c);

struct Fn802DBA6C_Class {
    s32 m_unk0;
    s32 m_unk4;
    s32 m_unk8;
};

// TODO: cleanup (shape from jj-link/brawl fn_802DBA6C)
extern "C" void fn_802DBA6C(Fn802DBA6C_Class* self, s32 arg1) {
    s32 val = self->m_unk8 & 0x3FFFFFFF;
    if (val < arg1) {
        s32 doubled = val * 2;
        hkArrayUtil::_reserve((hkArrayBase*)self, (arg1 < doubled) ? doubled : arg1, 0x4C);
    }
    self->m_unk4 = arg1;
}
} // namespace jj_fn_802DBA6C

// fn_802DBAD4: body from https://github.com/jj-link/brawl src/havok/fn_802DBAD4.cpp (namespaced).
namespace jj_fn_802DBAD4 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBAD4)
extern "C" void* fn_802DBAD4(void** p, int index) {
    return (char*)*p + index * 4;
}
} // namespace jj_fn_802DBAD4

// fn_802DBB04: body from https://github.com/jj-link/brawl src/havok/fn_802DBB04.cpp (namespaced).
namespace jj_fn_802DBB04 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBB04)
extern "C" void fn_802DBB04(void* self, u32 value) {
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0) = value;
}
} // namespace jj_fn_802DBB04

// fn_802DBB0C: body from https://github.com/jj-link/brawl src/havok/fn_802DBB0C.cpp (namespaced).
namespace jj_fn_802DBB0C {
// TODO: cleanup (shape from jj-link/brawl fn_802DBB0C)
extern "C" void fn_802DBB0C(unsigned int* dst, const unsigned int* src)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}
} // namespace jj_fn_802DBB0C

// fn_802DBB28: body from https://github.com/jj-link/brawl src/havok/fn_802DBB28.cpp (namespaced).
namespace jj_fn_802DBB28 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBB28)
extern "C" void fn_802DBB28(float* dst, float* src) {
    dst[0] = src[0];
}
} // namespace jj_fn_802DBB28

// fn_802DBB34: body from https://github.com/jj-link/brawl src/havok/fn_802DBB34.cpp (namespaced).
namespace jj_fn_802DBB34 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBB34)
extern "C" void fn_802DBB34(int* destination, const int* source)
{
    *destination = *source;
}
} // namespace jj_fn_802DBB34

// fn_802DBB40: body from https://github.com/jj-link/brawl src/havok/fn_802DBB40.cpp (namespaced).
namespace jj_fn_802DBB40 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBB40)
extern "C" void fn_802DBB40(int* destination, const int* source)
{
    *destination = *source;
}
} // namespace jj_fn_802DBB40

// fn_802DBBB8: body from https://github.com/jj-link/brawl src/havok/fn_802DBBB8.cpp (namespaced).
namespace jj_fn_802DBBB8 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBBB8)
extern "C" u32 fn_802DBBB8(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DBBB8

// fn_802DBBC0: body from https://github.com/jj-link/brawl src/havok/fn_802DBBC0.cpp (namespaced).
namespace jj_fn_802DBBC0 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBBC0)
extern "C" u32 fn_802DBBC0(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DBBC0

// fn_802DBBC8: body from https://github.com/jj-link/brawl src/havok/fn_802DBBC8.cpp (namespaced).
namespace jj_fn_802DBBC8 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBBC8)
extern "C" u32 fn_802DBBC8(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DBBC8

// fn_802DBBD0: body from https://github.com/jj-link/brawl src/havok/fn_802DBBD0.cpp (namespaced).
namespace jj_fn_802DBBD0 {
extern "C" {
extern const float lbl_805A426C;

// TODO: cleanup (shape from jj-link/brawl fn_802DBBD0)
extern "C" void fn_802DBBD0(void* p) {
    float x = lbl_805A426C;
    *(float*)((char*)p + 8) = x;
    *(float*)((char*)p + 12) = x;
}
}
} // namespace jj_fn_802DBBD0

// fn_802DBBE0: body from https://github.com/jj-link/brawl src/havok/fn_802DBBE0.cpp (namespaced).
namespace jj_fn_802DBBE0 {
typedef struct Vec2f {
    float x;
    float y;
} Vec2f;

// TODO: cleanup (shape from jj-link/brawl fn_802DBBE0)
extern "C" void fn_802DBBE0(Vec2f *dst, const Vec2f *src)
{
    dst->x = src->x;
    dst->y = src->y;
}
} // namespace jj_fn_802DBBE0

// fn_802DBBF4: body from https://github.com/jj-link/brawl src/havok/fn_802DBBF4.cpp (namespaced).
namespace jj_fn_802DBBF4 {
extern "C" {
// TODO: cleanup (shape from jj-link/brawl fn_802DBBF4)
extern "C" void *fn_802DBBF4(void *self, int index)
{
    char *base = *(char **)self;
    return base + index * 0x4c;
}
}
} // namespace jj_fn_802DBBF4

// fn_802DBC04: body from https://github.com/jj-link/brawl src/havok/fn_802DBC04.cpp (namespaced).
namespace jj_fn_802DBC04 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBC04)
extern "C" void fn_802DBC04(int* destination, const int* source)
{
    *destination = *source;
}
} // namespace jj_fn_802DBC04

// fn_802DBC10: body from https://github.com/jj-link/brawl src/havok/fn_802DBC10.cpp (namespaced).
namespace jj_fn_802DBC10 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBC10)
extern "C" u32 fn_802DBC10(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DBC10

// fn_802DBC18: body from https://github.com/jj-link/brawl src/havok/fn_802DBC18.cpp (namespaced).
namespace jj_fn_802DBC18 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBC18)
extern "C" u32 fn_802DBC18(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DBC18

// fn_802DBC20: body from https://github.com/jj-link/brawl src/havok/fn_802DBC20.cpp (namespaced).
namespace jj_fn_802DBC20 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBC20)
extern "C" u32 fn_802DBC20(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x1c);
}
} // namespace jj_fn_802DBC20

// fn_802DBC28: body from https://github.com/jj-link/brawl src/havok/fn_802DBC28.cpp (namespaced).
namespace jj_fn_802DBC28 {
// TODO: cleanup (shape from jj-link/brawl fn_802DBC28)
extern "C" unsigned int fn_802DBC28(void) {
    return 0x01000000;
}
} // namespace jj_fn_802DBC28

// fn_802DBC30: body from https://github.com/jj-link/brawl src/havok/fn_802DBC30.cpp (namespaced).
namespace jj_fn_802DBC30 {
extern "C" {
int fn_802DABA0(void);
void fn_802DAB58(void);
void fn_802DAB8C(void);
extern char lbl_804121F0[];
extern unsigned int lbl_80532E50[4];
}

// TODO: cleanup (shape from jj-link/brawl fn_802DBC30)
extern "C" void fn_802DBC30(void) {
    int temp_r3 = fn_802DABA0();
    lbl_80532E50[0] = (unsigned int)lbl_804121F0;
    lbl_80532E50[1] = (unsigned int)fn_802DAB58;
    lbl_80532E50[2] = (unsigned int)fn_802DAB8C;
    lbl_80532E50[3] = (unsigned int)temp_r3;
}

__declspec(section ".ctors") extern void* const fn_802DBC30_ctor = (void*)fn_802DBC30;
} // namespace jj_fn_802DBC30
