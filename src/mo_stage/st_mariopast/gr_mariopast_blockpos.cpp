#include <types.h>

// MATCH-ONLY: this file only holds two small objects with a constructor (nothing reads them); the class that used to live
// here is not part of the game any more.
struct grMarioPastBlockPosDummy {
    int m_a;
    int m_b;
    grMarioPastBlockPosDummy(int a, int b) {
        m_a = a;
        m_b = b;
    }
};
static grMarioPastBlockPosDummy sDummyA(0xFF, 0);
static grMarioPastBlockPosDummy sDummyB(0xFF, 1);

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void fn_48_3500() {}
u8 fn_48_350C(u8* p) { return *(u8*)(p + 0x44); }
void fn_48_3618() {}
} // extern "C"
