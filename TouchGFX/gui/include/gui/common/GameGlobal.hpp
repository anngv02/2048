#ifndef GAME_GLOBAL_HPP
#define GAME_GLOBAL_HPP

#include <stdint.h>

class GameGlobal {
public:
    // Best scores riêng cho từng màn chơi
    static uint32_t bestScore4x4;          // MainScreen (4x4)
    static uint32_t bestScore3x3;           // Screen3x3
    static uint32_t bestScore5x5;           // Screen5x5
    static uint32_t bestScore5x5Super;      // Screen5x5_superMerging
    static uint32_t bestScore5x5Letter;     // Screen5x5_letterMerging
    
    // Score hiện tại (dùng chung để hiển thị ở GameOver screen)
    static uint32_t yourScore;
    
    // Best score chung (deprecated - giữ lại để tương thích)
    static uint32_t bestScore;
};

#endif
