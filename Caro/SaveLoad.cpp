#include "SaveLoad.h"
#include "Data.h"    // Để lấy các biến toàn cục (_A, _X, _Y, _GAME_STATE...)
#include <fstream>
#include <filesystem>

// ==========================================
// 1. GHI TOÀN BỘ TRẠNG THÁI GAME RA FILE
// ==========================================
void SaveGame(std::string filename) {
  // Tự động thêm tiền tố saves/ nếu chưa có
  std::string path = filename;
  if (filename.find("saves/") != 0) path = "saves/" + filename;

  std::ofstream file(path);
  if (file.is_open()) {
    file << _GAME_MODE << " " << _TURN << " " << _p1Moves << " " << _p2Moves << " " << _WINNER << " " << _ROUND << " " << _turnTimeLimit << "\n";
    file << _p1WinsPvP << " " << _p2WinsPvP << " " << _p1WinsPvE << " " << _botWins << "\n";
    file << _P1_POKEMON << " " << _P2_POKEMON << "\n";
    file << _P1_NAME << "\n";
    file << _P2_NAME << "\n";
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
  std::string path = filename;
  if (filename.find("saves/") != 0) path = "saves/" + filename;

  std::ifstream file(path);
  if (file.is_open()) {
    file >> _GAME_MODE >> _TURN >> _p1Moves >> _p2Moves >> _WINNER >> _ROUND >> _turnTimeLimit;
    _turnTimer = (float)_turnTimeLimit; // Reset timer khi load game
    file >> _p1WinsPvP >> _p2WinsPvP >> _p1WinsPvE >> _botWins;
    file >> _P1_POKEMON >> _P2_POKEMON;
    file.ignore(); // Bỏ qua ký tự xuống dòng sau số
    file.getline(_P1_NAME, 50);
    file.getline(_P2_NAME, 50);
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
  std::string path = filename;
  if (filename.find("saves/") != 0) path = "saves/" + filename;

  try {
    if (std::filesystem::exists(path)) {
      return std::filesystem::remove(path); // Hàm xóa file cứng trong ổ đĩa
    }
  }
  catch (const std::filesystem::filesystem_error& e) {
    // Bỏ qua nếu có lỗi hệ thống (file đang mở, v.v.)
  }
  return false;
}

// ==========================================
// 4. LẤY CHẾ ĐỘ CHƠI TỪ FILE
// ==========================================
int GetGameModeFromFile(std::string filename) {
  std::string path = filename;
  if (filename.find("saves/") != 0) path = "saves/" + filename;

  std::ifstream file(path);
  int mode = 1;
  if (file.is_open()) {
    file >> mode; // Đọc con số đầu tiên (chính là _GAME_MODE)
    file.close();
  }
  return mode;
}