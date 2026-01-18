#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <cstdio>  
#include <cstdlib>     // abs()
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
    : score(0), bestScore(0),
      dragStartX(0), dragStartY(0), dragEndX(0), dragEndY(0),
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

    // Khởi tạo random seed từ system tick để mỗi lần chơi có sequence khác nhau
    seed = HAL_GetTick();
    if (seed == 0) seed = 12345; // Fallback nếu tick = 0
    
    score = 0;
    bestScore = GameGlobal::bestScore4x4;  // Dùng bestScore riêng cho màn 4x4
    const int tileOffsetY = 80;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
    updateScoreText();

    // Khởi tạo tất cả tiles về 0 và đặt vị trí
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            tiles[i][j]->setValue(0); // ẩn ban đầu
            tiles[i][j]->moveTo((j) * TILE_SIZE, tileOffsetY + i * TILE_SIZE);
            tiles[i][j]->centerX = (j) * TILE_SIZE + TILE_SIZE / 2;
            tiles[i][j]->centerY = tileOffsetY + i * TILE_SIZE + TILE_SIZE / 2;
        }
    }

    // Spawn 2 tiles ban đầu
    tiles[0][0]->setValue(2);
    tiles[0][1]->setValue(2);

    MainScreenViewBase::setupScreen();
}

void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
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
            // Lưu điểm bắt đầu
            dragStartX = evt.getOldX();
            dragStartY = evt.getOldY();
            isDragging = true;
        }
        // Cập nhật điểm cuối liên tục
        dragEndX = evt.getNewX();
        dragEndY = evt.getNewY();
    }
    
    MainScreenViewBase::handleDragEvent(evt);
}

/**
 * @brief Xử lý sự kiện gesture (vuốt trên màn hình cảm ứng)
 * Touch gesture xử lý trực tiếp trong View (không qua Model)
 */
void MainScreenView::handleGestureEvent(const GestureEvent& evt)
{
    // Nếu chưa từng nhận Drag trước đó, bỏ qua
    if (!isDragging) return;

    saveGridState();
    
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

    // Nếu di chuyển ngang nhiều hơn dọc -> Là vuốt Ngang
    if (absX > absY) 
    {
        // Đây là vuốt NGANG
        if (deltaX > 0) moveRight();
        else            moveLeft();
    }
    else 
    {
        // Đây là vuốt DỌC
        // Lưu ý: Hệ tọa độ màn hình Y tăng dần xuống dưới
        // Sửa: Đảo ngược logic deltaY để khắc phục vấn đề vuốt xuống đi lên
    	if (deltaY > 0) moveDown();
    	else            moveUp();
    }

    // Reset trạng thái & Xử lý Game logic
    isDragging = false;
    processAfterMove();
}

/**
 * @brief Xử lý phím từ keyboard (cho simulator/debug)
 */
void MainScreenView::handleKeyEvent(uint8_t key)
{
    saveGridState();
    switch (key)
    {
    case '4':
        moveLeft();
        break;
    case '6':
        moveRight();
        break;
    case '8':
        moveUp();
        break;
    case '2':
        moveDown();
        break;
    default:
        return; // Không xử lý key khác
    }
    processAfterMove();
}

/**
 * @brief handleTickEvent - được gọi mỗi frame
 *
 * THAY ĐỔI MVP: Đã xóa logic GPIO polling trực tiếp
 * GPIO polling giờ được xử lý trong Model::tick()
 * View chỉ xử lý UI updates/animations nếu cần
 */
void MainScreenView::handleTickEvent()
{
    // ==============================================================================
    // LƯU Ý: KHÔNG CÒN GPIO POLLING Ở ĐÂY
    // GPIO polling được xử lý trong Model::tick()
    // View nhận events thông qua Presenter (MVP pattern)
    // ==============================================================================

    // Có thể thêm logic update animation ở đây nếu cần
}

// ==============================================================================
// PUBLIC METHODS - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

/**
 * @brief Xử lý khi Presenter báo nhấn nút UP
 */
void MainScreenView::onMoveUp()
{
    saveGridState();
    moveUp();
    processAfterMove();
}

/**
 * @brief Xử lý khi Presenter báo nhấn nút DOWN
 */
void MainScreenView::onMoveDown()
{
    saveGridState();
    moveDown();
    processAfterMove();
}

/**
 * @brief Xử lý khi Presenter báo nhấn nút LEFT
 */
void MainScreenView::onMoveLeft()
{
    saveGridState();
    moveLeft();
    processAfterMove();
}

/**
 * @brief Xử lý khi Presenter báo nhấn nút RIGHT
 */
void MainScreenView::onMoveRight()
{
    saveGridState();
    moveRight();
    processAfterMove();
}

/**
 * @brief Xử lý khi Presenter báo nhấn nút BACK
 */
void MainScreenView::onNavigateBack()
{
    application().gotoSelectedGameDesignScreenCoverTransitionEast();
}

/**
 * @brief Xử lý chung sau khi move (spawn tile, check game over)
 * Hàm private để tái sử dụng code
 */
void MainScreenView::processAfterMove()
{
    if (hasGridChanged()) {
        spawnRandomTile();
    }
    
    if (isGameOver()) {
        navigateToGameOverScreen();
    }
}

// ==============================================================================
// SCORE METHODS
// ==============================================================================

/**
 * @brief Cập nhật hiển thị điểm số
 */
void MainScreenView::updateScoreText()
{   
    GameGlobal::yourScore = score;
    GameGlobal::bestScore4x4 = bestScore;  // Lưu bestScore riêng cho màn 4x4
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
}

// ==============================================================================
// MOVE METHODS
// ==============================================================================

/**
 * @brief Di chuyển tiles sang trái
 */
void MainScreenView::moveLeft()
{
    for (int row = 0; row < 4; ++row)
    {   
        int merged[4] = {0}; // theo dõi các tile đã merge

        for (int col = 1; col < 4; ++col)
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

            // Nếu có thể gộp
            if (currentCol > 0 &&
                tiles[row][currentCol - 1]->getValue() == tiles[row][currentCol]->getValue() &&
                !merged[currentCol - 1])
            {   
                uint16_t newValue = tiles[row][currentCol - 1]->getValue() * 2;
                tiles[row][currentCol - 1]->setValue(newValue);
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

/**
 * @brief Di chuyển tiles sang phải
 */
void MainScreenView::moveRight()
{
    for (int row = 0; row < 4; ++row)
    {
        int merged[4] = {0};

        for (int col = 2; col >= 0; --col)
        {
            if (tiles[row][col]->getValue() == 0) continue;

            int currentCol = col;
            while (currentCol < 3 && tiles[row][currentCol + 1]->getValue() == 0)
            {   
                tiles[row][currentCol + 1]->setValue(tiles[row][currentCol]->getValue());
                tiles[row][currentCol]->setValue(0);
                currentCol++;
            }

            if (currentCol < 3 &&
                tiles[row][currentCol + 1]->getValue() == tiles[row][currentCol]->getValue() &&
                !merged[currentCol + 1])
            {   
                uint16_t newValue = tiles[row][currentCol + 1]->getValue() * 2;
                tiles[row][currentCol + 1]->setValue(newValue);
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

/**
 * @brief Di chuyển tiles lên trên
 */
void MainScreenView::moveUp()
{
    for (int col = 0; col < 4; ++col)
    {
        int merged[4] = {0};

        for (int row = 1; row < 4; ++row)
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

                // Cộng điểm
                score += newValue;
                if (score > bestScore)
                    bestScore = score;
                updateScoreText();
            }
        }
    }
}

/**
 * @brief Di chuyển tiles xuống dưới
 */
void MainScreenView::moveDown()
{
    for (int col = 0; col < 4; ++col)
    {
        int merged[4] = {0};

        for (int row = 2; row >= 0; --row)
        {
            if (tiles[row][col]->getValue() == 0) continue;

            int currentRow = row;
            while (currentRow < 3 && tiles[currentRow + 1][col]->getValue() == 0)
            {   
                tiles[currentRow + 1][col]->setValue(tiles[currentRow][col]->getValue());
                tiles[currentRow][col]->setValue(0);
                currentRow++;
            }

            if (currentRow < 3 &&
                tiles[currentRow + 1][col]->getValue() == tiles[currentRow][col]->getValue() &&
                !merged[currentRow + 1])
            {   
                uint16_t newValue = tiles[currentRow + 1][col]->getValue() * 2;
                tiles[currentRow + 1][col]->setValue(newValue);
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

// ==============================================================================
// GAME STATE METHODS
// ==============================================================================

/**
 * @brief Spawn tile ngẫu nhiên vào ô trống
 */
void MainScreenView::spawnRandomTile()
{
    // 1) Tạo danh sách các ô còn trống
    struct Pos { int row, col; };
    Pos empties[16];
    int emptyCount = 0;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
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

        // 3) Đặt giá trị 2 hoặc 4 (10% chance là 4)
        uint16_t newValue = (myRand() % 10 == 0) ? 4 : 2;
        tiles[rr][cc]->setValue(newValue);
        tiles[rr][cc]->animateSpawn(); // animation spawn
    }
    else {
        // KHÔNG còn ô trống - kiểm tra game over
        if (isGameOver()) {
            navigateToGameOverScreen();
        }
    }
}

/**
 * @brief Chuyển sang màn hình Game Over
 * Kích hoạt buzzer beep 1 giây trước khi chuyển màn hình
 */
void MainScreenView::navigateToGameOverScreen()
{
    presenter->notifyGameOver();  // Buzzer beep 1 giây
    static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}

/**
 * @brief Kiểm tra game over (không còn nước đi hợp lệ)
 * @return true nếu game over, false nếu còn nước đi
 */
bool MainScreenView::isGameOver()
{
    // 1. Kiểm tra còn ô trống không
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (tiles[r][c]->getValue() == 0)
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

/**
 * @brief Lưu trạng thái grid hiện tại (trước khi move)
 */
void MainScreenView::saveGridState()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            gridBeforeMove[i][j] = tiles[i][j]->getValue();
}

/**
 * @brief Kiểm tra grid có thay đổi sau khi move không
 * @return true nếu có thay đổi, false nếu không
 */
bool MainScreenView::hasGridChanged()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (gridBeforeMove[i][j] != tiles[i][j]->getValue())
                return true;
    return false;
}
