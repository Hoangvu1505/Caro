#ifndef SAVELOAD_H
#define SAVELOAD_H

#include <string>
#include "raylib.h"

// Khai báo các hàm xử lý file
void SaveGame(std::string filename);
bool LoadGame(std::string filename);
bool DeleteGame(std::string filename); 

// Khai báo hàm vẽ giao diện màn hình Tải game
void DrawAndHandleLoad(Texture2D background, Font gameFont);
int GetGameModeFromFile(std::string filename);

#endif
