#include "raylib.h"
#include "Data.h"
#include "Logic.h"
#include "View.h"
#include "Bot.h"
#include "SaveLoad.h"
#include "Audio.h"

// Định nghĩa biến toàn cục
_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN;
int _COMMAND;
int _X, _Y;
WinLine _winLine;
int _p1Moves = 0;
int _p2Moves = 0;
int _WINNER = 2;
int _GAME_STATE = 0;
int _GAME_MODE = 1;
bool _IS_PAUSED = false;
int _BOT_DIFFICULTY = 0;
bool _BGM_ON = true;
bool _SFX_ON = true;
float _MASTER_VOL = 1.0f;
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

//hàm main
int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(1920 , 1000 , "Do An Caro");
    SetTargetFPS(60);
    SetExitKey(0);
    Texture2D background = LoadTexture("background.png");
    Texture2D menuBg = LoadTexture("menu_bg.png");
    Texture2D huongdanImg = LoadTexture("huongdan.png");
    Font gameFont = LoadFont("game_font.ttf");

    ResetData();
    _p1Moves = 0; _p2Moves = 0; _WINNER = 2;

    InitGameAudio();
    while (!WindowShouldClose() && _GAME_STATE != -1) {
        UpdateGameAudio();
        
        if (_GAME_STATE == 0) {
            DrawAndHandleMenu(menuBg, gameFont);
        }
        else if (_GAME_STATE == 1) {
            DrawAndHandleGame(background, gameFont);
        }
        else if (_GAME_STATE == 2) {
            DrawAndHandleDifficultyMenu(background, gameFont);
        }
        else if (_GAME_STATE == 3) {
            DrawAndHandleLoad(menuBg, gameFont);
		}
        else if (_GAME_STATE == 4) {
            DrawAndHandleInfo(menuBg, gameFont);
        }
        else if (_GAME_STATE == 5) {
            DrawAndHandleInstructions(menuBg, huongdanImg, gameFont);
        }
        else if (_GAME_STATE == 6) {
            DrawAndHandleSettings(menuBg, gameFont);
        }
    }

    UnloadFont(gameFont);
    UnloadTexture(menuBg);

    ShutdownGameAudio();

    CloseWindow();
    return 0;
}