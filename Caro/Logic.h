#ifndef LOGIC_H
#define LOGIC_H


void ResetData();

// Tận dụng Bước 10 
int CheckBoard(int pX, int pY);

// Logic di chuyển tận dụng Bước 11 
void MoveRight(); 
void MoveLeft(); 
void MoveDown(); 
void MoveUp(); 
bool CheckDirection(int row, int col, int dr, int dc);
// Bước 9: Kiểm tra thắng thua (Sinh viên cần tự viết luật Caro) [cite: 103, 106]
int TestBoard(int pX,int pY);
#endif