#include <gui/common/GameGlobal.hpp>

// Khởi tạo best scores riêng cho từng màn chơi
uint32_t GameGlobal::bestScore4x4 = 0;
uint32_t GameGlobal::bestScore3x3 = 0;
uint32_t GameGlobal::bestScore5x5 = 0;
uint32_t GameGlobal::bestScore5x5Super = 0;
uint32_t GameGlobal::bestScore5x5Letter = 0;

uint32_t GameGlobal::yourScore = 0;

// Game mode hiện tại (mặc định là 4x4)
GameMode GameGlobal::currentGameMode = GAME_MODE_4X4;

// Best score chung (deprecated - giữ lại để tương thích)
uint32_t GameGlobal::bestScore = 0;

/**
 * @brief Lấy best score của game mode hiện tại
 */
uint32_t GameGlobal::getCurrentBestScore()
{
    switch (currentGameMode)
    {
        case GAME_MODE_4X4:
            return bestScore4x4;
        case GAME_MODE_3X3:
            return bestScore3x3;
        case GAME_MODE_5X5:
            return bestScore5x5;
        case GAME_MODE_5X5_SUPER:
            return bestScore5x5Super;
        case GAME_MODE_5X5_LETTER:
            return bestScore5x5Letter;
        default:
            return bestScore4x4;
    }
}

/**
 * @brief Cập nhật best score cho game mode hiện tại
 */
void GameGlobal::updateCurrentBestScore(uint32_t score)
{
    switch (currentGameMode)
    {
        case GAME_MODE_4X4:
            if (score > bestScore4x4) bestScore4x4 = score;
            break;
        case GAME_MODE_3X3:
            if (score > bestScore3x3) bestScore3x3 = score;
            break;
        case GAME_MODE_5X5:
            if (score > bestScore5x5) bestScore5x5 = score;
            break;
        case GAME_MODE_5X5_SUPER:
            if (score > bestScore5x5Super) bestScore5x5Super = score;
            break;
        case GAME_MODE_5X5_LETTER:
            if (score > bestScore5x5Letter) bestScore5x5Letter = score;
            break;
    }
}
