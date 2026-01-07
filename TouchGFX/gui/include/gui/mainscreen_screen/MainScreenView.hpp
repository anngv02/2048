#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>
#include <gui/containers/Tile.hpp>
#include <stdint.h>
class MainScreenView : public MainScreenViewBase
{
public:
    MainScreenView();
    virtual ~MainScreenView() {}
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
    Tile* tiles[4][4];
    uint16_t gridBeforeMove[4][4];
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

#endif // MAINSCREENVIEW_HPP
