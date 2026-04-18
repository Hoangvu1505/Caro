#include "Bot.h"
#include <vector>
#include <algorithm>
#include <limits>

// --- CÁC HÀM TIỆN ÍCH CỦA BOT ---

// Kiểm tra xem xung quanh ô (r, c) bán kính 1 ô có quân cờ nào không
bool Bot_HasNeighbor(int r, int c) {
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr, nc = c + dc;
            if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                if (_A[nr][nc].c != 0) return true;
            }
        }
    }
    return false;
}

// Lấy danh sách các ô trống có quân cờ nằm kề cạnh
std::vector<std::pair<int, int>> Bot_GetCandidateMoves() {
    std::vector<std::pair<int, int>> candidates;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c == 0 && Bot_HasNeighbor(i, j)) {
                candidates.push_back({ i, j });
            }
        }
    }
    return candidates;
}

// Đánh giá điểm heuristic của một hướng
int Bot_CheckSequences(int r, int c, int symbol) {
    int total = 0;
    int directions[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    for (int d = 0; d < 4; d++) {
        int dr = directions[d][0];
        int dc = directions[d][1];
        int consecutive = 1;
        int openEnds = 0; 

        // Kiểm tra hướng dương
        for (int i = 1; i < 5; i++) {
            int nr = r + dr * i, nc = c + dc * i;
            if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                if (_A[nr][nc].c == symbol) consecutive++;
                else if (_A[nr][nc].c == 0) { openEnds++; break; } // Tính đầu hở
                else break; 
            }
            else break; 
        }

        // Kiểm tra hướng âm
        for (int i = 1; i < 5; i++) {
            int nr = r - dr * i, nc = c - dc * i;
            if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                if (_A[nr][nc].c == symbol) consecutive++;
                else if (_A[nr][nc].c == 0) { openEnds++; break; } // Tính đầu hở
                else break;
            }
            else break;
        }

        total += GetPatternScore(consecutive, openEnds, (symbol == 1));
    }
    return total;
}

int Bot_CheckSequences_Hard(int r, int c, int symbol) {
    int total = 0;
    int directions[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    for (int d = 0; d < 4; d++) {
        int dr = directions[d][0], dc = directions[d][1];
        int consecutive = 1;
        int openEnds = 0;

        // Kiểm tra hướng dương
        for (int i = 1; i < 5; i++) {
            int nr = r + dr * i, nc = c + dc * i;
            if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                if (_A[nr][nc].c == symbol) consecutive++;
                else if (_A[nr][nc].c == 0) { openEnds++; break; }
                else break;
            } else { break; }
        }
        // Kiểm tra hướng âm
        for (int i = 1; i < 5; i++) {
            int nr = r - dr * i, nc = c - dc * i;
            if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                if (_A[nr][nc].c == symbol) consecutive++;
                else if (_A[nr][nc].c == 0) { openEnds++; break; }
                else break;
            } else { break; }
        }

        total += GetPatternScore(consecutive, openEnds, (symbol == 1));
    }
    return total;
}

// Sắp xếp các nước đi để cắt nhánh Alpha-Beta hiệu quả
struct MoveScore {
    int r, c, score;
};

bool CompareMoves(const MoveScore& a, const MoveScore& b) {
    return a.score > b.score;
}

std::vector<std::pair<int, int>> Bot_RankMoves(std::vector<std::pair<int, int>>& moves, int symbol) {
    std::vector<MoveScore> scoredMoves;
    int oppSymbol = (symbol == 1) ? -1 : 1;

    for (auto& move : moves) {
        int r = move.first;
        int c = move.second;
        // Điểm = Tấn công + Phòng thủ (Phòng thủ x 1.2)
        int attackScore = Bot_CheckSequences(r, c, symbol);
        int defenseScore = Bot_CheckSequences(r, c, oppSymbol);
        int totalScore = attackScore + (int)(defenseScore * 1.2f);
        scoredMoves.push_back({ r, c, totalScore });
    }

    std::sort(scoredMoves.begin(), scoredMoves.end(), CompareMoves);

    // Chỉ lấy Top 15 nước đi ngon nhất để chạy Minimax cho nhanh
    std::vector<std::pair<int, int>> topMoves;
    int limit = (scoredMoves.size() < 15) ? scoredMoves.size() : 15;
    for (int i = 0; i < limit; i++) {
        topMoves.push_back({ scoredMoves[i].r, scoredMoves[i].c });
    }
    return topMoves;
}

// Kiểm tra có nước nào thắng luôn không để cắt nhánh nhanh
bool Bot_CheckWinSimulation(int r, int c, int symbol) {
    return Bot_CheckSequences(r, c, symbol) >= 10000000;
}

// Tính tổng điểm bàn cờ tĩnh
int Bot_EvaluateBoard() {
    int scoreBot = 0;
    int scorePlayer = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c == 1) scoreBot += Bot_CheckSequences(i, j, 1);
            else if (_A[i][j].c == -1) scorePlayer += Bot_CheckSequences(i, j, -1);
        }
    }
    return scoreBot - scorePlayer;
}

// Tính tổng điểm bàn cờ tĩnh hard
int Bot_EvaluateBoard_Hard() {
    int scoreBot = 0;
    int scorePlayer = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c == 1) scoreBot += Bot_CheckSequences_Hard(i, j, 1);
            else if (_A[i][j].c == -1) scorePlayer += Bot_CheckSequences_Hard(i, j, -1);
        }
    }
    // Trả về hiệu số điểm (ưu tiên Bot thắng)
    return scoreBot - (int)(scorePlayer * 1.1f); 
}

// Thuật toán Minimax cốt lõi
int Bot_Minimax(int depth, bool isBotTurn, int alpha, int beta) {
    if (depth == 0) {
        if (_BOT_DIFFICULTY == 2) return Bot_EvaluateBoard_Hard();
        return Bot_EvaluateBoard();
    }

    std::vector<std::pair<int, int>> moves = Bot_GetCandidateMoves();
    if (moves.empty()) return Bot_EvaluateBoard();

    int currentSymbol = isBotTurn ? 1 : -1;
    moves = Bot_RankMoves(moves, currentSymbol);

    if (isBotTurn) {
        int maxEval = -20000000; // Thay cho -inf
        for (auto& move : moves) {
            int r = move.first, c = move.second;
            _A[r][c].c = 1;

            if (Bot_CheckWinSimulation(r, c, 1)) {
                _A[r][c].c = 0;
                return 10000000;
            }

            int eval = Bot_Minimax(depth - 1, false, alpha, beta);
            _A[r][c].c = 0;

            if (eval > maxEval) maxEval = eval;
            if (eval > alpha) alpha = eval;
            if (beta <= alpha) break;
        }
        return maxEval;
    }
    else {
        int minEval = 20000000; // Thay cho inf
        for (auto& move : moves) {
            int r = move.first, c = move.second;
            _A[r][c].c = -1;

            if (Bot_CheckWinSimulation(r, c, -1)) {
                _A[r][c].c = 0;
                return -10000000;
            }

            int eval = Bot_Minimax(depth - 1, true, alpha, beta);
            _A[r][c].c = 0;

            if (eval < minEval) minEval = eval;
            if (eval < beta) beta = eval;
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

// BƯỚC THỦ KHẨN CẤP
bool Bot_CheckUrgentDefense(int& out_r, int& out_c) {
    std::vector<std::pair<int, int>> candidates = Bot_GetCandidateMoves();

    // Ưu tiên 1: Bot có thể thắng luôn
    for (auto& move : candidates) {
        if (Bot_CheckSequences(move.first, move.second, 1) >= 50000) {
            out_r = move.first; out_c = move.second; return true;
        }
    }
    // Ưu tiên 2: Chặn Người thắng luôn (Người có 3 thoáng hoặc 4 chặn)
    for (auto& move : candidates) {
        if (Bot_CheckSequences(move.first, move.second, -1) >= 2000) {
            out_r = move.first; out_c = move.second; return true;
        }
    }
    return false;
}

// Logic bổ trợ cho Hard mode
long GetPatternScore(int continuous, int openEnds, bool isBot) {
    if (continuous >= 5) return 100000000; // Thắng tuyệt đối

    switch (continuous) {
        case 4:
            if (openEnds == 2) return 5000000; // 4 quân hở 2 đầu -> Chết chắc
            if (openEnds == 1) return 1000000; // 4 quân bị chặn 1 đầu
            break;
        case 3:
            if (openEnds == 2) return 500000;  // 3 quân hở 2 đầu (nước đôi)
            if (openEnds == 1) return 50000;   // 3 quân bị chặn 1 đầu
            break;
        case 2:
            if (openEnds == 2) return 5000;
            if (openEnds == 1) return 500;
            break;
    }
    return 0;
}

// Hàm tính điểm nhanh tại 1 ô
int EvaluatePosition(int r, int c) {
    return Bot_CheckSequences_Hard(r, c, 1) + Bot_CheckSequences_Hard(r, c, -1);
}

// Hàm bot độ khó thâp
void BotMove_Easy(std::vector<std::pair<int, int>>& moves) {
    int idx = rand() % moves.size();
    _X = _A[moves[idx].first][moves[idx].second].x;
    _Y = _A[moves[idx].first][moves[idx].second].y;
}

void BotMove_Normal() {
    int best_r = 7, best_c = 7; // Khởi tạo mặc định ra giữa bàn
    std::vector<std::pair<int, int>> possible_moves = Bot_GetCandidateMoves();

    // Nếu bàn cờ trống trơn, tự động đánh giữa bàn cờ
    if (possible_moves.empty()) {
        best_r = BOARD_SIZE / 2;
        best_c = BOARD_SIZE / 2;
        _X = _A[best_r][best_c].x;
        _Y = _A[best_r][best_c].y;
        return;
    }

    // 1. Kiểm tra đánh chặn khẩn cấp
    if (Bot_CheckUrgentDefense(best_r, best_c)) {
        _X = _A[best_r][best_c].x;
        _Y = _A[best_r][best_c].y;
        return;
    }

    // 2. Chạy Minimax (Độ sâu = 2, vì sâu hơn game sẽ lag)
    int search_depth = 2;
    int best_score = -20000000;
    int alpha = -20000000;
    int beta = 20000000;

    std::vector<std::pair<int, int>> ranked_moves = Bot_RankMoves(possible_moves, 1);

    for (auto& move : ranked_moves) {
        int r = move.first, c = move.second;
        _A[r][c].c = 1; // Bot đánh thử

        int score = Bot_Minimax(search_depth - 1, false, alpha, beta);

        _A[r][c].c = 0; // Trả lại bàn cờ cũ

        if (score > best_score) {
            best_score = score;
            best_r = r; best_c = c;
        }
        if (score > alpha) alpha = score;
        if (beta <= alpha) break;
    }

    _X = _A[best_r][best_c].x;
    _Y = _A[best_r][best_c].y;
}

// Hàm Bot độ khó cao
void BotMove_Hard() {
    int best_r = -1, best_c = -1;
    int alpha = -2000000000;
    int beta = 2000000000;
    int depth = 3; // Nâng lên depth 3 nhờ tối ưu move ordering

    // 1. Lấy ứng viên và SẮP XẾP chúng theo độ ưu tiên (Move Ordering)
    std::vector<std::pair<int, int>> candidates = Bot_GetCandidateMoves();
    
    // Sắp xếp: Ưu tiên những ô gần các quân cờ đã đánh để giảm không gian tìm kiếm
    std::sort(candidates.begin(), candidates.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return EvaluatePosition(a.first, a.second) > EvaluatePosition(b.first, b.second);
    });

    int best_score = -2000000000;

    for (auto& move : candidates) {
        int r = move.first, c = move.second;
        _A[r][c].c = 1; // Giả sử Bot đánh (Bot ID = 1)

        // Gọi Minimax
        int score = Bot_Minimax(depth - 1, false, alpha, beta);

        _A[r][c].c = 0; // Hoàn tác

        if (score > best_score) {
            best_score = score;
            best_r = r;
            best_c = c;
        }
        alpha = std::max(alpha, best_score);
    }

    if (best_r != -1) {
        _X = _A[best_r][best_c].x;
        _Y = _A[best_r][best_c].y;
    }
}

// Hàm điều phối logic bot chính
void BotMove() {
    std::vector<std::pair<int, int>> possible_moves = Bot_GetCandidateMoves();
    if (possible_moves.empty()) {
        _X = _A[BOARD_SIZE/2][BOARD_SIZE/2].x;
        _Y = _A[BOARD_SIZE/2][BOARD_SIZE/2].y;
        return;
    }

    switch (_BOT_DIFFICULTY) {
        case 0: BotMove_Easy(possible_moves); break;
        case 1: BotMove_Normal(); break;
        case 2: BotMove_Hard(); break;
        default: BotMove_Normal(); break;
    }
}