#ifndef AUDIO_H
#define AUDIO_H

void InitGameAudio();
void UpdateGameAudio();
void ShutdownGameAudio();

void PlayNavigateSfx();
void PlaySelectSfx();
void PlayBoardMoveSfx();
void PlayPlaceSfx();
void PlayPauseSfx();

#endif