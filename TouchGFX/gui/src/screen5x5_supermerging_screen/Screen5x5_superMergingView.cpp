#include <gui/screen5x5_supermerging_screen/Screen5x5_superMergingView.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <gui/common/FrontendApplication.hpp>
#include <vector>
#include <gui/common/GameGlobal.hpp>
#include <algorithm>

// ==============================================================================
// LƯU Ý: View KHÔNG truy cập GPIO trực tiếp - tuân thủ MVP pattern
// ==============================================================================

#define TILE_SIZE 48
static uint32_t seed = 1;

uint32_t Screen5x5_superMergingView::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}

Screen5x5_superMergingView::Screen5x5_superMergingView()
    : score(0), bestScore(0),
      dragStartX(0), dragStartY(0), dragEndX(0), dragEndY(0),
      isDragging(false)
{
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
}

void Screen5x5_superMergingView::setupScreen()
{
    // Set game mode hiện tại (để GameOver hiển thị đúng bestScore)
    GameGlobal::currentGameMode = GAME_MODE_5X5_SUPER;
    score = 0;
    bestScore = GameGlobal::bestScore5x5Super;
    const int tileOffsetY = 80;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
    updateScoreText();
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            tiles[i][j]->setValue(0);
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
            dragStartX = evt.getOldX();
            dragStartY = evt.getOldY();
            isDragging = true;
        }
        dragEndX = evt.getNewX();
        dragEndY = evt.getNewY();
    }
    
    Screen5x5_superMergingViewBase::handleDragEvent(evt);
}

void Screen5x5_superMergingView::handleGestureEvent(const GestureEvent &evt)
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
        if (deltaX > 0) moveRightSuperMerging();
        else            moveLeftSuperMerging();
    }
    else 
    {
        if (deltaY > 0) moveDownSuperMerging();
        else            moveUpSuperMerging();
    }

    isDragging = false;
    processAfterMove();
}

void Screen5x5_superMergingView::updateScoreText()
{
    GameGlobal::yourScore = score;
    GameGlobal::bestScore5x5Super = bestScore;
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
    case '4': moveLeftSuperMerging(); break;
    case '6': moveRightSuperMerging(); break;
    case '8': moveUpSuperMerging(); break;
    case '2': moveDownSuperMerging(); break;
    default: return;
    }
    processAfterMove();
}

void Screen5x5_superMergingView::spawnRandomTile()
{
    struct Pos { int row, col; };
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

    if (emptyCount > 0)
    {
        int idx = myRand() % emptyCount;
        int rr = empties[idx].row;
        int cc = empties[idx].col;

        uint16_t newValue = (myRand() % 10 == 0) ? 4 : 2;
        tiles[rr][cc]->setValue(newValue);
        tiles[rr][cc]->animateSpawn();
    }
    else
    {
        if (isGameOver())
        {
            navigateToGameOverScreen();
        }
    }
}

void Screen5x5_superMergingView::navigateToGameOverScreen()
{
    presenter->notifyGameOver();  // Buzzer beep 1 giây
    static_cast<FrontendApplication *>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}

bool Screen5x5_superMergingView::isGameOver()
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
    uint16_t values[5];
    int count = 0;

    for (int i = 0; i < 5; ++i)
    {
        if (line[i]->getValue() != 0)
        {
            values[count++] = line[i]->getValue();
        }
    }

    if (reverse)
    {
        for (int i = 0; i < count / 2; ++i)
            std::swap(values[i], values[count - 1 - i]);
    }

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

    if (reverse)
    {
        for (int i = 0; i < m / 2; ++i)
            std::swap(merged[i], merged[m - 1 - i]);
    }

    for (int i = 0; i < 5; ++i)
    {
        if (i < m)
            line[i]->setValue(merged[i]);
        else
            line[i]->setValue(0);
    }

    if (score > bestScore)
        bestScore = score;
    updateScoreText();
}

// ==============================================================================
// PUBLIC METHODS - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

void Screen5x5_superMergingView::onMoveUp()
{
    saveGridState();
    moveUpSuperMerging();
    processAfterMove();
}

void Screen5x5_superMergingView::onMoveDown()
{
    saveGridState();
    moveDownSuperMerging();
    processAfterMove();
}

void Screen5x5_superMergingView::onMoveLeft()
{
    saveGridState();
    moveLeftSuperMerging();
    processAfterMove();
}

void Screen5x5_superMergingView::onMoveRight()
{
    saveGridState();
    moveRightSuperMerging();
    processAfterMove();
}

void Screen5x5_superMergingView::onNavigateBack()
{
    application().gotoChosing_modeScreenWipeTransitionEast();
}

void Screen5x5_superMergingView::processAfterMove()
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
void Screen5x5_superMergingView::handleTickEvent()
{
    // GPIO polling được xử lý trong Model::tick()
}
