#include <gui/common/GameGlobal.hpp>

// Khởi tạo best scores riêng cho từng màn chơi
uint32_t GameGlobal::bestScore4x4 = 0;
uint32_t GameGlobal::bestScore3x3 = 0;
uint32_t GameGlobal::bestScore5x5 = 0;
uint32_t GameGlobal::bestScore5x5Super = 0;
uint32_t GameGlobal::bestScore5x5Letter = 0;

uint32_t GameGlobal::yourScore = 0;

// Best score chung (deprecated - giữ lại để tương thích)
uint32_t GameGlobal::bestScore = 0;