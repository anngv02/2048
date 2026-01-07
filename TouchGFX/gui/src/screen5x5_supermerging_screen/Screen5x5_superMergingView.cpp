#include <gui/screen5x5_supermerging_screen/Screen5x5_superMergingView.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <cstdio>  // hoặc stdio.h nếu bạn thích C-style
#include <cstdlib> // srand, rand - cho hàm abs()
#include <ctime>
#include <gui/common/FrontendApplication.hpp>
#include <vector>
#include <gui/common/GameGlobal.hpp>
#include <algorithm>
#include <stm32f4xx_hal.h>
#define TILE_SIZE 48
static uint32_t seed = 1;

uint32_t Screen5x5_superMergingView::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}
Screen5x5_superMergingView::Screen5x5_superMergingView()
{
    // Gán từng Tile từ Designer vào mảng 2D
    tiles[0][0] = &tile5x5_Super1;
    tiles[0][1] = &tile5x5_Super2;
    tiles[0][2] = &tile5x5_Super3;
    tiles[0][3] = &tile5x5_Super4;
    tiles[0][4] = &tile5x5_Super5;

    tiles[1][0] = &tile5x5_Super6;
    tiles[1][1] = &tile5x5_Super7;
    tiles[1][2] = &tile5x5_Super8;
    tiles[1][3] = &tile5x5_Super9;
    tiles[1][4] = &tile5x5_Super10;

    tiles[2][0] = &tile5x5_Super11;
    tiles[2][1] = &tile5x5_Super12;
    tiles[2][2] = &tile5x5_Super13;
    tiles[2][3] = &tile5x5_Super14;
    tiles[2][4] = &tile5x5_Super15;

    tiles[3][0] = &tile5x5_Super16;
    tiles[3][1] = &tile5x5_Super17;
    tiles[3][2] = &tile5x5_Super18;
    tiles[3][3] = &tile5x5_Super19;
    tiles[3][4] = &tile5x5_Super20;

    tiles[4][0] = &tile5x5_Super21;
    tiles[4][1] = &tile5x5_Super22;
    tiles[4][2] = &tile5x5_Super23;
    tiles[4][3] = &tile5x5_Super24;
    tiles[4][4] = &tile5x5_Super25;
    
    // Khởi tạo biến drag
    dragStartX = 0;
    dragStartY = 0;
    dragEndX = 0;
    dragEndY = 0;
    isDragging = false;
}

void Screen5x5_superMergingView::setupScreen()
{
    score = 0;
    bestScore = GameGlobal::bestScore5x5Super;  // Dùng bestScore riêng cho màn 5x5 Super Merging
    const int tileOffsetY = 80;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
    updateScoreText();
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            tiles[i][j]->setValue(0); // ẩn ban đầu
            tiles[i][j]->moveTo((j)*TILE_SIZE, tileOffsetY + i * TILE_SIZE);
            tiles[i][j]->centerX = (j)*TILE_SIZE + TILE_SIZE / 2;
            tiles[i][j]->centerY = tileOffsetY + i * TILE_SIZE + TILE_SIZE / 2;
        }
    }

    tiles[0][0]->setValue(2);
    tiles[0][1]->setValue(2);
    Screen5x5_superMergingViewBase::setupScreen();
}

void Screen5x5_superMergingView::tearDownScreen()
{
    Screen5x5_superMergingViewBase::tearDownScreen();
}

void Screen5x5_superMergingView::handleDragEvent(const DragEvent& evt)
{
    if (evt.getType() == DragEvent::DRAGGED)
    {
        if (!isDragging)
        {
            // Lưu điểm bắt đầu
            dragStartX = evt.getOldX();
            dragStartY = evt.getOldY();
            isDragging = true;
        }
        // Cập nhật điểm cuối liên tục
        dragEndX = evt.getNewX();
        dragEndY = evt.getNewY();
    }
    else if (evt.getType() == DragEvent::DRAGGED_OUT)
    {
        // Reset khi kéo ra khỏi vùng
        isDragging = false;
    }
    
    Screen5x5_superMergingViewBase::handleDragEvent(evt);
}

void Screen5x5_superMergingView::handleGestureEvent(const GestureEvent &evt)
{
    // Nếu chưa từng nhận Drag trước đó, bỏ qua
    if (!isDragging) return;

    saveGridState(); // luu trang thai truoc khi chuyen
    
    // Tính delta từ điểm bắt đầu và điểm cuối
    int16_t deltaX = dragEndX - dragStartX;
    int16_t deltaY = dragEndY - dragStartY;
    
    // Tính độ dài vector
    int16_t absX = abs(deltaX);
    int16_t absY = abs(deltaY);

    // 1. Kiểm tra độ dài tối thiểu (Lọc nhiễu rung tay)
    if (absX < MIN_SWIPE_DISTANCE && absY < MIN_SWIPE_DISTANCE) {
        isDragging = false;
        return;
    }

    // 2. Thuật toán "Dominant Axis" (Trục chiếm ưu thế)
    // Nếu di chuyển ngang nhiều hơn dọc -> Là vuốt Ngang
    if (absX > absY) 
    {
        // Đây là vuốt NGANG
        if (deltaX > 0) moveRightSuperMerging();
        else            moveLeftSuperMerging();
    }
    else 
    {
        // Đây là vuốt DỌC
        // Lưu ý: Hệ tọa độ màn hình Y tăng dần xuống dưới
        if (deltaY > 0) moveDownSuperMerging();
        else            moveUpSuperMerging();
    }

    // 3. Reset trạng thái & Xử lý Game logic
    isDragging = false;

    if (hasGridChanged())
    {
        spawnRandomTile(); // chi spawn neu co thay doi
    }
    
    if (isGameOver())
    {
        navigateToGameOverScreen();
    }
}
void Screen5x5_superMergingView::updateScoreText()
{
    GameGlobal::yourScore = score;
    GameGlobal::bestScore5x5Super = bestScore;  // Lưu bestScore riêng cho màn 5x5 Super Merging
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
}
void Screen5x5_superMergingView::moveLeftSuperMerging()
{
    for (int row = 0; row < 5; ++row)
    {
        Tile5x5_Super *line[5];
        for (int col = 0; col < 5; ++col)
            line[col] = tiles[row][col];
        mergeSuperTiles(line, false);
    }
}

void Screen5x5_superMergingView::moveRightSuperMerging()
{
    for (int row = 0; row < 5; ++row)
    {
        Tile5x5_Super *line[5];
        for (int col = 0; col < 5; ++col)
            line[col] = tiles[row][4 - col];
        mergeSuperTiles(line, true);
    }
}

void Screen5x5_superMergingView::moveUpSuperMerging()
{
    for (int col = 0; col < 5; ++col)
    {
        Tile5x5_Super *line[5];
        for (int row = 0; row < 5; ++row)
            line[row] = tiles[row][col];
        mergeSuperTiles(line, false);
    }
}

void Screen5x5_superMergingView::moveDownSuperMerging()
{
    for (int col = 0; col < 5; ++col)
    {
        Tile5x5_Super *line[5];
        for (int row = 0; row < 5; ++row)
            line[row] = tiles[4 - row][col];
        mergeSuperTiles(line, true);
    }
}

void Screen5x5_superMergingView::handleKeyEvent(uint8_t key)
{
    saveGridState();
    switch (key)
    {
    case '4':
        moveLeftSuperMerging();
        // spawnRandomTile();
        break;
    case '6':
        moveRightSuperMerging();
        // spawnRandomTile();
        break;
    case '8':
        moveUpSuperMerging();
        // spawnRandomTile();
        break;
    case '2':
        moveDownSuperMerging();
        // spawnRandomTile();
        break;
    default:
        break;
    }
    if (hasGridChanged())
    {
        spawnRandomTile();
    }
    // Sau khi di chuyển + spawn → kiểm tra thua
    if (isGameOver())
    {
        navigateToGameOverScreen();
    }
}
void Screen5x5_superMergingView::spawnRandomTile()
{
    // 1) Tạo danh sách các ô còn trống
    struct Pos
    {
        int row, col;
    };
    Pos empties[25];
    int emptyCount = 0;

    for (int r = 0; r < 5; r++)
    {
        for (int c = 0; c < 5; c++)
        {
            if (tiles[r][c]->getValue() == 0)
            {
                empties[emptyCount++] = {r, c};
            }
        }
    }

    // 2) Nếu có ô trống, chọn ngẫu nhiên một ô
    if (emptyCount > 0)
    {
        // Khởi tạo seed lần đầu (bạn có thể làm trong constructor)
        //        static bool seeded = false;
        //        if (!seeded) {
        //            std::srand(12345);
        //            seeded = true;
        //        }
        //        int idx = std::rand() % emptyCount;
        //        int rr = empties[idx].row;
        //        int cc = empties[idx].col;
        //
        //        // 3) Đặt giá trị 2 vào ô đó
        //        tiles[rr][cc]->setValue(2);
        int idx = myRand() % emptyCount;
        int rr = empties[idx].row;
        int cc = empties[idx].col;
        //        tiles[rr][cc]->setValue(2);

        uint16_t newValue = (myRand() % 10 == 0) ? 4 : 2;
        tiles[rr][cc]->setValue(newValue);
        tiles[rr][cc]->animateSpawn(); // animation spawn
    }
    else
    {
        // KHÔNG còn ô trống kiem tra xem co the gop duoc khong neu khong thi chuyen sang Game Over
        if (isGameOver())
        {
            navigateToGameOverScreen();
        }
    }
}
void Screen5x5_superMergingView::navigateToGameOverScreen()
{
    static_cast<FrontendApplication *>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}
bool Screen5x5_superMergingView::isGameOver() // kiem tra xem con co the gop cac o lai voi nhau khong
{
    // 1. Kiểm tra còn ô trống không
    for (int r = 0; r < 5; ++r)
    {
        for (int c = 0; c < 5; ++c)
        {
            if (tiles[r][c]->getValue() == 0)
                return false; // còn chỗ để spawn => chưa thua
        }
    }

    // 2. Kiểm tra còn ô nào có thể gộp không
    for (int r = 0; r < 5; ++r)
    {
        for (int c = 0; c < 5; ++c)
        {
            int current = tiles[r][c]->getValue();

            // Kiểm tra phải
            if (c < 4 && tiles[r][c + 1]->getValue() == current)
                return false;

            // Kiểm tra dưới
            if (r < 4 && tiles[r + 1][c]->getValue() == current)
                return false;
        }
    }

    // Không còn nước đi hợp lệ
    return true;
}
void Screen5x5_superMergingView::saveGridState()
{
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j)
            gridBeforeMove[i][j] = tiles[i][j]->getValue();
}
bool Screen5x5_superMergingView::hasGridChanged()
{
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j)
            if (gridBeforeMove[i][j] != tiles[i][j]->getValue())
                return true;
    return false;
}
void Screen5x5_superMergingView::mergeSuperTiles(Tile5x5_Super *line[5], bool reverse)
{
    // B1: Lấy các giá trị khác 0
    uint16_t values[5];
    int count = 0;

    for (int i = 0; i < 5; ++i)
    {
        if (line[i]->getValue() != 0)
        {
            values[count++] = line[i]->getValue();
        }
    }

    // B2: Nếu reverse (vuốt phải hoặc xuống), đảo thứ tự
    if (reverse)
    {
        for (int i = 0; i < count / 2; ++i)
            std::swap(values[i], values[count - 1 - i]);
    }

    // B3: Gộp super-merging
    uint16_t merged[5] = {0};
    int m = 0;
    int i = 0;

    while (i < count)
    {
        uint16_t val = values[i];
        int j = i + 1;
        while (j < count && values[j] == val)
            ++j;

        int sameCount = j - i;
        if (sameCount > 1)
        {
            merged[m++] = val * sameCount;
            score += val * sameCount;
        }
        else
        {
            merged[m++] = val;
        }
        i = j;
    }

    // B4: Nếu reverse thì đảo lại thứ tự
    if (reverse)
    {
        for (int i = 0; i < m / 2; ++i)
            std::swap(merged[i], merged[m - 1 - i]);
    }

    // B5: Gán lại vào các tile
    for (int i = 0; i < 5; ++i)
    {
        if (i < m)
            line[i]->setValue(merged[i]);
        else
            line[i]->setValue(0);
    }

    // B6: Cập nhật điểm
    if (score > bestScore)
        bestScore = score;
    updateScoreText();
}
void Screen5x5_superMergingView::handleTickEvent()
{
    static uint32_t lastPressTime = 0;
    const uint32_t debounceDelay = 200; // Thời gian chống dội (ms)
    uint32_t currentTime = HAL_GetTick();

    uint8_t currentState = 0;
    currentState |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET ? 1 : 0);  // PB10: UP
    currentState |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_SET ? 2 : 0);  // PB12: DOWN
    currentState |= (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_SET ? 4 : 0);   // PA2: LEFT
    currentState |= (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET ? 8 : 0);   // PA6: RIGHT
    currentState |= (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET ? 16 : 0);  // PA0: BACK

    static uint8_t lastState = 0;

    if (currentTime - lastPressTime > debounceDelay)
    {
        saveGridState();

        if ((currentState & 1) && !(lastState & 1)) // PB10: UP
        {
            moveUpSuperMerging();
            lastPressTime = currentTime;
        }
        else if ((currentState & 2) && !(lastState & 2)) // PB12: DOWN
        {
            moveDownSuperMerging();
            lastPressTime = currentTime;
        }
        else if ((currentState & 4) && !(lastState & 4)) // PA2: LEFT
        {
            moveLeftSuperMerging();
            lastPressTime = currentTime;
        }
        else if ((currentState & 8) && !(lastState & 8)) // PA6: RIGHT
        {
            moveRightSuperMerging();
            lastPressTime = currentTime;
        }
        else if ((currentState & 16) && !(lastState & 16)) // PA0: BACK
        {
            application().gotoChosing_modeScreenWipeTransitionEast();
            lastPressTime = currentTime;
        }

        if (hasGridChanged())
        {
            spawnRandomTile();
        }

        if (isGameOver())
        {
            navigateToGameOverScreen();
        }
    }

    lastState = currentState;
}

