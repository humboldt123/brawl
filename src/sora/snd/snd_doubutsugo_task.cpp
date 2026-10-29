#include <gf/gf_heap_manager.h>
#include <nw4r/snd/snd_MemorySoundArchive.h>
#include <nw4r/snd/snd_SoundArchivePlayer.h>
#include <snd/snd_doubutsugo_task.h>
#include <snd/snd_system.h>
#include <sr/sr_common.h>
#include <types.h>

// HYPOTHESIS: stand-ins for Doubutsugo (Doubutsugo.o) and sndSystem::getBalanceSE, which are still fn_.
extern "C" Doubutsugo* fn_803EEFCC(void* doubutsugo);
extern "C" void fn_803EF044(Doubutsugo* doubutsugo, s32 deleteSelf);
extern "C" void fn_803EF108(Doubutsugo* doubutsugo, nw4r::snd::SoundArchivePlayer* player);
extern "C" bool fn_803EF174(Doubutsugo* doubutsugo);
extern "C" void fn_803EF188(Doubutsugo* doubutsugo);
extern "C" void fn_803EF1D0(Doubutsugo* doubutsugo);
extern "C" void fn_803EF2AC(Doubutsugo* doubutsugo, const s16* words);
extern "C" void fn_803EF3CC(Doubutsugo* doubutsugo, const void* script);
extern "C" void fn_803EF478(Doubutsugo* doubutsugo, float volume);
extern "C" float fn_80077054(sndSystem* system);

sndDoubutsugoTask::sndDoubutsugoTask(const void* archiveData, Heaps::HeapType heapType) :
    gfTask("sndDoubutsugoTask", Category_None, 0xE, 6, true) {
    m_doubutsugo = NULL;
    m_isActive = true;
    m_archive = new (heapType) nw4r::snd::MemorySoundArchive;
    m_player = new (heapType) nw4r::snd::SoundArchivePlayer;
    m_archive->Setup(archiveData);
    u32 size = m_player->GetRequiredMemSize(m_archive);
    m_playerBuffer = gfHeapManager::alloc(heapType, size);
    m_streamBuffer = gfHeapManager::alloc(heapType, 0x20);
    m_player->Setup(m_archive, m_playerBuffer, size, m_streamBuffer, 0x20);
    void* mem = operator new(0xC8, heapType);
    if (mem != NULL) {
        mem = fn_803EEFCC(mem);
    }
    m_doubutsugo = static_cast<Doubutsugo*>(mem);
    if (!fn_803EF174(m_doubutsugo)) {
        fn_803EF108(m_doubutsugo, m_player);
    }
    sndSystem* system = g_sndSystem;
    float seVolume = system->unk70C;
    fn_803EF478(m_doubutsugo, 2.0f * (seVolume * fn_80077054(system)));
    unk9C = 0;
    for (int i = 0; i < 32; i++) {
        m_words[i] = 0xD3;
    }
}

sndDoubutsugoTask::~sndDoubutsugoTask() {
    if (fn_803EF174(m_doubutsugo)) {
        fn_803EF044(m_doubutsugo, 1);
        m_doubutsugo = NULL;
    }
    if (m_player->IsAvailable()) {
        m_player->Shutdown();
    }
    delete m_player;
    gfHeapManager::free(m_playerBuffer);
    gfHeapManager::free(m_streamBuffer);
    m_archive->Shutdown();
    delete m_archive;
}

void sndDoubutsugoTask::processDefault() {
    if (m_isActive) {
        fn_803EF188(m_doubutsugo);
    }
}

void sndDoubutsugoTask::playWord(s16 word) {
    m_words[0] = word;
    m_words[1] = 0xD3;
    fn_803EF2AC(m_doubutsugo, m_words);
}

void sndDoubutsugoTask::playSentence(s16* words) {
    for (int i = 0;; i++) {
        m_words[i] = *words;
        if (*words == 0xD3) {
            break;
        }
        words++;
    }
    fn_803EF2AC(m_doubutsugo, m_words);
}

// HYPOTHESIS: sndDoubutsugoTask::playScript; its script parameter type is unknown, so it keeps its address name.
extern "C" void fn_8007B0A4(sndDoubutsugoTask* task, const void* script) {
    fn_803EF3CC(task->m_doubutsugo, script);
}

void sndDoubutsugoTask::playPageSE() {
    fn_803EF1D0(m_doubutsugo);
}
