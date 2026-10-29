#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <sr/sr_common.h>
#include <types.h>

namespace nw4r {
namespace snd {
class MemorySoundArchive;
class SoundArchivePlayer;
} // namespace snd
} // namespace nw4r

// HYPOTHESIS: the Animalese voice player of Doubutsugo.o (map); its members are still fn_.
class Doubutsugo;

class sndDoubutsugoTask : public gfTask {
public:
    char _40[0x4];
    nw4r::snd::MemorySoundArchive* m_archive;  // 0x44
    nw4r::snd::SoundArchivePlayer* m_player;   // 0x48
    void* m_playerBuffer;                      // 0x4C
    void* m_streamBuffer;                      // 0x50
    Doubutsugo* m_doubutsugo;                  // 0x54
    bool m_isActive;                           // 0x58
    // HYPOTHESIS: a word list ended by 0xD3, as the Animalese player reads it.
    s16 m_words[32];                           // 0x5A
    u32 unk9C;                                 // 0x9C

    sndDoubutsugoTask(const void* archiveData, Heaps::HeapType heapType);
    virtual void processDefault();
    virtual ~sndDoubutsugoTask();

    // HYPOTHESIS: word types from the halfword word list.
    void playWord(s16 word);
    // HYPOTHESIS: a non-const list; each word is reloaded after its store, as for a pointer that may alias m_words.
    void playSentence(s16* words);
    void playPageSE();
};
static_assert(sizeof(sndDoubutsugoTask) == 0xA0, "Class is wrong size!");
