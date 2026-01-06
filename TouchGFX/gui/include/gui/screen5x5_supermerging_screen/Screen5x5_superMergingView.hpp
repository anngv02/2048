#ifndef SCREEN5X5_SUPERMERGINGVIEW_HPP
#define SCREEN5X5_SUPERMERGINGVIEW_HPP

#include <gui_generated/screen5x5_supermerging_screen/Screen5x5_superMergingViewBase.hpp>
#include <gui/screen5x5_supermerging_screen/Screen5x5_superMergingPresenter.hpp>
#include <gui/containers/Tile5x5_Super.hpp>
#include <vector>
class Screen5x5_superMergingView : public Screen5x5_superMergingViewBase
{
public:
    Screen5x5_superMergingView();
    virtual ~Screen5x5_superMergingView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleGestureEvent(const GestureEvent& evt);
    virtual void handleKeyEvent(uint8_t key);
    virtual void handleTickEvent();
    void spawnRandomTile();
    void navigateToGameOverScreen();
    void gotoGameOverScreen();
    bool isGameOver();
    void saveGridState();//Hàm lưu trạng thái hiện tại
    bool hasGridChanged();//Hàm kiểm tra xem lưới có thay đổi không
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
};

#endif // SCREEN5X5_SUPERMERGINGVIEW_HPP
