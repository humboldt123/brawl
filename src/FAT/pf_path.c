#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl.

int fn_803E72B4(int a, int b, int c);

int fn_803E6CDC(int a, int b) {
    return fn_803E72B4(a, b, 1) == 0;
}


int fn_803E7224(PF2_STR* path, PF2_STR* dir_path, PF2_STR* filename) {
    return fn_803E5F58(path, dir_path, filename, 0);
}

int fn_803E6E64(const char* sfn_name, const char* pattern) {
    return VFipf_strcmp(sfn_name, pattern) != 0;
}

int fn_803E722C(PF2_STR* path, PF2_STR* dir_path, PF2_STR* filename) {
    return fn_803E5F58(path, dir_path, filename, 1);
}
