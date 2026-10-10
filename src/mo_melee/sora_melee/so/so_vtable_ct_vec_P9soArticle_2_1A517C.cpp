#pragma force_active off
#include <so/article/so_article_array.h>
#include <new>

soArrayVector<soArticle*, 2>::soArrayVector(s32) :
    m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }

typedef soArticle* VelaElm;
typedef soArrayVector<VelaElm, 2> VelaVec;

// MATCH-ONLY: constructing the vector forces its vtable and special members into this unit.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaVec(); }
void vela_b(void* p) { new (p) VelaVec(1, 0); }
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
