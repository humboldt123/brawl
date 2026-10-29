#pragma once

// soArrayQueueImpl<V>: a queue over the array V it holds (the status change requests of the status modules, the AI
// history of the enemies). The same class as in ft/builder/ft_builder_status.h, which the units that include em/enemy.h
// cannot include (it pulls in the fighters' list of sora_melee array instances). TODO: have ft_builder_status.h include
// this header instead.
// soArrayQueue<T> is its base (module map: soArrayQueue<emAIInfo>::unshift, a weak copy of the enemy AI unit): the
// array pointer and the queue operations on it.

#include <so/so_array.h>
#include <types.h>

template <typename T>
class soArrayQueue {
protected:
    soArray<T>* m_array;
public:
    soArrayQueue(soArray<T>* array) : m_array(array) { }
    soArray<T>* getArray() { return m_array; }
    // Pushes element at the front, dropping the last element when the array is full.
#ifdef SO_ARRAY_QUEUE_UNSHIFT_IN_CLASS
    // Opt-in (the enemy AI module): defined in the class, the weak copy comes right after its first user, as in the
    // original; an out-of-class template member is emitted at the end of the unit.
    void unshift(const T& element) {
        if (m_array->isFull() == true) {
            m_array->pop();
        }
        m_array->unshift(element);
    }
#else
    void unshift(const T& element);
#endif
};

#ifndef SO_ARRAY_QUEUE_UNSHIFT_IN_CLASS
template <typename T>
void soArrayQueue<T>::unshift(const T& element) {
    if (m_array->isFull() == true) {
        m_array->pop();
    }
    m_array->unshift(element);
}
#endif

// The element type of an array class (also for the explicit soArrayVector specializations, which have no value_type).
template <typename V>
struct soArrayQueueElement;
template <typename T, s32 C>
struct soArrayQueueElement<soArrayVector<T, C> > {
    typedef T type;
};

template <typename V>
class soArrayQueueImpl : public soArrayQueue<typename soArrayQueueElement<V>::type> {
    V m_vector;
public:
    soArrayQueueImpl(const s32& initial)
        : soArrayQueue<typename soArrayQueueElement<V>::type>(&m_vector), m_vector(1, initial, 0) {
        soArray<s32>* array = &m_vector;
        array->capacity();
    }
    soArrayQueueImpl() : soArrayQueue<typename soArrayQueueElement<V>::type>(&m_vector), m_vector(0) {
        soArray<typename soArrayQueueElement<V>::type>* array = &m_vector;
        array->capacity();
    }
    template <class E>
    soArrayQueueImpl(s32 size, const E& initial)
        : soArrayQueue<typename soArrayQueueElement<V>::type>(&m_vector), m_vector(size, initial, 0) {
        soArray<E>* array = &m_vector;
        array->capacity();
    }
};
