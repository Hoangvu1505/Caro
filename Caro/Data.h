#ifndef DATA_H
#define DATA_H

#include "raylib.h"

// ==========================================
// 1. CONSTANTS (HẰNG SỐ CƠ BẢN)
// ==========================================
// Cấu hình kích thước và tọa độ bàn cờ
#define BOARD_SIZE 20
#define CELL_SIZE 30    // Kích thước của mỗi ô vuông trên bàn cờ
#define LEFT 428        // Tọa độ X bắt đầu vẽ bàn cờ (căn chỉnh cho đồ họa)
#define TOP 118         // Tọa độ Y bắt đầu vẽ bàn cờ

// ==========================================
// 2. DATA STRUCTURES (CẤU TRÚC DỮ LIỆU)
// ==========================================

// Trạng thái của một điểm/ô trên bàn cờ
struct _POINT {
    int x; // Tọa độ X trên màn hình
    int y; // Tọa độ Y trên màn hình
    int c; // Trạng thái ô (VD: -1, 0, 1 hoặc 0: trống, 1: X, 2: O)
};

// Lưu trữ thông tin đường thắng để vẽ highlight
struct WinLine {
    int r1, c1; // Tọa độ hàng/cột của quân cờ đầu tiên trong chuỗi thắng
    int r2, c2; // Tọa độ hàng/cột của quân cờ thứ 5 trong chuỗi thắng
};

// ==========================================
// 3. GLOBAL VARIABLES (BIẾN TOÀN CỤC)
// Sử dụng extern để dùng chung giữa các file mà không bị lỗi duplicate
// ==========================================

// --- Trạng thái hệ thống & Chế độ chơi ---
extern int _GAME_STATE;     // 0: Đang ở Menu, 1: Đang chơi game
extern int _GAME_MODE;      // 1: Người vs Người (PvP), 2: Người vs Máy (PvE)
extern bool _IS_PAUSED;     // Trạng thái tạm dừng trò chơi
extern int _COMMAND;        // Lệnh/thao tác hiện tại từ người dùng (bàn phím/chuột)
extern int _BOT_DIFFICULTY; // Mức độ Bot (Dễ - Trunh bình - Khó)

// --- Dữ liệu bàn cờ & Tọa độ ---
extern _POINT _A[BOARD_SIZE][BOARD_SIZE]; // Ma trận lưu trữ dữ liệu bàn cờ 2D
extern int _X, _Y;                        // Tọa độ hiện tại (hoặc tọa độ con trỏ đang trỏ tới)

// --- Trạng thái trận đấu & Người chơi ---
extern bool _TURN;          // Lượt chơi hiện tại (VD: true = P1, false = P2)
extern int _p1Moves;        // Số nước đã đánh của Người chơi 1
extern int _p2Moves;        // Số nước đã đánh của Người chơi 2
extern int _WINNER;         // Người chiến thắng (VD: 0: Chưa có/Hòa, 1: P1, 2: P2)
extern WinLine _winLine;    // Tọa độ đường thẳng chiến thắng (Để View.cpp biết vẽ đường highlight)

#endif // DATA_H