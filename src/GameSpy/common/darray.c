#include <types.h>
#include <stdlib.h>
#include <string.h>
#include <GameSpy/common/darray.h>

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own, and follow the GameSpy SDK's
// darray.c and hashtable.c. HYPOTHESIS: this unit holds both files; inside
// darray.c ArrayNth is inlined, while hashtable.c calls it.
// Checked: without this pragma 9 functions in this unit stop matching.
#pragma auto_inline off

static inline void* ArrayNthInline(DArray array, int n) {
    if (n < 0 || n >= array->count) {
        return NULL;
    }
    return (char*)array->list + array->elemsize * n;
}

DArray fn_8035A460(int elemSize, int numElemsToAllocate, ArrayElementFreeFn elemFreeFn) {
    DArray array;

    array = fn_8035DFE8(sizeof(DArrayImplementation));
    if (numElemsToAllocate == 0) {
        numElemsToAllocate = 8;
    }
    array->count = 0;
    array->capacity = numElemsToAllocate;
    array->elemsize = elemSize;
    array->growby = numElemsToAllocate;
    array->elemfreefn = elemFreeFn;
    if (array->capacity != 0) {
        array->list = fn_8035DFE8(array->capacity * array->elemsize);
    } else {
        array->list = NULL;
    }
    return array;
}

int fn_8035A590(DArray array) {
    return array->count;
}

void* fn_8035A598(DArray array, int n) {
    if (n < 0 || n >= array->count) {
        return NULL;
    }
    return (char*)array->list + array->elemsize * n;
}

void fn_8035A4FC(DArray array) {
    int i;

    for (i = 0; i < array->count; i++) {
        if (array->elemfreefn) {
            array->elemfreefn(ArrayNthInline(array, i));
        }
    }
    fn_8035E00C(array->list);
    fn_8035E00C(array);
}

static inline void ArrayGrow(DArray array) {
    array->capacity += array->growby;
    array->list = fn_8035DFF8(array->list, array->capacity * array->elemsize);
}

static inline void SetElement(DArray array, const void* elem, int pos) {
    memcpy(ArrayNthInline(array, pos), elem, array->elemsize);
}

static inline void ArrayInsertAtInline(DArray array, const void* newElem, int n) {
    if (array->count == array->capacity) {
        ArrayGrow(array);
    }
    array->count++;
    if (n < array->count - 1) {
        memmove(ArrayNthInline(array, n + 1), ArrayNthInline(array, n), (array->count - 1 - n) * array->elemsize);
    }
    SetElement(array, newElem, n);
}

void fn_8035A5C8(DArray array, const void* newElem) {
    if (array) {
        ArrayInsertAtInline(array, newElem, array->count);
    }
}

static inline void* mybsearch(const void* elem, void* base, int num, int elemsize, ArrayCompareFn comparator, int* found) {
    int L, H, I, C;

    L = 0;
    H = num - 1;
    *found = 0;
    while (L <= H) {
        I = (L + H) >> 1;
        C = comparator((char*)base + I * elemsize, elem);
        if (C == 0) {
            *found = 1;
        }
        if (C < 0) {
            L = I + 1;
        } else {
            H = I - 1;
        }
    }
    return (char*)base + L * elemsize;
}

void fn_8035A6EC(DArray array, const void* newElem, ArrayCompareFn comparator) {
    int n;
    void* res;
    int found;

    res = mybsearch(newElem, array->list, array->count, array->elemsize, comparator, &found);
    n = ((char*)res - (char*)array->list) / array->elemsize;
    ArrayInsertAtInline(array, newElem, n);
}

void fn_8035A86C(DArray array, int n) {
    if (n < array->count - 1) {
        memmove(ArrayNthInline(array, n), ArrayNthInline(array, n + 1), array->elemsize * (array->count - 1 - n));
    }
    array->count--;
}

static inline void ArrayDeleteAtInline(DArray array, int n) {
    if (array->elemfreefn) {
        array->elemfreefn(ArrayNthInline(array, n));
    }
    if (n < array->count - 1) {
        memmove(ArrayNthInline(array, n), ArrayNthInline(array, n + 1), array->elemsize * (array->count - 1 - n));
    }
    --array->count;
}

void fn_8035A918(DArray array, int n) {
    ArrayDeleteAtInline(array, n);
}

void fn_8035AA08(DArray array, const void* newElem, int n) {
    if (array->elemfreefn) {
        array->elemfreefn(ArrayNthInline(array, n));
    }
    memcpy(ArrayNthInline(array, n), newElem, array->elemsize);
}

void fn_8035AAC0(DArray array, ArrayCompareFn comparator) {
    qsort(array->list, array->count, array->elemsize, comparator);
}

void fn_8035AC70(DArray array, ArrayMapFn fn, void* clientData) {
    int i;

    for (i = array->count - 1; i >= 0; i--) {
        fn(ArrayNthInline(array, i), clientData);
    }
}

void* fn_8035AD0C(DArray array, ArrayMapFn2 fn, void* clientData) {
    void* elem;
    int i;

    for (i = array->count - 1; i >= 0; i--) {
        elem = ArrayNthInline(array, i);
        if (fn(elem, clientData) == 0) {
            return elem;
        }
    }
    return NULL;
}

void fn_8035ADB0(DArray array) {
    int i;

    for (i = array->count - 1; i >= 0; --i) {
        ArrayDeleteAtInline(array, i);
    }
}

HashTable fn_8035AEB4(int elemSize, int nBuckets, TableHashFn hashFn, TableCompareFn compFn, TableElementFreeFn freeFn) {
    return fn_8035AED0(elemSize, nBuckets, 4, hashFn, compFn, freeFn);
}

HashTable fn_8035AED0(int elemSize, int nBuckets, int nChains, TableHashFn hashFn, TableCompareFn compFn, TableElementFreeFn freeFn) {
    HashTable table;
    int i;

    table = fn_8035DFE8(sizeof(HashImplementation));
    table->buckets = fn_8035DFE8(nBuckets * sizeof(DArray));
    for (i = 0; i < nBuckets; i++) {
        table->buckets[i] = fn_8035A460(elemSize, nChains, freeFn);
    }
    table->nbuckets = nBuckets;
    table->freefn = freeFn;
    table->compfn = compFn;
    table->hashfn = hashFn;
    return table;
}

void fn_8035AF74(HashTable table) {
    int i;

    if (table == NULL) {
        return;
    }
    for (i = 0; i < table->nbuckets; i++) {
        fn_8035A4FC(table->buckets[i]);
    }
    fn_8035E00C(table->buckets);
    fn_8035E00C(table);
}

int fn_8035AFF0(HashTable table) {
    int i;
    int count = 0;

    if (table == NULL) {
        return 0;
    }
    for (i = 0; i < table->nbuckets; i++) {
        count += fn_8035A590(table->buckets[i]);
    }
    return count;
}

void fn_8035B078(HashTable table, const void* newElem) {
    int hash;
    int itempos;

    if (table == NULL) {
        return;
    }
    hash = table->hashfn(newElem, table->nbuckets);
    itempos = fn_8035AAD8(table->buckets[hash], newElem, table->compfn, 0, 0);
    if (itempos == GS_DARRAY_NOT_FOUND) {
        fn_8035A5C8(table->buckets[hash], newElem);
    } else {
        fn_8035AA08(table->buckets[hash], newElem, itempos);
    }
}

int fn_8035B120(HashTable table, const void* delElem) {
    int hash;
    int itempos;

    if (table == NULL) {
        return 0;
    }
    hash = table->hashfn(delElem, table->nbuckets);
    itempos = fn_8035AAD8(table->buckets[hash], delElem, table->compfn, 0, 0);
    if (itempos == GS_DARRAY_NOT_FOUND) {
        return 0;
    }
    fn_8035A918(table->buckets[hash], itempos);
    return 1;
}

void* fn_8035B1C4(HashTable table, const void* elemKey) {
    int hash;
    int itempos;

    if (table == NULL) {
        return NULL;
    }
    hash = table->hashfn(elemKey, table->nbuckets);
    itempos = fn_8035AAD8(table->buckets[hash], elemKey, table->compfn, 0, 0);
    if (itempos == GS_DARRAY_NOT_FOUND) {
        return NULL;
    }
    return fn_8035A598(table->buckets[hash], itempos);
}

void fn_8035B264(HashTable table, TableMapFn fn, void* clientData) {
    int i;

    for (i = 0; i < table->nbuckets; i++) {
        fn_8035AC70(table->buckets[i], fn, clientData);
    }
}

void* fn_8035B2D0(HashTable table, TableMapFn2 fn, void* clientData) {
    int i;
    void* elem;

    for (i = 0; i < table->nbuckets; i++) {
        elem = fn_8035AD0C(table->buckets[i], fn, clientData);
        if (elem != NULL) {
            return elem;
        }
    }
    return NULL;
}
