#include <gui/screen3x3_screen/Screen3x3View.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/Callback.hpp>
#include <cstdio>  // hoặc stdio.h nếu bạn thích C-style
#include <cstdlib>     // srand, rand
#include <ctime> 
#include <gui/common/FrontendApplication.hpp>
#include <gui/common/GameGlobal.hpp>
#include <stm32f4xx_hal.h>
#define TILE_SIZE 80
static uint32_t seed = 1;

uint32_t Screen3x3View::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}
Screen3x3View::Screen3x3View()
{
    tiles[0][0] = &tile3x31;
    tiles[0][1] = &tile3x32;
    tiles[0][2] = &tile3x33;

    tiles[1][0] = &tile3x34;
    tiles[1][1] = &tile3x36;
    tiles[1][2] = &tile3x38;

    tiles[2][0] = &tile3x35;
    tiles[2][1] = &tile3x37;
    tiles[2][2] = &tile3x39;
}

void Screen3x3View::setupScreen()
{   
    score = 0;
    bestScore = GameGlobal::bestScore;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
    updateScoreText();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            tiles[i][j]->setValue(0); // ẩn ban đầu
            tiles[i][j]->moveTo((j) * TILE_SIZE, (i+1) * TILE_SIZE);
            tiles[i][j]->centerX = (j) * TILE_SIZE + TILE_SIZE / 2;
            tiles[i][j]->centerY = (i + 1) * TILE_SIZE + TILE_SIZE / 2;
        }
    }

    tiles[0][0]->setValue(2);
    tiles[0][1]->setValue(2);
    Screen3x3ViewBase::setupScreen();
}

void Screen3x3View::tearDownScreen()
{
    Screen3x3ViewBase::tearDownScreen();
}
void Screen3x3View::handleGestureEvent(const GestureEvent& evt)
{   
    saveGridState();
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
    if(hasGridChanged()){
        spawnRandomTile();
    }
     // Sau khi di chuyển + spawn → kiểm tra thua
    if (isGameOver())
    {
        navigateToGameOverScreen();
    }
}
//cap nhat diem
void Screen3x3View::updateScoreText()
{   
    GameGlobal::yourScore = score;
    GameGlobal::bestScore = bestScore;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
}
//di chuyen trai
void Screen3x3View::moveLeft()
{
    for (int row = 0; row < 3; ++row)
    {   
        int merged[3] = {0}; // theo dõi các tile đã merge

        for (int col = 1; col < 3; ++col)
        {   
            if (tiles[row][col]->getValue() == 0) continue;

            int currentCol = col;
            while (currentCol > 0 &&
                   tiles[row][currentCol - 1]->getValue() == 0)
            {   
                //moveTileAnimated(row, currentCol, row, currentCol - 1); // them doan nay
                tiles[row][currentCol - 1]->setValue(tiles[row][currentCol]->getValue());
                tiles[row][currentCol]->setValue(0);
                currentCol--;
            }

            // Nếu có thể gộp
            if (currentCol > 0 &&
                tiles[row][currentCol - 1]->getValue() == tiles[row][currentCol]->getValue() &&
                !merged[currentCol - 1])
            {   
                uint16_t newValue = tiles[row][currentCol - 1]->getValue() * 2;
                //moveTileAnimated(row, currentCol, row, currentCol - 1); // them doan nay
                tiles[row][currentCol - 1]->setValue(
                    newValue);
                tiles[row][currentCol]->setValue(0);
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
void Screen3x3View::moveRight()
{
    for (int row = 0; row < 3; ++row)
    {
        int merged[3] = {0};

        for (int col = 1; col >= 0; --col)
        {
            if (tiles[row][col]->getValue() == 0) continue;

            int currentCol = col;
            while (currentCol < 2 && tiles[row][currentCol + 1]->getValue() == 0)
            {   
                //moveTileAnimated(row, currentCol, row, currentCol + 1); // them doan nay
                tiles[row][currentCol + 1]->setValue(tiles[row][currentCol]->getValue());
                tiles[row][currentCol]->setValue(0);
                currentCol++;
            }

            if (currentCol < 2 &&
                tiles[row][currentCol + 1]->getValue() == tiles[row][currentCol]->getValue() &&
                !merged[currentCol + 1])
            {   
                uint16_t newValue = tiles[row][currentCol + 1]->getValue() * 2;
                //moveTileAnimated(row, currentCol, row, currentCol + 1); 
                tiles[row][currentCol + 1]->setValue(
                    newValue);
                tiles[row][currentCol]->setValue(0);
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
void Screen3x3View::moveUp()
{
    for (int col = 0; col < 3; ++col)
    {
        int merged[3] = {0};

        for (int row = 1; row < 3; ++row)
        {
            if (tiles[row][col]->getValue() == 0) continue;

            int currentRow = row;
            while (currentRow > 0 && tiles[currentRow - 1][col]->getValue() == 0)
            {   
                //moveTileAnimated(currentRow, col, currentRow - 1, col);// them dong nay
                tiles[currentRow - 1][col]->setValue(tiles[currentRow][col]->getValue());
                tiles[currentRow][col]->setValue(0);
                currentRow--;
            }

            if (currentRow > 0 &&
                tiles[currentRow - 1][col]->getValue() == tiles[currentRow][col]->getValue() &&
                !merged[currentRow - 1])
            {   
                //moveTileAnimated(currentRow, col, currentRow - 1, col);//them dong nay
                uint16_t newValue = tiles[currentRow - 1][col]->getValue() * 2;
                tiles[currentRow - 1][col]->setValue(
                    newValue);
                tiles[currentRow][col]->setValue(0);
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
void Screen3x3View::moveDown()
{
    for (int col = 0; col < 3; ++col)
    {
        int merged[3] = {0};

        for (int row = 1; row >= 0; --row)
        {
            if (tiles[row][col]->getValue() == 0) continue;

            int currentRow = row;
            while (currentRow < 2 && tiles[currentRow + 1][col]->getValue() == 0)
            {   
                //moveTileAnimated(currentRow, col, currentRow + 1, col);//them dong nay
                tiles[currentRow + 1][col]->setValue(tiles[currentRow][col]->getValue());
                tiles[currentRow][col]->setValue(0);
                currentRow++;
            }

            if (currentRow < 2 &&
                tiles[currentRow + 1][col]->getValue() == tiles[currentRow][col]->getValue() &&
                !merged[currentRow + 1])
            {   
                uint16_t newValue = tiles[currentRow + 1][col]->getValue() * 2;
                //moveTileAnimated(currentRow, col, currentRow + 1, col);//them dong nay
                tiles[currentRow + 1][col]->setValue(
                    newValue);
                tiles[currentRow][col]->setValue(0);
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

void Screen3x3View::handleKeyEvent(uint8_t key)
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
void Screen3x3View::spawnRandomTile()
{
    // 1) Tạo danh sách các ô còn trống
    struct Pos { int row, col; };
    Pos empties[9];
    int emptyCount = 0;

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (tiles[r][c]->getValue() == 0) {
                empties[emptyCount++] = {r, c};
            }
        }
    }

        // 2) Nếu có ô trống, chọn ngẫu nhiên một ô
    if (emptyCount > 0) {
        int idx = myRand() % emptyCount;
        int rr = empties[idx].row;
        int cc = empties[idx].col;

        uint16_t newValue = (myRand() % 10 == 0) ? 4 : 2;
        tiles[rr][cc]->setValue(newValue);
        tiles[rr][cc]->animateSpawn();//animation spawn
    }
    else {
        // KHÔNG còn ô trống kiem tra xem co the gop duoc khong neu khong thi chuyen sang Game Over
        if (isGameOver()) {
        navigateToGameOverScreen();
    }
    }
}
void Screen3x3View::navigateToGameOverScreen()
{
   static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}
bool Screen3x3View::isGameOver() // kiem tra xem con co the gop cac o lai voi nhau khong
{
    // 1. Kiểm tra còn ô trống không
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            if (tiles[r][c]->getValue() == 0)
                return false; // còn chỗ để spawn => chưa thua
        }
    }

    // 2. Kiểm tra còn ô nào có thể gộp không
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            int current = tiles[r][c]->getValue();

            // Kiểm tra phải
            if (c < 2 && tiles[r][c + 1]->getValue() == current)
                return false;

            // Kiểm tra dưới
            if (r < 2 && tiles[r + 1][c]->getValue() == current)
                return false;
        }
    }

    // Không còn nước đi hợp lệ
    return true;
}
void Screen3x3View::saveGridState()
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            gridBeforeMove[i][j] = tiles[i][j]->getValue();
}
bool Screen3x3View::hasGridChanged()
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (gridBeforeMove[i][j] != tiles[i][j]->getValue())
                return true;
    return false;
}
void Screen3x3View::handleTickEvent()
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
        	application().gotoSelectedGameDesignScreenCoverTransitionEast();
            lastPressTime = currentTime;
        }

        lastState = currentState;

        if (isGameOver())
        {
            navigateToGameOverScreen();
        }
    }
}


