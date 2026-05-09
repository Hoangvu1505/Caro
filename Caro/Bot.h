#ifndef BOT_H
#define BOT_H

#include "Data.h"
#include <utility>
#include <vector>

// Các hàm tính toán tiện ích
bool Bot_HasNeighbor(int r, int c);
std::vector<std::pair<int, int>> Bot_GetCandidateMoves();

std::vector<std::pair<int, int>> Bot_RankMoves(std::vector<std::pair<int, int>>& moves, int symbol);

bool Bot_CheckWinSimulation(int r, int c, int symbol);

int Bot_EvaluateBoard();
int Bot_EvaluateBoard_Hard();

int Bot_Minimax(int depth, bool isBotTurn, int alpha, int beta);
long ScoreWindow(int cntBot, int cntPlayer);
long EvaluatePositionImproved(int r, int c);

// Phòng thủ khẩn cấp
bool Bot_CheckUrgentDefense(int& out_r, int& out_c);

// Phỏng đoán nước đi
long GetPatternScore(int continuous, int openEnds, bool isBot);
int EvaluatePosition(int r, int c);

// Bot move
void BotMove_Easy(std::vector<std::pair<int, int>>& moves);
void BotMove_Normal();
void BotMove_Hard();
void BotMove();

#endif