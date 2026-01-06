#ifndef SCREEN5X5VIEW_HPP
#define SCREEN5X5VIEW_HPP

#include <gui_generated/screen5x5_screen/Screen5x5ViewBase.hpp>
#include <gui/screen5x5_screen/Screen5x5Presenter.hpp>
#include <gui/containers/Tile5x5.hpp>
class Screen5x5View : public Screen5x5ViewBase
{
public:
    Screen5x5View();
    virtual ~Screen5x5View() {}
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
    bool hasGridChanged();
protected:
    Tile5x5* tiles[5][5];
    uint16_t gridBeforeMove[5][5];
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    uint32_t score = 0;
    uint32_t bestScore = 0;
    uint32_t myRand();
    void updateScoreText();
};

#endif // SCREEN5X5VIEW_HPP
