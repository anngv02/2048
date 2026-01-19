#ifndef SCREEN3X3VIEW_HPP
#define SCREEN3X3VIEW_HPP

#include <gui_generated/screen3x3_screen/Screen3x3ViewBase.hpp>
#include <gui/screen3x3_screen/Screen3x3Presenter.hpp>
#include <gui/containers/Tile3x3.hpp>
#include <gui/common/Game2048Engine.hpp>

/**
 * @class Screen3x3View
 * @brief View trong kiến trúc MVP cho Screen3x3 (Game 2048 3x3)
 * 
 * REFACTORED: Sử dụng Game2048Engine<3> để xử lý game logic
 */
class Screen3x3View : public Screen3x3ViewBase
{
public:
    Screen3x3View();
    virtual ~Screen3x3View() {}
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
    Game2048Engine<3> engine;
    
    // ==============================================================================
    // UI Components
    // ==============================================================================
    Tile3x3* tiles[3][3];
    
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

#endif // SCREEN3X3VIEW_HPP
