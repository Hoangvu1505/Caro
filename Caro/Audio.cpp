#include "Audio.h"
#include "Data.h"
#include "raylib.h"

namespace {
    Music gMenuMusic{};
    Music gGameMusic{};

    Sound gNavigateSfx{};
    Sound gSelectSfx{};
    Sound gMoveSfx{};
    Sound gPauseSfx{};
    Sound gPlaceSfx{};
    Sound gWinSfx{};
    Sound gLoseSfx{};
    Sound gDrawSfx{};

    bool gAudioInitialized = false;
    int gCurrentMusic = -1;      // 0 = menu, 1 = game
    int gPreviousWinner = 2;
}

void InitGameAudio() {
    InitAudioDevice();

    gMenuMusic = LoadMusicStream("assets/music/menu.ogg");
    gGameMusic = LoadMusicStream("assets/music/game.ogg");

    gNavigateSfx = LoadSound("assets/sfx/navigate.wav");
    gSelectSfx = LoadSound("assets/sfx/select.wav");
    gMoveSfx = LoadSound("assets/sfx/move.wav");
    gPauseSfx = LoadSound("assets/sfx/pause.wav");
    gPlaceSfx = LoadSound("assets/sfx/place.wav");
    gWinSfx = LoadSound("assets/sfx/win.wav");
    gLoseSfx = LoadSound("assets/sfx/lose.wav");
    gDrawSfx = LoadSound("assets/sfx/draw.wav");

    gMenuMusic.looping = true;
    gGameMusic.looping = true;

    SetMusicVolume(gMenuMusic, 0.45f);
    SetMusicVolume(gGameMusic, 0.35f);

    SetSoundVolume(gNavigateSfx, 0.55f);
    SetSoundVolume(gSelectSfx, 0.65f);
    SetSoundVolume(gMoveSfx, 0.45f);
    SetSoundVolume(gPauseSfx, 0.60f);
    SetSoundVolume(gPlaceSfx, 0.75f);
    SetSoundVolume(gWinSfx, 0.80f);
    SetSoundVolume(gLoseSfx, 0.75f);
    SetSoundVolume(gDrawSfx, 0.70f);

    PlayMusicStream(gMenuMusic);
    gCurrentMusic = 0;
    gPreviousWinner = _WINNER;
    gAudioInitialized = true;
}

void UpdateGameAudio() {
    if (!gAudioInitialized) return;

    int targetMusic = (_GAME_STATE == 1) ? 1 : 0;
    if (targetMusic != gCurrentMusic) {
        if (gCurrentMusic == 0) StopMusicStream(gMenuMusic);
        else if (gCurrentMusic == 1) StopMusicStream(gGameMusic);

        if (targetMusic == 0) PlayMusicStream(gMenuMusic);
        else PlayMusicStream(gGameMusic);

        gCurrentMusic = targetMusic;
    }

    UpdateMusicStream(gMenuMusic);
    UpdateMusicStream(gGameMusic);

    if (_WINNER != gPreviousWinner) {
        if (_WINNER == 0) {
            PlaySound(gDrawSfx);
        }
        else if (_WINNER == -1 || _WINNER == 1) {
            if (_GAME_MODE == 2) {
                if (_WINNER == -1) PlaySound(gWinSfx);
                else PlaySound(gLoseSfx);
            }
            else {
                PlaySound(gWinSfx);
            }
        }
        gPreviousWinner = _WINNER;
    }
}

void ShutdownGameAudio() {
    if (!gAudioInitialized) return;

    StopMusicStream(gMenuMusic);
    StopMusicStream(gGameMusic);

    UnloadSound(gNavigateSfx);
    UnloadSound(gSelectSfx);
    UnloadSound(gMoveSfx);
    UnloadSound(gPauseSfx);
    UnloadSound(gPlaceSfx);
    UnloadSound(gWinSfx);
    UnloadSound(gLoseSfx);
    UnloadSound(gDrawSfx);

    UnloadMusicStream(gMenuMusic);
    UnloadMusicStream(gGameMusic);
    CloseAudioDevice();

    gAudioInitialized = false;
    gCurrentMusic = -1;
}

void PlayNavigateSfx() {
    if (gAudioInitialized) PlaySound(gNavigateSfx);
}

void PlaySelectSfx() {
    if (gAudioInitialized) PlaySound(gSelectSfx);
}

void PlayBoardMoveSfx() {
    if (gAudioInitialized) PlaySound(gMoveSfx);
}

void PlayPlaceSfx() {
    if (gAudioInitialized) PlaySound(gPlaceSfx);
}

void PlayPauseSfx() {
    if (gAudioInitialized) PlaySound(gPauseSfx);
}