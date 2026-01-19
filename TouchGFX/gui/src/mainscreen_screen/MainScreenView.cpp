#include <gui/mainscreen_screen/MainScreenView.hpp>
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
// GPIO polling được xử lý trong Model
// Chỉ sử dụng HAL_GetTick() để lấy seed cho random (không phải GPIO)
// ==============================================================================
extern "C" {
#include "stm32f4xx_hal.h"  // Chỉ dùng HAL_GetTick() cho random seed
}

#define TILE_SIZE 60
static uint32_t seed = 1;

/**
 * @brief Custom random number generator (LCG algorithm)
 * @return Số ngẫu nhiên 32-bit
 */
uint32_t MainScreenView::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}

/**
 * @brief Constructor - Khởi tạo mảng tiles và biến drag
 */
MainScreenView::MainScreenView()
    : dragStartX(0), dragStartY(0), dragEndX(0), dragEndY(0),
      isDragging(false)
{
    // Gán từng Tile từ Designer vào mảng 2D
    tiles[0][0] = &tile00;
    tiles[0][1] = &tile01;
    tiles[0][2] = &tile02;
    tiles[0][3] = &tile03;

    tiles[1][0] = &tile10;
    tiles[1][1] = &tile11;
    tiles[1][2] = &tile12;
    tiles[1][3] = &tile13;

    tiles[2][0] = &tile20;
    tiles[2][1] = &tile21;
    tiles[2][2] = &tile22;
    tiles[2][3] = &tile23;

    tiles[3][0] = &tile30;
    tiles[3][1] = &tile31;
    tiles[3][2] = &tile32;
    tiles[3][3] = &tile33;
}

/**
 * @brief Setup screen - khởi tạo game state
 */
void MainScreenView::setupScreen()
{   
    // Set game mode hiện tại (để GameOver hiển thị đúng bestScore)
    GameGlobal::currentGameMode = GAME_MODE_4X4;

    // Khởi tạo random seed từ system tick
    seed = HAL_GetTick();
    if (seed == 0) seed = 12345;
    
    // Reset engine và set best score
    engine.reset();
    engine.setBestScore(GameGlobal::bestScore4x4);
    
    const int tileOffsetY = 80;

    // Khởi tạo UI tiles
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            tiles[i][j]->setValue(0);
            tiles[i][j]->moveTo((j) * TILE_SIZE, tileOffsetY + i * TILE_SIZE);
            tiles[i][j]->centerX = (j) * TILE_SIZE + TILE_SIZE / 2;
            tiles[i][j]->centerY = tileOffsetY + i * TILE_SIZE + TILE_SIZE / 2;
        }
    }

    // Spawn 2 tiles ban đầu vào engine
    engine.setValue(0, 0, 2);
    engine.setValue(0, 1, 2);
    
    // Sync engine -> UI
    syncEngineToUI();
    updateScoreText();

    MainScreenViewBase::setupScreen();
}

void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
}

// ==============================================================================
// ENGINE <-> UI SYNC METHODS
// ==============================================================================

/**
 * @brief Đồng bộ dữ liệu từ engine sang UI tiles
 */
void MainScreenView::syncEngineToUI()
{
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            tiles[r][c]->setValue(engine.grid[r][c]);
        }
    }
}

/**
 * @brief Đồng bộ dữ liệu từ UI tiles sang engine (nếu cần)
 */
void MainScreenView::syncUIToEngine()
{
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            engine.grid[r][c] = tiles[r][c]->getValue();
        }
    }
}

// ==============================================================================
// TOUCH/GESTURE EVENT HANDLERS
// ==============================================================================

/**
 * @brief Xử lý sự kiện drag (kéo ngón tay trên màn hình)
 */
void MainScreenView::handleDragEvent(const DragEvent& evt)
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
    
    MainScreenViewBase::handleDragEvent(evt);
}

/**
 * @brief Xử lý sự kiện gesture (vuốt trên màn hình cảm ứng)
 */
void MainScreenView::handleGestureEvent(const GestureEvent& evt)
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

/**
 * @brief Xử lý phím từ keyboard (cho simulator/debug)
 */
void MainScreenView::handleKeyEvent(uint8_t key)
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

/**
 * @brief handleTickEvent - được gọi mỗi frame
 */
void MainScreenView::handleTickEvent()
{
    // GPIO polling được xử lý trong Model::tick()
    // View nhận events thông qua Presenter (MVP pattern)
}

// ==============================================================================
// PUBLIC METHODS - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

void MainScreenView::onMoveUp()
{
    engine.saveGridState();
    engine.moveUp();
    processAfterMove();
}

void MainScreenView::onMoveDown()
{
    engine.saveGridState();
    engine.moveDown();
    processAfterMove();
}

void MainScreenView::onMoveLeft()
{
    engine.saveGridState();
    engine.moveLeft();
    processAfterMove();
}

void MainScreenView::onMoveRight()
{
    engine.saveGridState();
    engine.moveRight();
    processAfterMove();
}

void MainScreenView::onNavigateBack()
{
    application().gotoSelectedGameDesignScreenCoverTransitionEast();
}

/**
 * @brief Xử lý chung sau khi move
 */
void MainScreenView::processAfterMove()
{
    // Sync engine -> UI
    syncEngineToUI();
    updateScoreText();
    
    if (engine.hasGridChanged()) {
        spawnRandomTile();
    }
    
    if (engine.isGameOver()) {
        navigateToGameOverScreen();
    }
}

// ==============================================================================
// SCORE METHODS
// ==============================================================================

void MainScreenView::updateScoreText()
{   
    GameGlobal::yourScore = engine.score;
    GameGlobal::bestScore4x4 = engine.bestScore;
    scoreContainer.setScore(engine.score);
    bestContainer.setScore(engine.bestScore);
}

// ==============================================================================
// GAME STATE METHODS
// ==============================================================================

/**
 * @brief Spawn tile ngẫu nhiên vào ô trống
 */
void MainScreenView::spawnRandomTile()
{
    // Tìm vị trí trống để spawn
    int spawnRow = -1, spawnCol = -1;
    
    // Tìm tất cả ô trống
    struct Pos { int row, col; };
    Pos empties[16];
    int emptyCount = 0;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (engine.grid[r][c] == 0) {
                empties[emptyCount].row = r;
                empties[emptyCount].col = c;
                emptyCount++;
            }
        }
    }

    if (emptyCount > 0) {
        int idx = myRand() % emptyCount;
        spawnRow = empties[idx].row;
        spawnCol = empties[idx].col;

        // Spawn tile vào engine
        uint16_t newValue = (myRand() % 10 == 0) ? 4 : 2;
        engine.setValue(spawnRow, spawnCol, newValue);
        
        // Sync và animate
        syncEngineToUI();
        tiles[spawnRow][spawnCol]->animateSpawn();
    }
    else {
        if (engine.isGameOver()) {
            navigateToGameOverScreen();
        }
    }
}

/**
 * @brief Chuyển sang màn hình Game Over
 */
void MainScreenView::navigateToGameOverScreen()
{
    presenter->notifyGameOver();
    static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}

/**
 * @brief Kiểm tra game over
 */
bool MainScreenView::isGameOver()
{
    return engine.isGameOver();
}
