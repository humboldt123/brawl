#pragma once

#include <so/event/so_event_system.h>

class soEventUnitPureNull : public soEventUnit {
public:
    virtual bool isNull() const;
    virtual void eraseObserver(s16 id, s16 sendID);
    virtual s16 getEventId();
    virtual u32 getObserverCapacity(u32 index);
    virtual u32 getObserverSize(u32 index);
    virtual ~soEventUnitPureNull();
};
