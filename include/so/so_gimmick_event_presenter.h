#pragma once

#include <so/event/so_gimmick_event_presenter.h>

class soModuleAccesser;

// Constructor and presentEventGimmick both store/load the full sender word at
// +0xC; the observer list and its two short IDs belong to the canonical base.
class soGimmickEventPresenter : public soEventPresenter<soGimmickEventObserver> {
public:
#ifdef SO_GIMMICK_EVENT_PRESENTER_INLINE
    // The sora_melee gimmick statuses expand the presenter in place; the Sonic article keeps an out-of-line copy.
    soGimmickEventPresenter(int manageID, int sendID)
        : soEventPresenter<soGimmickEventObserver>(manageID, static_cast<short>(3)), m_sendID(sendID) { }
    virtual ~soGimmickEventPresenter() { }
#else
    soGimmickEventPresenter(int manageID, int sendID);
    virtual ~soGimmickEventPresenter();
#endif
    void presentEventGimmick(soGimmickEventArgs*, soModuleAccesser*, int observerID);
    int m_sendID;
};
static_assert(sizeof(soGimmickEventPresenter) == 0x10, "Gimmick presenter size");
