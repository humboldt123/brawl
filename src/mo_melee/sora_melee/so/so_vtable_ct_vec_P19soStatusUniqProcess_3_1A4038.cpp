#pragma force_active off
#include <so/status/so_status_proc_vector.h>
#include <new>

// Native article builders pass a zero word; the constructor ignores it and
// leaves all counts empty. Keep the vector's shared virtual methods external.
// MATCH-ONLY: retain the original out-of-line constructor call.
SO_DECLARE_STATUS_PROC_VECTOR(3, (s32) __attribute__((never_inline)))

inline soArrayVector<soStatusUniqProcess*, 3>::soArrayVector(s32) :
    m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }

typedef soArrayVector<soStatusUniqProcess*, 3> VelaVec;

// MATCH-ONLY: require the weak constructor; this unused caller is stripped.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaVec(0); }
#pragma dont_inline reset
