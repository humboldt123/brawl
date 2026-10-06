#pragma once
#include <types.h>
#include <stddef.h>

typedef void (*DWCCompletedCallback)(void* buffer, u32 size, s32 result, void* parameter);
typedef void (*DWCProgressCallback)(s32 state, const char* buffer, u32 bufferSize,
                                    u32 received, u32 total, void* parameter);

typedef struct DWCGHTTPParamEntry {
    void* parameter;
    DWCCompletedCallback completed;
    DWCProgressCallback progress;
    BOOL bufferClear;
    void* buffer;
    s32 request;
    struct DWCGHTTPParamEntry* next;
} DWCGHTTPParamEntry;

#ifdef __MWERKS__
typedef char DWCGHTTPParamEntrySizeCheck[sizeof(DWCGHTTPParamEntry) == 0x1C ? 1 : -1];
typedef char DWCGHTTPParamEntryBufferCheck[offsetof(DWCGHTTPParamEntry, buffer) == 0x10 ? 1 : -1];
typedef char DWCGHTTPParamEntryNextCheck[offsetof(DWCGHTTPParamEntry, next) == 0x18 ? 1 : -1];
#endif

BOOL DWC_InitGHTTP(void);
BOOL DWC_ShutdownGHTTP(void);
BOOL DWC_ProcessGHTTP(void);
void GHTTPProgressCallback(s32 request, s32 state, const char* buffer, u32 bufferSize,
                           u32 received, u32 total, DWCGHTTPParamEntry* entry);
void DWCi_RemoveDWCGHTTPParamEntry(DWCGHTTPParamEntry* entry);
