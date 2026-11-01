// mem_frameHeap: bump allocator that grows from both ends of a block, with a stack of recorded states
// that can be rolled back.
#include "revolution/mem/heapCommon.h"
#include "revolution/os.h"

#define MEM_FRM_HEAP_MAGIC 'FRMH'

#define MEM_FRM_HEAP_FREE_TO_HEAD 1
#define MEM_FRM_HEAP_FREE_TO_TAIL 2

typedef struct MEMiFrmHeapState MEMiFrmHeapState;

struct MEMiFrmHeapState {
    u32 id;
    void* head;
    void* tail;
    MEMiFrmHeapState* next;
};

typedef struct MEMiFrmHeapHead {
    void* head;
    void* tail;
    MEMiFrmHeapState* states;
} MEMiFrmHeapHead;

// size of the shared head plus the frame head
#define MEM_FRM_HEAP_HEAD_SIZE (sizeof(MEMiHeapHead) + sizeof(MEMiFrmHeapHead))
// BYTES: retail compares against 0x48, not head size + 4
#define MEM_FRM_HEAP_MIN_SIZE 0x48

static MEMiFrmHeapHead* GetFrmHeapHeadPtrFromHeapHead_(const MEMiHeapHead* heap) {
    return AddU32ToPtr((void*)heap, sizeof(MEMiHeapHead));
}

static MEMiHeapHead* GetHeapHeadPtrFromFrmHeapHead_(const MEMiFrmHeapHead* frm) {
    return SubU32ToPtr((void*)frm, sizeof(MEMiHeapHead));
}

static MEMiHeapHead* InitFrameHeap_(MEMiHeapHead* heap, void* end, u16 opt) {
    MEMiFrmHeapHead* frm = GetFrmHeapHeadPtrFromHeapHead_(heap);
    MEMiInitHeapHead(heap, MEM_FRM_HEAP_MAGIC, AddU32ToPtr(heap, MEM_FRM_HEAP_HEAD_SIZE), end, opt);
    frm->head = heap->heapStart;
    frm->tail = heap->heapEnd;
    frm->states = NULL;
    return heap;
}

static void* AllocFromHead_(MEMiFrmHeapHead* frm, u32 size, s32 align) {
    void* start;
    void* end;

    start = RoundUpPtr(frm->head, align);
    end = AddU32ToPtr(start, size);

    // out of room
    if (end > frm->tail) {
        return NULL;
    }

    FillAllocMemory(GetHeapHeadPtrFromFrmHeapHead_(frm), frm->head, GetOffsetFromPtr(frm->head, end));
    frm->head = end;

    return start;
}

static void* AllocFromTail_(MEMiFrmHeapHead* frm, u32 size, s32 align) {
    void* start;

    start = SubU32ToPtr(frm->tail, size);
    start = RoundDownPtr(start, align);

    if (start < frm->head) {
        return NULL;
    }

    FillAllocMemory(GetHeapHeadPtrFromFrmHeapHead_(frm), start, GetOffsetFromPtr(start, frm->tail));
    frm->tail = start;

    return start;
}

static void FreeHead_(MEMiHeapHead* heap) {
    MEMiFrmHeapHead* frm;

    frm = GetFrmHeapHeadPtrFromHeapHead_(heap);
    frm->head = heap->heapStart;
    frm->states = NULL;
}

static void FreeTail_(MEMiHeapHead* heap) {
    MEMiFrmHeapState* state;
    MEMiFrmHeapHead* frm;

    frm = GetFrmHeapHeadPtrFromHeapHead_(heap);
    for (state = frm->states; state != NULL; state = state->next) {
        state->tail = heap->heapEnd;
    }

    frm->tail = heap->heapEnd;
}

MEMiHeapHead* MEMCreateFrmHeapEx(void* start, u32 size, u16 opt) {
    void* end = AddU32ToPtr(start, size);

    end = RoundDownPtr(end, 4);
    start = RoundUpPtr(start, 4);

    if (GetUIntPtr(start) > GetUIntPtr(end) || GetOffsetFromPtr(start, end) < MEM_FRM_HEAP_MIN_SIZE) {
        return NULL;
    }

    return InitFrameHeap_(start, end, opt);
}

MEMiHeapHead* MEMDestroyFrmHeap(MEMiHeapHead* heap) {
    MEMiFinalizeHeap(heap);
    return heap;
}

void* MEMAllocFromFrmHeapEx(MEMiHeapHead* heap, u32 size, s32 align) {
    MEMiFrmHeapHead* frm;
    void* memBlock;

    frm = GetFrmHeapHeadPtrFromHeapHead_(heap);

    if (size == 0) {
        size = 1;
    }
    size = RoundUp(size, 4);

    LockHeap(heap);
    // the sign of the alignment picks the end to allocate from
    if (align >= 0) {
        memBlock = AllocFromHead_(frm, size, align);
    } else {
        memBlock = AllocFromTail_(frm, size, -align);
    }
    UnlockHeap(heap);

    return memBlock;
}

void MEMFreeToFrmHeap(MEMiHeapHead* heap, u32 flags) {
    LockHeap(heap);
    if (flags & MEM_FRM_HEAP_FREE_TO_HEAD) {
        FreeHead_(heap);
    }
    if (flags & MEM_FRM_HEAP_FREE_TO_TAIL) {
        FreeTail_(heap);
    }
    UnlockHeap(heap);
}

u32 MEMGetAllocatableSizeForFrmHeapEx(MEMiHeapHead* heap, s32 align) {
    MEMiFrmHeapHead* frm;
    BOOL enabled;
    void* start;
    u32 size;

    align = __abs(align);

    enabled = OSDisableInterrupts();
    frm = GetFrmHeapHeadPtrFromHeapHead_(heap);

    start = RoundUpPtr(frm->head, align);

    if (start > frm->tail) {
        size = 0;
    } else {
        size = GetOffsetFromPtr(start, frm->tail);
    }

    OSRestoreInterrupts(enabled);
    return size;
}

BOOL MEMRecordStateForFrmHeap(MEMiHeapHead* heap, u32 id) {
    MEMiFrmHeapState* state;
    MEMiFrmHeapHead* frm;
    BOOL success;
    u8* head;

    LockHeap(heap);
    frm = GetFrmHeapHeadPtrFromHeapHead_(heap);

    // the allocation below moves the head, keep the old one
    head = frm->head;
    state = AllocFromHead_(frm, sizeof(MEMiFrmHeapState), 4);

    if (state == NULL) {
        success = FALSE;
    } else {
        state->id = id;
        state->head = head;
        state->tail = frm->tail;

        state->next = frm->states;
        frm->states = state;

        success = TRUE;
    }

    UnlockHeap(heap);
    return success;
}

BOOL MEMFreeByStateToFrmHeap(MEMiHeapHead* heap, u32 id) {
    MEMiFrmHeapState* state;
    MEMiFrmHeapHead* frm;
    BOOL success;

    LockHeap(heap);
    frm = GetFrmHeapHeadPtrFromHeapHead_(heap);
    state = frm->states;

    // id 0 means the most recent state
    if (id != 0) {
        for (; state != NULL; state = state->next) {
            if (state->id == id) {
                break;
            }
        }
    }

    if (state == NULL) {
        success = FALSE;
    } else {
        frm->head = state->head;
        frm->tail = state->tail;
        frm->states = state->next;

        success = TRUE;
    }

    UnlockHeap(heap);
    return success;
}
