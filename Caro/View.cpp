#include "Data.h"
#include "View.h"
#include "Bot.h"
#include "Logic.h"

// ==========================================================
// HÀM VẼ BÀN CỜ VÀ QUÂN CỜ BẰNG RAYLIB
// ==========================================================
void DrawBoardRaylib() {
    // 1. Vẽ lưới bàn cờ (Sử dụng các hằng số LEFT, TOP, CELL_SIZE từ Data.h)
    for (int i = 0; i <= BOARD_SIZE; i++) {
        // Vẽ các đường ngang (Chạy từ trái sang phải)
        DrawLine(LEFT, TOP + i * CELL_SIZE, LEFT + BOARD_SIZE * CELL_SIZE, TOP + i * CELL_SIZE, BLACK);
        // Vẽ các đường dọc (Chạy từ trên xuống dưới)
        DrawLine(LEFT + i * CELL_SIZE, TOP, LEFT + i * CELL_SIZE, TOP + BOARD_SIZE * CELL_SIZE, BLACK);
    }

    // 2. Duyệt qua mảng _A để vẽ các quân cờ đã đánh
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            // Tinh chỉnh cộng thêm 10 (trục x) và 5 (trục y) để chữ X, O nằm ở tâm ô cờ
            if (_A[i][j].c == -1) DrawText("X", _A[i][j].x + 10, _A[i][j].y + 5, CELL_SIZE - 10, RED);
            if (_A[i][j].c == 1)  DrawText("O", _A[i][j].x + 10, _A[i][j].y + 5, CELL_SIZE - 10, BLUE);
        }
    }

    // 3. Vẽ khung chọn (Cursor) màu xanh lá để biết người chơi đang trỏ vào ô nào
    DrawRectangleLinesEx(Rectangle{ (float)_X, (float)_Y, (float)CELL_SIZE, (float)CELL_SIZE }, 4.0f, DARKGREEN);

    // 4. Nếu trận đấu đã có người thắng (-1 là X, 1 là O), vẽ đường thẳng nối 5 quân cờ
    if (_WINNER == -1 || _WINNER == 1) {
        // Tính tọa độ tâm của quân cờ đầu tiên trong chuỗi thắng
        float startX = _A[_winLine.r1][_winLine.c1].x + CELL_SIZE / 2.0f;
        float startY = _A[_winLine.r1][_winLine.c1].y + CELL_SIZE / 2.0f;

        // Tính tọa độ tâm của quân cờ cuối cùng trong chuỗi thắng
        float endX = _A[_winLine.r2][_winLine.c2].x + CELL_SIZE / 2.0f;
        float endY = _A[_winLine.r2][_winLine.c2].y + CELL_SIZE / 2.0f;

        Vector2 startPos = { startX, startY };
        Vector2 endPos = { endX, endY };

        // Chọn màu nét vẽ dựa trên người chiến thắng
        Color winColor = (_WINNER == -1) ? RED : BLUE;

        // Vẽ đường gạch chéo đi qua 5 quân cờ (Độ dày 10.0f)
        DrawLineEx(startPos, endPos, 10.0f, winColor);
    }
}

// ==========================================================
// HÀM XỬ LÝ VÀ VẼ MAIN MENU
// ==========================================================
void DrawAndHandleMenu(Texture2D background, Font gameFont) {
    // Biến static lưu vị trí nút đang được chọn (0: 2 Người chơi, 1: Đánh với máy)
    static int menuFocus = 0;

    // --- LOGIC NHẬN BÀN PHÍM (Đổi focus khi ấn W/S hoặc Lên/Xuống) ---
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) menuFocus = 0;
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) menuFocus = 1;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Vẽ ảnh nền toàn màn hình
    DrawTexturePro(background,
        Rectangle{ 0, 0, (float)background.width, (float)background.height },
        Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        Vector2{ 0, 0 }, 0.0f, WHITE);

    // Vẽ một lớp mờ màu đen đè lên hình nền để chữ dễ đọc hơn
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

    // Vẽ Tiêu đề Game
    DrawTextEx(gameFont, "GAME CO CARO", Vector2{ (float)GetScreenWidth() / 2 - 300, 200 }, 100, 2, WHITE);

    // Khai báo kích thước và vị trí của 2 nút bấm
    Rectangle btnPvP = { (float)GetScreenWidth() / 2 - 250, 400, 500, 80 };
    Rectangle btnPvE = { (float)GetScreenWidth() / 2 - 250, 520, 500, 80 };

    // --- LOGIC NHẬN CHUỘT ---
    // Cập nhật menuFocus nếu chuột di chuyển vào khu vực của nút
    bool mouseOnPvP = CheckCollisionPointRec(GetMousePosition(), btnPvP);
    bool mouseOnPvE = CheckCollisionPointRec(GetMousePosition(), btnPvE);
    if (mouseOnPvP) menuFocus = 0;
    if (mouseOnPvE) menuFocus = 1;

    // Xác định xem nút nào đang được hover (bằng cả chuột và phím) để đổi màu
    bool hoverPvP = (menuFocus == 0);
    bool hoverPvE = (menuFocus == 1);

    // Vẽ nút "ĐÁNH 2 NGƯỜI"
    DrawRectangleRec(btnPvP, hoverPvP ? DARKGRAY : LIGHTGRAY);
    DrawRectangleLinesEx(btnPvP, 4, BLACK);
    DrawTextEx(gameFont, "1. DANH 2 NGUOI", Vector2{ btnPvP.x + 80, btnPvP.y + 20 }, 40, 2, hoverPvP ? WHITE : BLACK);

    // Vẽ nút "ĐÁNH VỚI MÁY"
    DrawRectangleRec(btnPvE, hoverPvE ? DARKGRAY : LIGHTGRAY);
    DrawRectangleLinesEx(btnPvE, 4, BLACK);
    DrawTextEx(gameFont, "2. DANH VOI MAY", Vector2{ btnPvE.x + 80, btnPvE.y + 20 }, 40, 2, hoverPvE ? WHITE : BLACK);

    // --- XỬ LÝ KHI NGƯỜI DÙNG CLICK HOẶC BẤM ENTER ---
    if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (mouseOnPvP || mouseOnPvE))) {
        if (menuFocus == 0) {
            // Chế độ PvP
            _GAME_MODE = 1;
            _GAME_STATE = 1;
            _IS_PAUSED = false;
            ResetData();
            _p1Moves = 0;
            _p2Moves = 0;
            _WINNER = 2; // Đặt lại cờ chưa ai thắng
        }
        else if (menuFocus == 1) {
            // Chế độ PvE (Bot)
            _GAME_MODE = 2;
            _GAME_STATE = 2;
            _IS_PAUSED = false;
            ResetData();
            _p1Moves = 0;
            _p2Moves = 0;
            _WINNER = 2;
        }
    }
    EndDrawing();
}

// ==========================================================
// 1. HÀM XỬ LÝ NHẬP PHÍM VÀ LOGIC GAME KHI ĐANG CHƠI (KHÔNG VẼ)
// ==========================================================
void HandleGameInput() {
    // Bấm ESC để mở/đóng Menu tạm dừng (chỉ khi chưa ai thắng)
    if (IsKeyPressed(KEY_ESCAPE) && _WINNER == 2) {
        _IS_PAUSED = !_IS_PAUSED;
    }

    // Nếu game đang chạy bình thường (chưa ai thắng và không bị Pause)
    if (_WINNER == 2 && !_IS_PAUSED) {
        if (_TURN == true) { // --- LƯỢT NGƯỜI CHƠI 1 (Điều khiển bằng W A S D) ---
            if (IsKeyPressed(KEY_W)) MoveUp();
            if (IsKeyPressed(KEY_S)) MoveDown();
            if (IsKeyPressed(KEY_A)) MoveLeft();
            if (IsKeyPressed(KEY_D)) MoveRight();

            // Nhấn Enter để đánh cờ
            if (IsKeyPressed(KEY_ENTER)) {
                // CheckBoard trả về khác 0 nếu đánh thành công
                if (CheckBoard(_X, _Y) != 0) {
                    _p1Moves++; // Tăng số bước
                    _WINNER = TestBoard(_X, _Y); // Kiểm tra thắng thua
                    // Nếu chưa ai thắng thì đổi lượt cho người 2/Bot
                    if (_WINNER == 2) _TURN = !_TURN;
                }
            }
        }
        else { // --- LƯỢT NGƯỜI CHƠI 2 HOẶC MÁY ---
            if (_GAME_MODE == 1) { // LƯỢT NGƯỜI CHƠI 2 (Điều khiển bằng Phím mũi tên)
                if (IsKeyPressed(KEY_UP)) MoveUp();
                if (IsKeyPressed(KEY_DOWN)) MoveDown();
                if (IsKeyPressed(KEY_LEFT)) MoveLeft();
                if (IsKeyPressed(KEY_RIGHT)) MoveRight();

                // Dùng Numpad Enter hoặc Enter thường
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
                    if (CheckBoard(_X, _Y) != 0) {
                        _p2Moves++;
                        _WINNER = TestBoard(_X, _Y);
                        if (_WINNER == 2) _TURN = !_TURN;
                    }
                }
            }
            else if (_GAME_MODE == 2) { // LƯỢT CỦA MÁY (BOT)
                BotMove(); // Bot tính toán và di chuyển con trỏ _X, _Y

                // Đánh cờ tại vị trí Bot vừa quyết định
                if (CheckBoard(_X, _Y) != 0) {
                    _p2Moves++;
                    _WINNER = TestBoard(_X, _Y);
                    if (_WINNER == 2) _TURN = !_TURN;
                }
            }
        }
    }
}

// ==========================================================
// 2. HÀM VẼ GIAO DIỆN NỀN VÀ THÔNG SỐ (LỚP CƠ BẢN NHẤT)
// ==========================================================
void DrawGameUI(Texture2D background, Font gameFont) {
    // Vẽ nền
    DrawTexturePro(background,
        Rectangle{ 0, 0, (float)background.width, (float)background.height },
        Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        Vector2{ 0, 0 }, 0.0f, WHITE);

    // Vẽ bàn cờ và các quân cờ lên trên nền
    DrawBoardRaylib();

    // Vẽ thông tin Người chơi 1
    DrawTextEx(gameFont, "PLAYER 1 (X)", Vector2{ 50, 200 }, 50, 2, RED);
    DrawTextEx(gameFont, TextFormat("number of moves: %d", _p1Moves), Vector2{ 50, 250 }, 30, 2, BLACK);

    // Vẽ thông tin Người chơi 2 / Máy
    if (_GAME_MODE == 1) DrawTextEx(gameFont, "PLAYER 2 (O)", Vector2{ 50, 700 }, 50, 2, BLUE);
    else DrawTextEx(gameFont, "COMPUTER (O)", Vector2{ 50, 700 }, 50, 2, BLUE);
    DrawTextEx(gameFont, TextFormat("number of moves: %d", _p2Moves), Vector2{ 50, 750 }, 30, 2, BLACK);

    // Vẽ thanh thông báo lượt đánh (Chỉ hiển thị khi game chưa kết thúc)
    if (_WINNER == 2) {
        if (_TURN) DrawTextEx(gameFont, "TURN: PLAYER 1", Vector2{ 50, 470 }, 50, 2, RED);
        else {
            if (_GAME_MODE == 1) DrawTextEx(gameFont, "TURN: PLAYER 2", Vector2{ 50, 470 }, 50, 2, BLUE);
            else DrawTextEx(gameFont, "COMPUTER THINKING...", Vector2{ 50, 470 }, 60, 2, BLUE);
        }
    }
}

// ==========================================================
// 3. HÀM XỬ LÝ VÀ VẼ BẢNG GAME OVER (LỚP PHỦ SỐ 1)
// ==========================================================
void DrawAndHandleGameOver(Font gameFont) {
    if (_WINNER == 2) return; // Nếu game chưa kết thúc thì bỏ qua hàm này

    static int gameOverFocus = 0; // 0: Nút Play Again, 1: Nút Menu
    static int waitTimer = 0;
    waitTimer++;
    // Nhận phím điều hướng trái phải
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) gameOverFocus = 0;
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) gameOverFocus = 1;

    // Phủ một lớp màu đen mờ lên toàn bộ màn hình game
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));

    // Tính toán vị trí xuất hiện của Hộp thoại Game Over (Nằm giữa màn hình)
    int boxWidth = 800; int boxHeight = 400;
    int boxX = (GetScreenWidth() - boxWidth) / 2;
    int boxY = (GetScreenHeight() - boxHeight) / 2;

    // Vẽ hộp thoại Game Over
    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(RAYWHITE, 0.3f));
    DrawRectangleLinesEx(Rectangle{ (float)boxX, (float)boxY, (float)boxWidth, (float)boxHeight }, 5, DARKGRAY);
    DrawTextEx(gameFont, "GAME OVER", Vector2{ (float)boxX + 200, (float)boxY + 50 }, 80, 2, DARKGRAY);

    // In ra kết quả trận đấu
    if (_WINNER == -1) DrawTextEx(gameFont, "PLAYER 1 (X) WIN!", Vector2{ (float)boxX + 100, (float)boxY + 150 }, 55, 2, RED);
    else if (_WINNER == 1) DrawTextEx(gameFont, (_GAME_MODE == 1) ? "PLAYER 2 (O) WIN!" : "COMPUTER WIN!", Vector2{ (float)boxX + 200, (float)boxY + 150 }, 55, 2, BLUE);
    else if (_WINNER == 0) DrawTextEx(gameFont, "DRAW!", Vector2{ (float)boxX + 180, (float)boxY + 150 }, 55, 2, DARKGRAY);

    // Tạo 2 nút bấm
    Rectangle btnPlayAgain = { (float)boxX + 100, (float)boxY + 280, 280, 80 };
    Rectangle btnMenu = { (float)boxX + 420, (float)boxY + 280, 280, 80 };

    // Cập nhật focus bằng chuột
    bool mouseOnPlay = CheckCollisionPointRec(GetMousePosition(), btnPlayAgain);
    bool mouseOnMenu = CheckCollisionPointRec(GetMousePosition(), btnMenu);
    if (mouseOnPlay) gameOverFocus = 0;
    if (mouseOnMenu) gameOverFocus = 1;

    bool isHoverPlay = (gameOverFocus == 0);
    bool isHoverMenu = (gameOverFocus == 1);

    // Vẽ nút "PLAY AGAIN"
    DrawRectangleRec(btnPlayAgain, isHoverPlay ? DARKGRAY : LIGHTGRAY); DrawRectangleLinesEx(btnPlayAgain, 3, BLACK);
    DrawTextEx(gameFont, "PLAY AGAIN", Vector2{ btnPlayAgain.x + 40, btnPlayAgain.y + 20 }, 45, 2, isHoverPlay ? WHITE : BLACK);

    // Vẽ nút "MENU"
    DrawRectangleRec(btnMenu, isHoverMenu ? DARKGRAY : LIGHTGRAY); DrawRectangleLinesEx(btnMenu, 3, BLACK);
    DrawTextEx(gameFont, "MENU", Vector2{ btnMenu.x + 50, btnMenu.y + 20 }, 45, 2, isHoverMenu ? WHITE : BLACK);

    // Xử lý khi nhấn nút
    if (waitTimer > 30) {
        if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (mouseOnPlay || mouseOnMenu))) {
            if (gameOverFocus == 0) {
                int currentMode = _GAME_MODE;
                ResetData();
                _p1Moves = 0; _p2Moves = 0; _WINNER = 2;
                _GAME_MODE = currentMode;
                gameOverFocus = 0;

                waitTimer = 0; // Trả lại timer về 0 cho ván sau
            }
            else if (gameOverFocus == 1) {
                _GAME_STATE = 0;

                waitTimer = 0; // Trả lại timer về 0
            }
        }
    }
}

// ==========================================================
// 4. HÀM XỬ LÝ VÀ VẼ BẢNG TẠM DỪNG (LỚP PHỦ SỐ 2)
// ==========================================================
void DrawAndHandlePauseMenu(Font gameFont) {
    if (!_IS_PAUSED || _WINNER != 2) return; // Chỉ chạy khi đang Pause và Game chưa kết thúc

    static int pauseFocus = 0; // 0: Resume, 1: Menu

    // Nhận phím Lên/Xuống
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) pauseFocus = 0;
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) pauseFocus = 1;

    // Phủ lớp đen mờ
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));

    // Vẽ hộp thoại Pause
    int boxWidth = 500; int boxHeight = 400;
    int boxX = (GetScreenWidth() - boxWidth) / 2;
    int boxY = (GetScreenHeight() - boxHeight) / 2;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(RAYWHITE, 0.9f));
    DrawRectangleLinesEx(Rectangle{ (float)boxX, (float)boxY, (float)boxWidth, (float)boxHeight }, 5, DARKGRAY);
    DrawTextEx(gameFont, "PAUSE", Vector2{ (float)boxX + 100, (float)boxY + 50 }, 70, 2, DARKGRAY);

    // Tạo 2 nút bấm
    Rectangle btnContinue = { (float)boxX + 100, (float)boxY + 160, 300, 70 };
    Rectangle btnMenu = { (float)boxX + 100, (float)boxY + 260, 300, 70 };

    // Update focus bằng chuột
    bool mouseOnContinue = CheckCollisionPointRec(GetMousePosition(), btnContinue);
    bool mouseOnMenu = CheckCollisionPointRec(GetMousePosition(), btnMenu);
    if (mouseOnContinue) pauseFocus = 0;
    if (mouseOnMenu) pauseFocus = 1;

    bool isHoverContinue = (pauseFocus == 0);
    bool isHoverMenu = (pauseFocus == 1);

    // Vẽ nút "RESUME"
    DrawRectangleRec(btnContinue, isHoverContinue ? DARKGRAY : LIGHTGRAY); DrawRectangleLinesEx(btnContinue, 3, BLACK);
    DrawTextEx(gameFont, "RESUME", Vector2{ btnContinue.x + 60, btnContinue.y + 15 }, 45, 2, isHoverContinue ? WHITE : BLACK);

    // Vẽ nút "MENU"
    DrawRectangleRec(btnMenu, isHoverMenu ? DARKGRAY : LIGHTGRAY); DrawRectangleLinesEx(btnMenu, 3, BLACK);
    DrawTextEx(gameFont, "MENU", Vector2{ btnMenu.x + 70, btnMenu.y + 15 }, 45, 2, isHoverMenu ? WHITE : BLACK);

    // Xử lý khi ấn nút
    if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (mouseOnContinue || mouseOnMenu))) {
        if (pauseFocus == 0) {
            _IS_PAUSED = false; // Tiếp tục chơi
        }
        else if (pauseFocus == 1) {
            _GAME_STATE = 0;    // Về màn hình chính
            _IS_PAUSED = false; // Phải tắt Pause đi để lần sau chơi game không bị lỗi dính Menu
        }
    }
}

// ==========================================================
// HÀM XỬ LÝ MENU CHỌN CHẾ ĐỘ BOT
// ==========================================================
// Trong View.cpp
void DrawAndHandleDifficultyMenu(Font gameFont) {
    static int diffFocus = 1; // 0: Easy, 1: Normal, 2: Hard

    // 1. Nhận phím điều hướng (A/D hoặc Trái/Phải)
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
        diffFocus = (diffFocus - 1 + 3) % 3;
    }
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
        diffFocus = (diffFocus + 1) % 3;
    }

    BeginDrawing();
    ClearBackground(WHITE); 

    DrawTextEx(gameFont, "Chon do kho", Vector2{ (float)GetScreenWidth()/2 - 250, 200 }, 60, 2, BLACK);

    const char* diffLabels[] = { "De", "Trung binh", "Kho" };
    for (int i = 0; i < 3; i++) {
        Rectangle btn = { (float)GetScreenWidth()/2 - 450 + (i * 320), 400, 260, 100 };
        bool isHover = (diffFocus == i);

        // Logic hover bằng chuột: Di chuyển chuột vào nút nào thì focus vào nút đó
        if (CheckCollisionPointRec(GetMousePosition(), btn)) diffFocus = i;

        DrawRectangleRec(btn, isHover ? RED : GRAY);
        DrawRectangleLinesEx(btn, 4, WHITE);
        
        // Căn lề chữ vào giữa nút (tùy chỉnh tọa độ +50, +30 cho khớp font của bạn)
        DrawTextEx(gameFont, diffLabels[i], Vector2{ btn.x + 50, btn.y + 30 }, 40, 2, isHover ? WHITE : BLACK);

        // Xử lý Click chuột trái
        if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            _BOT_DIFFICULTY = diffFocus;
            _GAME_MODE = 2;   // Xác nhận đánh với máy
            _GAME_STATE = 1;  // Vào game
            ResetData();      // Xóa bàn cờ cũ
            _p1Moves = 0; _p2Moves = 0; // Reset số bước đi
        }
    }

    // 2. Nhấn Enter để xác nhận
    if (IsKeyPressed(KEY_ENTER)) {
        _BOT_DIFFICULTY = diffFocus;
        _GAME_MODE = 2;
        _GAME_STATE = 1;
        ResetData();
        _p1Moves = 0; _p2Moves = 0;
    }
    
    // 3. Nhấn ESC để quay lại Menu chính
    if (IsKeyPressed(KEY_ESCAPE)) _GAME_STATE = 0;

    EndDrawing();
}

// ==========================================================
// HÀM CHÍNH ĐIỀU PHỐI TẤT CẢ LOGIC VÀ ĐỒ HỌA TRONG GAME
// ==========================================================
void DrawAndHandleGame(Texture2D background, Font gameFont) {

    // 1. Cập nhật các thay đổi từ bàn phím (di chuyển, đánh cờ, pause...)
    HandleGameInput();

    // 2. Render đồ họa ra màn hình
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Vẽ các layer từ dưới lên trên (Layer dưới sẽ bị layer trên đè lên)
    DrawGameUI(background, gameFont);        // Layer Dưới cùng: Bàn cờ, Avatar, Text thông báo
    DrawAndHandleGameOver(gameFont);         // Layer Giữa: Bảng Game Over (Chỉ hiện khi có người thắng)
    DrawAndHandlePauseMenu(gameFont);        // Layer Trên cùng: Bảng Pause (Chỉ hiện khi ấn ESC)

    EndDrawing();
}