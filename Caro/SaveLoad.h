#ifndef SAVELOAD_H
#define SAVELOAD_H

#include "raylib.h"
#include <string>

// Khai báo các hàm xử lý file
void SaveGame(std::string filename);
bool LoadGame(std::string filename);
bool DeleteGame(std::string filename);
int GetGameModeFromFile(std::string filename);

#endif
