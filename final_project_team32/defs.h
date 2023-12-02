#ifndef DEFS_H
#define DEFS_H

enum class VoicePrompt{
    START_CPR, PUSH_HARDER, GOOD_COMPRESSIONS, STOP_CPR,
    INSTALL_BATTERIES,
};

enum class ECGWaveform{
    HEALTHY,
};

enum class AgeStage {
    ADULT, CHILD, INFANT
};

#endif // DEFS_H
