#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>
#include <gui/containers/Tile.hpp>
#include <gui/common/Game2048Engine.hpp>
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
 * 
 * REFACTORED: Sử dụng Game2048Engine<4> để xử lý game logic
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
    void onMoveUp();
    void onMoveDown();
    void onMoveLeft();
    void onMoveRight();
    void onNavigateBack();

    // ==============================================================================
    // Game Logic Methods
    // ==============================================================================
    void spawnRandomTile();
    void navigateToGameOverScreen();
    void gotoGameOverScreen();
    bool isGameOver();

protected:
    // ==============================================================================
    // Game Engine (handles all game logic)
    // ==============================================================================
    Game2048Engine<4> engine;
    
    // ==============================================================================
    // UI Components
    // ==============================================================================
    Tile* tiles[4][4];  // UI tile widgets

    // ==============================================================================
    // Helper Methods
    // ==============================================================================
    uint32_t myRand();
    void syncEngineToUI();    // Sync engine.grid -> tiles UI
    void syncUIToEngine();    // Sync tiles UI -> engine.grid (if needed)
    void updateScoreText();

    // ==============================================================================
    // Drag/Gesture State
    // ==============================================================================
    int16_t dragStartX;
    int16_t dragStartY;
    int16_t dragEndX;
    int16_t dragEndY;
    bool isDragging;
    
    static const int16_t MIN_SWIPE_DISTANCE = 30;

private:
    void processAfterMove();
};

#endif // MAINSCREENVIEW_HPP
