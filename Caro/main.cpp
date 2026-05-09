#include "raylib.h"
#include "Data.h"
#include "Logic.h"
#include "View.h"
#include "Audio.h"

// Định nghĩa biến toàn cục
_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN;
int _COMMAND;
int _X, _Y;
WinLine _winLine;
int _p1Moves = 0;
int _p2Moves = 0;
int _p1WinsPvP = 0;
int _p2WinsPvP = 0;
int _p1WinsPvE = 0;
int _botWins = 0;
int _WINNER = 2;
int _GAME_STATE = 0;
int _GAME_MODE = 1;
bool _IS_PAUSED = false;
int _BOT_DIFFICULTY = 0;
bool _BGM_ON = true;
bool _SFX_ON = true;
float _MASTER_VOL = 1.0f;
char _P1_NAME[51] = "PLAYER 1";
char _P2_NAME[51] = "PLAYER 2";
int _P1_POKEMON = 0;
int _P2_POKEMON = 1;
int _ROUND = 1;
float _turnTimer = 15.0f;
int _turnTimeLimit = 15;
#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

//hàm main
int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(1920, 1000 , "Do An Caro");
    SetTargetFPS(60);
    SetExitKey(0);
    Texture2D background = LoadTexture("assets/images/menu_bg.png");
    Texture2D menuBg = LoadTexture("assets/images/menu_bg.png");
    Texture2D huongdanImg = LoadTexture("assets/images/huongdan.png");
    int codepointsCount = 0;
    char *fileText = LoadFileText("assets/font/vietnamese_chars.txt");
    int *codepoints = LoadCodepoints(fileText, &codepointsCount);
    Font gameFont = LoadFontEx("assets/font/Montserrat-Black.ttf", 256, codepoints, codepointsCount);
    Font pieceFont = LoadFontEx("assets/font/game_font.ttf", 256, codepoints, codepointsCount);
    SetTextureFilter(gameFont.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(pieceFont.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(background, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(menuBg, TEXTURE_FILTER_BILINEAR);
    
    UnloadCodepoints(codepoints);
    UnloadFileText(fileText);

    ResetData();
    _p1Moves = 0; _p2Moves = 0; _WINNER = 2;

    InitGameAudio();
    while (!WindowShouldClose() && _GAME_STATE != -1) {
        UpdateGameAudio();
        
        if (_GAME_STATE == 0) {
            DrawAndHandleMenu(menuBg, gameFont);
        }
        else if (_GAME_STATE == 1) {
            DrawAndHandleGame(background, gameFont, pieceFont);
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
        else if (_GAME_STATE == 7) {
            DrawAndHandleSetup(menuBg, gameFont);
        }
    }

    UnloadFont(gameFont);
    UnloadTexture(menuBg);

    ShutdownGameAudio();

    CloseWindow();
    return 0;
}