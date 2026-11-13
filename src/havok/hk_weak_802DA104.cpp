// Havok translation unit hk_weak_802DA104.o (main.dol 0x802DA104-0x802DA49C), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802DA104   140  fn_802DA104
//   0x802DA190   292  fn_802DA190
//   0x802DA2B4    20  fn_802DA2B4
//   0x802DA2C8   160  fn_802DA2C8
//   0x802DA368   200  fn_802DA368
//   0x802DA430   108  fn_802DA430

#include <types.h>
#include <havok/hkString.h>

// fn_802DA104: body from https://github.com/jj-link/brawl src/havok/fn_802DA104.cpp (namespaced).
#include <havok/hkThreadMemory.h>
namespace jj_fn_802DA104 {
typedef unsigned char u8;
typedef short s16;
typedef unsigned int u32;

extern "C" void* fn_8027EA20(void*, unsigned int, int);

// TODO: cleanup (shape from jj-link/brawl fn_802DA104)
extern "C" void fn_802DA104(int arg0) {
    unsigned int size = (arg0 * 0x34 + 0x3F) & ~0xF;
    u8* p = (u8*)hkThreadMemory::s_instance->allocateChunk(size, 4);
    *(s16*)(p + 0x8) = 0;
    *(s16*)(p + 0xA) = 3;
    *(u32*)(p + 0xC) = 0;
    *(u32*)(p + 0x10) = 0;
    *(u32*)(p + 0x14) = 0;
    *(u32*)(p + 0x18) = 0;
    *(u32*)(p + 0x1C) = 0;
    *(u32*)(p + 0x20) = 0;
    *(u32*)(p + 0x24) = 0;
    *(s16*)(p + 0x0) = 0x16;
    *(s16*)(p + 0x2) = size;
    *(s16*)(p + 0x4) = 0;
    *(s16*)(p + 0x6) = arg0;
}
} // namespace jj_fn_802DA104

// fn_802DA190: body from https://github.com/jj-link/brawl src/havok/fn_802DA190.cpp (namespaced).
namespace jj_fn_802DA190 {
typedef unsigned short u16;
typedef unsigned char u8;
typedef float f32;
typedef int s32;

struct ElemA {
    f32 v[8];
    ElemA& operator=(const ElemA& o) {
        v[0] = o.v[0]; v[1] = o.v[1]; v[2] = o.v[2]; v[3] = o.v[3];
        v[4] = o.v[4]; v[5] = o.v[5]; v[6] = o.v[6]; v[7] = o.v[7];
        return *this;
    }
};

struct ElemB {
    f32 a;
    f32 b;
    s32 c;
    u16 d;
    u8 e;
    u8 f;
    f32 g;
    ElemB& operator=(const ElemB& o) {
        a = o.a; b = o.b; c = o.c; d = o.d; e = o.e; f = o.f; g = o.g;
        return *this;
    }
};

struct Info {
    s32 w[7];
};

struct Buf {
    s32 unk0;
    u16 count;
    u16 capacity;
    u16 unk8;
    u16 unkA;
    Info info;
    u8 data[1];
};

// TODO: cleanup (shape from jj-link/brawl fn_802DA190)
extern "C" void fn_802DA190(Buf* dst, Buf* src) {
    dst->unk8 = src->unk8;
    ElemA* dA = (ElemA*)(((unsigned long)dst + 0x37) & ~0xF);
    ElemB* dB = (ElemB*)((u8*)dA + (dst->capacity << 5));
    ElemA* sA = (ElemA*)(((unsigned long)src + 0x37) & ~0xF);
    dst->unkA = src->unkA;
    dst->info = src->info;
    dst->count = src->count;
    ElemB* sB = (ElemB*)((u8*)sA + (src->capacity << 5));
    for (int i = 0; i < src->count; i++) {
        *dA++ = *sA++;
        *dB++ = *sB++;
    }
}
} // namespace jj_fn_802DA190

// fn_802DA2B4: body from https://github.com/jj-link/brawl src/havok/fn_802DA2B4.cpp (namespaced).
#include <havok/hkThreadMemory.h>
namespace jj_fn_802DA2B4 {
// main/sora_minigame - fn_802DA2B4

struct S0 {
    unsigned short pad0;
    unsigned short unk2;
};

extern "C" {
void fn_8027EABC(void*, void*, unsigned int, int);

// TODO: cleanup (shape from jj-link/brawl fn_802DA2B4)
extern "C" void fn_802DA2B4(S0* arg0) {
    hkThreadMemory::s_instance->deallocateChunk(arg0, arg0->unk2, 4);
}
}
} // namespace jj_fn_802DA2B4

// fn_802DA2C8: body from https://github.com/jj-link/brawl src/havok/fn_802DA2C8.cpp (namespaced).
namespace jj_fn_802DA2C8 {
typedef unsigned short u16;

struct S0 {
    u16 pad0;
    u16 pad2;
    u16 unk4;
    u16 unk6;
};

extern "C" {
extern void* fn_802DA104(int);
extern void fn_802DA190(void*, void*);
extern void fn_802DA2B4(void*);
extern void fn_80281D64(int, int, int, int);

// TODO: cleanup (shape from jj-link/brawl fn_802DA2C8)
extern "C" void* fn_802DA2C8(void* arg0) {
    S0* ptr = (S0*)arg0;
    int val = ptr->unk4 + 1;
    ptr->unk4 = val;

    if ((u16)val > ptr->unk6) {
        ptr = (S0*)fn_802DA104((val * 2) & 0x1FFFE);
        fn_802DA190(ptr, arg0);
        fn_802DA2B4(arg0);
    }

    int aligned = ((int)ptr + 0x37) & 0xFFFFFFF0;
    int offset = aligned + (ptr->unk6 << 5);
    hkString::memSet((void*)((ptr->unk4 - 1) * 0x14 + offset), 0, 0x14);
    return ptr;
}
}
} // namespace jj_fn_802DA2C8

// fn_802DA430: body from https://github.com/jj-link/brawl src/havok/fn_802DA430.cpp (namespaced).
namespace jj_fn_802DA430 {
extern "C" void* fn_802DA104(int);
extern "C" void fn_802DA190(void*, void*);
extern "C" void fn_802DA2B4(void*);

struct S0 {
    unsigned char pad[4];
    unsigned short unk4;
    unsigned short unk6;
};

// TODO: cleanup (shape from jj-link/brawl fn_802DA430)
extern "C" void* fn_802DA430(void* arg0, int arg1) {
    S0* p = (S0*)arg0;
    void* var_r31;
    unsigned short temp_r3;

    var_r31 = arg0;
    temp_r3 = p->unk6;
    if ((int)((p->unk4 + arg1) * 2) <= (int)temp_r3) {
        var_r31 = fn_802DA104((temp_r3 >> 1) & 0x7FFF);
        fn_802DA190(var_r31, arg0);
        fn_802DA2B4(arg0);
    }
    return var_r31;
}
} // namespace jj_fn_802DA430
