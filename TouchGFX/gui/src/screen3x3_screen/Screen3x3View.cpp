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
// ==============================================================================

#define TILE_SIZE 80
static uint32_t seed = 1;

uint32_t Screen3x3View::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}

Screen3x3View::Screen3x3View()
    : dragStartX(0), dragStartY(0), dragEndX(0), dragEndY(0),
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
    // Set game mode hiện tại
    GameGlobal::currentGameMode = GAME_MODE_3X3;

    // Reset engine và set best score
    engine.reset();
    engine.setBestScore(GameGlobal::bestScore3x3);
    
    // Khởi tạo UI tiles
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

    // Spawn 2 tiles ban đầu
    engine.setValue(0, 0, 2);
    engine.setValue(0, 1, 2);
    
    syncEngineToUI();
    updateScoreText();
    
    Screen3x3ViewBase::setupScreen();
}

void Screen3x3View::tearDownScreen()
{
    Screen3x3ViewBase::tearDownScreen();
}

// ==============================================================================
// ENGINE -> UI SYNC
// ==============================================================================

void Screen3x3View::syncEngineToUI()
{
    for (int r = 0; r < 3; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            tiles[r][c]->setValue(engine.grid[r][c]);
        }
    }
}

// ==============================================================================
// TOUCH/GESTURE HANDLERS
// ==============================================================================

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

void Screen3x3View::handleKeyEvent(uint8_t key)
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

void Screen3x3View::handleTickEvent()
{
    // GPIO polling được xử lý trong Model::tick()
}

// ==============================================================================
// SCORE & UI UPDATE
// ==============================================================================

void Screen3x3View::updateScoreText()
{   
    GameGlobal::yourScore = engine.score;
    GameGlobal::bestScore3x3 = engine.bestScore;
    scoreContainer.setScore(engine.score);
    bestContainer.setScore(engine.bestScore);
}

// ==============================================================================
// GAME STATE METHODS
// ==============================================================================

void Screen3x3View::spawnRandomTile()
{
    struct Pos { int row, col; };
    Pos empties[9];
    int emptyCount = 0;

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
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

void Screen3x3View::navigateToGameOverScreen()
{
    presenter->notifyGameOver();
    static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}

bool Screen3x3View::isGameOver()
{
    return engine.isGameOver();
}

// ==============================================================================
// PUBLIC METHODS - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

void Screen3x3View::onMoveUp()
{
    engine.saveGridState();
    engine.moveUp();
    processAfterMove();
}

void Screen3x3View::onMoveDown()
{
    engine.saveGridState();
    engine.moveDown();
    processAfterMove();
}

void Screen3x3View::onMoveLeft()
{
    engine.saveGridState();
    engine.moveLeft();
    processAfterMove();
}

void Screen3x3View::onMoveRight()
{
    engine.saveGridState();
    engine.moveRight();
    processAfterMove();
}

void Screen3x3View::onNavigateBack()
{
    application().gotoSelectedGameDesignScreenCoverTransitionEast();
}

void Screen3x3View::processAfterMove()
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
