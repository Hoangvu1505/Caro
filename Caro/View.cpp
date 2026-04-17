#include "Data.h"
#include "View.h"
#include "Bot.h"
#include "Logic.h"
#include "SaveLoad.h"
#include "raylib.h" 

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
    const int NUM_BUTTONS = 5;

    // Mảng chứa tên các nút (Dễ dàng thêm/bớt sau này)
    const char* btnLabels[NUM_BUTTONS] = {
        "1. DANH 2 NGUOI",
        "2. DANH VOI MAY",
        "3. FILE GAME",
        "4. THONG TIN",
        "5. HUONG DAN"
    };

    // --- LOGIC NHẬN BÀN PHÍM ---
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) menuFocus = (menuFocus - 1 + NUM_BUTTONS) % NUM_BUTTONS;
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) menuFocus = (menuFocus + 1) % NUM_BUTTONS;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Vẽ nền và tiêu đề
    DrawTexturePro(background, Rectangle{ 0, 0, (float)background.width, (float)background.height }, Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
    DrawTextEx(gameFont, "GAME CO CARO", Vector2{ (float)GetScreenWidth() / 2 - 300, 100 }, 100, 2, WHITE);

    // Thông số kích thước chung cho tất cả các nút
    int btnWidth = 500;
    int btnHeight = 80;
    int startX = GetScreenWidth() / 2 - btnWidth / 2;
    int startY = 280;
    int gap = 110; // Khoảng cách giữa các nút

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

    // Nhận thêm phím ENTER
    if (IsKeyPressed(KEY_ENTER)) {
        isClicked = true;
        clickedIndex = menuFocus;
    }

    // --- XỬ LÝ CHUYỂN CẢNH KHI ĐÃ CLICK ---
    if (isClicked) {
        if (clickedIndex == 0) { // PvP
            _GAME_MODE = 1; _GAME_STATE = 1; _IS_PAUSED = false;
            ResetData(); _p1Moves = 0; _p2Moves = 0; _WINNER = 2;
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
        else if (clickedIndex == 2) { // File Game
            _GAME_STATE = 2;
        }
        else if (clickedIndex == 3) { // Thông tin
            _GAME_STATE = 3;
        }
        else if (clickedIndex == 4) { // Hướng dẫn
            _GAME_STATE = 4;
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

    // Mảng tên 4 nút
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

    // --- LOGIC NHẬN BÀN PHÍM ---
    if (!isTypingSave) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) pauseFocus = (pauseFocus - 1 + NUM_BUTTONS) % NUM_BUTTONS;
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) pauseFocus = (pauseFocus + 1) % NUM_BUTTONS;
    }
    else {
        // --- LOGIC GÕ TÊN FILE VÀ LƯU GAME ---
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
            SaveGame(fileName); // GỌI HÀM LƯU TỪ TỆP SaveLoad.h
            isTypingSave = false;
            _IS_PAUSED = false; // Lưu xong thì đóng menu pause
        }
        if (IsKeyPressed(KEY_ESCAPE)) isTypingSave = false; // Hủy gõ
    }

    // --- VẼ GIAO DIỆN ---
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    // Nếu đang gõ tên thì bảng ngắn lại, nếu đang hiện 4 nút thì bảng dài ra
    int boxWidth = 500;
    int boxHeight = isTypingSave ? 300 : 550;
    int boxX = (GetScreenWidth() - boxWidth) / 2;
    int boxY = (GetScreenHeight() - boxHeight) / 2;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(RAYWHITE, 0.95f));
    DrawRectangleLinesEx(Rectangle{ (float)boxX, (float)boxY, (float)boxWidth, (float)boxHeight }, 5, DARKGRAY);

    if (isTypingSave) {
        // GIAO DIỆN KHUNG NHẬP CHỮ
        DrawTextEx(gameFont, "NHAP TEN FILE:", Vector2{ (float)boxX + 50, (float)boxY + 50 }, 50, 2, DARKGRAY);
        DrawRectangle(boxX + 40, boxY + 130, 420, 60, LIGHTGRAY);
        DrawTextEx(gameFont, saveName, Vector2{ (float)boxX + 50, (float)boxY + 140 }, 40, 2, RED);
        if (((int)(GetTime() * 2)) % 2 == 0) DrawText("_", boxX + 50 + MeasureText(saveName, 40), boxY + 140, 40, RED);
        DrawText("ENTER: Luu file | ESC: Huy bo", boxX + 90, boxY + 220, 20, DARKGRAY);
    }
    else {
        // GIAO DIỆN 4 NÚT
        DrawTextEx(gameFont, "TAM DUNG", Vector2{ (float)boxX + 110, (float)boxY + 40 }, 65, 2, DARKGRAY);

        int btnWidth = 360;
        int btnHeight = 70;
        int startX = boxX + (boxWidth - btnWidth) / 2;
        int startY = boxY + 140;
        int gap = 90;

        bool isClicked = false;
        int clickedIndex = -1;

        // Vòng lặp vẽ 4 nút
        for (int i = 0; i < NUM_BUTTONS; i++) {
            Rectangle btnRec = { (float)startX, (float)(startY + i * gap), (float)btnWidth, (float)btnHeight };

            if (CheckCollisionPointRec(GetMousePosition(), btnRec)) {
                pauseFocus = i;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    isClicked = true;
                    clickedIndex = i;
                }
            }

            bool isFocused = (pauseFocus == i);

            DrawRectangleRec(btnRec, isFocused ? DARKGRAY : LIGHTGRAY);
            DrawRectangleLinesEx(btnRec, isFocused ? 5.0f : 3.0f, BLACK);

            // Tự động đo kích thước chữ và căn giữa hoàn hảo
            Vector2 textSize = MeasureTextEx(gameFont, btnLabels[i], 45, 2);
            DrawTextEx(gameFont, btnLabels[i], Vector2{ btnRec.x + (btnWidth - textSize.x) / 2, btnRec.y + (btnHeight - textSize.y) / 2 }, 45, 2, isFocused ? WHITE : BLACK);
        }

        if (IsKeyPressed(KEY_ENTER)) {
            isClicked = true;
            clickedIndex = pauseFocus;
        }

        // --- XỬ LÝ KHI CHỌN NÚT ---
        if (isClicked) {
            if (clickedIndex == 0) {
                // 1. Tiếp tục
                _IS_PAUSED = false;
            }
            else if (clickedIndex == 1) {
                // 2. Cài đặt (Tính năng sẽ phát triển sau)
                // _GAME_STATE = 4; // thêm màn hình Cài Đặt thì bỏ comment dòng này
            }
            else if (clickedIndex == 2) {
                // 3. Lưu game
                isTypingSave = true;
                saveName[0] = '\0';
                nameCount = 0;
            }
            else if (clickedIndex == 3) {
                // 4. Về Menu
                _GAME_STATE = 0;
                _IS_PAUSED = false;
            }
        }
    }
}
void DrawAndHandleInstructions(Texture2D background, Texture2D huongdanImg, Font gameFont) {
    // Bấm ESC để quay về Menu chính
    if (IsKeyPressed(KEY_ESCAPE)) {
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
    int imgY = (GetScreenHeight() - huongdanImg.height) / 2;
    DrawTexture(huongdanImg, imgX, imgY, WHITE);

    // 4. Dòng chữ nhắc nhở bấm ESC (vẽ đè lên trên cùng)
    DrawTextEx(gameFont, "Nhan ESC de quay lai Menu", Vector2{ (float)GetScreenWidth() / 2 - 200, (float)GetScreenHeight() - 40 }, 30, 2, RED);

    EndDrawing();
}
// MÀN HÌNH THÔNG TIN NHÓM
void DrawAndHandleInfo(Texture2D background, Font gameFont) {
    // Bấm ESC để quay về Menu chính
    if (IsKeyPressed(KEY_ESCAPE)) {
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

    DrawTextEx(gameFont, "Nhom thuc hien: nhom 10", Vector2{ (float)startX, (float)startY + 210 }, 40, 2, YELLOW);

    // ĐIỀN TÊN VÀ MSSV CỦA BẠN & ĐỒNG ĐỘI VÀO ĐÂY
    DrawTextEx(gameFont, "1. Luong Nguyen Hoang Vu - 24120493", Vector2{ (float)startX + 50, (float)startY + 270 }, 35, 2, WHITE);
    DrawTextEx(gameFont, "2. Dinh Duc Hieu - 24120002", Vector2{ (float)startX + 50, (float)startY + 320 }, 35, 2, WHITE);
    DrawTextEx(gameFont, "3. Dao Thanh Phong - 24120006", Vector2{ (float)startX + 50, (float)startY + 370 }, 35, 2, WHITE);
	DrawTextEx(gameFont, "4. Le Hung Thang - 24120137", Vector2{ (float)startX + 50, (float)startY + 420 }, 35, 2, WHITE);
	DrawTextEx(gameFont, "5. Nguyen Anh Duc - 24120289", Vector2{ (float)startX + 50, (float)startY + 470 }, 35, 2, WHITE);

    DrawTextEx(gameFont, "Cong nghe: C++ & Thu vien Raylib", Vector2{ (float)startX, (float)startY + 540 }, 35, 2, LIGHTGRAY);

    // Hướng dẫn thoát
    DrawText("Nhan ESC de quay lai Menu", GetScreenWidth() / 2 - 150, GetScreenHeight() - 80, 20, RED);

    EndDrawing();
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