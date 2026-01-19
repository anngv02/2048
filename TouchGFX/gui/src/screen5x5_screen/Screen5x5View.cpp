#include <gui/screen5x5_screen/Screen5x5View.hpp>
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

#define TILE_SIZE 48
static uint32_t seed = 1;

uint32_t Screen5x5View::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}

Screen5x5View::Screen5x5View()
    : dragStartX(0), dragStartY(0), dragEndX(0), dragEndY(0),
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
    // Set game mode hiện tại
    GameGlobal::currentGameMode = GAME_MODE_5X5;

    // Reset engine và set best score
    engine.reset();
    engine.setBestScore(GameGlobal::bestScore5x5);
    
    const int tileOffsetY = 80;
    
    // Khởi tạo UI tiles
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

    // Spawn 2 tiles ban đầu
    engine.setValue(0, 0, 2);
    engine.setValue(0, 1, 2);
    
    syncEngineToUI();
    updateScoreText();
    
    Screen5x5ViewBase::setupScreen();
}

void Screen5x5View::tearDownScreen()
{
    Screen5x5ViewBase::tearDownScreen();
}

// ==============================================================================
// ENGINE -> UI SYNC
// ==============================================================================

void Screen5x5View::syncEngineToUI()
{
    for (int r = 0; r < 5; r++)
    {
        for (int c = 0; c < 5; c++)
        {
            tiles[r][c]->setValue(engine.grid[r][c]);
        }
    }
}

// ==============================================================================
// TOUCH/GESTURE HANDLERS
// ==============================================================================

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

    engine.saveGridState();
    
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
        if (deltaX > 0) engine.moveRight();
        else            engine.moveLeft();
    }
    else 
    {
        if (deltaY > 0) engine.moveDown();
        else            engine.moveUp();
    }

    isDragging = false;
    processAfterMove();
}

void Screen5x5View::handleKeyEvent(uint8_t key)
{   
    engine.saveGridState();
    switch (key)
    {
    case '4': engine.moveLeft();  break;
    case '6': engine.moveRight(); break;
    case '8': engine.moveUp();    break;
    case '2': engine.moveDown();  break;
    default: return;
    }
    processAfterMove();
}

void Screen5x5View::handleTickEvent()
{
    // GPIO polling được xử lý trong Model::tick()
}

// ==============================================================================
// SCORE & UI UPDATE
// ==============================================================================

void Screen5x5View::updateScoreText()
{   
    GameGlobal::yourScore = engine.score;
    GameGlobal::bestScore5x5 = engine.bestScore;
    scoreContainer.setScore(engine.score);
    bestContainer.setScore(engine.bestScore);
}

// ==============================================================================
// GAME STATE METHODS
// ==============================================================================

void Screen5x5View::spawnRandomTile()
{
    struct Pos { int row, col; };
    Pos empties[25];
    int emptyCount = 0;

    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 5; c++) {
            if (engine.grid[r][c] == 0) {
                empties[emptyCount].row = r;
                empties[emptyCount].col = c;
                emptyCount++;
            }
        }
    }

    if (emptyCount > 0) {
        int idx = myRand() % emptyCount;
        int rr = empties[idx].row;
        int cc = empties[idx].col;

        uint16_t newValue = (myRand() % 10 == 0) ? 4 : 2;
        engine.setValue(rr, cc, newValue);
        
        syncEngineToUI();
        tiles[rr][cc]->animateSpawn();
    }
    else {
        if (engine.isGameOver()) {
            navigateToGameOverScreen();
        }
    }
}

void Screen5x5View::navigateToGameOverScreen()
{
    presenter->notifyGameOver();
    static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}

bool Screen5x5View::isGameOver()
{
    return engine.isGameOver();
}

// ==============================================================================
// PUBLIC METHODS - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

void Screen5x5View::onMoveUp()
{
    engine.saveGridState();
    engine.moveUp();
    processAfterMove();
}

void Screen5x5View::onMoveDown()
{
    engine.saveGridState();
    engine.moveDown();
    processAfterMove();
}

void Screen5x5View::onMoveLeft()
{
    engine.saveGridState();
    engine.moveLeft();
    processAfterMove();
}

void Screen5x5View::onMoveRight()
{
    engine.saveGridState();
    engine.moveRight();
    processAfterMove();
}

void Screen5x5View::onNavigateBack()
{
    application().gotoSelectedGameDesignScreenCoverTransitionEast();
}

void Screen5x5View::processAfterMove()
{
    syncEngineToUI();
    updateScoreText();
    
    if (engine.hasGridChanged()) {
        spawnRandomTile();
    }
    if (engine.isGameOver()) {
        navigateToGameOverScreen();
    }
}
