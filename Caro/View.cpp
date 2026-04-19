#include "Data.h"
#include "View.h"
#include "Bot.h"
#include "Logic.h"
#include "SaveLoad.h"
#include "raylib.h" 
#include "Audio.h"

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
    static int menuFocus = 0;
    const int NUM_BUTTONS = 7;
    int oldFocus = menuFocus;

    // Mảng chứa tên các nút (Dễ dàng thêm/bớt sau này)
    const char* btnLabels[NUM_BUTTONS] = {
        "1. DANH 2 NGUOI",
        "2. DANH VOI MAY",
        "3. FILE GAME",
        "4. THONG TIN",
        "5. HUONG DAN",
        "6. CAI DAT",
        "7. THOAT GAME"
        
    };

    // --- LOGIC NHẬN BÀN PHÍM ---
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) menuFocus = (menuFocus - 1 + NUM_BUTTONS) % NUM_BUTTONS;
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) menuFocus = (menuFocus + 1) % NUM_BUTTONS;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Vẽ nền và tiêu đề
    DrawTexturePro(background, Rectangle{ 0, 0, (float)background.width, (float)background.height }, Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
    DrawTextEx(gameFont, "GAME CO CARO", Vector2{ (float)GetScreenWidth() / 2 - 300, 80 }, 100, 2, WHITE);

    // Thông số kích thước chung cho tất cả các nút
    int btnWidth = 500;
    int btnHeight = 80;
    int startX = GetScreenWidth() / 2 - btnWidth / 2;
    int startY = 220;
    int gap = 100; // Khoảng cách giữa các nút

    bool isClicked = false;
    int clickedIndex = -1;

    // --- VÒNG LẶP VẼ VÀ XỬ LÝ CÁC NÚT ---
    for (int i = 0; i < NUM_BUTTONS; i++) {
        Rectangle btnRec = { (float)startX, (float)(startY + i * gap), (float)btnWidth, (float)btnHeight };

        // Kiểm tra chuột có trỏ vào nút thứ i không
        if (CheckCollisionPointRec(GetMousePosition(), btnRec)) {
            menuFocus = i; // Chuột đè quyền của bàn phím
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                isClicked = true;
                clickedIndex = i;
            }
        }

        bool isFocused = (menuFocus == i);

        // Vẽ nút: Đổi màu và làm viền dày lên (6px) nếu đang được chọn
        DrawRectangleRec(btnRec, isFocused ? DARKGRAY : LIGHTGRAY);
        DrawRectangleLinesEx(btnRec, isFocused ? 6.0f : 4.0f, BLACK);

        // Vẽ chữ (tự động nội suy từ mảng btnLabels)
        DrawTextEx(gameFont, btnLabels[i], Vector2{ btnRec.x + 80, btnRec.y + 20 }, 40, 2, isFocused ? WHITE : BLACK);
    }
    
    if (menuFocus != oldFocus) PlayNavigateSfx();

    // Nhận thêm phím ENTER
    if (IsKeyPressed(KEY_ENTER)) {
        isClicked = true;
        clickedIndex = menuFocus;
    }

    // --- XỬ LÝ CHUYỂN CẢNH KHI ĐÃ CLICK ---
    if (isClicked) {
        PlaySelectSfx();
        if (clickedIndex == 0) { // PvP
            _GAME_MODE = 1; _GAME_STATE = 1; _IS_PAUSED = false;
            ResetData(); _p1Moves = 0; _p2Moves = 0; _WINNER = 2;
        }
        else if (clickedIndex == 1) {
            // Chế độ PvE (Bot)
            _GAME_MODE = 2;
            _GAME_STATE = 2;
            _IS_PAUSED = false;
            ResetData();
            _p1Moves = 0;
            _p2Moves = 0;
            _WINNER = 2;
        }
        else if (clickedIndex == 2) { // File Game
            _GAME_STATE = 3;
        }
        else if (clickedIndex == 3) { // Thông tin
            _GAME_STATE = 4;
        }
        else if (clickedIndex == 4) { // Hướng dẫn
            _GAME_STATE = 5;
        }
        else if (clickedIndex == 5) {
            _GAME_STATE = 6;
        }
        else if (clickedIndex == 6) {
            _GAME_STATE = -1;
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
        PlayPauseSfx();
    }

    // Nếu game đang chạy bình thường (chưa ai thắng và không bị Pause)
    if (_WINNER == 2 && !_IS_PAUSED) {
        if (_TURN == true) { // --- LƯỢT NGƯỜI CHƠI 1 (Điều khiển bằng W A S D) ---
            int oldX = _X;
            int oldY = _Y;

            if (IsKeyPressed(KEY_W)) MoveUp();
            if (IsKeyPressed(KEY_S)) MoveDown();
            if (IsKeyPressed(KEY_A)) MoveLeft();
            if (IsKeyPressed(KEY_D)) MoveRight();

            if (_X != oldX || _Y != oldY) PlayBoardMoveSfx();

            // Nhấn Enter để đánh cờ
            if (IsKeyPressed(KEY_ENTER)) {
                // CheckBoard trả về khác 0 nếu đánh thành công
                if (CheckBoard(_X, _Y) != 0) {
                    PlayPlaceSfx();
                    _p1Moves++; // Tăng số bước
                    _WINNER = TestBoard(_X, _Y); // Kiểm tra thắng thua
                    // Nếu chưa ai thắng thì đổi lượt cho người 2/Bot
                    if (_WINNER == 2) _TURN = !_TURN;
                }
            }
        }
        else { // --- LƯỢT NGƯỜI CHƠI 2 HOẶC MÁY ---
            if (_GAME_MODE == 1) { // LƯỢT NGƯỜI CHƠI 2 (Điều khiển bằng Phím mũi tên)
                int oldX = _X;
                int oldY = _Y;

                if (IsKeyPressed(KEY_UP)) MoveUp();
                if (IsKeyPressed(KEY_DOWN)) MoveDown();
                if (IsKeyPressed(KEY_LEFT)) MoveLeft();
                if (IsKeyPressed(KEY_RIGHT)) MoveRight();

                if (_X != oldX || _Y != oldY) PlayBoardMoveSfx();

                // Dùng Numpad Enter hoặc Enter thường
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
                    if (CheckBoard(_X, _Y) != 0) {
                        PlayPlaceSfx();
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
                    PlayPlaceSfx();
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
    DrawTextEx(gameFont, "PLAYER 1 (X)", Vector2{ 1100, 100 }, 80, 2, RED);
    DrawTextEx(gameFont, TextFormat("number of moves: %d", _p1Moves), Vector2{ 1100, 200 }, 40, 2, BLACK);

    // Vẽ thông tin Người chơi 2 / Máy
    if (_GAME_MODE == 1) DrawTextEx(gameFont, "PLAYER 2 (O)", Vector2{ 1100, 700 }, 80, 2, BLUE);
    else DrawTextEx(gameFont, "COMPUTER (O)", Vector2{ 1100, 700 }, 50, 2, BLUE);
    DrawTextEx(gameFont, TextFormat("number of moves: %d", _p2Moves), Vector2{ 1100, 800 }, 40, 2, BLACK);

    // Vẽ thanh thông báo lượt đánh (Chỉ hiển thị khi game chưa kết thúc)
    if (_WINNER == 2) {
        if (_TURN) DrawTextEx(gameFont, "TURN: PLAYER 1", Vector2{ 1100, 450 }, 70, 2, RED);
        else {
            if (_GAME_MODE == 1) DrawTextEx(gameFont, "TURN: PLAYER 2", Vector2{ 1100, 450 }, 70, 2, BLUE);
            else DrawTextEx(gameFont, "COMPUTER THINKING...", Vector2{ 1100, 450 }, 70, 2, BLUE);
        }
    }
}

// ==========================================================
// 3. HÀM XỬ LÝ VÀ VẼ BẢNG GAME OVER (LỚP PHỦ SỐ 1)
// ==========================================================
void DrawAndHandleGameOver(Font gameFont) {
    if (_WINNER == 2) return; // Nếu game chưa kết thúc thì bỏ qua hàm này

    static int gameOverFocus = 0; // 0: Nút Play Again, 1: Nút Menu
    int oldFocus = gameOverFocus;
    static int waitTimer = 0;
    waitTimer++;
    // Nhận phím điều hướng trái phải
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) gameOverFocus = 0;
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) gameOverFocus = 1;

    if (gameOverFocus != oldFocus) PlayNavigateSfx();

    // Phủ một lớp màu đen mờ lên toàn bộ màn hình game
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));

    // Tính toán vị trí xuất hiện của Hộp thoại Game End (Nằm giữa màn hình)
    int boxWidth = 800; int boxHeight = 400;
    int boxX = (GetScreenWidth() - boxWidth) / 2;
    int boxY = (GetScreenHeight() - boxHeight) / 2;

    // Vẽ hộp thoại Game Over
    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(RAYWHITE, 0.3f));
    DrawRectangleLinesEx(Rectangle{ (float)boxX, (float)boxY, (float)boxWidth, (float)boxHeight }, 5, DARKGRAY);
    DrawTextEx(gameFont, "GAME END!", Vector2{ (float)boxX + 200, (float)boxY + 50 }, 80, 2, DARKGRAY);

    // In ra kết quả trận đấu
    if (_WINNER == -1) DrawTextEx(gameFont, "PLAYER 1 (X) WIN!", Vector2{ (float)boxX + 150, (float)boxY + 150 }, 55, 2, RED);
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
            PlaySelectSfx();

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
    if (!_IS_PAUSED || _WINNER != 2) return; // Không pause thì thoát hàm

    const int NUM_BUTTONS = 4;
    static int pauseFocus = 0;
    int oldFocus = pauseFocus;

    // Mảng tên 4 nút chính
    const char* btnLabels[NUM_BUTTONS] = {
        "TIEP TUC",
        "CAI DAT",
        "LUU GAME",
        "VE MENU",
    };

    // Các biến phục vụ việc gõ tên file lưu
    static bool isTypingSave = false;
    static char saveName[50] = "\0";
    static int nameCount = 0;

    // CÁC BIẾN PHỤC VỤ BẢNG CÀI ĐẶT MINI
    static bool isSettings = false;
    static int setFocus = 0;
    int oldSetFocus = setFocus;
    bool justExitedSettings = false;
    // ==========================================
    // LOGIC NHẬN BÀN PHÍM & XỬ LÝ DỮ LIỆU
    // ==========================================
    if (isSettings) {
        // --- LOGIC TRONG MÀN HÌNH CÀI ĐẶT MINI ---
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) setFocus = (setFocus - 1 + 4) % 4;
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) setFocus = (setFocus + 1) % 4;

        if (setFocus != oldSetFocus) PlayNavigateSfx();

        if (setFocus == 0 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
            _BGM_ON = !_BGM_ON; PlaySelectSfx();
        }
        if (setFocus == 1 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
            _SFX_ON = !_SFX_ON; PlaySelectSfx();
        }
        if (setFocus == 2) {
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
                _MASTER_VOL -= 0.1f; if (_MASTER_VOL < 0.0f) _MASTER_VOL = 0.0f;
                SetMasterVolume(_MASTER_VOL); PlaySelectSfx();
            }
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
                _MASTER_VOL += 0.1f; if (_MASTER_VOL > 1.0f) _MASTER_VOL = 1.0f;
                SetMasterVolume(_MASTER_VOL); PlaySelectSfx();
            }
        }
        if ((setFocus == 3 && IsKeyPressed(KEY_ENTER)) || IsKeyPressed(KEY_ESCAPE)) {
            isSettings = false; // Thoát bảng cài đặt mini
            justExitedSettings = true;
            PlaySelectSfx();
        }
    }
    else if (isTypingSave) {
        // --- LOGIC GÕ TÊN FILE LƯU GAME ---
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (nameCount < 40)) {
                saveName[nameCount] = (char)key;
                saveName[nameCount + 1] = '\0';
                nameCount++;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && nameCount > 0) {
            nameCount--; saveName[nameCount] = '\0';
        }
        if (IsKeyPressed(KEY_ENTER) && nameCount > 0) {
            std::string fileName = std::string(saveName) + ".sav";
            PlaySelectSfx();
            SaveGame(fileName);
            isTypingSave = false;
            _IS_PAUSED = false;
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            isTypingSave = false;
            PlaySelectSfx();
        }
    }
    else {
        // --- LOGIC MÀN HÌNH PAUSE CHÍNH ---
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) pauseFocus = (pauseFocus - 1 + NUM_BUTTONS) % NUM_BUTTONS;
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) pauseFocus = (pauseFocus + 1) % NUM_BUTTONS;

        if (pauseFocus != oldFocus) PlayNavigateSfx();
    }

    // ==========================================
    // VẼ GIAO DIỆN CHUNG (LỚP MỜ VÀ CÁI HỘP)
    // ==========================================
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    // Điều chỉnh kích thước hộp linh hoạt theo từng trạng thái
    int boxWidth = isSettings ? 600 : 500;
    int boxHeight = isTypingSave ? 300 : (isSettings ? 500 : 550);
    int boxX = (GetScreenWidth() - boxWidth) / 2;
    int boxY = (GetScreenHeight() - boxHeight) / 2;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(RAYWHITE, 0.95f));
    DrawRectangleLinesEx(Rectangle{ (float)boxX, (float)boxY, (float)boxWidth, (float)boxHeight }, 5, DARKGRAY);

    // ==========================================
    // VẼ NỘI DUNG BÊN TRONG HỘP
    // ==========================================
    if (isSettings) {
        // ---- GIAO DIỆN BẢNG CÀI ĐẶT MINI ----
        DrawTextEx(gameFont, "CAI DAT", Vector2{ (float)boxX + 210, (float)boxY + 30 }, 55, 2, DARKGRAY);
        DrawText("A/D (Trai/Phai) de dieu chinh", boxX + 180, boxY + 100, 18, GREEN);

        int btnWidth = 500;
        int btnHeight = 60;
        int startX = boxX + 50;
        int startY = boxY + 140;
        int gap = 80;

        for (int i = 0; i < 4; i++) {
            Rectangle btnRec = { (float)startX, (float)(startY + i * gap), (float)btnWidth, (float)btnHeight };
            bool isHover = (setFocus == i);

            if (CheckCollisionPointRec(GetMousePosition(), btnRec)) {
                setFocus = i;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (i == 0) { 
                        _BGM_ON = !_BGM_ON; 
                        PlaySelectSfx(); 
                    }
                    if (i == 1) { 
                        _SFX_ON = !_SFX_ON; 
                        PlaySelectSfx(); 
                    }
                    if (i == 3) { 
                        isSettings = false; 
                        justExitedSettings = true; 
                        PlaySelectSfx(); 
                    }
                }
            }

            DrawRectangleRec(btnRec, isHover ? DARKGRAY : LIGHTGRAY);
            DrawRectangleLinesEx(btnRec, isHover ? 4.0f : 2.0f, BLACK);

            // Chữ nhỏ lại một chút (size 35) để vừa với hộp
            if (i == 0) {
                DrawTextEx(gameFont, "NHAC NEN:", Vector2{ btnRec.x + 20, btnRec.y + 15 }, 35, 2, isHover ? WHITE : BLACK);
                DrawTextEx(gameFont, _BGM_ON ? "[ BAT ]" : "[ TAT ]", Vector2{ btnRec.x + 350, btnRec.y + 15 }, 35, 2, _BGM_ON ? GREEN : RED);
            }
            else if (i == 1) {
                DrawTextEx(gameFont, "HIEU UNG:", Vector2{ btnRec.x + 20, btnRec.y + 15 }, 35, 2, isHover ? WHITE : BLACK);
                DrawTextEx(gameFont, _SFX_ON ? "[ BAT ]" : "[ TAT ]", Vector2{ btnRec.x + 350, btnRec.y + 15 }, 35, 2, _SFX_ON ? GREEN : RED);
            }
            else if (i == 2) {
                DrawTextEx(gameFont, "AM LUONG:", Vector2{ btnRec.x + 20, btnRec.y + 15 }, 35, 2, isHover ? WHITE : BLACK);
                int volPercent = (int)(_MASTER_VOL * 100 + 0.5f);
                DrawTextEx(gameFont, TextFormat("< %d%% >", volPercent), Vector2{ btnRec.x + 330, btnRec.y + 15 }, 35, 2, isHover ? YELLOW : BLACK);
            }
            else if (i == 3) {
                DrawTextEx(gameFont, "QUAY LAI", Vector2{ btnRec.x + 180, btnRec.y + 15 }, 35, 2, isHover ? WHITE : BLACK);
            }
        }
    }
    else if (isTypingSave) {
        // ---- GIAO DIỆN KHUNG NHẬP CHỮ LƯU GAME ----
        DrawTextEx(gameFont, "NHAP TEN FILE:", Vector2{ (float)boxX + 50, (float)boxY + 50 }, 50, 2, DARKGRAY);
        DrawRectangle(boxX + 40, boxY + 130, 420, 60, LIGHTGRAY);
        DrawTextEx(gameFont, saveName, Vector2{ (float)boxX + 50, (float)boxY + 140 }, 40, 2, RED);
        if (((int)(GetTime() * 2)) % 2 == 0) DrawText("_", boxX + 50 + MeasureText(saveName, 40), boxY + 140, 40, RED);
        DrawText("ENTER: Luu file | ESC: Huy bo", boxX + 90, boxY + 220, 20, DARKGRAY);
    }
    else {
        // ---- GIAO DIỆN 4 NÚT MENU PAUSE CHÍNH ----
        DrawTextEx(gameFont, "TAM DUNG", Vector2{ (float)boxX + 110, (float)boxY + 40 }, 65, 2, DARKGRAY);

        int btnWidth = 360;
        int btnHeight = 70;
        int startX = boxX + (boxWidth - btnWidth) / 2;
        int startY = boxY + 140;
        int gap = 90;

        bool isClicked = false;
        int clickedIndex = -1;

        for (int i = 0; i < NUM_BUTTONS; i++) {
            Rectangle btnRec = { (float)startX, (float)(startY + i * gap), (float)btnWidth, (float)btnHeight };

            if (CheckCollisionPointRec(GetMousePosition(), btnRec)) {
                pauseFocus = i;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    isClicked = true;
                    clickedIndex = i;
                }
            }

            bool isHover = (pauseFocus == i);
            DrawRectangleRec(btnRec, isHover ? DARKGRAY : LIGHTGRAY);
            DrawRectangleLinesEx(btnRec, isHover ? 5.0f : 3.0f, BLACK);

            Vector2 textSize = MeasureTextEx(gameFont, btnLabels[i], 45, 2);
            DrawTextEx(gameFont, btnLabels[i], Vector2{ btnRec.x + (btnWidth - textSize.x) / 2, btnRec.y + (btnHeight - textSize.y) / 2 }, 45, 2, isHover ? WHITE : BLACK);
        }

        if (IsKeyPressed(KEY_ENTER) && !justExitedSettings) {
            isClicked = true;
            clickedIndex = pauseFocus;
        }

        // XỬ LÝ SAU KHI CLICK NÚT TRONG PAUSE MENU
        if (isClicked) {
            PlaySelectSfx();
            if (clickedIndex == 0) {
                _IS_PAUSED = false;       // Tiếp tục game
            }
            else if (clickedIndex == 1) {
                isSettings = true;        // MỞ BẢNG CÀI ĐẶT MINI
                setFocus = 0;
            }
            else if (clickedIndex == 2) {
                isTypingSave = true;      // Mở bảng nhập tên Save
                saveName[0] = '\0';
                nameCount = 0;
            }
            else if (clickedIndex == 3) {
                _GAME_STATE = 0;          // Về menu chính
                _IS_PAUSED = false;
            }
        }
    }
}

//Hàm vẽ hướng dẫn
void DrawAndHandleInstructions(Texture2D background, Texture2D huongdanImg, Font gameFont) {
    // Bấm ESC để quay về Menu chính
    if (IsKeyPressed(KEY_ESCAPE)) {
        PlaySelectSfx();
        _GAME_STATE = 0;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // 1. Vẽ ảnh nền Menu ở dưới cùng
    DrawTexturePro(background, Rectangle{ 0, 0, (float)background.width, (float)background.height }, Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);

    // 2. Phủ lớp đen mờ 60%
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));

    // 3. Vẽ bức ảnh Hướng dẫn vào chính giữa màn hình
    int imgX = (GetScreenWidth() - huongdanImg.width) / 2;
    int imgY = (GetScreenHeight() - huongdanImg.height) / 2 -20;
    DrawTexture(huongdanImg, imgX, imgY, WHITE);

    // 4. Dòng chữ nhắc nhở bấm ESC (vẽ đè lên trên cùng)
    DrawTextEx(gameFont, "Nhan ESC de quay lai Menu", Vector2{ (float)GetScreenWidth() / 2 - 200, (float)GetScreenHeight() - 70 }, 30, 2, RED);

    EndDrawing();
}
// MÀN HÌNH THÔNG TIN NHÓM
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

    DrawTextEx(gameFont, "THONG TIN DO AN", Vector2{ (float)GetScreenWidth() / 2 - 250, 80 }, 70, 2, WHITE);

    // Căn lề trái cho đoạn text
    int startX = GetScreenWidth() / 2 - 350;
    int startY = 220;

    DrawTextEx(gameFont, "Truong: DH Khoa hoc Tu nhien - TPHCM", Vector2{ (float)startX, (float)startY }, 40, 2, LIGHTGRAY);
    DrawTextEx(gameFont, "Mon hoc: Co so lap trinh", Vector2{ (float)startX, (float)startY + 60 }, 40, 2, GREEN);
    DrawTextEx(gameFont, "Giang vien: Thay Truong Toan Thinh", Vector2{ (float)startX, (float)startY + 120 }, 40, 2, GREEN);
    DrawTextEx(gameFont, "Do an: co Caro", Vector2{ (float)startX, (float)startY + 170 }, 40, 2, GREEN);

    DrawTextEx(gameFont, "Nhom thuc hien: nhom 10", Vector2{ (float)startX, (float)startY + 220 }, 40, 2, YELLOW);

    // ĐIỀN TÊN VÀ MSSV VÀO ĐÂY
    DrawTextEx(gameFont, "1. Luong Nguyen Hoang Vu - 24120493", Vector2{ (float)startX + 50, (float)startY + 270 }, 35, 2, WHITE);
    DrawTextEx(gameFont, "2. Dinh Duc Hieu - 24120002", Vector2{ (float)startX + 50, (float)startY + 320 }, 35, 2, WHITE);
    DrawTextEx(gameFont, "3. Dao Thanh Phong - 24120006", Vector2{ (float)startX + 50, (float)startY + 370 }, 35, 2, WHITE);
	DrawTextEx(gameFont, "4. Le Hung Thang - 24120137", Vector2{ (float)startX + 50, (float)startY + 420 }, 35, 2, WHITE);
	DrawTextEx(gameFont, "5. Nguyen Anh Duc - 24120289", Vector2{ (float)startX + 50, (float)startY + 470 }, 35, 2, WHITE);

    DrawTextEx(gameFont, "Cong nghe: C++ & Thu vien Raylib", Vector2{ (float)startX, (float)startY + 540 }, 35, 2, LIGHTGRAY);

    // Hướng dẫn thoát
    DrawTextEx(gameFont, "Nhan ESC de quay lai Menu", Vector2{ (float)GetScreenWidth() / 2 - 200, (float)GetScreenHeight() - 70 }, 30, 2, RED);

    EndDrawing();
}

// ==========================================================
// HÀM XỬ LÝ MENU CHỌN CHẾ ĐỘ BOT
// ==========================================================
// Trong View.cpp
void DrawAndHandleDifficultyMenu(Texture2D background, Font gameFont) {
    static int diffFocus = 1; // 0: Easy, 1: Normal, 2: Hard
    int oldFocus = diffFocus;

    // =========================
    // 1. Nhận phím điều hướng
    // =========================
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
        diffFocus = (diffFocus - 1 + 3) % 3;
    }

    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
        diffFocus = (diffFocus + 1) % 3;
    }

    if (diffFocus != oldFocus) PlayNavigateSfx();

    BeginDrawing();

    // =========================
    // 2. VẼ ẢNH BACKGROUND
    // =========================
    DrawTexturePro(
        background,
        Rectangle{ 0, 0, (float)background.width, (float)background.height },
        Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        Vector2{ 0, 0 },
        0.0f,
        WHITE
    );

    // phủ lớp mờ đen cho nổi menu
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.45f));

    // =========================
    // 3. TIÊU ĐỀ
    // =========================
    DrawTextEx(
        gameFont,
        "CHON DO KHO",
        Vector2{ (float)GetScreenWidth() / 2 - 230, 180 },
        65,
        2,
        WHITE
    );

    const char* diffLabels[] = { "DE", "TRUNG BINH", "KHO" };

    // =========================
    // 4. VẼ 3 NÚT
    // =========================
    for (int i = 0; i < 3; i++) {
        Rectangle btn = {
            (float)GetScreenWidth() / 2 - 450 + (i * 320),
            400,
            260,
            100
        };

        // hover chuột
        if (CheckCollisionPointRec(GetMousePosition(), btn))
            diffFocus = i;

        bool isHover = (diffFocus == i);

        DrawRectangleRec(btn, isHover ? RED : GRAY);
        DrawRectangleLinesEx(btn, 4, WHITE);

        Vector2 textSize = MeasureTextEx(gameFont, diffLabels[i], 40, 2);

        DrawTextEx(
            gameFont,
            diffLabels[i],
            Vector2{
                btn.x + (btn.width - textSize.x) / 2,
                btn.y + (btn.height - textSize.y) / 2
            },
            40,
            2,
            isHover ? WHITE : BLACK
        );

        // click chuột
        if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            PlaySelectSfx();
            _BOT_DIFFICULTY = diffFocus;
            _GAME_MODE = 2;
            _GAME_STATE = 1;
            ResetData();
            _p1Moves = 0;
            _p2Moves = 0;
        }
    }

    // =========================
    // 5. ENTER xác nhận
    // =========================
    if (IsKeyPressed(KEY_ENTER)) {
        PlaySelectSfx();
        _BOT_DIFFICULTY = diffFocus;
        _GAME_MODE = 2;
        _GAME_STATE = 1;
        ResetData();
        _p1Moves = 0;
        _p2Moves = 0;
    }
    
    // 3. Nhấn ESC để quay lại Menu chính
    if (IsKeyPressed(KEY_ESCAPE)) {
        PlaySelectSfx();
        _GAME_STATE = 0;
    }

    EndDrawing();
}

// ==========================================================
// MÀN HÌNH CÀI ĐẶT ÂM THANH
// ==========================================================
void DrawAndHandleSettings(Texture2D background, Font gameFont) {
    static int setFocus = 0; // 0: Nhạc nền, 1: Âm thanh, 2: Âm lượng, 3: Quay lại
    const int NUM_SETTINGS = 4;

    // --- LOGIC ĐIỀU HƯỚNG LÊN XUỐNG ---
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) setFocus = (setFocus - 1 + NUM_SETTINGS) % NUM_SETTINGS;
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) setFocus = (setFocus + 1) % NUM_SETTINGS;

    // --- LOGIC TƯƠNG TÁC (TRÁI/PHẢI/ENTER) ---
    if (setFocus == 0 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        _BGM_ON = !_BGM_ON; // Bật/tắt Nhạc nền
    }
    if (setFocus == 1 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        _SFX_ON = !_SFX_ON; // Bật/tắt Hiệu ứng âm thanh
    }
    if (setFocus == 2) {
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            _MASTER_VOL -= 0.1f; // Giảm 10%
            if (_MASTER_VOL < 0.0f) _MASTER_VOL = 0.0f;
            SetMasterVolume(_MASTER_VOL); // Gọi hàm hệ thống của Raylib
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            _MASTER_VOL += 0.1f; // Tăng 10%
            if (_MASTER_VOL > 1.0f) _MASTER_VOL = 1.0f;
            SetMasterVolume(_MASTER_VOL);
        }
    }
    if ((setFocus == 3 && IsKeyPressed(KEY_ENTER)) || IsKeyPressed(KEY_ESCAPE)) {
        _GAME_STATE = 0; // Về Menu
    }

    // --- VẼ GIAO DIỆN ---
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexturePro(background, Rectangle{ 0, 0, (float)background.width, (float)background.height }, Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f)); // Nền đen mờ 70%

    DrawTextEx(gameFont, "CAI DAT AM THANH", Vector2{ (float)GetScreenWidth() / 2 - 320, 150 }, 70, 2, WHITE);
    DrawText("Dung A/D hoac Mui ten Trai/Phai de dieu chinh", GetScreenWidth() / 2 - 250, 250, 20, GREEN);

    int startY = 320;
    int gap = 110;

    for (int i = 0; i < NUM_SETTINGS; i++) {
        Rectangle btnRec = { (float)GetScreenWidth() / 2 - 300, (float)(startY + i * gap), 600, 80 };
        bool isHover = (setFocus == i);

        // Bắt sự kiện chuột
        if (CheckCollisionPointRec(GetMousePosition(), btnRec)) {
            setFocus = i;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (i == 0) _BGM_ON = !_BGM_ON;
                if (i == 1) _SFX_ON = !_SFX_ON;
                if (i == 3) _GAME_STATE = 0;
            }
        }

        DrawRectangleRec(btnRec, isHover ? DARKGRAY : LIGHTGRAY);
        DrawRectangleLinesEx(btnRec, isHover ? 6.0f : 4.0f, BLACK);

        // Vẽ Text hiển thị trạng thái
        if (i == 0) {
            DrawTextEx(gameFont, "NHAC NEN:", Vector2{ btnRec.x + 30, btnRec.y + 20 }, 40, 2, isHover ? WHITE : BLACK);
            DrawTextEx(gameFont, _BGM_ON ? "[ BAT ]" : "[ TAT ]", Vector2{ btnRec.x + 400, btnRec.y + 20 }, 40, 2, _BGM_ON ? GREEN : RED);
        }
        else if (i == 1) {
            DrawTextEx(gameFont, "HIEU UNG:", Vector2{ btnRec.x + 30, btnRec.y + 20 }, 40, 2, isHover ? WHITE : BLACK);
            DrawTextEx(gameFont, _SFX_ON ? "[ BAT ]" : "[ TAT ]", Vector2{ btnRec.x + 400, btnRec.y + 20 }, 40, 2, _SFX_ON ? GREEN : RED);
        }
        else if (i == 2) {
            DrawTextEx(gameFont, "AM LUONG:", Vector2{ btnRec.x + 30, btnRec.y + 20 }, 40, 2, isHover ? WHITE : BLACK);

            // Tính phần trăm âm lượng
            int volPercent = (int)(_MASTER_VOL * 100 + 0.5f);
            DrawTextEx(gameFont, TextFormat("< %d%% >", volPercent), Vector2{ btnRec.x + 380, btnRec.y + 20 }, 40, 2, isHover ? YELLOW : BLACK);
        }
        else if (i == 3) {
            DrawTextEx(gameFont, "QUAY LAI MENU", Vector2{ btnRec.x + 160, btnRec.y + 20 }, 40, 2, isHover ? WHITE : BLACK);
        }
    }

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