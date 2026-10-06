#include <DWC/dwc_ghttp.h>

extern void DWC_Printf(u32 level, const char* format, ...);
extern BOOL DWCi_IsError(void);
extern void DWC_Free(int kind, void* allocation, int size);
extern void fn_8037EA50(void);
extern void fn_8037EAA0(void);
extern void fn_8037EEDC(void);
extern const char lbl_80489310[];
extern const char lbl_80489320[];
extern DWCGHTTPParamEntry* lbl_805A0F68;
extern s32 lbl_805A0F6C;

BOOL DWC_InitGHTTP(void) {
    DWC_Printf(4, lbl_80489310);
    fn_8037EA50();
    ++lbl_805A0F6C;
    return 1;
}

BOOL DWC_ShutdownGHTTP(void) {
    DWC_Printf(4, lbl_80489320);
    if (lbl_805A0F6C > 0) {
        fn_8037EAA0();
        --lbl_805A0F6C;
        if (lbl_805A0F6C == 0) {
            DWCGHTTPParamEntry* entry = lbl_805A0F68;
            while (entry != NULL) {
                DWCGHTTPParamEntry* next = entry->next;
                if (entry->buffer != NULL) DWC_Free(6, entry->buffer, 0);
                DWC_Free(6, entry, 0);
                entry = next;
            }
            lbl_805A0F68 = NULL;
        }
    }
    return 1;
}

BOOL DWC_ProcessGHTTP(void) {
    BOOL error = DWCi_IsError();
    if (!error) fn_8037EEDC();
    return error == 0;
}

void GHTTPProgressCallback(s32 request, s32 state, const char* buffer, u32 bufferSize,
                           u32 received, u32 total, DWCGHTTPParamEntry* entry) {
    if (entry->progress != NULL) entry->progress(state, buffer, bufferSize, received, total, entry->parameter);
}

void DWCi_RemoveDWCGHTTPParamEntry(DWCGHTTPParamEntry* entry) {
    DWCGHTTPParamEntry* current = lbl_805A0F68;
    DWCGHTTPParamEntry* previous;
    if (current == NULL) return;
    if (current == entry) {
        DWCGHTTPParamEntry* next = current->next;
        DWC_Free(6, current, 0);
        lbl_805A0F68 = next;
        return;
    }
    do {
        previous = current;
        current = previous->next;
        if (current == NULL) return;
    } while (current != entry);
    previous->next = current->next;
    DWC_Free(6, current, 0);
}
