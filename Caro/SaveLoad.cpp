#include "SaveLoad.h"
#include "Data.h"    // Để lấy các biến toàn cục (_A, _X, _Y, _GAME_STATE...)
#include <fstream>
#include <filesystem>
#include <vector>

// ==========================================
// 1. GHI TOÀN BỘ TRẠNG THÁI GAME RA FILE
// ==========================================
void SaveGame(std::string filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << _GAME_MODE << " " << _TURN << " " << _p1Moves << " " << _p2Moves << " " << _WINNER << "\n";
        file << _X << " " << _Y << "\n";
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                file << _A[i][j].c << " ";
            }
            file << "\n";
        }
        file.close();
    }
}

// ==========================================
// 2. ĐỌC FILE VÀ NẠP LẠI VÀO GAME
// ==========================================
bool LoadGame(std::string filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        file >> _GAME_MODE >> _TURN >> _p1Moves >> _p2Moves >> _WINNER;
        file >> _X >> _Y;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                file >> _A[i][j].c;
            }
        }
        file.close();
        return true;
    }
    return false;
}

// ==========================================
// 3. XÓA FILE LƯU (NEW)
// ==========================================
bool DeleteGame(std::string filename) {
    try {
        if (std::filesystem::exists(filename)) {
            return std::filesystem::remove(filename); // Hàm xóa file cứng trong ổ đĩa
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        // Bỏ qua nếu có lỗi hệ thống (file đang mở, v.v.)
    }
    return false;
}

// ==========================================
// 4. GIAO DIỆN QUẢN LÝ FILE (LOAD / DELETE)
// ==========================================

int GetGameModeFromFile(std::string filename) {
    std::ifstream file(filename);
    int mode = 1;
    if (file.is_open()) {
        file >> mode; // Đọc con số đầu tiên (chính là _GAME_MODE)
        file.close();
    }
    return mode;
}

void DrawAndHandleLoad(Texture2D background, Font gameFont) {
    static std::vector<std::string> pvpFiles; // Danh sách 2 người
    static std::vector<std::string> pveFiles; // Danh sách đánh với máy
    static bool isLoaded = false;
    static int focus = 0;
    static int currentTab = 0; // 0: Đánh 2 người, 1: Đánh với máy

    // Phân loại file vào 2 nhóm khi vừa mở màn hình
    if (!isLoaded) {
        pvpFiles.clear();
        pveFiles.clear();
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.path().extension() == ".sav") {
                std::string fname = entry.path().filename().string();
                if (GetGameModeFromFile(fname) == 1) {
                    pvpFiles.push_back(fname);
                }
                else {
                    pveFiles.push_back(fname);
                }
            }
        }
        focus = 0;
        currentTab = 0; // Mặc định hiển thị tab Đánh 2 người trước
        isLoaded = true;
    }

    // Lấy cái danh sách mà người dùng đang xem để thao tác
    std::vector<std::string>& currentList = (currentTab == 0) ? pvpFiles : pveFiles;

    // --- BẮT PHÍM ĐIỀU HƯỚNG TAB (TRÁI/PHẢI) ---
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) { currentTab = 0; focus = 0; }
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) { currentTab = 1; focus = 0; }

    // --- BẮT PHÍM CHỌN FILE (LÊN/XUỐNG) ---
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) { if (focus > 0) focus--; }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) { if (focus < (int)currentList.size() - 1) focus++; }

    // --- THOÁT ---
    if (IsKeyPressed(KEY_ESCAPE)) { _GAME_STATE = 0; isLoaded = false; }

    // --- BẮT PHÍM ENTER (LOAD FILE) ---
    if (IsKeyPressed(KEY_ENTER) && !currentList.empty()) {
        if (LoadGame(currentList[focus])) {
            _GAME_STATE = 1;
            _IS_PAUSED = false;
        }
        isLoaded = false; // Reset danh sách cho lần mở sau
    }

    // --- BẮT PHÍM DELETE (XÓA FILE) ---
    if (IsKeyPressed(KEY_DELETE) && !currentList.empty()) {
        if (DeleteGame(currentList[focus])) {
            currentList.erase(currentList.begin() + focus);
            if (focus >= (int)currentList.size() && focus > 0) focus--;
        }
    }

    // ==========================================
    // VẼ GIAO DIỆN
    // ==========================================
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexturePro(background, Rectangle{ 0, 0, (float)background.width, (float)background.height }, Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    DrawTextEx(gameFont, "QUAN LY FILE LUU", Vector2{ (float)GetScreenWidth() / 2 - 250, 60 }, 70, 2, WHITE);

    // Cập nhật dòng hướng dẫn
    DrawText("A/D (Trai/Phai): Doi che do | W/S: Chon | ENTER: Tai | DELETE: Xoa", GetScreenWidth() / 2 - 320, 150, 20, GREEN);

    // --- VẼ 2 TAB PHÂN LOẠI ---
    Rectangle tabPvP = { (float)GetScreenWidth() / 2 - 320, 200, 300, 60 };
    Rectangle tabPvE = { (float)GetScreenWidth() / 2 + 20, 200, 300, 60 };

    DrawRectangleRec(tabPvP, (currentTab == 0) ? DARKGRAY : LIGHTGRAY); DrawRectangleLinesEx(tabPvP, 3, BLACK);
    DrawTextEx(gameFont, "DANH 2 NGUOI", Vector2{ tabPvP.x + 40, tabPvP.y + 15 }, 35, 2, (currentTab == 0) ? WHITE : BLACK);

    DrawRectangleRec(tabPvE, (currentTab == 1) ? DARKGRAY : LIGHTGRAY); DrawRectangleLinesEx(tabPvE, 3, BLACK);
    DrawTextEx(gameFont, "DANH VOI MAY", Vector2{ tabPvE.x + 40, tabPvE.y + 15 }, 35, 2, (currentTab == 1) ? WHITE : BLACK);

    // --- VẼ DANH SÁCH FILE ---
    if (currentList.empty()) {
        DrawTextEx(gameFont, "CHUA CO FILE NAO!", Vector2{ (float)GetScreenWidth() / 2 - 250, 350 }, 50, 2, RED);
    }
    else {
        int startY = 300;
        for (size_t i = 0; i < currentList.size(); i++) {
            Color textColor = (focus == i) ? RED : WHITE;
            if (focus == i) DrawRectangle(GetScreenWidth() / 2 - 300, startY + i * 60 - 5, 600, 50, Fade(DARKGRAY, 0.8f));
            DrawTextEx(gameFont, currentList[i].c_str(), Vector2{ (float)GetScreenWidth() / 2 - 250, (float)startY + i * 60 }, 40, 2, textColor);
        }
    }
    EndDrawing();
}