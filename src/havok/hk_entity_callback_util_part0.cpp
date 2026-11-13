// Havok translation unit hk_entity_callback_util_part0.o (main.dol 0x802E3CD4-0x802E4044), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802E3CD4   124  fn_802E3CD4
//   0x802E3D50   256  fn_802E3D50
//   0x802E3E50   124  fn_802E3E50
//   0x802E3ECC   124  fireEntityDeleted__20hkEntityCallbackUtilFP8hkEntity
//   0x802E3F48   252  fn_802E3F48

#include <types.h>

// fn_802E3CD4: body from https://github.com/jj-link/brawl src/havok/fn_802E3CD4.cpp (namespaced).
namespace jj_fn_802E3CD4 {
struct Owner;

struct Item {
    virtual void v0();
    virtual void detach(Owner*);
};

struct Owner {
    u8 pad0[0x1CC];
    Item** items;
    s32 count;
};

// TODO: cleanup (shape from jj-link/brawl fn_802E3CD4)
extern "C" void fn_802E3CD4(Owner* self) {
    for (s32 i = self->count - 1; i >= 0; --i) {
        Item* item = self->items[i];
        if (item) {
            item->detach(self);
        }
    }
}
} // namespace jj_fn_802E3CD4

// fn_802E3D50: body from https://github.com/jj-link/brawl src/havok/fn_802E3D50.cpp (namespaced).
namespace jj_fn_802E3D50 {
struct Owner;

struct Item {
    virtual void v0();
    virtual void v1();
    virtual void remove(Owner*);
    virtual void detach(Owner*);
    virtual void v4();
    virtual void unlink(Owner*);
};

struct Owner {
    u8 pad0[0x6C];
    Item** listA;
    s32 countA;
    u8 pad74[4];
    Item** listB;
    s32 countB;
    u8 pad80[0x14C];
    Item** items;
    s32 count;
    u8 pad1D4[4];
    Item** listC;
    s32 countC;
};

// TODO: cleanup (shape from jj-link/brawl fn_802E3D50)
extern "C" void fn_802E3D50(Owner* self) {
    for (s32 i = self->count - 1; i >= 0; --i) {
        Item* item = self->items[i];
        if (item) {
            item->remove(self);
        }
    }

    while (self->countA != 0) {
        (*self->listA)->detach(self);
    }

    while (self->countB != 0) {
        (*self->listB)->detach(self);
    }

    while (self->countC != 0) {
        (*self->listC)->unlink(self);
    }
}
} // namespace jj_fn_802E3D50

// fn_802E3E50: body from https://github.com/jj-link/brawl src/havok/fn_802E3E50.cpp (namespaced).
namespace jj_fn_802E3E50 {
struct Owner;

struct Item {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void unlink(Owner*);
};

struct Owner {
    u8 pad0[0x1CC];
    Item** items;
    s32 count;
};

// TODO: cleanup (shape from jj-link/brawl fn_802E3E50)
extern "C" void fn_802E3E50(Owner* self) {
    for (s32 i = self->count - 1; i >= 0; --i) {
        Item* item = self->items[i];
        if (item) {
            item->unlink(self);
        }
    }
}
} // namespace jj_fn_802E3E50

// fn_802E3F48: body from https://github.com/jj-link/brawl src/havok/fn_802E3F48.cpp (namespaced).
namespace jj_fn_802E3F48 {
struct Manager;

struct Context {
    char pad[0xc];
    Manager* owner;
};

class Entry {
public:
    virtual void run(Context*);
};

struct Manager {
    char pad[0x1b4];
    Entry** entries;
    int count;

    void compact() {
        for (int i = count - 1; i >= 0; --i) {
            if (entries[i] == 0) {
                remove(i);
            }
        }
    }

    void remove(int index) {
        --count;
        for (; index < count; ++index) {
            entries[index] = entries[index + 1];
        }
    }
};

// TODO: cleanup (shape from jj-link/brawl fn_802E3F48)
extern "C" void fn_802E3F48(Manager* self, Context* context) {
    context->owner = self;
    for (int i = self->count - 1; i >= 0; --i) {
        Entry* entry = self->entries[i];
        if (entry) {
            entry->run(context);
        }
    }
    self->compact();
}
} // namespace jj_fn_802E3F48
