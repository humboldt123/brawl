#pragma once

namespace nw4r {
namespace snd {
class RemoteSpeaker {
public:
    bool Setup(void (*callback)(long, long));
    void Shutdown(void (*callback)(long, long));
    bool EnableOutput(bool enable);
    bool IsEnabledOutput() const;
    // HYPOTHESIS: nw4r::snd::RemoteSpeaker::IsAvailable (the speaker state 4 = SPEAKER_PLAY, member at 0xC);
    // sndSystem::update tests it inline.
    bool IsAvailable() const { return mState == 4; }

private:
    char unk0[0xC];
    int mState;
};
}
}
