#include <gui/screen5x5_lettermerging_screen/Screen5x5_letterMergingView.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime> 
#include <gui/common/FrontendApplication.hpp>
#include <gui/common/GameGlobal.hpp>

// ==============================================================================
// LƯU Ý: View KHÔNG truy cập GPIO trực tiếp - tuân thủ MVP pattern
// ==============================================================================

#define TILE_SIZE 60
static uint32_t seed = 1;

uint32_t Screen5x5_letterMergingView::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}

Screen5x5_letterMergingView::Screen5x5_letterMergingView()
    : score(0), bestScore(0),
      dragStartX(0), dragStartY(0), dragEndX(0), dragEndY(0),
      isDragging(false)
{
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
    // Set game mode hiện tại (để GameOver hiển thị đúng bestScore)
    GameGlobal::currentGameMode = GAME_MODE_5X5_LETTER;
    score = 0;
    bestScore = GameGlobal::bestScore5x5Letter;
    const int tileOffsetY = 80;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
    updateScoreText();
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            tiles[i][j]->setValue(0xFFFF); // ẩn ban đầu
            tiles[i][j]->moveTo((j) * TILE_SIZE, tileOffsetY + i * TILE_SIZE);
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

void Screen5x5_letterMergingView::handleDragEvent(const DragEvent& evt)
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
    
    Screen5x5_letterMergingViewBase::handleDragEvent(evt);
}

void Screen5x5_letterMergingView::handleGestureEvent(const GestureEvent& evt)
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

void Screen5x5_letterMergingView::updateScoreText()
{   
    GameGlobal::yourScore = score;
    GameGlobal::bestScore5x5Letter = bestScore;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
}

void Screen5x5_letterMergingView::moveLeft()
{
    for (int row = 0; row < 4; ++row)
    {
        int merged[4] = {0};

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

            if (currentCol > 0 &&
                tiles[row][currentCol - 1]->getValue() == tiles[row][currentCol]->getValue() &&
                !merged[currentCol - 1])
            {   
                uint16_t newValue = tiles[row][currentCol - 1]->getValue() + 1;
                tiles[row][currentCol - 1]->setValue(newValue);
                tiles[row][currentCol]->setValue(0xFFFF);
                merged[currentCol - 1] = 1;
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
                tiles[row][currentCol + 1]->setValue(newValue);
                tiles[row][currentCol]->setValue(0xFFFF);
                merged[currentCol + 1] = 1;
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
                tiles[currentRow - 1][col]->setValue(newValue);
                tiles[currentRow][col]->setValue(0xFFFF);
                merged[currentRow - 1] = 1;
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
                tiles[currentRow + 1][col]->setValue(newValue);
                tiles[currentRow][col]->setValue(0xFFFF);
                merged[currentRow + 1] = 1;
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
    case '4': moveLeft(); break;
    case '6': moveRight(); break;
    case '8': moveUp(); break;
    case '2': moveDown(); break;
    default: return;
    }
    processAfterMove();
}

void Screen5x5_letterMergingView::spawnRandomTile()
{
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

    if (emptyCount > 0) {
        int idx = myRand() % emptyCount;
        int rr = empties[idx].row;
        int cc = empties[idx].col;
        
        tiles[rr][cc]->setValue(0);
        tiles[rr][cc]->animateSpawn();
    }
    else {
        if (isGameOver()) {
            navigateToGameOverScreen();
        }
    }
}

void Screen5x5_letterMergingView::navigateToGameOverScreen()
{
    presenter->notifyGameOver();  // Buzzer beep 1 giây
    static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}

bool Screen5x5_letterMergingView::isGameOver()
{
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (tiles[r][c]->getValue() == 0xFFFF)
                return false;
        }
    }

    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            int current = tiles[r][c]->getValue();
            if (c < 3 && tiles[r][c + 1]->getValue() == current)
                return false;
            if (r < 3 && tiles[r + 1][c]->getValue() == current)
                return false;
        }
    }

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

// ==============================================================================
// PUBLIC METHODS - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

void Screen5x5_letterMergingView::onMoveUp()
{
    saveGridState();
    moveUp();
    processAfterMove();
}

void Screen5x5_letterMergingView::onMoveDown()
{
    saveGridState();
    moveDown();
    processAfterMove();
}

void Screen5x5_letterMergingView::onMoveLeft()
{
    saveGridState();
    moveLeft();
    processAfterMove();
}

void Screen5x5_letterMergingView::onMoveRight()
{
    saveGridState();
    moveRight();
    processAfterMove();
}

void Screen5x5_letterMergingView::onNavigateBack()
{
    application().gotoChosing_modeScreenWipeTransitionEast();
}

void Screen5x5_letterMergingView::processAfterMove()
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
void Screen5x5_letterMergingView::handleTickEvent()
{
    // GPIO polling được xử lý trong Model::tick()
}
