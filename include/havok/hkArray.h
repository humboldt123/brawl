#pragma once

#include <havok/hkBase.h>
#include <havok/hkThreadMemory.h>

// Untyped part of hkArray<T>: data pointer, element count, and capacity with flag bits.
struct hkArrayBase {
    enum {
        CAPACITY_MASK = 0x3FFFFFFF,
        FORCE_SIGN_FLAG = 0x40000000,
        DONT_DEALLOCATE_FLAG = 0x80000000,
    };

    void* m_data;            // 0x00
    int m_size;              // 0x04
    int m_capacityAndFlags;  // 0x08

    int getSize() const {
        return m_size;
    }
    bool mustDeallocate() const {
        return (m_capacityAndFlags & DONT_DEALLOCATE_FLAG) == 0;
    }
};

struct hkArrayUtil {
    static void _reserve(hkArrayBase* array, int newCapacity, int elemSize);
    static void _reserveMore(hkArrayBase* array, int elemSize);
    static void _reduce(hkArrayBase* array, int elemSize, void* buffer, int bufferCapacity);
};

// Element-wise copy (hkArrayUtil::copy in Havok); arguments are evaluated right to left.
template <typename T>
static inline void hkArray_copy(T* dst, const T* src, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

// Typed dynamic array on top of hkArrayBase (storage comes from the thread memory).
template <typename T>
struct hkArray : hkArrayBase {
    static void* operator new(unsigned long nbytes) {
        return hkMemory::getInstance().allocateChunk(nbytes, 0x15);
    }
    // Sized: the deleting destructor of an hkInplaceArray passes its own size (hkShapeCollectionAgent.o).
    static void operator delete(void* p, unsigned long nbytes) {
        hkMemory::getInstance().deallocateChunk(p, nbytes, 0x15);
    }

    hkArray() {
        m_data = 0;
        m_size = 0;
        m_capacityAndFlags = DONT_DEALLOCATE_FLAG;
    }
    // Wraps caller-owned storage (never deallocated).
    hkArray(T* ptr, int size, int capacity) {
        m_data = ptr;
        m_size = size;
        m_capacityAndFlags = capacity | DONT_DEALLOCATE_FLAG;
    }
    // Load-time constructor: the members already hold the packfile data, nothing is written.
    hkArray(hkFinishLoadedObjectFlag) {}
    T* begin() const {
        return (T*)m_data;
    }
    // Same as hkArrayBase::getSize; a member of each hkArray<T> (its out-of-line copies carry the element type,
    // e.g. hk_array_q218hk_linked_collidable14_collision_entry.o).
    int getSize() const {
        return m_size;
    }
    // The flag test lives in its own inline function: this keeps the capacity load from being
    // shared with the size computation, as in the original code (hkVersionUtil, hkVersionRegistry).
    bool mustDeallocate() const {
        return (m_capacityAndFlags & DONT_DEALLOCATE_FLAG) == 0;
    }
    // The flag is tested in place and the size taken through getCapacity(), which reloads the word (hkBvTreeAgent's
    // static queries, where copy propagation is off, keep the test as one clrrwi.).
    ~hkArray() {
        if ((m_capacityAndFlags & DONT_DEALLOCATE_FLAG) == 0) {
            hkThreadMemory::s_instance->deallocateChunk(m_data, getCapacity() * sizeof(T), 0x15);
        }
    }

    int getCapacity() const {
        return m_capacityAndFlags & CAPACITY_MASK;
    }
    hkArray<T>& operator=(const hkArray<T>& other) {
        if ((m_capacityAndFlags & CAPACITY_MASK) < other.m_size) {
            if (mustDeallocate()) {
                hkThreadMemory::s_instance->deallocateChunk(m_data, (m_capacityAndFlags & CAPACITY_MASK) * sizeof(T), 0x15);
            }
            m_data = hkThreadMemory::s_instance->allocateChunk(other.m_size * sizeof(T), 0x15);
            m_capacityAndFlags = other.m_size | (m_capacityAndFlags & FORCE_SIGN_FLAG);
        }
        m_size = other.m_size;
        hkArray_copy<T>((T*)m_data, (const T*)other.m_data, m_size);
        return *this;
    }
    void reserve(int n) {
        if (n > getCapacity()) {
            reserveSmart(n);
        }
    }
    void reserveSmart(int n) {
        int capacity = getCapacity();
        if (capacity < n) {
            int c = capacity * 2;
            hkArrayUtil::_reserve(this, n < c ? c : n, sizeof(T));
        }
    }
    // Grows the size by n and returns the first new element (contents are not initialised).
    T* expandBy(int n) {
        int oldSize = m_size;
        int newSize = oldSize + n;
        if (getCapacity() < newSize) {
            int c = getCapacity() * 2;
            int m = newSize;
            if (newSize < c) {
                m = c;
            }
            hkArrayUtil::_reserve(this, m, sizeof(T));
        }
        m_size = newSize;
        return (T*)m_data + oldSize;
    }
    // Sets the element count, growing the storage first when needed (new elements are not initialised).
    void setSize(int n) {
        reserveSmart(n);
        m_size = n;
    }
    // Opens a gap of numToInsert elements at index (the later elements move up) and returns its first element
    // (contents are not initialised).
    T* expandAt(int index, int numToInsert) {
        int newSize = m_size + numToInsert;
        int numToMove = m_size - index;
        reserve(newSize);
        T* dst = (T*)m_data + index + numToInsert;
        T* src = (T*)m_data + index;
        for (int k = numToMove - 1; k >= 0; k--) {
            dst[k] = src[k];
        }
        m_size = newSize;
        return (T*)m_data + index;
    }
    // Same without the capacity check (the caller reserved the room).
    T* expandByUnchecked(int n) {
        int oldSize = m_size;
        m_size = oldSize + n;
        return (T*)m_data + oldSize;
    }
    void swap(hkArray<T>& other);
    void insertAt(int i, const T& t);
    void insertAt(int i, const hkArray<T>& other);

    const T& operator[](int i) const {
        return ((const T*)m_data)[i];
    }
    // Appends an uninitialised element and returns it.
    T& expandOne() {
        if (m_size == getCapacity()) {
            hkArrayUtil::_reserveMore(this, sizeof(T));
        }
        return ((T*)m_data)[m_size++];
    }

    // Appends without checking the capacity (the caller has reserved it).
    void pushBackUnchecked(const T& t) {
        ((T*)m_data)[m_size++] = t;
    }
    void pushBack(const T& t) {
        if (m_size == getCapacity()) {
            hkArrayUtil::_reserveMore(this, sizeof(T));
        }
        ((T*)m_data)[m_size++] = t;
    }
    T& operator[](int i) {
        return ((T*)m_data)[i];
    }
    int indexOf(const T& t) const {
        for (int i = 0; i < m_size; i++) {
            if (((const T*)m_data)[i] == t) {
                return i;
            }
        }
        return -1;
    }
    // Removes the element at index; the last element takes its place (order not kept).
    void removeAt(int index) {
        m_size--;
        ((T*)m_data)[index] = ((T*)m_data)[m_size];
    }
    // Removes the element at index, moving the later elements down one place (order kept).
    void removeAtAndCopy(int index) {
        m_size--;
        for (int i = index; i < m_size; i++) {
            ((T*)m_data)[i] = ((T*)m_data)[i + 1];
        }
    }
};

// Out-of-line template members (not inline: they are instantiated as separate functions).
template <typename T>
void hkArray<T>::insertAt(int i, const T& t) {
    hkArray<T> one((T*)&t, 1, 1);
    insertAt(i, one);
}

template <typename T>
void hkArray<T>::insertAt(int i, const hkArray<T>& other) {
    int n = other.m_size;
    int newSize = n + m_size;
    int numToMove = m_size - i;
    reserve(newSize);
    T* src = (T*)m_data + i;
    T* dst = src + n;
    for (int k = numToMove - 1; k >= 0; k--) {
        dst[k] = src[k];
    }
    const T* from = (const T*)other.m_data;
    T* to = (T*)m_data + i;
    for (int k = n - 1; k >= 0; k--) {
        to[k] = from[k];
    }
    m_size = newSize;
}

template <typename T>
void hkArray<T>::swap(hkArray<T>& other) {
    void* d = m_data;
    m_data = other.m_data;
    other.m_data = d;
    int sz = m_size;
    m_size = other.m_size;
    other.m_size = sz;
    int cap = m_capacityAndFlags;
    m_capacityAndFlags = other.m_capacityAndFlags;
    other.m_capacityAndFlags = cap;
}

// Array with inline storage for N elements.
template <typename T, int N>
struct hkInplaceArray : hkArray<T> {
    T m_storage[N];

    hkInplaceArray(int size = 0) : hkArray<T>(m_storage, size, N) {}
};
