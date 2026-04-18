#ifndef VIEW_H
#define VIEW_H
void DrawBoardRaylib();

// Hàm quản lý Màn hình Menu
void DrawAndHandleMenu(Texture2D background, Font gameFont);

void HandleGameInput();
void DrawGameUI(Texture2D background, Font gameFont);
void DrawAndHandleGameOver(Font gameFont);
void DrawAndHandlePauseMenu(Font gameFont);
void DrawAndHandleDifficultyMenu(Texture2D background, Font gameFont);
void DrawAndHandleInfo(Texture2D background, Font gameFont);
void DrawAndHandleInstructions(Texture2D background, Texture2D huongdanImg, Font gameFont);

void DrawAndHandleGame(Texture2D background, Font gameFont);
void DrawAndHandleSettings(Texture2D background, Font gameFont);
#endif
