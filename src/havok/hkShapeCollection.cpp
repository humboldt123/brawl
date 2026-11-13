#include <types.h>

// Function body below is taken from https://github.com/jj-link/brawl (src/havok/fn_802D0818.cpp).
// Null-checked upcast to the second base at +0xC (unnamed in the map).
// TODO: cleanup (shape from jj-link/brawl fn_802D0818)
extern "C" char* fn_802D0818(char* p) {
    return p ? p + 0xC : p;
}
