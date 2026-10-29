#pragma force_active off
// MATCH-ONLY: the size constructor is specialized below, so the generic one has to be defined out of the class.
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#include <so/so_array.h>
#include <so/templates/so_array_value_soAreaContactLog.h>
#include <new>

typedef soAreaContactLog VelaElm;
typedef soArrayVector<VelaElm, 44> VelaVec;

// The constructor takes a size and ignores it: the enemy area module, its caller, passes 0 and the vector starts empty.
template <>
inline VelaVec::soArrayVector(s32) : m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }

// MATCH-ONLY: constructing the vector forces its vtable and special members into this unit.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaVec(0); }
void vela_b(void* p) { new (p) VelaVec(1, 0); }
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
