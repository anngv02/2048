#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>
#include <gui/containers/Tile.hpp>
#include <stdint.h>

/**
 * @class MainScreenView
 * @brief View trong kiến trúc MVP cho MainScreen (Game 2048 4x4)
 *
 * View chịu trách nhiệm:
 * - Hiển thị UI (tiles, score, ...)
 * - Nhận input từ touch/gesture
 * - KHÔNG truy cập trực tiếp hardware (GPIO) - thông qua Presenter
 *
 * Luồng Button: Model -> Presenter -> View (onMoveXxx methods)
 * Luồng Touch: View trực tiếp xử lý (handleDragEvent, handleGestureEvent)
 */
class MainScreenView : public MainScreenViewBase
{
public:
    MainScreenView();
    virtual ~MainScreenView() {}

    virtual void setupScreen();
    virtual void tearDownScreen();

    // ==============================================================================
    // Touch/Gesture Event Handlers (xử lý trực tiếp trong View)
    // ==============================================================================
    virtual void handleDragEvent(const DragEvent& evt);
    virtual void handleGestureEvent(const GestureEvent& evt);
    virtual void handleKeyEvent(uint8_t key);
    virtual void handleTickEvent();

    // ==============================================================================
    // Public methods để Presenter gọi (MVP pattern)
    // ==============================================================================

    /**
     * @brief Xử lý khi Presenter báo nhấn nút UP
     */
    void onMoveUp();

    /**
     * @brief Xử lý khi Presenter báo nhấn nút DOWN
     */
    void onMoveDown();

    /**
     * @brief Xử lý khi Presenter báo nhấn nút LEFT
     */
    void onMoveLeft();

    /**
     * @brief Xử lý khi Presenter báo nhấn nút RIGHT
     */
    void onMoveRight();

    /**
     * @brief Xử lý khi Presenter báo nhấn nút BACK
     */
    void onNavigateBack();

    // ==============================================================================
    // Game Logic Methods (public để có thể test/access)
    // ==============================================================================
    void spawnRandomTile();
    void navigateToGameOverScreen();
    void gotoGameOverScreen();
    bool isGameOver();
    void saveGridState();
    bool hasGridChanged();

protected:
    // ==============================================================================
    // Game State
    // ==============================================================================
    Tile* tiles[4][4];                    // Mảng 2D con trỏ đến Tile widgets
    uint16_t gridBeforeMove[4][4];        // Lưu trạng thái trước khi move (để check thay đổi)
    uint32_t score;                       // Điểm hiện tại
    uint32_t bestScore;                   // Điểm cao nhất

    // ==============================================================================
    // Internal Move Methods
    // ==============================================================================
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    
    // ==============================================================================
    // Helper Methods
    // ==============================================================================
    uint32_t myRand();                    // Custom random number generator
    void updateScoreText();               // Cập nhật hiển thị điểm

    // ==============================================================================
    // Drag/Gesture State (cho touch input)
    // ==============================================================================
    int16_t dragStartX;
    int16_t dragStartY;
    int16_t dragEndX;
    int16_t dragEndY;
    bool isDragging;
    
    // Ngưỡng tối thiểu để tính là 1 lần vuốt (pixel)
    static const int16_t MIN_SWIPE_DISTANCE = 30;

private:
    /**
     * @brief Xử lý chung sau khi move (spawn tile, check game over)
     */
    void processAfterMove();
};

#endif // MAINSCREENVIEW_HPP
