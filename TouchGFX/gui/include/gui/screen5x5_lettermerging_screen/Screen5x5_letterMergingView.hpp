#ifndef SCREEN5X5_LETTERMERGINGVIEW_HPP
#define SCREEN5X5_LETTERMERGINGVIEW_HPP

#include <gui_generated/screen5x5_lettermerging_screen/Screen5x5_letterMergingViewBase.hpp>
#include <gui/screen5x5_lettermerging_screen/Screen5x5_letterMergingPresenter.hpp>

/**
 * @class Screen5x5_letterMergingView
 * @brief View trong kiến trúc MVP cho Screen5x5_letterMerging
 */
class Screen5x5_letterMergingView : public Screen5x5_letterMergingViewBase
{
public:
    Screen5x5_letterMergingView();
    virtual ~Screen5x5_letterMergingView() {}
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
    void saveGridState();
    bool hasGridChanged();

protected:
    Tile_letter* tiles[4][4];
    uint16_t gridBeforeMove[4][4];
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    uint32_t score = 0;
    uint32_t bestScore = 0;
    uint32_t myRand();
    void updateScoreText();
    
    int16_t dragStartX;
    int16_t dragStartY;
    int16_t dragEndX;
    int16_t dragEndY;
    bool isDragging;
    
    static const int16_t MIN_SWIPE_DISTANCE = 30;

private:
    void processAfterMove();
};

#endif // SCREEN5X5_LETTERMERGINGVIEW_HPP
