// Havok translation unit hkSolverCheck.o (main.dol 0x8029F098-0x8029F334).
// Functions in address order (method names from the Havok TU map):
//   0x8029F098    48  findChar   [map: hkSolverCheck__findChar]
//   0x8029F0C8   620  processFlyingColors   [map: hkSolverCheck__processFlyingColors]

// Returns the first occurrence of c in s (including the terminator position is not searched), or 0.
const char* findChar(const char* s, char c) {
    while (*s != c) {
        if (*s == 0) {
            return 0;
        }
        s++;
    }
    return s;
}

// Not yet decompiled in this unit:
//   0x8029F0C8   620  processFlyingColors   [map: hkSolverCheck__processFlyingColors]
