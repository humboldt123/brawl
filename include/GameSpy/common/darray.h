#ifndef GAMESPY_COMMON_DARRAY_H
#define GAMESPY_COMMON_DARRAY_H

// GameSpy SDK dynamic array and hash table (darray.c / hashtable.c). The
// layouts are the SDK's DArrayImplementation and HashImplementation; our
// symbols do not name these functions yet, so the SDK names are noted next to
// each placeholder (HYPOTHESIS: by address order and behavior).
#include <types.h>
#include <GameSpy/common/gsPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GS_DARRAY_NOT_FOUND (-1)

typedef void (*ArrayElementFreeFn)(void* elem);
typedef int (*ArrayCompareFn)(const void* elem1, const void* elem2);
typedef void (*ArrayMapFn)(void* elem, void* clientData);
typedef int (*ArrayMapFn2)(void* elem, void* clientData);

typedef struct DArrayImplementation {
    int count;                       // 0x0
    int capacity;                    // 0x4
    int elemsize;                    // 0x8
    int growby;                      // 0xC
    ArrayElementFreeFn elemfreefn;   // 0x10
    void* list;                      // 0x14
} DArrayImplementation;
typedef DArrayImplementation* DArray;

typedef int (*TableHashFn)(const void* elem, int numBuckets);
typedef int (*TableCompareFn)(const void* elem1, const void* elem2);
typedef void (*TableElementFreeFn)(void* elem);
typedef void (*TableMapFn)(void* elem, void* clientData);
typedef int (*TableMapFn2)(void* elem, void* clientData);

typedef struct HashImplementation {
    DArray* buckets;                 // 0x0
    int nbuckets;                    // 0x4
    TableElementFreeFn freefn;       // 0x8
    TableHashFn hashfn;              // 0xC
    TableCompareFn compfn;           // 0x10
} HashImplementation;
typedef HashImplementation* HashTable;

__static_assert(sizeof(DArrayImplementation) == 0x18, "DArrayImplementation");
__static_assert(sizeof(HashImplementation) == 0x14, "HashImplementation");

DArray fn_8035A460(int elemSize, int numElemsToAllocate, ArrayElementFreeFn elemFreeFn);  // ArrayNew
void fn_8035A4FC(DArray array);                                                          // ArrayFree
int fn_8035A590(DArray array);                                                           // ArrayLength
void* fn_8035A598(DArray array, int n);                                                  // ArrayNth
void fn_8035A5C8(DArray array, const void* newElem);                                     // ArrayAppend
void fn_8035A6EC(DArray array, const void* newElem, ArrayCompareFn comparator);          // ArrayInsertSorted
void fn_8035A86C(DArray array, int n);                                                   // ArrayRemoveAt
void fn_8035A918(DArray array, int n);                                                   // ArrayDeleteAt
void fn_8035AA08(DArray array, const void* newElem, int n);                              // ArrayReplaceAt
void fn_8035AAC0(DArray array, ArrayCompareFn comparator);                               // ArraySort
int fn_8035AAD8(DArray array, const void* key, ArrayCompareFn comparator, int fromIndex, int isSorted); // ArraySearch
void fn_8035AC70(DArray array, ArrayMapFn fn, void* clientData);                         // ArrayMapBackwards
void* fn_8035AD0C(DArray array, ArrayMapFn2 fn, void* clientData);                       // ArrayMapBackwards2
void fn_8035ADB0(DArray array);                                                          // ArrayClear

HashTable fn_8035AEB4(int elemSize, int nBuckets, TableHashFn hashFn, TableCompareFn compFn, TableElementFreeFn freeFn); // TableNew
HashTable fn_8035AED0(int elemSize, int nBuckets, int nChains, TableHashFn hashFn, TableCompareFn compFn, TableElementFreeFn freeFn); // TableNew2
void fn_8035AF74(HashTable table);                                                       // TableFree
int fn_8035AFF0(HashTable table);                                                        // TableCount
void fn_8035B078(HashTable table, const void* newElem);                                  // TableEnter
int fn_8035B120(HashTable table, const void* delElem);                                   // TableRemove
void* fn_8035B1C4(HashTable table, const void* elemKey);                                 // TableLookup
void fn_8035B264(HashTable table, TableMapFn fn, void* clientData);                      // TableMapSafe
void* fn_8035B2D0(HashTable table, TableMapFn2 fn, void* clientData);                    // TableMapSafe2

#ifdef __cplusplus
}
#endif

#endif
