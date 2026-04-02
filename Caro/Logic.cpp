#include "Data.h"
#include "Logic.h"

// ==========================================
// THIẾT LẬP DỮ LIỆU BAN ĐẦU
// ==========================================
void ResetData() {
    // Duyệt qua toàn bộ ma trận bàn cờ để khởi tạo tọa độ và trạng thái
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            _A[i][j].x = LEFT + j * CELL_SIZE; // Tính toán tọa độ X trên màn hình
            _A[i][j].y = TOP + i * CELL_SIZE;  // Tính toán tọa độ Y trên màn hình
            _A[i][j].c = 0; // Đặt trạng thái ô là trống (0: chưa đánh, -1: X, 1: O)
        }
    }

    _TURN = true; // Lượt đầu tiên luôn dành cho Người chơi 1 (True = X)

    // Đặt vị trí con trỏ ban đầu ở chính giữa bàn cờ
    int center = BOARD_SIZE / 2;
    _X = _A[center][center].x;
    _Y = _A[center][center].y;
}

// ==========================================
// XỬ LÝ ĐÁNH CỜ
// ==========================================
// Hàm kiểm tra và đánh dấu quân cờ tại tọa độ màn hình (pX, pY)
int CheckBoard(int pX, int pY) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            // Tìm ô vuông trùng với tọa độ click/Enter và ô đó phải đang trống
            if (_A[i][j].x == pX && _A[i][j].y == pY && _A[i][j].c == 0) {
                if (_TURN == true)
                    _A[i][j].c = -1; // Lượt P1 đánh X
                else
                    _A[i][j].c = 1;  // Lượt P2 đánh O

                return _A[i][j].c; // Trả về giá trị quân cờ vừa đánh
            }
        }
    }
    return 0; // Trả về 0 nếu đánh vào ô không hợp lệ hoặc ô đã có cờ
}

// ==========================================
// ĐIỀU KHIỂN CON TRỎ (Bằng bàn phím)
// ==========================================
void MoveRight() {
    // Nếu chưa chạm mép phải của bàn cờ thì dịch tọa độ X sang phải 1 ô
    if (_X < _A[0][BOARD_SIZE - 1].x) _X += CELL_SIZE;
}
void MoveLeft() {
    // Nếu chưa chạm mép trái của bàn cờ thì dịch tọa độ X sang trái 1 ô
    if (_X > _A[0][0].x) _X -= CELL_SIZE;
}
void MoveDown() {
    // Nếu chưa chạm đáy bàn cờ thì dịch tọa độ Y xuống dưới 1 ô
    if (_Y < _A[BOARD_SIZE - 1][0].y) _Y += CELL_SIZE;
}
void MoveUp() {
    // Nếu chưa chạm đỉnh bàn cờ thì dịch tọa độ Y lên trên 1 ô
    if (_Y > _A[0][0].y) _Y -= CELL_SIZE;
}

// ==========================================
// THUẬT TOÁN KIỂM TRA THẮNG THUA
// ==========================================
// Hàm đếm số quân cờ liên tiếp theo một vector hướng (dr, dc)
bool CheckDirection(int row, int col, int dr, int dc) {
    int value = _A[row][col].c; // Lấy giá trị của quân cờ vừa đánh (-1 hoặc 1)
    if (value == 0) return false;

    int count = 1; // Đếm số quân cờ liên tiếp (bắt đầu bằng chính quân vừa đánh)

    // Khởi tạo 2 đầu mút của chuỗi cờ (Dùng để vẽ highlight khi thắng)
    int r1 = row, c1 = col;
    int r2 = row, c2 = col;

    // 1. Kiểm tra tiến về phía trước theo hướng (dr, dc)
    for (int i = 1; i < 5; i++) {
        int r = row + dr * i;
        int c = col + dc * i;

        // Nếu tọa độ vẫn nằm trong bàn cờ và quân cờ giống màu với ô gốc
        if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && _A[r][c].c == value) {
            count++;
            r1 = r; c1 = c; // Cập nhật liên tục tọa độ đầu mút 1
        }
        else break; // Ngắt chuỗi ngay lập tức nếu gặp quân địch, ô trống hoặc ra ngoài biên
    }

    // 2. Kiểm tra lùi về phía sau theo hướng ngược lại (-dr, -dc)
    for (int i = 1; i < 5; i++) {
        int r = row - dr * i;
        int c = col - dc * i;

        if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && _A[r][c].c == value) {
            count++;
            r2 = r; c2 = c; // Cập nhật liên tục tọa độ đầu mút 2
        }
        else break;
    }

    // 3. Nếu đếm đủ 5 quân cờ liên tiếp thì xác nhận thắng
    if (count >= 5) {
        _winLine.r1 = r1; _winLine.c1 = c1; // Lưu tọa độ đầu chuỗi
        _winLine.r2 = r2; _winLine.c2 = c2; // Lưu tọa độ cuối chuỗi
        return true;
    }

    return false;
}

// Hàm tổng hợp: Phân tích trạng thái bàn cờ hiện tại
// Trả về: -1 (X thắng), 1 (O thắng), 0 (Hòa), 2 (Chưa ai thắng, chơi tiếp)
int TestBoard(int pX, int pY) {
    int row = -1, col = -1;

    // 1. Chuyển đổi từ tọa độ màn hình (pX, pY) sang chỉ số mảng (row, col)
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].x == pX && _A[i][j].y == pY) {
                row = i; col = j;
                break;
            }
        }
    }

    // Nếu tìm không thấy vị trí hợp lệ trên bàn cờ
    if (row == -1) return 2;

    // 2. Kiểm tra chuỗi thắng ở cả 4 trục (dọc, ngang, chéo xuôi, chéo ngược)
    if (CheckDirection(row, col, 1, 0) || // Trục dọc (Đẩy row lên, col giữ nguyên)
        CheckDirection(row, col, 0, 1) || // Trục ngang (Đẩy col lên, row giữ nguyên)
        CheckDirection(row, col, 1, 1) || // Trục chéo xuôi (\)
        CheckDirection(row, col, 1, -1))  // Trục chéo ngược (/)
    {
        return (_A[row][col].c == -1) ? -1 : 1; // Trả về người chiến thắng dựa trên màu cờ
    }

    // 3. Kiểm tra trường hợp Hòa (Bàn cờ không còn ô trống nào)
    bool isFull = true;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c == 0) {
                isFull = false; // Vẫn còn ô trống
                break;
            }
        }
    }
    if (isFull) return 0; // Hòa

    // 4. Trận đấu vẫn đang tiếp diễn
    return 2;
}