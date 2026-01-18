#ifndef GAME_GLOBAL_HPP
#define GAME_GLOBAL_HPP

#include <stdint.h>

/**
 * @brief Enum để định danh các chế độ game
 */
enum GameMode {
    GAME_MODE_4X4 = 0,          // MainScreen (4x4)
    GAME_MODE_3X3,              // Screen3x3
    GAME_MODE_5X5,              // Screen5x5
    GAME_MODE_5X5_SUPER,        // Screen5x5_superMerging
    GAME_MODE_5X5_LETTER        // Screen5x5_letterMerging
};

class GameGlobal {
public:
    // Best scores riêng cho từng màn chơi
    static uint32_t bestScore4x4;          // MainScreen (4x4)
    static uint32_t bestScore3x3;          // Screen3x3
    static uint32_t bestScore5x5;          // Screen5x5
    static uint32_t bestScore5x5Super;     // Screen5x5_superMerging
    static uint32_t bestScore5x5Letter;    // Screen5x5_letterMerging
    
    // Score hiện tại (dùng chung để hiển thị ở GameOver screen)
    static uint32_t yourScore;
    
    // Game mode hiện tại đang chơi (để GameOver lấy đúng bestScore)
    static GameMode currentGameMode;

    /**
     * @brief Lấy best score của game mode hiện tại
     * @return Best score tương ứng với currentGameMode
     */
    static uint32_t getCurrentBestScore();

    /**
     * @brief Cập nhật best score cho game mode hiện tại
     * @param score Score mới để so sánh và cập nhật
     */
    static void updateCurrentBestScore(uint32_t score);

    // Best score chung (deprecated - giữ lại để tương thích)
    static uint32_t bestScore;
};

#endif
