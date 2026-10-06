#include <types.h>
#include <stdio.h>
#include <string.h>

extern const char lbl_80489450[];

s32 DWC_SetCommonKeyValueString(const char* key, const char* value, char* text, char delimiter) {
    snprintf(text, 0x1000, lbl_80489450, delimiter, key, delimiter, value);
    return strlen(text);
}

s32 DWC_AddCommonKeyValueString(const char* key, const char* value, char* text, char delimiter) {
    char* end = strchr(text, 0);
    snprintf(end, 0x1000, lbl_80489450, delimiter, key, delimiter, value);
    (void)strlen(end);
    return strlen(text);
}

s32 DWC_GetCommonValueString(const char* key, char* value, const char* text, char delimiter) {
    const char* field;
    if (value == NULL) return -1;
    for (field = strchr(text, delimiter); field != NULL; field = strchr(field + 1, delimiter)) {
        if (strncmp(field + 1, key, strlen(key)) == 0 && field[strlen(key) + 1] == delimiter) {
            const char* end;
            u32 length;
            field = strchr(field + 1, delimiter);
            if (field == NULL) return -1;
            end = strchr(field + 1, delimiter);
            length = end == NULL ? strlen(field + 1) : end - (field + 1);
            strncpy(value, field + 1, length);
            value[length] = 0;
            return length;
        }
        field = strchr(field + 1, delimiter);
        if (field == NULL) return -1;
    }
    return -1;
}
