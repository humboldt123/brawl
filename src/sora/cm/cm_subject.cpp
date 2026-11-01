#include <types.h>
#include <cm/cm_subject.h>
#include <nw4r/ut.h>

extern nw4r::ut::LinkList<cmSubject, 0> lbl_8049DEDC;

cmSubject::~cmSubject() {
    if (m_isRegist == 1) {
        lbl_8049DEDC.Erase(this);
        m_isRegist = 0;
    }
}

cmSubject* cmSubjectList::getSubjectByPlayerNo(u32 playerNo) {
    cmSubject* found = NULL;
    for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = lbl_8049DEDC.GetBeginIter();
         it != lbl_8049DEDC.GetEndIter(); ++it) {
        u8 kind = it->m_stateB;
        if ((kind == 2 || kind == 3) && playerNo == it->m_playerNo) {
            found = &*it;
            break;
        }
    }
    return found;
}
