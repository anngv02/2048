#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <cstdio>  
#include <cstdlib>     // srand, rand - cho hàm abs()
#include <ctime> 
#include <gui/common/FrontendApplication.hpp>
#include <gui/common/GameGlobal.hpp>
#include <stm32f4xx_hal.h>
#define TILE_SIZE 60
static uint32_t seed = 1;

uint32_t MainScreenView::myRand()
{
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}
MainScreenView::MainScreenView()
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
    
    // Khởi tạo biến drag
    dragStartX = 0;
    dragStartY = 0;
    dragEndX = 0;
    dragEndY = 0;
    isDragging = false;
}

void MainScreenView::setupScreen()
{   
    // Khởi tạo random seed từ timer để mỗi lần chơi có sequence khác nhau
    seed = HAL_GetTick();
    
    score = 0;
    bestScore = GameGlobal::bestScore4x4;  // Dùng bestScore riêng cho màn 4x4
    const int tileOffsetY = 80;
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
    updateScoreText();
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            tiles[i][j]->setValue(0); // ẩn ban đầu
            tiles[i][j]->moveTo((j) * TILE_SIZE,tileOffsetY + i * TILE_SIZE);
            tiles[i][j]->centerX = (j) * TILE_SIZE + TILE_SIZE / 2;
            tiles[i][j]->centerY = tileOffsetY + i * TILE_SIZE + TILE_SIZE / 2;
        }
    }

    tiles[0][0]->setValue(2);
    tiles[0][1]->setValue(2);
    MainScreenViewBase::setupScreen();
}

void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
}

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
    else if (evt.getType() == DragEvent::DRAGGED_OUT)
    {
        // Reset khi kéo ra khỏi vùng
        isDragging = false;
    }
    
    MainScreenViewBase::handleDragEvent(evt);
}

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

    // 2. Thuật toán "Dominant Axis" (Trục chiếm ưu thế)
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
        if (deltaY > 0) moveUp();
        else            moveDown();
    }

    // 3. Reset trạng thái & Xử lý Game logic
    isDragging = false;

    if (hasGridChanged()) {
        spawnRandomTile();
    }
    
    if (isGameOver()) {
        navigateToGameOverScreen();
    }
}
//cap nhat diem
void MainScreenView::updateScoreText()
{   
    GameGlobal::yourScore = score;
    GameGlobal::bestScore4x4 = bestScore;  // Lưu bestScore riêng cho màn 4x4
    scoreContainer.setScore(score);
    bestContainer.setScore(bestScore);
}
//di chuyen trai - Thuật toán mới: xử lý tuần tự
void MainScreenView::moveLeft()
{
    for (int row = 0; row < 4; ++row)
    {
        // Tạo mảng tạm thời để lưu kết quả
        uint16_t newRow[4] = {0};
        int writePos = 0;
        int lastMergedPos = -1; // Vị trí tile cuối cùng đã merge
        
        // Duyệt từ trái sang phải (từ cột 0 đến 3)
        for (int col = 0; col < 4; ++col)
        {
            uint16_t value = tiles[row][col]->getValue();
            if (value == 0) continue; // Bỏ qua ô trống
            
            // Nếu có thể merge với tile cuối cùng trong newRow
            if (writePos > 0 && 
                newRow[writePos - 1] == value && 
                lastMergedPos != writePos - 1) // Chưa merge trong lượt này
            {
                // Merge: gấp đôi giá trị
                newRow[writePos - 1] = value * 2;
                lastMergedPos = writePos - 1;
                score += value * 2;
            }
            else
            {
                // Không merge được, thêm vào vị trí mới
                newRow[writePos++] = value;
            }
        }
        
        // Cập nhật lại tiles từ mảng tạm
        for (int col = 0; col < 4; ++col)
        {
            tiles[row][col]->setValue(newRow[col]);
        }
    }
    
    // Cập nhật điểm sau khi xử lý tất cả các hàng
    if (score > bestScore)
        bestScore = score;
    updateScoreText();
}
void MainScreenView::moveRight()
{
    for (int row = 0; row < 4; ++row)
    {
        // Tạo mảng tạm thời để lưu kết quả
        uint16_t newRow[4] = {0};
        int writePos = 0;
        int lastMergedPos = -1;
        
        // Duyệt từ phải sang trái (từ cột 3 về 0)
        for (int col = 3; col >= 0; --col)
        {
            uint16_t value = tiles[row][col]->getValue();
            if (value == 0) continue;
            
            // Nếu có thể merge với tile cuối cùng trong newRow
            if (writePos > 0 && 
                newRow[writePos - 1] == value && 
                lastMergedPos != writePos - 1)
            {
                // Merge
                newRow[writePos - 1] = value * 2;
                lastMergedPos = writePos - 1;
                score += value * 2;
            }
            else
            {
                // Không merge được, thêm vào vị trí mới
                newRow[writePos++] = value;
            }
        }
        
        // Đảo ngược mảng để đặt vào đúng vị trí (từ phải sang trái)
        for (int i = 0; i < writePos / 2; ++i)
        {
            uint16_t temp = newRow[i];
            newRow[i] = newRow[writePos - 1 - i];
            newRow[writePos - 1 - i] = temp;
        }
        
        // Cập nhật lại tiles từ mảng tạm
        for (int col = 0; col < 4; ++col)
        {
            tiles[row][col]->setValue(newRow[col]);
        }
    }
    
    if (score > bestScore)
        bestScore = score;
    updateScoreText();
}
void MainScreenView::moveUp()
{
    for (int col = 0; col < 4; ++col)
    {
        // Tạo mảng tạm thời để lưu kết quả
        uint16_t newCol[4] = {0};
        int writePos = 0;
        int lastMergedPos = -1;
        
        // Duyệt từ trên xuống dưới (từ hàng 0 đến 3)
        for (int row = 0; row < 4; ++row)
        {
            uint16_t value = tiles[row][col]->getValue();
            if (value == 0) continue;
            
            // Nếu có thể merge với tile cuối cùng trong newCol
            if (writePos > 0 && 
                newCol[writePos - 1] == value && 
                lastMergedPos != writePos - 1)
            {
                // Merge
                newCol[writePos - 1] = value * 2;
                lastMergedPos = writePos - 1;
                score += value * 2;
            }
            else
            {
                // Không merge được, thêm vào vị trí mới
                newCol[writePos++] = value;
            }
        }
        
        // Cập nhật lại tiles từ mảng tạm
        for (int row = 0; row < 4; ++row)
        {
            tiles[row][col]->setValue(newCol[row]);
        }
    }
    
    if (score > bestScore)
        bestScore = score;
    updateScoreText();
}
void MainScreenView::moveDown()
{
    for (int col = 0; col < 4; ++col)
    {
        // Tạo mảng tạm thời để lưu kết quả
        uint16_t newCol[4] = {0};
        int writePos = 0;
        int lastMergedPos = -1;
        
        // Duyệt từ dưới lên trên (từ hàng 3 về 0)
        for (int row = 3; row >= 0; --row)
        {
            uint16_t value = tiles[row][col]->getValue();
            if (value == 0) continue;
            
            // Nếu có thể merge với tile cuối cùng trong newCol
            if (writePos > 0 && 
                newCol[writePos - 1] == value && 
                lastMergedPos != writePos - 1)
            {
                // Merge
                newCol[writePos - 1] = value * 2;
                lastMergedPos = writePos - 1;
                score += value * 2;
            }
            else
            {
                // Không merge được, thêm vào vị trí mới
                newCol[writePos++] = value;
            }
        }
        
        // Đảo ngược mảng để đặt vào đúng vị trí (từ dưới lên trên)
        for (int i = 0; i < writePos / 2; ++i)
        {
            uint16_t temp = newCol[i];
            newCol[i] = newCol[writePos - 1 - i];
            newCol[writePos - 1 - i] = temp;
        }
        
        // Cập nhật lại tiles từ mảng tạm
        for (int row = 0; row < 4; ++row)
        {
            tiles[row][col]->setValue(newCol[row]);
        }
    }
    
    if (score > bestScore)
        bestScore = score;
    updateScoreText();
}

void MainScreenView::handleKeyEvent(uint8_t key)
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
void MainScreenView::navigateToGameOverScreen()
{
   static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}
bool MainScreenView::isGameOver() // kiem tra xem con co the gop cac o lai voi nhau khong
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
void MainScreenView::saveGridState()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            gridBeforeMove[i][j] = tiles[i][j]->getValue();
}
bool MainScreenView::hasGridChanged()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (gridBeforeMove[i][j] != tiles[i][j]->getValue())
                return true;
    return false;
}
void MainScreenView::handleTickEvent()
{
    static uint32_t lastPressTime = 0;
    const uint32_t debounceDelay = 200;
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
            saveGridState();  // Lưu state trước khi move
            moveUp();
            if (hasGridChanged()) {  // Chỉ spawn nếu có thay đổi
                spawnRandomTile();
            }
            if (isGameOver()) {
                navigateToGameOverScreen();
            }
            lastPressTime = currentTime;
        }
        else if ((currentState & 2) && !(lastState & 2)) // PB12: DOWN
        {
            saveGridState();
            moveDown();
            if (hasGridChanged()) {
                spawnRandomTile();
            }
            if (isGameOver()) {
                navigateToGameOverScreen();
            }
            lastPressTime = currentTime;
        }
        else if ((currentState & 4) && !(lastState & 4)) // PA2: LEFT
        {
            saveGridState();
            moveLeft();
            if (hasGridChanged()) {
                spawnRandomTile();
            }
            if (isGameOver()) {
                navigateToGameOverScreen();
            }
            lastPressTime = currentTime;
        }
        else if ((currentState & 8) && !(lastState & 8)) // PA6: RIGHT
        {
            saveGridState();
            moveRight();
            if (hasGridChanged()) {
                spawnRandomTile();
            }
            if (isGameOver()) {
                navigateToGameOverScreen();
            }
            lastPressTime = currentTime;
        }
        else if ((currentState & 16) && !(lastState & 16)) // PA0: BACK
        {
            // Về màn hình Chosing_mode
        	application().gotoSelectedGameDesignScreenCoverTransitionEast();
            lastPressTime = currentTime;
        }

        lastState = currentState;
    }
}
