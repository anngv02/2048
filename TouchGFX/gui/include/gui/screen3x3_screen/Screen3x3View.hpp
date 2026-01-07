#ifndef SCREEN3X3VIEW_HPP
#define SCREEN3X3VIEW_HPP

#include <gui_generated/screen3x3_screen/Screen3x3ViewBase.hpp>
#include <gui/screen3x3_screen/Screen3x3Presenter.hpp>
#include <gui/containers/Tile3x3.hpp>
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
    void spawnRandomTile();
    void navigateToGameOverScreen();
    void gotoGameOverScreen();
    bool isGameOver();
    void saveGridState();//Hàm lưu trạng thái hiện tại
    bool hasGridChanged();//Hàm kiểm tra xem lưới có thay đổi không
protected: 
    Tile3x3* tiles[3][3];
    uint16_t gridBeforeMove[3][3];
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

#endif // SCREEN3X3VIEW_HPP
