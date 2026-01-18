#include <gui/screen3x3_screen/Screen3x3View.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <touchgfx/Callback.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime> 
#include <gui/common/FrontendApplication.hpp>
#include <gui/common/GameGlobal.hpp>

// ==============================================================================
// LƯU Ý: View KHÔNG truy cập GPIO trực tiếp - tuân thủ MVP pattern
// GPIO polling được xử lý trong Model
// ==============================================================================

#define TILE_SIZE 80
static uint32_t seed = 1;

uint32_t Screen3x3View::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}

Screen3x3View::Screen3x3View()
    : score(0), bestScore(0),
      dragStartX(0), dragStartY(0), dragEndX(0), dragEndY(0),
      isDragging(false)
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
    // Set game mode hiện tại (để GameOver hiển thị đúng bestScore)
    GameGlobal::currentGameMode = GAME_MODE_3X3;

    score = 0;
    bestScore = GameGlobal::bestScore3x3;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
    updateScoreText();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            tiles[i][j]->setValue(0);
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

void Screen3x3View::handleDragEvent(const DragEvent& evt)
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
    
    Screen3x3ViewBase::handleDragEvent(evt);
}

void Screen3x3View::handleGestureEvent(const GestureEvent& evt)
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

void Screen3x3View::updateScoreText()
{   
    GameGlobal::yourScore = score;
    GameGlobal::bestScore3x3 = bestScore;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
}

void Screen3x3View::moveLeft()
{
    for (int row = 0; row < 3; ++row)
    {   
        int merged[3] = {0};

        for (int col = 1; col < 3; ++col)
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
                tiles[row][currentCol + 1]->setValue(tiles[row][currentCol]->getValue());
                tiles[row][currentCol]->setValue(0);
                currentCol++;
            }

            if (currentCol < 2 &&
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
                tiles[currentRow + 1][col]->setValue(tiles[currentRow][col]->getValue());
                tiles[currentRow][col]->setValue(0);
                currentRow++;
            }

            if (currentRow < 2 &&
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

void Screen3x3View::handleKeyEvent(uint8_t key)
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

void Screen3x3View::spawnRandomTile()
{
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

void Screen3x3View::navigateToGameOverScreen()
{
    presenter->notifyGameOver();  // Buzzer beep 1 giây
    static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}

bool Screen3x3View::isGameOver()
{
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            if (tiles[r][c]->getValue() == 0)
                return false;
        }
    }

    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            int current = tiles[r][c]->getValue();
            if (c < 2 && tiles[r][c + 1]->getValue() == current)
                return false;
            if (r < 2 && tiles[r + 1][c]->getValue() == current)
                return false;
        }
    }

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

// ==============================================================================
// PUBLIC METHODS - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

void Screen3x3View::onMoveUp()
{
    saveGridState();
    moveUp();
    processAfterMove();
}

void Screen3x3View::onMoveDown()
{
    saveGridState();
    moveDown();
    processAfterMove();
}

void Screen3x3View::onMoveLeft()
{
    saveGridState();
    moveLeft();
    processAfterMove();
}

void Screen3x3View::onMoveRight()
{
    saveGridState();
    moveRight();
    processAfterMove();
}

void Screen3x3View::onNavigateBack()
{
    application().gotoSelectedGameDesignScreenCoverTransitionEast();
}

void Screen3x3View::processAfterMove()
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
void Screen3x3View::handleTickEvent()
{
    // GPIO polling được xử lý trong Model::tick()
    // View nhận events thông qua Presenter (MVP pattern)
}
