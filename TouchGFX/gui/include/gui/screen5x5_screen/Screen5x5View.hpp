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
    virtual void handleDragEvent(const DragEvent& evt);
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
    
    // Biến để xử lý drag và gesture
    int16_t dragStartX;
    int16_t dragStartY;
    int16_t dragEndX;
    int16_t dragEndY;
    bool isDragging;
    
    // Ngưỡng tối thiểu để tính là 1 lần vuốt (pixel)
    static const int16_t MIN_SWIPE_DISTANCE = 30;
};

#endif // SCREEN5X5VIEW_HPP
