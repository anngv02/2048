#include <gui/screen5x5_lettermerging_screen/Screen5x5_letterMergingView.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <cstdio>  // hoặc stdio.h nếu bạn thích C-style
#include <cstdlib>     // srand, rand
#include <ctime> 
#include <gui/common/FrontendApplication.hpp>
#include <gui/common/GameGlobal.hpp>
#include <stm32f4xx_hal.h>
#define TILE_SIZE 60
static uint32_t seed = 1;

uint32_t Screen5x5_letterMergingView::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}
Screen5x5_letterMergingView::Screen5x5_letterMergingView()
{
     // Gán từng Tile từ Designer vào mảng 2D
    tiles[0][0] = &tile_letter1;
    tiles[0][1] = &tile_letter2;
    tiles[0][2] = &tile_letter3;
    tiles[0][3] = &tile_letter4;

    tiles[1][0] = &tile_letter5;
    tiles[1][1] = &tile_letter6;
    tiles[1][2] = &tile_letter7;
    tiles[1][3] = &tile_letter8;

    tiles[2][0] = &tile_letter9;
    tiles[2][1] = &tile_letter10;
    tiles[2][2] = &tile_letter11;
    tiles[2][3] = &tile_letter12;

    tiles[3][0] = &tile_letter13;
    tiles[3][1] = &tile_letter14;
    tiles[3][2] = &tile_letter15;
    tiles[3][3] = &tile_letter16;
}

void Screen5x5_letterMergingView::setupScreen()
{    
    score = 0;
    bestScore = GameGlobal::bestScore;
    const int tileOffsetY = 80;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
    updateScoreText();
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            tiles[i][j]->setValue(0xFFFF); // ẩn ban đầu
            tiles[i][j]->moveTo((j) * TILE_SIZE,tileOffsetY + i * TILE_SIZE);
            tiles[i][j]->centerX = (j) * TILE_SIZE + TILE_SIZE / 2;
            tiles[i][j]->centerY = tileOffsetY + i * TILE_SIZE + TILE_SIZE / 2;
        }
    }

    tiles[0][0]->setValue(0);
    tiles[0][1]->setValue(0);
    Screen5x5_letterMergingViewBase::setupScreen();
}

void Screen5x5_letterMergingView::tearDownScreen()
{
    Screen5x5_letterMergingViewBase::tearDownScreen();
}
void Screen5x5_letterMergingView::handleGestureEvent(const GestureEvent& evt)
{
    if (evt.getType() == GestureEvent::SWIPE_HORIZONTAL)
    {
        if (evt.getVelocity() > 0)
        {
            // Vuốt sang phải
             moveRight();

        }
        else
        {
            // Vuốt sang trái
            moveLeft();
        }
    }
    else if (evt.getType() == GestureEvent::SWIPE_VERTICAL)
    {
        if (evt.getVelocity() > 0)
        {
            // Vuốt xuống
             moveDown();
        }
        else
        {
            // Vuốt lên
            moveUp();
        }
    }
    if (hasGridChanged()) {
        spawnRandomTile();  // chi spawn neu co thay doi
    }
     // Sau khi di chuyển + spawn → kiểm tra thua
    if (isGameOver())
    {
        navigateToGameOverScreen();
    }
}
//cap nhat diem
void Screen5x5_letterMergingView::updateScoreText()
{   
    GameGlobal::yourScore = score;
    GameGlobal::bestScore = bestScore;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
}
//di chuyen trai
void Screen5x5_letterMergingView::moveLeft()
{
    for (int row = 0; row < 4; ++row)
    {
        int merged[4] = {0}; // theo dõi các tile đã merge

        for (int col = 1; col < 4; ++col)
        {
            if (tiles[row][col]->getValue() == 0xFFFF) continue;

            int currentCol = col;
            while (currentCol > 0 &&
                   tiles[row][currentCol - 1]->getValue() == 0xFFFF)
            {
                tiles[row][currentCol - 1]->setValue(tiles[row][currentCol]->getValue());
                tiles[row][currentCol]->setValue(0xFFFF);
                currentCol--;
            }

            // Nếu có thể gộp
            if (currentCol > 0 &&
                tiles[row][currentCol - 1]->getValue() == tiles[row][currentCol]->getValue() &&
                !merged[currentCol - 1])
            {   
                uint16_t newValue = tiles[row][currentCol - 1]->getValue() + 1;
                tiles[row][currentCol - 1]->setValue(
                    newValue);
                tiles[row][currentCol]->setValue(0xFFFF);
                merged[currentCol - 1] = 1;
                // Cộng điểm
                score += newValue;
                if (score > bestScore)
                bestScore = score;
                updateScoreText();
            }
        }
    }
}
void Screen5x5_letterMergingView::moveRight()
{
    for (int row = 0; row < 4; ++row)
    {
        int merged[4] = {0};

        for (int col = 2; col >= 0; --col)
        {
            if (tiles[row][col]->getValue() == 0xFFFF) continue;

            int currentCol = col;
            while (currentCol < 3 && tiles[row][currentCol + 1]->getValue() == 0xFFFF)
            {
                tiles[row][currentCol + 1]->setValue(tiles[row][currentCol]->getValue());
                tiles[row][currentCol]->setValue(0xFFFF);
                currentCol++;
            }

            if (currentCol < 3 &&
                tiles[row][currentCol + 1]->getValue() == tiles[row][currentCol]->getValue() &&
                !merged[currentCol + 1])
            {   
                uint16_t newValue = tiles[row][currentCol + 1]->getValue() + 1;
                tiles[row][currentCol + 1]->setValue(
                    newValue);
                tiles[row][currentCol]->setValue(0xFFFF);
                merged[currentCol + 1] = 1;

                // Cộng điểm
                score += newValue;
                if (score > bestScore)
                bestScore = score;
                updateScoreText();
            }
        }
    }
}
void Screen5x5_letterMergingView::moveUp()
{
    for (int col = 0; col < 4; ++col)
    {
        int merged[4] = {0};

        for (int row = 1; row < 4; ++row)
        {
            if (tiles[row][col]->getValue() == 0xFFFF) continue;

            int currentRow = row;
            while (currentRow > 0 && tiles[currentRow - 1][col]->getValue() == 0xFFFF)
            {
                tiles[currentRow - 1][col]->setValue(tiles[currentRow][col]->getValue());
                tiles[currentRow][col]->setValue(0xFFFF);
                currentRow--;
            }

            if (currentRow > 0 &&
                tiles[currentRow - 1][col]->getValue() == tiles[currentRow][col]->getValue() &&
                !merged[currentRow - 1])
            {   
                uint16_t newValue = tiles[currentRow - 1][col]->getValue() + 1;
                tiles[currentRow - 1][col]->setValue(
                    newValue);
                tiles[currentRow][col]->setValue(0xFFFF);
                merged[currentRow - 1] = 1;

                // Cộng điểm
                score += newValue;
                if (score > bestScore)
                bestScore = score;
                updateScoreText();
            }
        }
    }
}
void Screen5x5_letterMergingView::moveDown()
{
    for (int col = 0; col < 4; ++col)
    {
        int merged[4] = {0};

        for (int row = 2; row >= 0; --row)
        {
            if (tiles[row][col]->getValue() == 0xFFFF) continue;

            int currentRow = row;
            while (currentRow < 3 && tiles[currentRow + 1][col]->getValue() == 0xFFFF)
            {
                tiles[currentRow + 1][col]->setValue(tiles[currentRow][col]->getValue());
                tiles[currentRow][col]->setValue(0xFFFF);
                currentRow++;
            }

            if (currentRow < 3 &&
                tiles[currentRow + 1][col]->getValue() == tiles[currentRow][col]->getValue() &&
                !merged[currentRow + 1])
            {   
                uint16_t newValue = tiles[currentRow + 1][col]->getValue() + 1;
                tiles[currentRow + 1][col]->setValue(
                    newValue);
                tiles[currentRow][col]->setValue(0xFFFF);
                merged[currentRow + 1] = 1;

                // Cộng điểm
                score += newValue;
                if (score > bestScore)
                bestScore = score;
                updateScoreText();
            }
        }
    }
}

void Screen5x5_letterMergingView::handleKeyEvent(uint8_t key)
{   
    saveGridState();
    switch (key)
    {
    case '4':
        moveLeft();
        //spawnRandomTile(); 
        break;
    case '6':
        moveRight();
        //spawnRandomTile();  
        break;
    case '8':
        moveUp();
        //spawnRandomTile(); 
        break;
    case '2':
        moveDown();
        //spawnRandomTile();     
        break;
    default:
        break;
    }
    if(hasGridChanged()){
        spawnRandomTile();
    }
     // Sau khi di chuyển + spawn → kiểm tra thua
    if (isGameOver())
    {
        navigateToGameOverScreen();
    }
}
void Screen5x5_letterMergingView::spawnRandomTile()
{
    // 1) Tạo danh sách các ô còn trống
    struct Pos { int row, col; };
    Pos empties[16];
    int emptyCount = 0;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (tiles[r][c]->getValue() == 0xFFFF) {
                empties[emptyCount++] = {r, c};
            }
        }
    }

        // 2) Nếu có ô trống, chọn ngẫu nhiên một ô
    if (emptyCount > 0) {
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

        
        tiles[rr][cc]->setValue(0);
        tiles[rr][cc]->animateSpawn();//animation spawn
    }
    else {
        // KHÔNG còn ô trống kiem tra xem co the gop duoc khong neu khong thi chuyen sang Game Over
        if (isGameOver()) {
        navigateToGameOverScreen();
    }
    }
}
void Screen5x5_letterMergingView::navigateToGameOverScreen()
{
   static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}
bool Screen5x5_letterMergingView::isGameOver() // kiem tra xem con co the gop cac o lai voi nhau khong
{
    // 1. Kiểm tra còn ô trống không
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (tiles[r][c]->getValue() == 0xFFFF)
                return false; // còn chỗ để spawn => chưa thua
        }
    }

    // 2. Kiểm tra còn ô nào có thể gộp không
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            int current = tiles[r][c]->getValue();

            // Kiểm tra phải
            if (c < 3 && tiles[r][c + 1]->getValue() == current)
                return false;

            // Kiểm tra dưới
            if (r < 3 && tiles[r + 1][c]->getValue() == current)
                return false;
        }
    }

    // Không còn nước đi hợp lệ
    return true;
}
void Screen5x5_letterMergingView::saveGridState()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            gridBeforeMove[i][j] = tiles[i][j]->getValue();
}
bool Screen5x5_letterMergingView::hasGridChanged()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (gridBeforeMove[i][j] != tiles[i][j]->getValue())
                return true;
    return false;
}
void Screen5x5_letterMergingView::handleTickEvent()
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
        if ((currentState & 1) && !(lastState & 1)) // PB10: UP
        {
            moveUp();
            spawnRandomTile();
            lastPressTime = currentTime;
        }
        else if ((currentState & 2) && !(lastState & 2)) // PB12: DOWN
        {
            moveDown();
            spawnRandomTile();
            lastPressTime = currentTime;
        }
        else if ((currentState & 4) && !(lastState & 4)) // PA2: LEFT
        {
            moveLeft();
            spawnRandomTile();
            lastPressTime = currentTime;
        }
        else if ((currentState & 8) && !(lastState & 8)) // PA6: RIGHT
        {
            moveRight();
            spawnRandomTile();
            lastPressTime = currentTime;
        }
        else if ((currentState & 16) && !(lastState & 16)) // PA0: BACK
        {
            // Về màn hình Chosing_mode
            application().gotoChosing_modeScreenWipeTransitionEast();
            lastPressTime = currentTime;
        }

        lastState = currentState;

        if (isGameOver())
        {
            navigateToGameOverScreen();
        }
    }
}

