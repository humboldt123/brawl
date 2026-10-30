#pragma once

#include <so/so_array.h>

class soStatusUniqProcess;

// Keep the shared constructor declaration consistent with the article builders.
// The virtual methods and vtables remain external to these declarations.
#define SO_DECLARE_STATUS_PROC_VECTOR(N, CTOR_ARGS) \
    template <> \
    class soArrayVector<soStatusUniqProcess*, N> : public soArrayVectorAbstract<soStatusUniqProcess*> { \
        s32 m_topIndex : sizeof(bit_width<N>) + 1; \
        s32 m_lastIndex : sizeof(bit_width<N>) + 1; \
        s32 m_size : sizeof(bit_width<N>) + 1; \
        u32 m_isFull : 1; \
        soStatusUniqProcess* m_elements[N]; \
    public: \
        soArrayVector CTOR_ARGS; \
        virtual s32 size() const; \
        virtual ~soArrayVector(); \
        virtual s32 capacity() const; \
        virtual bool isFull() const; \
        virtual soStatusUniqProcess*& atFastAbstractSub(s32 index) const; \
        virtual soStatusUniqProcess*& getArrayValueConst(s32 index); \
        virtual s32 getTopIndex() const; \
        virtual s32 getLastIndex() const; \
        virtual void setSize(s32 size); \
        virtual void setTopIndex(s32 topIndex); \
        virtual void setLastIndex(s32 lastIndex); \
        virtual void onFull(); \
        virtual void offFull(); \
    };
