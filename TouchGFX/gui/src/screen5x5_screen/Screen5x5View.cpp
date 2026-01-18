#include <gui/screen5x5_screen/Screen5x5View.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime> 
#include <gui/common/FrontendApplication.hpp>
#include <vector>
#include <gui/common/GameGlobal.hpp>

// ==============================================================================
// LƯU Ý: View KHÔNG truy cập GPIO trực tiếp - tuân thủ MVP pattern
// ==============================================================================

#define TILE_SIZE 48
static uint32_t seed = 1;

uint32_t Screen5x5View::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}

Screen5x5View::Screen5x5View()
    : score(0), bestScore(0),
      dragStartX(0), dragStartY(0), dragEndX(0), dragEndY(0),
      isDragging(false)
{
    tiles[0][0] = &tile5x51;
    tiles[0][1] = &tile5x52;
    tiles[0][2] = &tile5x53;
    tiles[0][3] = &tile5x54;
    tiles[0][4] = &tile5x55;

    tiles[1][0] = &tile5x56;
    tiles[1][1] = &tile5x57;
    tiles[1][2] = &tile5x58;
    tiles[1][3] = &tile5x59;
    tiles[1][4] = &tile5x510;

    tiles[2][0] = &tile5x511;
    tiles[2][1] = &tile5x512;
    tiles[2][2] = &tile5x513;
    tiles[2][3] = &tile5x514;
    tiles[2][4] = &tile5x515;

    tiles[3][0] = &tile5x516;
    tiles[3][1] = &tile5x517;
    tiles[3][2] = &tile5x518;
    tiles[3][3] = &tile5x519;
    tiles[3][4] = &tile5x520;

    tiles[4][0] = &tile5x521;
    tiles[4][1] = &tile5x522;
    tiles[4][2] = &tile5x523;
    tiles[4][3] = &tile5x524;
    tiles[4][4] = &tile5x525;
}

void Screen5x5View::setupScreen()
{   
    // Set game mode hiện tại (để GameOver hiển thị đúng bestScore)
    GameGlobal::currentGameMode = GAME_MODE_5X5;

    score = 0;
    bestScore = GameGlobal::bestScore5x5;
    const int tileOffsetY = 80;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
    updateScoreText();
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            tiles[i][j]->setValue(0);
            tiles[i][j]->moveTo((j) * TILE_SIZE, tileOffsetY + i * TILE_SIZE);
            tiles[i][j]->centerX = (j) * TILE_SIZE + TILE_SIZE / 2;
            tiles[i][j]->centerY = tileOffsetY + i * TILE_SIZE + TILE_SIZE / 2;
        }
    }

    tiles[0][0]->setValue(2);
    tiles[0][1]->setValue(2);
    Screen5x5ViewBase::setupScreen();
}

void Screen5x5View::tearDownScreen()
{
    Screen5x5ViewBase::tearDownScreen();
}

void Screen5x5View::handleDragEvent(const DragEvent& evt)
{
    if (evt.getType() == DragEvent::DRAGGED)
    {
        if (!isDragging)
        {
            dragStartX = evt.getOldX();
            dragStartY = evt.getOldY();
            isDragging = true;
        }
        dragEndX = evt.getNewX();
        dragEndY = evt.getNewY();
    }
    
    Screen5x5ViewBase::handleDragEvent(evt);
}

void Screen5x5View::handleGestureEvent(const GestureEvent& evt)
{   
    if (!isDragging) return;

    saveGridState();
    
    int16_t deltaX = dragEndX - dragStartX;
    int16_t deltaY = dragEndY - dragStartY;
    
    int16_t absX = abs(deltaX);
    int16_t absY = abs(deltaY);

    if (absX < MIN_SWIPE_DISTANCE && absY < MIN_SWIPE_DISTANCE) {
        isDragging = false;
        return;
    }

    if (absX > absY) 
    {
        if (deltaX > 0) moveRight();
        else            moveLeft();
    }
    else 
    {
        if (deltaY > 0) moveDown();
        else            moveUp();
    }

    isDragging = false;
    processAfterMove();
}

void Screen5x5View::updateScoreText()
{   
    GameGlobal::yourScore = score;
    GameGlobal::bestScore5x5 = bestScore;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
}

void Screen5x5View::moveLeft()
{
    for (int row = 0; row < 5; ++row)
    {
        int merged[5] = {0};

        for (int col = 1; col < 5; ++col)
        {
            if (tiles[row][col]->getValue() == 0) continue;

            int currentCol = col;
            while (currentCol > 0 &&
                   tiles[row][currentCol - 1]->getValue() == 0)
            {
                tiles[row][currentCol - 1]->setValue(tiles[row][currentCol]->getValue());
                tiles[row][currentCol]->setValue(0);
                currentCol--;
            }

            if (currentCol > 0 &&
                tiles[row][currentCol - 1]->getValue() == tiles[row][currentCol]->getValue() &&
                !merged[currentCol - 1])
            {   
                uint16_t newValue = tiles[row][currentCol - 1]->getValue() * 2;
                tiles[row][currentCol - 1]->setValue(newValue);
                tiles[row][currentCol]->setValue(0);
                merged[currentCol - 1] = 1;
                score += newValue;
                if (score > bestScore)
                    bestScore = score;
                updateScoreText();
            }
        }
    }
}

void Screen5x5View::moveRight()
{
    for (int row = 0; row < 5; ++row)
    {
        int merged[5] = {0};

        for (int col = 3; col >= 0; --col)
        {
            if (tiles[row][col]->getValue() == 0) continue;

            int currentCol = col;
            while (currentCol < 4 && tiles[row][currentCol + 1]->getValue() == 0)
            {
                tiles[row][currentCol + 1]->setValue(tiles[row][currentCol]->getValue());
                tiles[row][currentCol]->setValue(0);
                currentCol++;
            }

            if (currentCol < 4 &&
                tiles[row][currentCol + 1]->getValue() == tiles[row][currentCol]->getValue() &&
                !merged[currentCol + 1])
            {   
                uint16_t newValue = tiles[row][currentCol + 1]->getValue() * 2;
                tiles[row][currentCol + 1]->setValue(newValue);
                tiles[row][currentCol]->setValue(0);
                merged[currentCol + 1] = 1;
                score += newValue;
                if (score > bestScore)
                    bestScore = score;
                updateScoreText();
            }
        }
    }
}

void Screen5x5View::moveUp()
{
    for (int col = 0; col < 5; ++col)
    {
        int merged[5] = {0};

        for (int row = 1; row < 5; ++row)
        {
            if (tiles[row][col]->getValue() == 0) continue;

            int currentRow = row;
            while (currentRow > 0 && tiles[currentRow - 1][col]->getValue() == 0)
            {
                tiles[currentRow - 1][col]->setValue(tiles[currentRow][col]->getValue());
                tiles[currentRow][col]->setValue(0);
                currentRow--;
            }

            if (currentRow > 0 &&
                tiles[currentRow - 1][col]->getValue() == tiles[currentRow][col]->getValue() &&
                !merged[currentRow - 1])
            {   
                uint16_t newValue = tiles[currentRow - 1][col]->getValue() * 2;
                tiles[currentRow - 1][col]->setValue(newValue);
                tiles[currentRow][col]->setValue(0);
                merged[currentRow - 1] = 1;
                score += newValue;
                if (score > bestScore)
                    bestScore = score;
                updateScoreText();
            }
        }
    }
}

void Screen5x5View::moveDown()
{
    for (int col = 0; col < 5; ++col)
    {
        int merged[5] = {0};

        for (int row = 3; row >= 0; --row)
        {
            if (tiles[row][col]->getValue() == 0) continue;

            int currentRow = row;
            while (currentRow < 4 && tiles[currentRow + 1][col]->getValue() == 0)
            {
                tiles[currentRow + 1][col]->setValue(tiles[currentRow][col]->getValue());
                tiles[currentRow][col]->setValue(0);
                currentRow++;
            }

            if (currentRow < 4 &&
                tiles[currentRow + 1][col]->getValue() == tiles[currentRow][col]->getValue() &&
                !merged[currentRow + 1])
            {   
                uint16_t newValue = tiles[currentRow + 1][col]->getValue() * 2;
                tiles[currentRow + 1][col]->setValue(newValue);
                tiles[currentRow][col]->setValue(0);
                merged[currentRow + 1] = 1;
                score += newValue;
                if (score > bestScore)
                    bestScore = score;
                updateScoreText();
            }
        }
    }
}

void Screen5x5View::handleKeyEvent(uint8_t key)
{   
    saveGridState();
    switch (key)
    {
    case '4': moveLeft(); break;
    case '6': moveRight(); break;
    case '8': moveUp(); break;
    case '2': moveDown(); break;
    default: return;
    }
    processAfterMove();
}

void Screen5x5View::spawnRandomTile()
{
    struct Pos { int row, col; };
    Pos empties[25];
    int emptyCount = 0;

    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 5; c++) {
            if (tiles[r][c]->getValue() == 0) {
                empties[emptyCount++] = {r, c};
            }
        }
    }

    if (emptyCount > 0) {
        int idx = myRand() % emptyCount;
        int rr = empties[idx].row;
        int cc = empties[idx].col;

        uint16_t newValue = (myRand() % 10 == 0) ? 4 : 2;
        tiles[rr][cc]->setValue(newValue);
        tiles[rr][cc]->animateSpawn();
    }
    else {
        if (isGameOver()) {
            navigateToGameOverScreen();
        }
    }
}

void Screen5x5View::navigateToGameOverScreen()
{
    presenter->notifyGameOver();  // Buzzer beep 1 giây
    static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}

bool Screen5x5View::isGameOver()
{
    for (int r = 0; r < 5; ++r)
    {
        for (int c = 0; c < 5; ++c)
        {
            if (tiles[r][c]->getValue() == 0)
                return false;
        }
    }

    for (int r = 0; r < 5; ++r)
    {
        for (int c = 0; c < 5; ++c)
        {
            int current = tiles[r][c]->getValue();
            if (c < 4 && tiles[r][c + 1]->getValue() == current)
                return false;
            if (r < 4 && tiles[r + 1][c]->getValue() == current)
                return false;
        }
    }

    return true;
}

void Screen5x5View::saveGridState()
{
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j)
            gridBeforeMove[i][j] = tiles[i][j]->getValue();
}

bool Screen5x5View::hasGridChanged()
{
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j)
            if (gridBeforeMove[i][j] != tiles[i][j]->getValue())
                return true;
    return false;
}

// ==============================================================================
// PUBLIC METHODS - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

void Screen5x5View::onMoveUp()
{
    saveGridState();
    moveUp();
    processAfterMove();
}

void Screen5x5View::onMoveDown()
{
    saveGridState();
    moveDown();
    processAfterMove();
}

void Screen5x5View::onMoveLeft()
{
    saveGridState();
    moveLeft();
    processAfterMove();
}

void Screen5x5View::onMoveRight()
{
    saveGridState();
    moveRight();
    processAfterMove();
}

void Screen5x5View::onNavigateBack()
{
    application().gotoSelectedGameDesignScreenCoverTransitionEast();
}

void Screen5x5View::processAfterMove()
{
    if (hasGridChanged()) {
        spawnRandomTile();
    }
    if (isGameOver()) {
        navigateToGameOverScreen();
    }
}

/**
 * @brief handleTickEvent - GPIO polling đã được chuyển sang Model
 */
void Screen5x5View::handleTickEvent()
{
    // GPIO polling được xử lý trong Model::tick()
}
