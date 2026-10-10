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
