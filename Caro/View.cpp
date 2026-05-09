#include "Data.h"
#include "View.h"
#include "Bot.h"
#include "Logic.h"
#include "SaveLoad.h"
#include "raylib.h"
#include "Audio.h"
#include <vector>
#include <filesystem>
#include <string>
#include <cmath>

// ==========================================
// TÀI NGUYÊN & HÌNH ẢNH
// ==========================================
Texture2D _POKEMON_TEXTURES[15];
bool _POKEMON_LOADED = false;

void LoadPokemonTextures() {
    if (_POKEMON_LOADED) return;

    const char* paths[15] = {
        "assets/images/pokemon/0006-Mega-X.png", "assets/images/pokemon/0006-Mega-Y.png",
        "assets/images/pokemon/0484.png",        "assets/images/pokemon/0483.png",
        "assets/images/pokemon/0251.png",        "assets/images/pokemon/0146-Galar.png",
        "assets/images/pokemon/0133-Gmax.png",   "assets/images/pokemon/0025-Gmax.png",
        "assets/images/pokemon/0094-Gmax.png",   "assets/images/pokemon/0131-Gmax.png",
        "assets/images/pokemon/0208-Mega.png",   "assets/images/pokemon/0244.png",
        "assets/images/pokemon/0245.png",        "assets/images/pokemon/0382-Primal.png",
        "assets/images/pokemon/0249.png"
    };

    for (int i = 0; i < 15; i++) {
        _POKEMON_TEXTURES[i] = LoadTexture(paths[i]);
        SetTextureFilter(_POKEMON_TEXTURES[i], TEXTURE_FILTER_BILINEAR);
    }
    _POKEMON_LOADED = true;
}

// ==========================================
// GIAO DIỆN BÀN CỜ
// ==========================================
void DrawBoardRaylib(Font gameFont) {
    // Vẽ các đường lưới của bàn cờ
    for (int i = 0; i <= BOARD_SIZE; i++) {
        DrawLine(LEFT, TOP + i * CELL_SIZE, LEFT + BOARD_SIZE * CELL_SIZE, TOP + i * CELL_SIZE, BLACK);
        DrawLine(LEFT + i * CELL_SIZE, TOP, LEFT + i * CELL_SIZE, TOP + BOARD_SIZE * CELL_SIZE, BLACK);
    }

    // Vẽ các quân cờ X và O
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c == -1) {
                DrawTextEx(gameFont, "X", Vector2{(float)_A[i][j].x + 12, (float)_A[i][j].y + 2}, CELL_SIZE - 5, 2, RED);
            }
            else if (_A[i][j].c == 1) {
                DrawTextEx(gameFont, "O", Vector2{(float)_A[i][j].x + 10, (float)_A[i][j].y + 2}, CELL_SIZE - 5, 2, BLUE);
            }
        }
    }

    // Vẽ khung chọn (Cursor) tại vị trí hiện tại
    Color cursorColor = _TURN ? RED : BLUE;
    DrawRectangleLinesEx(Rectangle{ (float)_X, (float)_Y, (float)CELL_SIZE, (float)CELL_SIZE }, 5.0f, cursorColor);

    // Vẽ đường gạch nối 5 quân cờ khi có người thắng
    if (_WINNER == -1 || _WINNER == 1) {
        Vector2 startPos = { _A[_winLine.r1][_winLine.c1].x + CELL_SIZE / 2.0f, _A[_winLine.r1][_winLine.c1].y + CELL_SIZE / 2.0f };
        Vector2 endPos = { _A[_winLine.r2][_winLine.c2].x + CELL_SIZE / 2.0f, _A[_winLine.r2][_winLine.c2].y + CELL_SIZE / 2.0f };
        DrawLineEx(startPos, endPos, 10.0f, (_WINNER == -1) ? RED : BLUE);
    }
}

// ==========================================
// XỬ LÝ VÀ VẼ MAIN MENU
// ==========================================
void DrawAndHandleMenu(Texture2D background, Font gameFont) {
    static int menuFocus = 0;
    const int NUM_BUTTONS = 7;
    const char* btnLabels[] = {
        "ĐÁNH 2 NGƯỜI", "ĐÁNH VỚI MÁY", "FILE GAME", 
        "THÔNG TIN", "HƯỚNG DẪN", "CÀI ĐẶT", "THOÁT GAME"
    };

    int oldFocus = menuFocus;
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) menuFocus = (menuFocus - 1 + NUM_BUTTONS) % NUM_BUTTONS;
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) menuFocus = (menuFocus + 1) % NUM_BUTTONS;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Vẽ hình nền và lớp phủ
    DrawTexturePro(background,
        Rectangle{ 0, 0, (float)background.width, (float)background.height },
        Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        Vector2{ 0, 0 }, 0.0f, WHITE);

    // Tiêu đề game với hiệu ứng đổ bóng
    Vector2 titleSize = MeasureTextEx(gameFont, "GAME CỜ CARO", 90, 4);
    Vector2 titlePos = { (float)GetScreenWidth() / 2 - titleSize.x / 2, 60 };
    DrawTextEx(gameFont, "GAME CỜ CARO", { titlePos.x + 4, titlePos.y + 4 }, 90, 4, BLACK);
    DrawTextEx(gameFont, "GAME CỜ CARO", titlePos, 90, 4, WHITE);

    int btnWidth = 450;
    int btnHeight = 70;
    int startY = 180;
    int gap = 15;
    bool isClicked = false;
    int clickedIndex = -1;

    // Vẽ các nút bấm
    for (int i = 0; i < NUM_BUTTONS; i++) {
        Rectangle r = {
            (float)GetScreenWidth() / 2 - btnWidth / 2,
            (float)(startY + i * (btnHeight + gap)),
            (float)btnWidth,
            (float)btnHeight
        };
        bool focused = (menuFocus == i);

        if (CheckCollisionPointRec(GetMousePosition(), r)) {
            menuFocus = i;
        }

        // Vẽ nền nút bấm bo góc, không dùng viền (Modern Style)
        float roundness = 0.2f;
        DrawRectangleRounded(r, roundness, 0, focused ? Fade(SKYBLUE, 0.5f) : Fade(BLACK, 0.25f));
        
        // Hiệu ứng điểm nhấn khi focused: Thanh sáng bên trái
        if (focused) {
            // Thanh indicator dọc bên trái
            DrawRectangleRounded({ r.x - 15, r.y + 5, 8, r.height - 10 }, 1.0f, 0, YELLOW);
            // Hiệu ứng phát sáng nhẹ (Glow) lan tỏa - Sửa lỗi chữ ký hàm Raylib
            DrawRectangleRoundedLines(r, roundness, 10, Fade(YELLOW, 0.3f));
        }

        // Vẽ chữ trên nút
        Vector2 s = MeasureTextEx(gameFont, btnLabels[i], 35, 2);
        DrawTextEx(gameFont, btnLabels[i],
            Vector2{ r.x + (btnWidth - s.x) / 2, r.y + (btnHeight - s.y) / 2 },
            35, 2, focused ? YELLOW : WHITE);

        if (focused && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isClicked = true;
            clickedIndex = i;
        }
    }

    // Xử lý âm thanh và lựa chọn
    if (menuFocus != oldFocus) {
        PlayNavigateSfx();
    }

    if (IsKeyPressed(KEY_ENTER)) {
        isClicked = true;
        clickedIndex = menuFocus;
    }

    if (isClicked) {
        PlaySelectSfx();
        switch (clickedIndex) {
            case 0: // Chế độ Người vs Người (PvP)
                _GAME_MODE = 1;
                _GAME_STATE = 7;
                _IS_PAUSED = false;
                ResetData();
                _p1Moves = _p2Moves = 0;
                _WINNER = 2;
                _ROUND = 1;
                break;

            case 1: // Chế độ Người vs Máy (PvE)
                _GAME_MODE = 2;
                _GAME_STATE = 2;
                _IS_PAUSED = false;
                ResetData();
                _p1Moves = _p2Moves = 0;
                _WINNER = 2;
                _ROUND = 1;
                break;

            case 2: _GAME_STATE = 3; break; // TẢI GAME
            case 3: _GAME_STATE = 4; break; // THÔNG TIN
            case 4: _GAME_STATE = 5; break; // HƯỚNG DẪN
            case 5: _GAME_STATE = 6; break; // CÀI ĐẶT
            case 6: _GAME_STATE = -1; break; // THOÁT
        }
    }
    EndDrawing();
}

// ==========================================
// XỬ LÝ ĐIỀU KHIỂN TRONG GAME
// ==========================================
void HandleGameInput() {
    // Xử lý tạm dừng game
    if (IsKeyPressed(KEY_ESCAPE) && _WINNER == 2) {
        _IS_PAUSED = !_IS_PAUSED;
        PlayPauseSfx();
    }

    if (_WINNER != 2 || _IS_PAUSED) return;

    int oldX = _X;
    int oldY = _Y;

    if (_TURN) { // Lượt của Người chơi 1 (X)
        if (IsKeyPressed(KEY_W)) MoveUp();
        if (IsKeyPressed(KEY_S)) MoveDown();
        if (IsKeyPressed(KEY_A)) MoveLeft();
        if (IsKeyPressed(KEY_D)) MoveRight();

        if (IsKeyPressed(KEY_ENTER)) {
            if (CheckBoard(_X, _Y) != 0) {
                PlayPlaceSfx();
                _p1Moves++;
                _WINNER = TestBoard(_X, _Y);

                if (_WINNER == 2) {
                    _TURN = !_TURN;
                    _turnTimer = (float)_turnTimeLimit;
                }
                else if (_WINNER == -1) {
                    if (_GAME_MODE == 1) _p1WinsPvP++;
                    else _p1WinsPvE++;
                }
                else if (_WINNER == 1) {
                    if (_GAME_MODE == 1) _p2WinsPvP++;
                    else _botWins++;
                }
            }
        }
    }
    else { // Lượt của Người chơi 2 (O) hoặc Máy (Bot)
        if (_GAME_MODE == 1) { // Chế độ Người vs Người
            if (IsKeyPressed(KEY_UP)) MoveUp();
            if (IsKeyPressed(KEY_DOWN)) MoveDown();
            if (IsKeyPressed(KEY_LEFT)) MoveLeft();
            if (IsKeyPressed(KEY_RIGHT)) MoveRight();

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
                if (CheckBoard(_X, _Y) != 0) {
                    PlayPlaceSfx();
                    _p2Moves++;
                    _WINNER = TestBoard(_X, _Y);

                    if (_WINNER == 2) {
                        _TURN = !_TURN;
                        _turnTimer = (float)_turnTimeLimit;
                    }
                    else if (_WINNER == -1) {
                        _p1WinsPvP++;
                    }
                    else if (_WINNER == 1) {
                        _p2WinsPvP++;
                    }
                }
            }
        }
        else { // Chế độ đánh với Máy (Bot)
            BotMove();
            if (CheckBoard(_X, _Y) != 0) {
                PlayPlaceSfx();
                _p2Moves++;
                _WINNER = TestBoard(_X, _Y);

                if (_WINNER == 2) {
                    _TURN = !_TURN;
                    _turnTimer = (float)_turnTimeLimit;
                }
                else if (_WINNER == -1) {
                    _p1WinsPvE++;
                }
                else if (_WINNER == 1) {
                    _botWins++;
                }
            }
        }
    }

    // Phát âm thanh nếu vị trí con trỏ thay đổi
    if (_X != oldX || _Y != oldY) {
        PlayBoardMoveSfx();
    }

    // Logic đếm ngược thời gian lượt chơi
    _turnTimer -= GetFrameTime();
    if (_turnTimer <= 0) {
        _turnTimer = 0;
        _WINNER = _TURN ? 1 : -1;

        if (_WINNER == -1) {
            if (_GAME_MODE == 1) _p1WinsPvP++;
            else _p1WinsPvE++;
        }
        else {
            if (_GAME_MODE == 1) _p2WinsPvP++;
            else _botWins++;
        }
        PlayLoseSfx();
    }
}

static Texture2D _FIRE_CHARIZARD, _FIRE_GROUDON, _WATER_KYOGRE, _WATER_GYARADOS;
static bool _DECOR_LOADED = false;

// ==========================================
// TÀI NGUYÊN TRANG TRÍ
// ==========================================
void LoadDecorTextures() {
    if (_DECOR_LOADED) return;

    _FIRE_CHARIZARD = LoadTexture("assets/images/pokemon/6.png");
    _FIRE_GROUDON = LoadTexture("assets/images/pokemon/383.png");
    _WATER_KYOGRE = LoadTexture("assets/images/pokemon/382.png");
    _WATER_GYARADOS = LoadTexture("assets/images/pokemon/10041.png");

    SetTextureFilter(_FIRE_CHARIZARD, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(_FIRE_GROUDON, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(_WATER_KYOGRE, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(_WATER_GYARADOS, TEXTURE_FILTER_BILINEAR);

    _DECOR_LOADED = true;
}

// ==========================================
// GIAO DIỆN CHÍNH TRONG GAME
// ==========================================
void DrawGameUI(Texture2D background, Font gameFont, Font pieceFont) {
    LoadDecorTextures();

    // Vẽ màu nền Gradient cho 2 bên
    // Khôi phục lại nền cũ theo ý bạn
    DrawRectangleGradientH(0, 0, GetScreenWidth() / 2, GetScreenHeight(), Color{ 255, 140, 0, 255 }, Color{ 215, 38, 0, 255 });
    DrawRectangleGradientH(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), Color{ 0, 68, 129, 255 }, Color{ 0, 191, 255, 255 });

    // Vẽ khung cho bàn cờ
    DrawRectangle(LEFT - 20, TOP - 20, BOARD_SIZE * CELL_SIZE + 40, BOARD_SIZE * CELL_SIZE + 40, Fade(WHITE, 0.95f));

    Color cBC = _TURN ? RED : BLUE;
    DrawRectangleLinesEx(Rectangle{ (float)LEFT - 20, (float)TOP - 20, (float)BOARD_SIZE * CELL_SIZE + 40, (float)BOARD_SIZE * CELL_SIZE + 40 }, 10, cBC);

    // Hiển thị thông tin vòng đấu (Round)
    DrawTextEx(gameFont, TextFormat("ROUND %d", _ROUND), Vector2{ (float)GetScreenWidth() / 2 - 102, 22 }, 50, 2, BLACK);
    DrawTextEx(gameFont, TextFormat("ROUND %d", _ROUND), Vector2{ (float)GetScreenWidth() / 2 - 100, 20 }, 50, 2, WHITE);

    DrawBoardRaylib(pieceFont);
    LoadPokemonTextures();

    int cW = 320;
    int cH = 550;
    int cY = 225; // (1000 - 550) / 2
    int p1X = 130; // (585 - 320) / 2
    int p2X = 1470; // 1335 + (585 - 320) / 2

    // --- BẢNG THÔNG TIN NGƯỜI CHƠI 1 ---
    DrawRectangle(p1X, cY, cW, cH, Fade(WHITE, 0.85f));
    DrawRectangleLinesEx(Rectangle{ (float)p1X, (float)cY, (float)cW, (float)cH }, (_TURN && _WINNER == 2) ? 8 : 4, RED);

    DrawTextEx(gameFont, _P1_NAME,
        Vector2{ (float)p1X + cW / 2 - MeasureTextEx(gameFont, _P1_NAME, 35, 2).x / 2, (float)cY + 25 },
        35, 2, RED);
    DrawTextEx(gameFont, "PLAYER X",
        Vector2{ (float)p1X + cW / 2 - MeasureTextEx(gameFont, "PLAYER X", 20, 2).x / 2, (float)cY + 65 },
        20, 2, DARKGRAY);

    DrawLine(p1X + 30, cY + 100, p1X + cW - 30, cY + 100, LIGHTGRAY);
    DrawTexturePro(_POKEMON_TEXTURES[_P1_POKEMON],
        { 0, 0, (float)_POKEMON_TEXTURES[_P1_POKEMON].width, (float)_POKEMON_TEXTURES[_P1_POKEMON].height },
        { (float)p1X + 45, (float)cY + 120, 230, 230 }, { 0, 0 }, 0.0f, WHITE);
    DrawLine(p1X + 30, cY + 370, p1X + cW - 30, cY + 370, LIGHTGRAY);

    int dP1W = (_GAME_MODE == 1) ? _p1WinsPvP : _p1WinsPvE;
    DrawTextEx(gameFont, TextFormat("WINS : %d", dP1W), { (float)p1X + 50, (float)cY + 385 }, 30, 2, Color{ 180, 0, 0, 255 });
    DrawTextEx(gameFont, TextFormat("MOVES: %d", _p1Moves), { (float)p1X + 50, (float)cY + 435 }, 30, 2, Color{ 180, 0, 0, 255 });
    DrawTextEx(gameFont, "TIME :", Vector2{ (float)p1X + 50, (float)cY + 485 }, 30, 2, DARKGRAY);

    if (_TURN && _WINNER == 2) {
        DrawTextEx(gameFont, TextFormat("%02d s", (int)ceilf(_turnTimer)),
            { (float)p1X + 160, (float)cY + 480 },
            40, 2, (_turnTimer <= 5) ? RED : Color{ 180, 0, 0, 255 });
    }

    // --- BẢNG THÔNG TIN NGƯỜI CHƠI 2 ---
    DrawRectangle(p2X, cY, cW, cH, Fade(WHITE, 0.85f));
    DrawRectangleLinesEx(Rectangle{ (float)p2X, (float)cY, (float)cW, (float)cH }, (!_TURN && _WINNER == 2) ? 8 : 4, BLUE);

    const char* p2n = (_GAME_MODE == 1) ? _P2_NAME : "COMPUTER";
    DrawTextEx(gameFont, p2n,
        Vector2{ (float)p2X + cW / 2 - MeasureTextEx(gameFont, p2n, 35, 2).x / 2, (float)cY + 25 },
        35, 2, BLUE);

    const char* sub2 = (_GAME_MODE == 1) ? "PLAYER O" : "BOT O";
    DrawTextEx(gameFont, sub2,
        Vector2{ (float)p2X + cW / 2 - MeasureTextEx(gameFont, sub2, 20, 2).x / 2, (float)cY + 65 },
        20, 2, DARKGRAY);

    DrawLine(p2X + 30, cY + 100, p2X + cW - 30, cY + 100, LIGHTGRAY);
    DrawTexturePro(_POKEMON_TEXTURES[_P2_POKEMON],
        { 0, 0, (float)_POKEMON_TEXTURES[_P2_POKEMON].width, (float)_POKEMON_TEXTURES[_P2_POKEMON].height },
        { (float)p2X + 45, (float)cY + 120, 230, 230 }, { 0, 0 }, 0.0f, WHITE);
    DrawLine(p2X + 30, cY + 370, p2X + cW - 30, cY + 370, LIGHTGRAY);

    int dP2W = (_GAME_MODE == 1) ? _p2WinsPvP : _botWins;
    DrawTextEx(gameFont, TextFormat("WINS : %d", dP2W), { (float)p2X + 50, (float)cY + 385 }, 30, 2, Color{ 0, 68, 129, 255 });
    DrawTextEx(gameFont, TextFormat("MOVES: %d", _p2Moves), { (float)p2X + 50, (float)cY + 435 }, 30, 2, Color{ 0, 68, 129, 255 });
    DrawTextEx(gameFont, "TIME :", Vector2{ (float)p2X + 50, (float)cY + 485 }, 30, 2, DARKGRAY);

    if (!_TURN && _WINNER == 2) {
        DrawTextEx(gameFont, TextFormat("%02d s", (int)ceilf(_turnTimer)),
            { (float)p2X + 160, (float)cY + 480 },
            40, 2, (_turnTimer <= 5) ? RED : Color{ 0, 68, 129, 255 });
    }
}

// ==========================================
// MÀN HÌNH KẾT THÚC GAME (GAME OVER)
// ==========================================
void DrawAndHandleGameOver(Font gameFont) {
    if (_WINNER == 2) return;

    static int gFocus = 0;
    static int waitT = 0;
    waitT++;

    // Logic điều hướng bằng phím bấm
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) gFocus = 0;
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) gFocus = 1;

    // Lớp phủ nền mờ
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));

    // Kích thước và tọa độ hộp thoại
    int bW = 800;
    int bH = 400;
    int bX = (GetScreenWidth() - bW) / 2;
    int bY = (GetScreenHeight() - bH) / 2;

    // Vẽ hộp thoại thông báo bo góc (Modern style)
    float roundness = 0.1f;
    DrawRectangleRounded({ (float)bX, (float)bY, (float)bW, (float)bH }, roundness, 10, Fade(RAYWHITE, 0.95f));
    DrawRectangleRoundedLines({ (float)bX, (float)bY, (float)bW, (float)bH }, roundness, 10, Fade(DARKGRAY, 0.5f));

    // Hiển thị thông báo người chiến thắng
    const char* rT = (_WINNER == 0) ? "HÒA CỜ!" :
        (_WINNER == -1 ? "PLAYER X THẮNG!" :
        (_GAME_MODE == 1 ? "PLAYER O THẮNG!" : "MÁY THẮNG!"));
    Color rC = (_WINNER == 0) ? DARKGRAY : (_WINNER == -1 ? RED : BLUE);
    DrawTextEx(gameFont, rT,
        { (float)bX + (bW - MeasureTextEx(gameFont, rT, 55, 2).x) / 2, (float)bY + 80 },
        55, 2, rC);

    // Hàm hỗ trợ vẽ nút bấm bo góc hiện đại
    auto DB = [&](Rectangle r, const char* t, bool h) {
        float rR = 0.3f;
        DrawRectangleRounded(r, rR, 10, h ? Fade(SKYBLUE, 0.6f) : Fade(LIGHTGRAY, 0.8f));
        if (h) {
            DrawRectangleRoundedLines(r, rR, 10, YELLOW);
            // Indicator nhỏ phía dưới nút
            DrawRectangleRounded({ r.x + 20, r.y + r.height - 8, r.width - 40, 5 }, 1.0f, 0, YELLOW);
        }
        DrawTextEx(gameFont, t,
            { r.x + (r.width - MeasureTextEx(gameFont, t, 45, 2).x) / 2,
              r.y + (r.height - MeasureTextEx(gameFont, t, 45, 2).y) / 2 },
            45, 2, h ? WHITE : BLACK);
    };

    Rectangle bPA = { (float)bX + 100, (float)bY + 260, 280, 80 };
    Rectangle bM = { (float)bX + 420, (float)bY + 260, 280, 80 };

    // Xử lý chọn bằng chuột
    if (CheckCollisionPointRec(GetMousePosition(), bPA)) gFocus = 0;
    if (CheckCollisionPointRec(GetMousePosition(), bM)) gFocus = 1;

    DB(bPA, "CHƠI LẠI", gFocus == 0);
    DB(bM, "MENU", gFocus == 1);

    // Xử lý xác nhận lựa chọn
    if (waitT > 30) {
        bool enterPressed = IsKeyPressed(KEY_ENTER);
        bool mouseClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            (CheckCollisionPointRec(GetMousePosition(), bPA) || CheckCollisionPointRec(GetMousePosition(), bM));

        if (enterPressed || mouseClicked) {
            PlaySelectSfx();
            if (gFocus == 0) {
                int m = _GAME_MODE;
                ResetData();
                _GAME_MODE = m;
                _p1Moves = _p2Moves = 0;
                _WINNER = 2;
                _IS_PAUSED = false; 
                _ROUND++;
            }
            else {
                _GAME_STATE = 0;
                _IS_PAUSED = false;
                _ROUND = 1;
            }
            waitT = 0;
        }
    }
}

// ==========================================
// MÀN HÌNH TẠM DỪNG (PAUSE MENU)
// ==========================================
void DrawAndHandlePauseMenu(Font gameFont) {
    if (!_IS_PAUSED || _WINNER != 2) return;

    static int pFocus = 0;
    static int sFocus = 0;
    static int nC = 0;
    static bool isS = false; // Menu cài đặt con
    static bool isT = false; // Menu lưu game con
    static char sN[51] = "\0";

    // --- LOGIC XỬ LÝ ĐIỀU KHIỂN ---
    if (isS) { // Logic cho Menu cài đặt con
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) sFocus = (sFocus + 4) % 5;
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) sFocus = (sFocus + 1) % 5;

        if (sFocus == 0 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D))) {
            _BGM_ON = !_BGM_ON;
            PlaySelectSfx();
        }
        if (sFocus == 1 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D))) {
            _SFX_ON = !_SFX_ON;
            PlaySelectSfx();
        }
        if (sFocus == 2) {
            if (IsKeyPressed(KEY_A)) {
                _MASTER_VOL = fmaxf(0, _MASTER_VOL - 0.1f);
                SetMasterVolume(_MASTER_VOL);
            }
            if (IsKeyPressed(KEY_D)) {
                _MASTER_VOL = fminf(1, _MASTER_VOL + 0.1f);
                SetMasterVolume(_MASTER_VOL);
            }
        }
        if (sFocus == 3 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D))) {
            _turnTimeLimit = (_turnTimeLimit == 15 ? 30 : (_turnTimeLimit == 30 ? 60 : 15));
            _turnTimer = (float)_turnTimeLimit;
            PlaySelectSfx();
        }
        if ((sFocus == 4 && IsKeyPressed(KEY_ENTER)) || IsKeyPressed(KEY_ESCAPE)) {
            isS = false;
            PlaySelectSfx();
        }
    }
    else if (isT) { // Logic cho Menu Lưu game con (Nhập liệu văn bản)
        int k = GetCharPressed();
        while (k > 0) {
            if (k >= 32 && k <= 125 && nC < 40) {
                sN[nC++] = (char)k;
                sN[nC] = '\0';
            }
            k = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && nC > 0) {
            sN[--nC] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER) && nC > 0) {
            SaveGame(std::string(sN) + ".sav");
            isT = _IS_PAUSED = false;
            PlaySelectSfx();
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            isT = false;
            PlaySelectSfx();
        }
    }
    else { // Logic cho Menu Tạm dừng chính
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) pFocus = (pFocus + 3) % 4;
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) pFocus = (pFocus + 1) % 4;

        if (IsKeyPressed(KEY_ENTER)) {
            PlaySelectSfx();
            if (pFocus == 0) {
                _IS_PAUSED = false;
            }
            else if (pFocus == 1) {
                isS = true;
            }
            else if (pFocus == 2) {
                isT = true;
                nC = 0;
                sN[0] = '\0';
            }
            else {
                _GAME_STATE = 0;
                _IS_PAUSED = false;
                isS = false;
                isT = false;
                pFocus = 0;
            }
        }
    }

    // --- VẼ GIAO DIỆN ---
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.45f));

    int bW = isS ? 600 : 500;
    int bH = isT ? 350 : 550;
    int bX = (GetScreenWidth() - bW) / 2;
    int bY = (GetScreenHeight() - bH) / 2;

    float roundness = 0.1f;
    DrawRectangleRounded({ (float)bX, (float)bY, (float)bW, (float)bH }, roundness, 10, Fade(RAYWHITE, 0.98f));
    DrawRectangleRoundedLines({ (float)bX, (float)bY, (float)bW, (float)bH }, roundness, 10, Fade(DARKGRAY, 0.5f));

    if (isS) { // --- GIAO DIỆN MENU CÀI ĐẶT CON NÂNG CẤP ---
        DrawTextEx(gameFont, "CÀI ĐẶT",
            { (float)bX + (bW - MeasureTextEx(gameFont, "CÀI ĐẶT", 45, 2).x) / 2, (float)bY + 40 },
            45, 2, RED);

        const char* opts[] = { "NHẠC NỀN", "ÂM THANH", "ÂM LƯỢNG", "THỜI GIAN", "QUAY LẠI" };
        int gapS = 75;

        for (int i = 0; i < 5; i++) {
            Rectangle r = { (float)bX + 50, (float)bY + 110 + i * gapS, (float)bW - 100, 65 };
            bool h = (sFocus == i);
            float rR = 0.2f;

            // Vẽ nền nút
            DrawRectangleRounded(r, rR, 10, h ? Fade(SKYBLUE, 0.5f) : Fade(BLACK, 0.1f));
            if (h) {
                DrawRectangleRoundedLines(r, rR, 10, YELLOW);
                DrawRectangleRounded({ r.x - 12, r.y + 10, 6, r.height - 20 }, 1.0f, 0, YELLOW);
            }

            // Vẽ tên mục cài đặt
            DrawTextEx(gameFont, opts[i], { r.x + 20, r.y + 18 }, 30, 2, h ? YELLOW : BLACK);

            // Vẽ giá trị/trạng thái
            if (i == 0) {
                const char* t = _BGM_ON ? "BẬT" : "TẮT";
                Color c = _BGM_ON ? GREEN : RED;
                DrawTextEx(gameFont, t, { r.x + r.width - 80, r.y + 18 }, 30, 2, c);
            }
            else if (i == 1) {
                const char* t = _SFX_ON ? "BẬT" : "TẮT";
                Color c = _SFX_ON ? GREEN : RED;
                DrawTextEx(gameFont, t, { r.x + r.width - 80, r.y + 18 }, 30, 2, c);
            }
            else if (i == 2) {
                int vol = (int)(_MASTER_VOL * 100 + 0.5f);
                DrawTextEx(gameFont, TextFormat("%d%%", vol), { r.x + r.width - 100, r.y + 18 }, 30, 2, h ? YELLOW : DARKGRAY);
            }
            else if (i == 3) {
                DrawTextEx(gameFont, TextFormat("%ds", _turnTimeLimit), { r.x + r.width - 80, r.y + 18 }, 30, 2, h ? YELLOW : DARKGRAY);
            }
        }
    }
    else if (isT) { // --- GIAO DIỆN MENU LƯU GAME CON ---
        DrawTextEx(gameFont, "LƯU TRẠNG THÁI",
            { (float)bX + (bW - MeasureTextEx(gameFont, "LƯU TRẠNG THÁI", 40, 2).x) / 2, (float)bY + 40 },
            40, 2, BLACK);

        DrawTextEx(gameFont, "NHẬP TÊN FILE:", { (float)bX + 50, (float)bY + 120 }, 30, 2, DARKGRAY);

        Rectangle r = { (float)bX + 50, (float)bY + 160, (float)bW - 100, 70 };
        DrawRectangleRec(r, WHITE);
        DrawRectangleLinesEx(r, 2, BLACK);
        DrawTextEx(gameFont, sN, { r.x + 20, r.y + 15 }, 40, 2, RED);

        const char* hint = "Nhấn ENTER để lưu, ESC để hủy";
        DrawTextEx(gameFont, hint,
            { (float)bX + (bW - MeasureTextEx(gameFont, hint, 25, 2).x) / 2, (float)bY + 270 },
            25, 2, DARKGRAY);
    }
    else { // --- GIAO DIỆN MENU TẠM DỪNG CHÍNH ---
        DrawTextEx(gameFont, "TẠM DỪNG",
            { (float)bX + (bW - MeasureTextEx(gameFont, "TẠM DỪNG", 50, 2).x) / 2, (float)bY + 50 },
            50, 2, RED);

        const char* menuOpts[] = { "TIẾP TỤC", "CÀI ĐẶT", "LƯU GAME", "MENU CHÍNH" };
        for (int i = 0; i < 4; i++) {
            Rectangle r = { (float)bX + 75, (float)bY + 150 + i * 90, (float)bW - 150, 70 };
            bool h = (pFocus == i);
            float rR = 0.2f;

            DrawRectangleRounded(r, rR, 10, h ? Fade(SKYBLUE, 0.6f) : Fade(LIGHTGRAY, 0.8f));
            if (h) {
                DrawRectangleRoundedLines(r, rR, 10, YELLOW);
                DrawRectangleRounded({ r.x - 10, r.y + 10, 6, r.height - 20 }, 1.0f, 0, YELLOW); // Indicator
            }

            DrawTextEx(gameFont, menuOpts[i],
                { r.x + (r.width - MeasureTextEx(gameFont, menuOpts[i], 35, 2).x) / 2, r.y + 17 },
                35, 2, h ? WHITE : BLACK);
        }
    }
}

// ==========================================
// MÀN HÌNH HƯỚNG DẪN
// ==========================================
void DrawAndHandleInstructions(Texture2D background, Texture2D huongdanImg, Font gameFont) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        PlaySelectSfx();
        _GAME_STATE = 0;
    }

    BeginDrawing();

    // Background
    DrawTexturePro(background,
        { 0, 0, (float)background.width, (float)background.height },
        { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        { 0, 0 }, 0.0f, WHITE);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));

    // Title
    DrawTextEx(gameFont, "HƯỚNG DẪN CHƠI",
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, "HƯỚNG DẪN CHƠI", 70, 2).x / 2, 60 },
        70, 2, WHITE);

    // Hình ảnh hướng dẫn
    DrawTexture(huongdanImg,
        (GetScreenWidth() - huongdanImg.width) / 2,
        (GetScreenHeight() - huongdanImg.height) / 2 - 20,
        WHITE);

    // Gợi ý quay lại
    const char* escT = "Nhấn ESC để quay lại MENU";
    DrawTextEx(gameFont, escT,
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, escT, 30, 2).x / 2, (float)GetScreenHeight() - 70 },
        30, 2, RED);

    EndDrawing();
}

// ==========================================
// MÀN HÌNH THÔNG TIN NHÓM
// ==========================================
void DrawAndHandleInfo(Texture2D background, Font gameFont) {
    // Bấm ESC để quay về Menu chính
    if (IsKeyPressed(KEY_ESCAPE)) {
        PlaySelectSfx();
        _GAME_STATE = 0;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexturePro(background, Rectangle{ 0, 0, (float)background.width, (float)background.height }, Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);

    // Nền đen làm mờ 80% để nổi bật chữ
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));

    // Vẽ tiêu đề căn giữa tuyệt đối
    float titleSize = 70;
    Vector2 titlePos = { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, "THÔNG TIN ĐỒ ÁN", titleSize, 2).x / 2, 80 };
    DrawTextEx(gameFont, "THÔNG TIN ĐỒ ÁN", titlePos, titleSize, 2, WHITE);

    // Căn lề trái cho đoạn text
    int startX = GetScreenWidth() / 2 - 350;
    int startY = 220;

    DrawTextEx(gameFont, "Truong: DH Khoa hoc Tu nhien - TPHCM", Vector2{ (float)startX, (float)startY }, 40, 2, LIGHTGRAY);
    DrawTextEx(gameFont, "Mon hoc: Co so lap trinh", Vector2{ (float)startX, (float)startY + 60 }, 40, 2, GREEN);
    DrawTextEx(gameFont, "Giang vien: Thay Truong Toan Thinh", Vector2{ (float)startX, (float)startY + 120 }, 40, 2, GREEN);
    DrawTextEx(gameFont, "Do an: co Caro", Vector2{ (float)startX, (float)startY + 170 }, 40, 2, GREEN);
    DrawTextEx(gameFont, "Nhom thuc hien: nhom 10", Vector2{ (float)startX, (float)startY + 220 }, 40, 2, YELLOW);

    // Danh sách thành viên
    DrawTextEx(gameFont, "1. Luong Nguyen Hoang Vu - 24120493", Vector2{ (float)startX + 50, (float)startY + 270 }, 35, 2, WHITE);
    DrawTextEx(gameFont, "2. Dinh Duc Hieu - 24120002", Vector2{ (float)startX + 50, (float)startY + 320 }, 35, 2, WHITE);
    DrawTextEx(gameFont, "3. Dao Thanh Phong - 24120006", Vector2{ (float)startX + 50, (float)startY + 370 }, 35, 2, WHITE);
    DrawTextEx(gameFont, "4. Le Hung Thang - 24120137", Vector2{ (float)startX + 50, (float)startY + 420 }, 35, 2, WHITE);
    DrawTextEx(gameFont, "5. Nguyen Anh Duc - 24120289", Vector2{ (float)startX + 50, (float)startY + 470 }, 35, 2, WHITE);

    DrawTextEx(gameFont, "Công nghệ: C++ & Thư viện Raylib", Vector2{ (float)startX, (float)startY + 540 }, 35, 2, LIGHTGRAY);

    // Hướng dẫn thoát căn giữa
    const char* hintEsc = "Nhấn ESC để quay lại Menu";
    DrawTextEx(gameFont, hintEsc,
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, hintEsc, 30, 2).x / 2, (float)GetScreenHeight() - 70 },
        30, 2, RED);

    EndDrawing();
}

// ==========================================
// MÀN HÌNH CHỌN ĐỘ KHÓ (PVE)
// ==========================================
void DrawAndHandleDifficultyMenu(Texture2D background, Font gameFont) {
    static int focus = 1;

    // Điều hướng chọn độ khó
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) focus = (focus + 2) % 3;
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) focus = (focus + 1) % 3;

    if (IsKeyPressed(KEY_ESCAPE)) {
        PlaySelectSfx();
        _GAME_STATE = 0;
    }

    BeginDrawing();

    // Background
    DrawTexturePro(background,
        { 0, 0, (float)background.width, (float)background.height },
        { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        { 0, 0 }, 0.0f, WHITE);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

    // Title
    DrawTextEx(gameFont, "CHỌN ĐỘ KHÓ",
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, "CHỌN ĐỘ KHÓ", 60, 2).x / 2, 150 },
        60, 2, WHITE);

    const char* labs[] = { "DỄ", "TRUNG BÌNH", "KHÓ" };
    int btnW = 350;
    int btnH = 90;
    int totalW = 3 * btnW + 2 * 40; // 3 nút và 2 khoảng cách 40px
    int startX = (GetScreenWidth() - totalW) / 2;

    for (int i = 0; i < 3; i++) {
        Rectangle r = { (float)(startX + i * (btnW + 40)), 380, (float)btnW, (float)btnH };
        bool h = (focus == i);
        float rR = 0.2f;

        DrawRectangleRounded(r, rR, 10, h ? Fade(SKYBLUE, 0.6f) : Fade(BLACK, 0.25f));
        if (h) {
            DrawRectangleRoundedLines(r, rR, 10, YELLOW);
            // Indicator nhỏ phía dưới nút
            DrawRectangleRounded({ r.x + 40, r.y + r.height - 8, r.width - 80, 5 }, 1.0f, 0, YELLOW);
        }

        DrawTextEx(gameFont, labs[i],
            { r.x + (350 - MeasureTextEx(gameFont, labs[i], 40, 2).x) / 2,
              r.y + (90 - MeasureTextEx(gameFont, labs[i], 40, 2).y) / 2 },
            40, 2, h ? YELLOW : WHITE);

        if (h && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER))) {
            PlaySelectSfx();
            _BOT_DIFFICULTY = i;
            _GAME_MODE = 2;
            _GAME_STATE = 7;
            ResetData();
        }
    }

    const char* escB = "Nhấn ESC để quay lại Menu";
    DrawTextEx(gameFont, escB,
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, escB, 30, 2).x / 2, (float)GetScreenHeight() - 70 },
        30, 2, RED);

    EndDrawing();
}

// ==========================================
// MÀN HÌNH CÀI ĐẶT (SETTINGS)
// ==========================================
void DrawAndHandleSettings(Texture2D background, Font gameFont) {
    static int setFocus = 0; // 0: Nhạc nền, 1: Âm thanh, 2: Âm lượng, 3: Thời gian, 4: Quay lại
    const int NUM_SETTINGS = 5;
    int oldFocus = setFocus;

    // --- LOGIC ĐIỀU HƯỚNG LÊN XUỐNG ---
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) setFocus = (setFocus - 1 + NUM_SETTINGS) % NUM_SETTINGS;
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) setFocus = (setFocus + 1) % NUM_SETTINGS;

    if (setFocus != oldFocus) PlayNavigateSfx();

    // --- LOGIC TƯƠNG TÁC (TRÁI/PHẢI/ENTER) ---
    if (setFocus == 0 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        _BGM_ON = !_BGM_ON; // Bật/tắt Nhạc nền
        PlaySelectSfx();
    }
    if (setFocus == 1 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        _SFX_ON = !_SFX_ON; // Bật/tắt Hiệu ứng âm thanh
        PlaySelectSfx();
    }
    if (setFocus == 2) {
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            _MASTER_VOL -= 0.1f; // Giảm 10%
            if (_MASTER_VOL < 0.0f) _MASTER_VOL = 0.0f;
            SetMasterVolume(_MASTER_VOL); 
            PlaySelectSfx();
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            _MASTER_VOL += 0.1f; // Tăng 10%
            if (_MASTER_VOL > 1.0f) _MASTER_VOL = 1.0f;
            SetMasterVolume(_MASTER_VOL); 
            PlaySelectSfx();
        }
    }
    if (setFocus == 3 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        _turnTimeLimit = (_turnTimeLimit == 15 ? 30 : (_turnTimeLimit == 30 ? 60 : 15));
        _turnTimer = (float)_turnTimeLimit;
        PlaySelectSfx();
    }
    if ((setFocus == 4 && IsKeyPressed(KEY_ENTER)) || IsKeyPressed(KEY_ESCAPE)) {
        PlaySelectSfx();
        _GAME_STATE = 0; // Quay về Menu chính
    }

    // --- VẼ GIAO DIỆN SÁNG HƠN ---
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexturePro(background, Rectangle{ 0, 0, (float)background.width, (float)background.height }, Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.35f)); // Lớp phủ cực mỏng (35%)

    // Tiêu đề sáng
    DrawTextEx(gameFont, "CÀI ĐẶT TRÒ CHƠI", Vector2{ (float)GetScreenWidth() / 2 - 320, 100 }, 70, 2, WHITE);
    const char* hintS = "Dùng A/D hoặc Mũi tên để điều chỉnh";
    DrawTextEx(gameFont, hintS, 
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, hintS, 25, 2).x / 2, 180 }, 25, 2, SKYBLUE);

    int startY = 260;
    int gap = 100;

    for (int i = 0; i < NUM_SETTINGS; i++) {
        Rectangle btnRec = { (float)GetScreenWidth() / 2 - 300, (float)(startY + i * gap), 600, 80 };
        bool isHover = (setFocus == i);

        // --- XỬ LÝ CHUỘT ---
        if (CheckCollisionPointRec(GetMousePosition(), btnRec)) {
            setFocus = i;
            isHover = true;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySelectSfx();
                if (i == 0) _BGM_ON = !_BGM_ON;
                else if (i == 1) _SFX_ON = !_SFX_ON;
                else if (i == 3) {
                    _turnTimeLimit = (_turnTimeLimit == 15 ? 30 : (_turnTimeLimit == 30 ? 60 : 15));
                    _turnTimer = (float)_turnTimeLimit;
                }
                else if (i == 4) _GAME_STATE = 0;
            }
        }

        // Hiệu ứng White Glass Modern
        float rR = 0.2f;
        DrawRectangleRounded(btnRec, rR, 10, isHover ? Fade(SKYBLUE, 0.6f) : Fade(WHITE, 0.15f));
        
        if (isHover) {
            DrawRectangleRoundedLines(btnRec, rR, 10, YELLOW);
            // Indicator sáng bên trái
            DrawRectangleRounded({ btnRec.x - 15, btnRec.y + 10, 8, btnRec.height - 20 }, 1.0f, 0, YELLOW);
        }

        // Vẽ Text hiển thị trạng thái với độ tương phản cao (Kích thước 30 để tránh tràn)
        if (i == 0) {
            DrawTextEx(gameFont, "NHẠC NỀN:", Vector2{ btnRec.x + 30, btnRec.y + 22 }, 30, 2, WHITE);
            DrawTextEx(gameFont, _BGM_ON ? "BẬT" : "TẮT", Vector2{ btnRec.x + 420, btnRec.y + 22 }, 30, 2, _BGM_ON ? GREEN : RED);
        }
        else if (i == 1) {
            DrawTextEx(gameFont, "HIỆU ỨNG:", Vector2{ btnRec.x + 30, btnRec.y + 22 }, 30, 2, WHITE);
            DrawTextEx(gameFont, _SFX_ON ? "BẬT" : "TẮT", Vector2{ btnRec.x + 420, btnRec.y + 22 }, 30, 2, _SFX_ON ? GREEN : RED);
        }
        else if (i == 2) {
            DrawTextEx(gameFont, "ÂM LƯỢNG:", Vector2{ btnRec.x + 30, btnRec.y + 22 }, 30, 2, WHITE);
            int volPercent = (int)(_MASTER_VOL * 100 + 0.5f);
            DrawTextEx(gameFont, TextFormat("< %d%% >", volPercent), Vector2{ btnRec.x + 400, btnRec.y + 22 }, 30, 2, isHover ? YELLOW : SKYBLUE);
        }
        else if (i == 3) {
            DrawTextEx(gameFont, "GIỚI HẠN GIÂY:", Vector2{ btnRec.x + 30, btnRec.y + 22 }, 30, 2, WHITE);
            DrawTextEx(gameFont, TextFormat("%d GIÂY", _turnTimeLimit), Vector2{ btnRec.x + 400, btnRec.y + 22 }, 30, 2, isHover ? YELLOW : SKYBLUE);
        }
        else if (i == 4) {
            DrawTextEx(gameFont, "QUAY LẠI MENU", Vector2{ btnRec.x + (btnRec.width - MeasureTextEx(gameFont, "QUAY LẠI MENU", 30, 2).x) / 2, btnRec.y + 22 }, 30, 2, isHover ? YELLOW : WHITE);
        }
    }

    EndDrawing();
}

// ==========================================
// VÒNG LẶP ĐIỀU PHỐI GAME CHÍNH
// ==========================================
void DrawAndHandleGame(Texture2D background, Font gameFont, Font pieceFont) {
    HandleGameInput();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawGameUI(background, gameFont, pieceFont);
    DrawAndHandleGameOver(gameFont);
    DrawAndHandlePauseMenu(gameFont);

    EndDrawing();
}

// ==========================================
// MÀN HÌNH TẢI GAME (LOAD GAME)
// ==========================================
void DrawAndHandleLoad(Texture2D background, Font gameFont) {
    static std::vector<std::string> pvp, pve;
    static bool l = false;
    static int f = 0, t = 0;

    if (!l) {
        pvp.clear();
        pve.clear();
        if (std::filesystem::exists("saves") && std::filesystem::is_directory("saves")) {
            for (const auto& e : std::filesystem::directory_iterator("saves")) {
                if (e.path().extension() == ".sav") {
                    std::string n = e.path().filename().string();
                    if (GetGameModeFromFile(n) == 1) pvp.push_back(n);
                    else pve.push_back(n);
                }
            }
        }
        f = 0;
        t = 0;
        l = true;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        PlaySelectSfx();
        _GAME_STATE = 0;
        l = false;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Vẽ nền
    DrawTexturePro(background,
        { 0, 0, (float)background.width, (float)background.height },
        { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        { 0, 0 }, 0.0f, WHITE);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.45f));

    // Tiêu đề trang
    DrawTextEx(gameFont, "TẢI TRẠNG THÁI GAME",
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, "TẢI TRẠNG THÁI GAME", 60, 2).x / 2, 60 },
        60, 2, WHITE);

    // --- LOGIC TAB & DANH SÁCH ---
    std::vector<std::string>& currentList = (t == 0) ? pvp : pve;
    int maxF = (int)currentList.size();

    // Điều hướng Tab (Trái/Phải)
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
        t = 1 - t;
        f = 0; // Reset focus khi đổi tab
        PlayNavigateSfx();
    }

    // Điều hướng Danh sách (Lên/Xuống)
    if (maxF > 0) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            f = (f - 1 + maxF) % maxF;
            PlayNavigateSfx();
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            f = (f + 1) % maxF;
            PlayNavigateSfx();
        }
    }

    // --- VẼ TAB ---
    Rectangle rPvP = { (float)GetScreenWidth() / 2 - 420, 160, 400, 70 };
    Rectangle rPvE = { (float)GetScreenWidth() / 2 + 20, 160, 400, 70 };

    // Vẽ Tab PvP
    DrawRectangleRounded(rPvP, 0.2f, 10, (t == 0) ? Fade(SKYBLUE, 0.6f) : Fade(BLACK, 0.3f));
    if (t == 0) DrawRectangleRoundedLines(rPvP, 0.2f, 10, YELLOW);
    DrawTextEx(gameFont, "PVP SAVES", { rPvP.x + (400 - MeasureTextEx(gameFont, "PVP SAVES", 30, 2).x) / 2, rPvP.y + 20 }, 30, 2, (t == 0) ? YELLOW : WHITE);

    // Vẽ Tab PvE bo góc
    DrawRectangleRounded(rPvE, 0.2f, 10, (t == 1) ? Fade(SKYBLUE, 0.6f) : Fade(BLACK, 0.3f));
    if (t == 1) DrawRectangleRoundedLines(rPvE, 0.2f, 10, YELLOW);
    DrawTextEx(gameFont, "PVE SAVES", { rPvE.x + (400 - MeasureTextEx(gameFont, "PVE SAVES", 30, 2).x) / 2, rPvE.y + 20 }, 30, 2, (t == 1) ? YELLOW : WHITE);

    // --- VẼ DANH SÁCH FILE ---
    Rectangle listArea = { (float)GetScreenWidth() / 2 - 450, 260, 900, 500 };
    DrawRectangleRounded(listArea, 0.05f, 10, Fade(RAYWHITE, 0.9f));
    DrawRectangleRoundedLines(listArea, 0.05f, 10, Fade(DARKGRAY, 0.5f));

    if (maxF == 0) {
        DrawTextEx(gameFont, "HIỆN CHƯA CÓ FILE NÀO ĐƯỢC LƯU",
            { listArea.x + (listArea.width - MeasureTextEx(gameFont, "HIỆN CHƯA CÓ FILE NÀO ĐƯỢC LƯU", 30, 2).x) / 2, listArea.y + 200 },
            30, 2, GRAY);
    }
    else {
        for (int i = 0; i < maxF; i++) {
            Rectangle itemR = { listArea.x + 20, listArea.y + 20 + i * 65, listArea.width - 40, 60 };
            bool isFocused = (f == i);

            // Xử lý chuột
            if (CheckCollisionPointRec(GetMousePosition(), itemR)) {
                if (f != i) { f = i; PlayNavigateSfx(); }
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (LoadGame(currentList[f])) {
                        PlaySelectSfx();
                        _GAME_STATE = 1;
                        l = false;
                    }
                }
            }

            DrawRectangleRounded(itemR, 0.2f, 10, isFocused ? Fade(SKYBLUE, 0.6f) : Fade(BLACK, 0.1f));
            if (isFocused) {
                DrawRectangleRoundedLines(itemR, 0.2f, 10, YELLOW);
                DrawRectangleRounded({ itemR.x - 10, itemR.y + 10, 6, itemR.height - 20 }, 1.0f, 0, YELLOW);
            }
            DrawTextEx(gameFont, currentList[i].c_str(), { itemR.x + 30, itemR.y + 15 }, 30, 2, isFocused ? YELLOW : BLACK);
        }
    }

    // Xử lý phím Enter để Load
    if (IsKeyPressed(KEY_ENTER) && maxF > 0) {
        if (LoadGame(currentList[f])) {
            PlaySelectSfx();
            _GAME_STATE = 1;
            l = false;
        }
    }

    const char* hint = "Dùng WASD/Chuột để chọn - ENTER để Tải - ESC để Quay lại";
    DrawTextEx(gameFont, hint,
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, hint, 25, 2).x / 2, (float)GetScreenHeight() - 120 },
        25, 2, WHITE);

    const char* escB = "Nhấn ESC để quay lại Menu";
    DrawTextEx(gameFont, escB,
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, escB, 30, 2).x / 2, (float)GetScreenHeight() - 70 },
        30, 2, RED);

    EndDrawing();
}

// ==========================================
// MÀN HÌNH THIẾT LẬP NGƯỜI CHƠI (SETUP)
// ==========================================
void DrawAndHandleSetup(Texture2D background, Font gameFont) {
    static int step = 0;
    static int pokFocus = 0;
    static int nameCount = 0;
    static char name[51] = "\0";
    static bool init = false;

    if (!init) {
        step = pokFocus = nameCount = 0;
        name[0] = '\0';
        init = true;
    }

    LoadPokemonTextures();

    // --- BƯỚC 0 & 2: CHỌN POKEMON ---
    if (step == 0 || step == 2) {
        int oldF = pokFocus;

        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) pokFocus = (pokFocus + 14) % 15;
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) pokFocus = (pokFocus + 1) % 15;
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) pokFocus = (pokFocus + 10) % 15;
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) pokFocus = (pokFocus + 5) % 15;

        if (pokFocus != oldF) PlayNavigateSfx();

        if (IsKeyPressed(KEY_ENTER)) {
            PlaySelectSfx();
            if (step == 0) _P1_POKEMON = pokFocus;
            else _P2_POKEMON = pokFocus;
            step++;
            name[0] = '\0';
            nameCount = 0;
        }
    }
    else { // --- BƯỚC 1 & 3: NHẬP TÊN NGƯỜI CHƠI ---
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && nameCount < 40) {
                int blen = 0;
                const char* u8 = CodepointToUTF8(key, &blen);
                if (nameCount + blen < 50) {
                    for (int i = 0; i < blen; i++) {
                        name[nameCount++] = u8[i];
                    }
                    name[nameCount] = '\0';
                }
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && nameCount > 0) {
            do {
                nameCount--;
            } while (nameCount > 0 && (name[nameCount] & 0xC0) == 0x80);
            name[nameCount] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER) && nameCount > 0) {
            PlaySelectSfx();
            if (step == 1) {
                TextCopy(_P1_NAME, name);
                if (_GAME_MODE == 2) { // Vào game ngay nếu là chế độ PvE
                    _GAME_STATE = 1;
                    init = false;
                }
                else { // Chuyển sang chọn P2 nếu là PvP
                    step++;
                    pokFocus = 0;
                }
            }
            else {
                TextCopy(_P2_NAME, name);
                _GAME_STATE = 1;
                init = false;
            }
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        PlaySelectSfx();
        _GAME_STATE = 0;
        init = false;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Hình nền và lớp phủ
    DrawTexturePro(background,
        { 0, 0, (float)background.width, (float)background.height },
        { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        { 0, 0 }, 0.0f, WHITE);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.35f)); // Giảm xuống 35% cho sáng

    // Tiêu đề trang
    const char* title = (step < 2 ? "THIẾT LẬP NGƯỜI CHƠI 1 (X)" : "THIẾT LẬP NGƯỜI CHƠI 2 (O)");
    DrawTextEx(gameFont, title,
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, title, 60, 2).x / 2, 40 },
        60, 2, WHITE);

    bool isP1 = (step < 2);
    float gridX = isP1 ? 175 : 725;   // Căn lề trái 175px, khoảng cách giữa 2 phần là 100px
    float cardX = isP1 ? 1295 : 175;
    float midY = GetScreenHeight() / 2 - 300;

    // --- DANH SÁCH POKEMON (Bright Style like Name Board) ---
    float gridR = 0.05f;
    DrawRectangleRounded({ gridX - 20, midY - 20, 1020, 600 }, gridR, 10, Fade(RAYWHITE, 0.9f)); // Sáng giống bảng tên
    DrawRectangleRoundedLines({ gridX - 20, midY - 20, 1020, 600 }, gridR, 10, Fade(DARKGRAY, 0.2f));

    for (int i = 0; i < 15; i++) {
        Rectangle r = { gridX + (i % 5) * 200, midY + (i / 5) * 190, 160, 160 };
        bool isFocused = (pokFocus == i && (step == 0 || step == 2));

        if (isFocused) {
            DrawRectangleRounded(r, 0.2f, 10, Fade(SKYBLUE, 0.4f));
            DrawRectangleRoundedLines(r, 0.2f, 10, isP1 ? RED : BLUE);
            // Indicator sáng bên dưới
            DrawRectangleRounded({ r.x + 20, r.y + r.height - 5, r.width - 40, 4 }, 1.0f, 0, isP1 ? RED : BLUE);
        }
        else {
            DrawRectangleRounded(r, 0.2f, 10, Fade(BLACK, 0.05f)); // Màu xám cực nhẹ để phân biệt ô
            DrawRectangleRoundedLines(r, 0.2f, 10, Fade(GRAY, 0.2f));
        }

        DrawTexturePro(_POKEMON_TEXTURES[i],
            { 0, 0, (float)_POKEMON_TEXTURES[i].width, (float)_POKEMON_TEXTURES[i].height },
            { r.x + 10, r.y + 10, 140, 140 }, { 0, 0 }, 0.0f, WHITE);
    }

    // --- THẺ XEM TRƯỚC THÔNG TIN ---
    int cardW = 450;
    int cardH = 750;
    float cardY = GetScreenHeight() / 2 - cardH / 2;

    DrawRectangleRounded({ cardX, cardY, (float)cardW, (float)cardH }, 0.05f, 10, Fade(RAYWHITE, 0.9f));
    DrawRectangleRoundedLines({ cardX, cardY, (float)cardW, (float)cardH }, 0.05f, 10, Fade(isP1 ? RED : BLUE, 0.5f));

    // Tiêu đề thẻ
    // Tiêu đề thẻ bo góc trên
    DrawRectangleRounded({ cardX, cardY, (float)cardW, 80 }, 0.2f, 10, isP1 ? RED : BLUE);
    const char* hText = isP1 ? "PLAYER X INFO" : "PLAYER O INFO";
    DrawTextEx(gameFont, hText,
        { cardX + (cardW - MeasureTextEx(gameFont, hText, 35, 2).x) / 2, cardY + 22 },
        35, 2, WHITE);

    // Hình ảnh Pokemon đang chọn
    Texture2D currentTex = _POKEMON_TEXTURES[pokFocus];
    if (step == 1) currentTex = _POKEMON_TEXTURES[_P1_POKEMON];
    if (step == 3) currentTex = _POKEMON_TEXTURES[_P2_POKEMON];

    DrawTexturePro(currentTex,
        { 0, 0, (float)currentTex.width, (float)currentTex.height },
        { cardX + 50, cardY + 120, 350, 350 }, { 0, 0 }, 0.0f, WHITE);

    // Ô nhập tên người chơi
    DrawTextEx(gameFont, "TÊN NGƯỜI CHƠI:", { cardX + 40, cardY + 500 }, 25, 2, DARKGRAY);

    Rectangle nameBox = { cardX + 30, cardY + 540, (float)cardW - 60, 80 };
    DrawRectangleRounded(nameBox, 0.2f, 10, (step % 2 == 1) ? WHITE : LIGHTGRAY);
    if (step % 2 == 1) DrawRectangleRoundedLines(nameBox, 0.2f, 10, isP1 ? RED : BLUE);

    if (step % 2 == 1) {
        DrawTextEx(gameFont, name, { nameBox.x + 20, nameBox.y + 20 }, 40, 2, isP1 ? RED : BLUE);
        // Hiệu ứng con trỏ nhấp nháy
        if (((int)(GetTime() * 2)) % 2 == 0) {
            DrawTextEx(gameFont, "_",
                { nameBox.x + 25 + MeasureTextEx(gameFont, name, 40, 2).x, nameBox.y + 20 },
                40, 2, isP1 ? RED : BLUE);
        }
    }
    else {
        DrawTextEx(gameFont, "Đang chọn Pokemon...", { nameBox.x + 20, nameBox.y + 25 }, 25, 2, GRAY);
    }

    // Gợi ý điều hướng
    const char* hint = (step % 2 == 0) ? "Dùng WASD để chọn hình" : "Nhập tên và nhấn ENTER";
    DrawTextEx(gameFont, hint,
        { cardX + (cardW - MeasureTextEx(gameFont, hint, 25, 2).x) / 2, cardY + 680 },
        25, 2, DARKGRAY);

    // Global Footer Hint
    const char* escB = "Nhấn ESC để quay lại Menu";
    DrawTextEx(gameFont, escB,
        { (float)GetScreenWidth() / 2 - MeasureTextEx(gameFont, escB, 30, 2).x / 2, (float)GetScreenHeight() - 70 },
        30, 2, RED);

    EndDrawing();
}