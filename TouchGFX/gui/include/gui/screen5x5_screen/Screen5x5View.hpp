#ifndef SCREEN5X5VIEW_HPP
#define SCREEN5X5VIEW_HPP

#include <gui_generated/screen5x5_screen/Screen5x5ViewBase.hpp>
#include <gui/screen5x5_screen/Screen5x5Presenter.hpp>
#include <gui/containers/Tile5x5.hpp>
#include <gui/common/Game2048Engine.hpp>

/**
 * @class Screen5x5View
 * @brief View trong kiến trúc MVP cho Screen5x5 (Game 2048 5x5)
 * 
 * REFACTORED: Sử dụng Game2048Engine<5> để xử lý game logic
 */
class Screen5x5View : public Screen5x5ViewBase
{
public:
    Screen5x5View();
    virtual ~Screen5x5View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
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

    void spawnRandomTile();
    void navigateToGameOverScreen();
    void gotoGameOverScreen();
    bool isGameOver();

protected:
    // ==============================================================================
    // Game Engine
    // ==============================================================================
    Game2048Engine<5> engine;
    
    // ==============================================================================
    // UI Components
    // ==============================================================================
    Tile5x5* tiles[5][5];
    
    // ==============================================================================
    // Helper Methods
    // ==============================================================================
    uint32_t myRand();
    void syncEngineToUI();
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

#endif // SCREEN5X5VIEW_HPP
