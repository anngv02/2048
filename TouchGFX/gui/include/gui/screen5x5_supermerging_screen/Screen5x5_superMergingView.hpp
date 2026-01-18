#ifndef SCREEN5X5_SUPERMERGINGVIEW_HPP
#define SCREEN5X5_SUPERMERGINGVIEW_HPP

#include <gui_generated/screen5x5_supermerging_screen/Screen5x5_superMergingViewBase.hpp>
#include <gui/screen5x5_supermerging_screen/Screen5x5_superMergingPresenter.hpp>
#include <gui/containers/Tile5x5_Super.hpp>
#include <vector>

/**
 * @class Screen5x5_superMergingView
 * @brief View trong kiến trúc MVP cho Screen5x5_superMerging
 */
class Screen5x5_superMergingView : public Screen5x5_superMergingViewBase
{
public:
    Screen5x5_superMergingView();
    virtual ~Screen5x5_superMergingView() {}
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
    void mergeSuperTiles(Tile5x5_Super* line[5], bool reverse);

protected:
    Tile5x5_Super* tiles[5][5];
    uint16_t gridBeforeMove[5][5];
    void moveLeftSuperMerging();
    void moveRightSuperMerging();
    void moveUpSuperMerging();
    void moveDownSuperMerging();
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

#endif // SCREEN5X5_SUPERMERGINGVIEW_HPP
