#ifndef VIEW_H
#define VIEW_H

#include "raylib.h"

void DrawBoardRaylib(Font gameFont);

// Hàm quản lý Màn hình Menu
void DrawAndHandleMenu(Texture2D background, Font gameFont);

void HandleGameInput();
void DrawGameUI(Texture2D background, Font gameFont, Font pieceFont);
void DrawAndHandleGameOver(Font gameFont);
void DrawAndHandlePauseMenu(Font gameFont);
void DrawAndHandleDifficultyMenu(Texture2D background, Font gameFont);
void DrawAndHandleInfo(Texture2D background, Font gameFont);
void DrawAndHandleInstructions(Texture2D background, Texture2D huongdanImg, Font gameFont);

void DrawAndHandleGame(Texture2D background, Font gameFont, Font pieceFont);
void DrawAndHandleSettings(Texture2D background, Font gameFont);
void DrawAndHandleLoad(Texture2D background, Font gameFont);
void DrawAndHandleSetup(Texture2D background, Font gameFont);
#endif
