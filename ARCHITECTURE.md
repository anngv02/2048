# Phân Tích Chi Tiết Kiến Trúc và Code - Game 2048 STM32F429

## Mục Lục
1. [Tổng Quan Kiến Trúc Hệ Thống](#1-tổng-quan-kiến-trúc-hệ-thống)
2. [Luồng Khởi Động Hệ Thống](#2-luồng-khởi-động-hệ-thống)
3. [Kiến Trúc TouchGFX và MVP Pattern](#3-kiến-trúc-touchgfx-và-mvp-pattern)
4. [Quản Lý Bộ Nhớ và Frame Buffer](#4-quản-lý-bộ-nhớ-và-frame-buffer)
5. [Xử Lý Input (Touch và GPIO)](#5-xử-lý-input-touch-và-gpio)
6. [Game Logic và Thuật Toán](#6-game-logic-và-thuật-toán)
7. [UI Components và Animation](#7-ui-components-và-animation)
8. [Đồng Bộ Hóa và RTOS](#8-đồng-bộ-hóa-và-rtos)
9. [Hardware Abstraction Layer](#9-hardware-abstraction-layer)

---

## 1. Tổng Quan Kiến Trúc Hệ Thống

### 1.1 Kiến Trúc Tổng Thể

Hệ thống được xây dựng theo kiến trúc phân lớp (layered architecture) với các thành phần chính:

```
┌─────────────────────────────────────────────────────────┐
│              Application Layer (Game Logic)            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐ │
│  │ MainScreen   │  │ Screen3x3     │  │ Screen5x5   │ │
│  │ Screen5x5SM │  │ Screen5x5LM   │  │ GameOver     │ │
│  └──────────────┘  └──────────────┘  └──────────────┘ │
└─────────────────────────────────────────────────────────┘
                        ↕ MVP Pattern
┌─────────────────────────────────────────────────────────┐
│              TouchGFX Framework Layer                   │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐ │
│  │ Application  │  │ HAL          │  │ Widgets       │ │
│  │ Model        │  │ Presenter    │  │ Containers    │ │
│  └──────────────┘  └──────────────┘  └──────────────┘ │
└─────────────────────────────────────────────────────────┘
                        ↕ HAL Interface
┌─────────────────────────────────────────────────────────┐
│              Hardware Abstraction Layer                 │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐ │
│  │ TouchGFXHAL  │  │ TouchCtrl     │  │ DMA2D        │ │
│  │ LTDC         │  │ GPIO          │  │ SDRAM        │ │
│  └──────────────┘  └──────────────┘  └──────────────┘ │
└─────────────────────────────────────────────────────────┘
                        ↕ STM32 HAL
┌─────────────────────────────────────────────────────────┐
│              Hardware Layer (STM32F429)                 │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐ │
│  │ LTDC         │  │ DMA2D         │  │ I2C3 (Touch) │ │
│  │ SDRAM        │  │ SPI5 (LCD)    │  │ GPIO Buttons │ │
│  └──────────────┘  └──────────────┘  └──────────────┘ │
└─────────────────────────────────────────────────────────┘
```

### 1.2 Các Thành Phần Chính

#### **Hardware Components:**
- **STM32F429ZIT6**: MCU ARM Cortex-M4F @ 180MHz
- **LTDC (LCD TFT Display Controller)**: Điều khiển hiển thị LCD
- **DMA2D**: Graphics accelerator cho copy/fill operations
- **SDRAM**: Frame buffer storage (8MB)
- **STMPE811**: Touch controller qua I2C3
- **ILI9341**: LCD driver qua SPI5
- **GPIO**: Buttons cho điều khiển game

#### **Software Components:**
- **FreeRTOS**: Real-time operating system (CMSIS-RTOS v2)
- **TouchGFX 4.25.0**: GUI framework
- **STM32 HAL**: Hardware abstraction library
- **Game Logic**: Custom implementation cho game 2048

---

## 2. Luồng Khởi Động Hệ Thống

### 2.1 Boot Sequence Chi Tiết

Luồng khởi động được thực hiện trong `main.c`:

```c
int main(void)
{
    // Bước 1: Khởi tạo HAL và System Clock
    HAL_Init();                    // Reset peripherals, init Systick
    SystemClock_Config();          // Cấu hình clock 180MHz
    
    // Bước 2: Khởi tạo Peripherals
    MX_GPIO_Init();                // GPIO pins (buttons, LCD control)
    MX_CRC_Init();                 // CRC peripheral
    MX_I2C3_Init();                // I2C3 cho touch controller
    MX_SPI5_Init();                // SPI5 cho LCD
    MX_FMC_Init();                 // FMC cho SDRAM
    MX_LTDC_Init();                // LTDC cho display
    MX_DMA2D_Init();               // DMA2D cho graphics
    
    // Bước 3: Khởi tạo TouchGFX (trước RTOS)
    MX_TouchGFX_Init();            // Khởi tạo TouchGFX framework
    MX_TouchGFX_PreOSInit();      // Pre-OS initialization
    
    // Bước 4: Khởi tạo RTOS Kernel
    osKernelInitialize();         // Khởi tạo FreeRTOS scheduler
    
    // Bước 5: Tạo Tasks
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, ...);
    GUI_TaskHandle = osThreadNew(TouchGFX_Task, NULL, ...);
    
    // Bước 6: Start Scheduler
    osKernelStart();               // Bắt đầu multitasking
    // Không bao giờ đến đây vì scheduler đã chiếm quyền điều khiển
}
```

### 2.2 System Clock Configuration

Hệ thống sử dụng clock 180MHz được cấu hình như sau:

```c
void SystemClock_Config(void)
{
    // HSE (External Oscillator) = 8MHz
    // PLL Configuration:
    //   PLLM = 8      → VCO input = 8MHz / 8 = 1MHz
    //   PLLN = 360    → VCO output = 1MHz * 360 = 360MHz
    //   PLLP = 2      → SYSCLK = 360MHz / 2 = 180MHz
    //   PLLQ = 4      → USB clock = 360MHz / 4 = 90MHz
    
    // Over-Drive Mode: Cho phép MCU chạy ở 180MHz với hiệu năng cao
    
    // Clock Distribution:
    //   SYSCLK = 180MHz
    //   HCLK = 180MHz (AHB)
    //   PCLK1 = 45MHz (APB1)
    //   PCLK2 = 90MHz (APB2)
}
```

**Lý do chọn 180MHz:**
- Đủ mạnh để xử lý TouchGFX GUI với frame rate tốt
- Hỗ trợ Over-Drive mode cho hiệu năng tối ưu
- Tương thích với các peripheral timings

### 2.3 TouchGFX Initialization Flow

```cpp
// TouchGFX/target/generated/TouchGFXConfiguration.cpp
void touchgfx_init()
{
    // 1. Đăng ký Bitmap Database
    Bitmap::registerBitmapDatabase(...);
    
    // 2. Đăng ký Text Database
    TypedText::registerTexts(&texts);
    
    // 3. Setup Font Provider
    FontManager::setFontProvider(&fontProvider);
    
    // 4. Khởi tạo FrontendHeap (memory cho UI objects)
    FrontendHeap& heap = FrontendHeap::getInstance();
    
    // 5. Khởi tạo HAL
    hal.initialize();
}

void TouchGFXHAL::initialize()
{
    // Gọi parent implementation
    TouchGFXGeneratedHAL::initialize();
    
    // Setup animation storage
    setAnimationStorage((void*)animationStorage);
}
```

### 2.4 RTOS Task Creation

Hệ thống tạo 2 tasks chính:

```c
// Task 1: Default Task (idle task)
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,        // 512 bytes
    .priority = osPriorityNormal,
};

// Task 2: GUI Task (TouchGFX main loop)
const osThreadAttr_t GUI_Task_attributes = {
    .name = "GUI_Task",
    .stack_size = 8192 * 4,        // 32KB - lớn vì TouchGFX cần nhiều stack
    .priority = osPriorityNormal,
};
```

**Lưu ý:** GUI_Task có stack size lớn (32KB) vì TouchGFX framework cần nhiều không gian stack cho:
- Widget rendering
- Event handling
- Animation processing
- Memory allocation

---

## 3. Kiến Trúc TouchGFX và MVP Pattern

### 3.1 MVP (Model-View-Presenter) Pattern

TouchGFX sử dụng MVP pattern để tách biệt logic và presentation:

```
┌─────────────┐         ┌─────────────┐         ┌─────────────┐
│    Model    │◄────────┤  Presenter  │────────►│    View     │
│             │         │             │         │             │
│ - Data      │         │ - Logic     │         │ - UI        │
│ - State     │         │ - Mediator  │         │ - Display   │
└─────────────┘         └─────────────┘         └─────────────┘
     ▲                          ▲                       ▲
     │                          │                       │
     └──────────────────────────┴───────────────────────┘
                    Application
```

#### **Model (`Model.cpp`):**
```cpp
class Model : public ModelListener
{
    // Model chứa dữ liệu và state của ứng dụng
    // Trong project này, Model khá đơn giản
    // Game state được quản lý trực tiếp trong View
};
```

#### **View (`MainScreenView`):**
```cpp
class MainScreenView : public MainScreenViewBase
{
protected:
    Tile* tiles[4][4];              // Mảng 2D các tile
    uint16_t gridBeforeMove[4][4];  // Lưu state trước khi move
    uint32_t score = 0;
    uint32_t bestScore = 0;
    
    // Game logic methods
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void spawnRandomTile();
    bool isGameOver();
};
```

**Đặc điểm:**
- View chứa cả UI và game logic (không phải MVP thuần túy)
- View tự quản lý state của game
- Presenter chỉ làm nhiệm vụ điều hướng giữa các screens

#### **Presenter (`MainScreenPresenter`):**
```cpp
class MainScreenPresenter : public touchgfx::Presenter, public ModelListener
{
    MainScreenView& view;
    
    void activate()   // Khi screen được kích hoạt
    void deactivate() // Khi screen bị deactivate
};
```

**Trong project này, Presenter khá đơn giản vì:**
- Game logic được implement trực tiếp trong View
- Model không chứa nhiều state phức tạp
- Presenter chủ yếu làm navigation

### 3.2 Application Flow

```cpp
// TouchGFX/gui/include/gui/common/FrontendApplication.hpp
class FrontendApplication : public FrontendApplicationBase
{
    // Application quản lý navigation giữa các screens
    // và lifecycle của ứng dụng
};

// FrontendApplication được khởi tạo trong FrontendHeap
FrontendHeap::FrontendHeap()
{
    // Tạo Model và Application instance
    Model m;
    FrontendApplication app(m, *this);
}
```

### 3.3 Screen Navigation

TouchGFX sử dụng transition system để chuyển đổi giữa các screens:

```cpp
// Ví dụ: Chuyển sang Game Over screen
void MainScreenView::navigateToGameOverScreen()
{
    static_cast<FrontendApplication*>(Application::getInstance())
        ->gotoGameOverScreenScreenSlideTransitionEast();
}
```

**Các loại transition:**
- `SlideTransitionEast/West/North/South`: Slide animation
- `CoverTransition`: Cover screen mới lên screen cũ
- `NoTransition`: Chuyển ngay lập tức

---

## 4. Quản Lý Bộ Nhớ và Frame Buffer

### 4.1 Frame Buffer Architecture

TouchGFX sử dụng double buffering để tránh flickering:

```cpp
// TouchGFX/target/generated/TouchGFXGeneratedHAL.cpp
namespace
{
    // Frame buffer được đặt trong section đặc biệt
    LOCATION_PRAGMA_NOLOAD("TouchGFX_Framebuffer")
    uint32_t frameBuf[(240 * 320 * 2 + 3) / 4 * 2] 
        LOCATION_ATTRIBUTE_NOLOAD("TouchGFX_Framebuffer");
    // Size: 240 * 320 * 2 bytes * 2 buffers = 307,200 bytes
}

void TouchGFXGeneratedHAL::initialize()
{
    // Setup 2 frame buffers (double buffering)
    setFrameBufferStartAddresses(
        (void*)frameBuf,                    // Buffer 1
        (void*)(frameBuf + sizeof(frameBuf) / (sizeof(uint32_t) * 2)), // Buffer 2
        (void*)0                            // Buffer 3 (không dùng)
    );
}
```

**Double Buffering Flow:**
```
┌──────────────┐         ┌──────────────┐
│ Frame Buffer │         │ Frame Buffer │
│     1        │         │     2        │
│  (Active)    │         │  (Rendering) │
└──────────────┘         └──────────────┘
      │                        │
      │                        │
      ▼                        ▼
   ┌──────────────────────────────┐
   │      LTDC Display             │
   │   (Hiển thị Buffer 1)        │
   └──────────────────────────────┘
```

### 4.2 Frame Buffer Location

Frame buffer được đặt trong SDRAM thông qua linker script:

```c
// Linker script định nghĩa section "TouchGFX_Framebuffer"
// Frame buffer được map vào SDRAM address space
// SDRAM: 0xD0000000 - 0xD07FFFFF (8MB)
```

**Lý do dùng SDRAM:**
- Frame buffer lớn (307KB) không thể fit trong internal SRAM
- SDRAM có bandwidth cao, phù hợp cho graphics
- LTDC có thể truy cập SDRAM trực tiếp qua FMC

### 4.3 VSYNC và Frame Synchronization

```cpp
// TouchGFX/target/TouchGFXHAL.cpp
void TouchGFXHAL::taskEntry()
{
    enableLCDControllerInterrupt();
    enableInterrupts();
    
    OSWrappers::waitForVSync();  // Đợi VSYNC đầu tiên
    backPorchExited();
    
    LCD_IO_WriteReg(0x29);       // LCD Display ON
    
    for (;;)
    {
        OSWrappers::waitForVSync();  // Đợi VSYNC
        backPorchExited();           // Signal rendering có thể bắt đầu
    }
}
```

**VSYNC Interrupt Handler:**
```cpp
// TouchGFX/target/generated/TouchGFXGeneratedHAL.cpp
void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef* hltdc)
{
    if (LTDC->LIPCR == lcd_int_active_line)
    {
        // Entering active area
        HAL_LTDC_ProgramLineEvent(hltdc, lcd_int_porch_line);
        HAL::getInstance()->vSync();
        OSWrappers::signalVSync();  // Signal GUI task
        
        // Swap frame buffers
        HAL::getInstance()->swapFrameBuffers();
        GPIO::set(GPIO::VSYNC_FREQ);  // Performance pin
    }
    else
    {
        // Exiting active area (back porch)
        HAL_LTDC_ProgramLineEvent(hltdc, lcd_int_active_line);
        HAL::getInstance()->frontPorchEntered();
        GPIO::clear(GPIO::VSYNC_FREQ);
    }
}
```

**Timing Diagram:**
```
VSYNC ──────────────────────────────────────┐
                                             │
Active Area ────────────┐                    │
                        │                    │
Back Porch ─────────────┴────────────────────┘
                        │
                        ▼
                  Swap Buffers
                  Render Next Frame
```

### 4.4 Memory Layout

```
Memory Map:
┌─────────────────────────────────────┐
│ Flash (0x08000000)                  │
│  - Code                              │
│  - Constants                         │
└─────────────────────────────────────┘
┌─────────────────────────────────────┐
│ SRAM (0x20000000)                   │
│  - Stack (defaultTask: 512B)        │
│  - Stack (GUI_Task: 32KB)          │
│  - Heap (FreeRTOS)                  │
│  - Global variables                  │
└─────────────────────────────────────┘
┌─────────────────────────────────────┐
│ SDRAM (0xD0000000)                  │
│  - Frame Buffer 1 (153,600 bytes)  │
│  - Frame Buffer 2 (153,600 bytes)  │
│  - Animation Storage (~76KB)        │
│  - Free space                       │
└─────────────────────────────────────┘
```

---

## 5. Xử Lý Input (Touch và GPIO)

### 5.1 Touch Controller (STMPE811)

Touch controller được điều khiển qua I2C3:

```cpp
// TouchGFX/target/STM32TouchController.cpp
void STM32TouchController::init()
{
    // Khởi tạo touch controller với kích thước màn hình
    BSP_TS_Init(240, 320);
}

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{
    TS_StateTypeDef state;
    BSP_TS_GetState(&state);
    
    if (state.TouchDetected)
    {
        x = state.X;
        y = state.Y;
        return true;
    }
    return false;
}
```

**Touch Coordinate Calibration:**
```cpp
void BSP_TS_GetState(TS_StateTypeDef* TsState)
{
    // Đọc raw coordinates từ STMPE811
    TsDrv->GetXY(TS_I2C_ADDRESS, &x, &y);
    
    // Calibration cho Rev D và các revision cũ khác nhau
    if (isRevD)
    {
        // Rev D có calibration khác
        y -= 180;
        y = 3520 - y;
    }
    else
    {
        y -= 360;
    }
    
    // Scale về kích thước màn hình (240x320)
    yr = y / 11;
    xr = x / 15;
    
    // Filter noise (chỉ update nếu thay đổi > 5 pixels)
    if (xDiff + yDiff > 5)
    {
        _x = x;
        _y = y;
    }
}
```

**Touch Sampling Rate:**
- TouchGFX mặc định sample touch mỗi tick (thường là 16ms)
- Có thể điều chỉnh bằng `HAL::setTouchSampleRate(int8_t)`

### 5.2 Gesture Detection

TouchGFX tự động detect gestures từ touch events:

```cpp
// TouchGFX/gui/src/mainscreen_screen/MainScreenView.cpp
void MainScreenView::handleGestureEvent(const GestureEvent& evt)
{
    if (evt.getType() == GestureEvent::SWIPE_HORIZONTAL)
    {
        if (evt.getVelocity() > 0)
            moveRight();  // Swipe right
        else
            moveLeft();   // Swipe left
    }
    else if (evt.getType() == GestureEvent::SWIPE_VERTICAL)
    {
        if (evt.getVelocity() > 0)
            moveDown();   // Swipe down
        else
            moveUp();     // Swipe up
    }
    
    // Spawn tile mới nếu grid thay đổi
    if (hasGridChanged())
        spawnRandomTile();
    
    // Kiểm tra game over
    if (isGameOver())
        navigateToGameOverScreen();
}
```

**Gesture Types:**
- `SWIPE_HORIZONTAL`: Vuốt ngang (trái/phải)
- `SWIPE_VERTICAL`: Vuốt dọc (lên/xuống)
- `DRAG`: Kéo
- `CLICK`: Click

### 5.3 GPIO Button Input

Ngoài touch, game còn hỗ trợ điều khiển bằng GPIO buttons:

```cpp
// TouchGFX/gui/src/mainscreen_screen/MainScreenView.cpp
void MainScreenView::handleTickEvent()
{
    static uint32_t lastPressTime = 0;
    const uint32_t debounceDelay = 200; // ms
    uint32_t currentTime = HAL_GetTick();
    
    // Đọc trạng thái các buttons
    uint8_t currentState = 0;
    currentState |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET ? 1 : 0);  // UP
    currentState |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_SET ? 2 : 0);  // DOWN
    currentState |= (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_SET ? 4 : 0);   // LEFT
    currentState |= (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET ? 8 : 0);   // RIGHT
    currentState |= (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET ? 16 : 0);  // BACK
    
    static uint8_t lastState = 0;
    
    // Debounce: Chỉ xử lý nếu đã qua debounce delay
    if (currentTime - lastPressTime > debounceDelay)
    {
        // Detect edge (từ LOW sang HIGH)
        if ((currentState & 1) && !(lastState & 1))      // UP
            moveUp();
        else if ((currentState & 2) && !(lastState & 2)) // DOWN
            moveDown();
        else if ((currentState & 4) && !(lastState & 4)) // LEFT
            moveLeft();
        else if ((currentState & 8) && !(lastState & 8)) // RIGHT
            moveRight();
        else if ((currentState & 16) && !(lastState & 16)) // BACK
            application().gotoSelectedGameDesignScreenCoverTransitionEast();
        
        lastState = currentState;
        lastPressTime = currentTime;
    }
}
```

**GPIO Pin Mapping:**
```
PB10 → UP button
PB12 → DOWN button
PA2  → LEFT button
PA6  → RIGHT button
PA0  → BACK button
```

**Debouncing:**
- Sử dụng software debouncing với delay 200ms
- Chỉ xử lý edge (LOW → HIGH) để tránh multiple triggers
- `handleTickEvent()` được gọi mỗi frame (~16ms)

---

## 6. Game Logic và Thuật Toán

### 6.1 Grid Representation

Game sử dụng mảng 2D để biểu diễn grid:

```cpp
class MainScreenView
{
protected:
    Tile* tiles[4][4];              // Mảng con trỏ đến các Tile widget
    uint16_t gridBeforeMove[4][4]; // Lưu state để detect changes
};
```

**Lưu ý:** `tiles` là mảng con trỏ, không phải mảng giá trị. Mỗi `Tile*` trỏ đến một widget UI.

### 6.2 Move Algorithm (moveLeft example)

```cpp
void MainScreenView::moveLeft()
{
    for (int row = 0; row < 4; ++row)
    {
        int merged[4] = {0}; // Track các tile đã merge
        
        // Xử lý từ trái sang phải (từ cột 1 đến 3)
        for (int col = 1; col < 4; ++col)
        {
            if (tiles[row][col]->getValue() == 0) 
                continue; // Bỏ qua ô trống
            
            int currentCol = col;
            
            // Bước 1: Di chuyển tile về phía trái
            while (currentCol > 0 && 
                   tiles[row][currentCol - 1]->getValue() == 0)
            {
                tiles[row][currentCol - 1]->setValue(
                    tiles[row][currentCol]->getValue());
                tiles[row][currentCol]->setValue(0);
                currentCol--;
            }
            
            // Bước 2: Merge nếu có thể
            if (currentCol > 0 &&
                tiles[row][currentCol - 1]->getValue() == 
                tiles[row][currentCol]->getValue() &&
                !merged[currentCol - 1])  // Chưa merge trong lượt này
            {
                uint16_t newValue = tiles[row][currentCol - 1]->getValue() * 2;
                tiles[row][currentCol - 1]->setValue(newValue);
                tiles[row][currentCol]->setValue(0);
                merged[currentCol - 1] = 1;  // Đánh dấu đã merge
                
                // Cập nhật điểm
                score += newValue;
                if (score > bestScore)
                    bestScore = score;
                updateScoreText();
            }
        }
    }
}
```

**Thuật toán hoạt động:**
1. **Slide Phase**: Di chuyển tất cả tiles về phía trái (hoặc hướng di chuyển)
2. **Merge Phase**: Gộp các tiles có cùng giá trị
3. **Score Update**: Cập nhật điểm khi merge

**Lưu ý quan trọng:**
- Mỗi tile chỉ merge một lần trong một lượt di chuyển
- Array `merged[]` đảm bảo không merge nhiều lần
- Thuật toán tương tự cho `moveRight()`, `moveUp()`, `moveDown()`

### 6.3 Spawn Random Tile

```cpp
void MainScreenView::spawnRandomTile()
{
    // Bước 1: Tìm tất cả ô trống
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
    
    // Bước 2: Chọn ngẫu nhiên một ô trống
    if (emptyCount > 0) {
        int idx = myRand() % emptyCount;
        int rr = empties[idx].row;
        int cc = empties[idx].col;
        
        // Bước 3: Spawn tile (90% là 2, 10% là 4)
        uint16_t newValue = (myRand() % 10 == 0) ? 4 : 2;
        tiles[rr][cc]->setValue(newValue);
        tiles[rr][cc]->animateSpawn();  // Animation
    }
}
```

**Random Number Generator:**
```cpp
static uint32_t seed = 1;

uint32_t MainScreenView::myRand()
{
    // Linear Congruential Generator (LCG)
    seed = seed * 1664525UL + 1013904223UL;
    return seed;
}
```

**Lý do dùng custom RNG:**
- `std::rand()` có thể không thread-safe trong RTOS
- Custom RNG đơn giản, nhanh, không cần seed từ time
- Đủ tốt cho game logic

### 6.4 Game Over Detection

```cpp
bool MainScreenView::isGameOver()
{
    // Bước 1: Kiểm tra còn ô trống không
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (tiles[r][c]->getValue() == 0)
                return false; // Còn chỗ spawn
        }
    }
    
    // Bước 2: Kiểm tra còn có thể merge không
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            int current = tiles[r][c]->getValue();
            
            // Kiểm tra tile bên phải
            if (c < 3 && tiles[r][c + 1]->getValue() == current)
                return false; // Có thể merge
            
            // Kiểm tra tile bên dưới
            if (r < 3 && tiles[r + 1][c]->getValue() == current)
                return false; // Có thể merge
        }
    }
    
    return true; // Không còn nước đi
}
```

**Thuật toán:**
1. Nếu còn ô trống → chưa game over
2. Nếu không còn ô trống, kiểm tra xem có 2 tiles liền kề nào có cùng giá trị không
3. Nếu không có → game over

### 6.5 Change Detection

Để tránh spawn tile khi không có thay đổi:

```cpp
void MainScreenView::saveGridState()
{
    // Lưu state trước khi move
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            gridBeforeMove[i][j] = tiles[i][j]->getValue();
}

bool MainScreenView::hasGridChanged()
{
    // So sánh state trước và sau move
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (gridBeforeMove[i][j] != tiles[i][j]->getValue())
                return true;
    return false;
}
```

**Flow:**
```
User Input → saveGridState() → move() → hasGridChanged()?
                                         ├─ true → spawnRandomTile()
                                         └─ false → skip spawn
```

---

## 7. UI Components và Animation

### 7.1 Tile Component

`Tile` là component cơ bản để hiển thị một ô số:

```cpp
// TouchGFX/gui/src/containers/Tile.cpp
class Tile : public TileBase
{
    void setValue(uint16_t value)
    {
        if (value == 0)
        {
            // Ô trống: ẩn text, set màu nhạt
            textArea1.setVisible(false);
            boxWithBorder1.setColor(Color::getColorFromRGB(238, 228, 218));
            boxWithBorder1.setAlpha(35);
        }
        else
        {
            // Ô có giá trị: hiển thị text, set màu theo giá trị
            Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%u", value);
            textArea1.setVisible(true);
            
            // Màu sắc theo giá trị
            switch (value)
            {
                case 2:   color = RGB(238, 228, 218); break;
                case 4:   color = RGB(237, 224, 200); break;
                case 8:   color = RGB(242, 177, 121); break;
                case 16:  color = RGB(245, 149, 99); break;
                case 32:  color = RGB(246, 124, 95); break;
                case 64:  color = RGB(246, 94, 59); break;
                case 128: color = RGB(237, 207, 114); break;
                case 256: color = RGB(237, 204, 97); break;
                case 512: color = RGB(237, 200, 80); break;
                default:  color = RGB(60, 58, 50); break; // >512
            }
            
            boxWithBorder1.setColor(color);
            boxWithBorder1.setAlpha(200);
        }
        
        // Invalidate để trigger redraw
        boxWithBorder1.invalidate();
        textArea1.invalidate();
    }
};
```

**Component Structure:**
```
Tile (Container)
├── boxWithBorder1 (Box widget - background)
└── textArea1 (TextArea widget - số)
```

### 7.2 Spawn Animation

Animation khi spawn tile mới:

```cpp
void Tile::animateSpawn()
{
    currentStep = 0;
    
    // Bắt đầu từ kích thước nhỏ (10x10)
    setWidthHeight(10, 10);
    moveTo(centerX - 5, centerY - 5);
    setVisible(true);
    
    // Đăng ký với Application để nhận tick events
    Application::getInstance()->registerTimerWidget(this);
}

void Tile::handleTickEvent()
{
    if (currentStep >= totalSteps)  // totalSteps = 6
    {
        // Hoàn tất: về kích thước đầy đủ (60x60)
        setWidthHeight(60, 60);
        moveTo(centerX - 30, centerY - 30);
        invalidate();
        
        // Hủy đăng ký timer
        Application::getInstance()->unregisterTimerWidget(this);
        return;
    }
    
    // Interpolate kích thước từ 10 đến 60
    int size = 10 + ((60 - 10) * currentStep) / totalSteps;
    int offset = size / 2;
    setWidthHeight(size, size);
    moveTo(centerX - offset, centerY - offset);
    invalidate();
    
    currentStep++;
}
```

**Animation Flow:**
```
Spawn → Register Timer → handleTickEvent() (6 lần)
                         ├─ Step 0: 10x10
                         ├─ Step 1: 20x20
                         ├─ Step 2: 30x30
                         ├─ Step 3: 40x40
                         ├─ Step 4: 50x50
                         └─ Step 5: 60x60 → Unregister
```

**Timing:**
- `handleTickEvent()` được gọi mỗi frame (~16ms)
- 6 steps × 16ms = ~96ms animation duration
- Linear interpolation cho smooth animation

### 7.3 Score Component

```cpp
// TouchGFX/gui/src/containers/Score.cpp
void Score::setScore(uint32_t score)
{
    // Format số thành string
    Unicode::snprintf(scoreBuffer, sizeof(scoreBuffer), "%u", score);
    
    // Invalidate để trigger redraw
    scoreText.invalidate();
}
```

**Score Display:**
- `Score` container hiển thị điểm hiện tại
- `Best` container hiển thị điểm cao nhất
- Cả hai đều dùng `TextArea` với wildcard buffer

### 7.4 Widget Invalidation và Redraw

TouchGFX sử dụng invalidation system để optimize redraw:

```cpp
// Khi thay đổi widget
widget.invalidate();

// TouchGFX sẽ:
// 1. Đánh dấu widget cần redraw
// 2. Tính toán dirty region
// 3. Chỉ redraw phần cần thiết trong frame tiếp theo
```

**Invalidation Flow:**
```
Widget Change → invalidate() → Mark Dirty Region
                                    ↓
                            Next VSYNC
                                    ↓
                            Render Dirty Region
                                    ↓
                            DMA2D Copy to Frame Buffer
```

---

## 8. Đồng Bộ Hóa và RTOS

### 8.1 FreeRTOS Integration

TouchGFX được tích hợp với FreeRTOS qua CMSIS-RTOS v2:

```cpp
// TouchGFX/target/generated/OSWrappers.cpp
void OSWrappers::initialize()
{
    // Binary semaphore cho frame buffer access
    frame_buffer_sem = osSemaphoreNew(1, 1, NULL);
    
    // Message queue cho VSYNC signal
    vsync_queue = osMessageQueueNew(1, 4, NULL);
}
```

### 8.2 Frame Buffer Semaphore

Semaphore đảm bảo chỉ một task có thể access frame buffer tại một thời điểm:

```cpp
void OSWrappers::takeFrameBufferSemaphore()
{
    osSemaphoreAcquire(frame_buffer_sem, osWaitForever);
}

void OSWrappers::giveFrameBufferSemaphore()
{
    osSemaphoreRelease(frame_buffer_sem);
}
```

**Usage:**
- GUI task acquire semaphore trước khi render
- Release sau khi render xong
- ISR có thể release từ interrupt context

### 8.3 VSYNC Queue

Message queue để signal VSYNC từ ISR đến GUI task:

```cpp
void OSWrappers::signalVSync()
{
    // Called from LTDC ISR
    osMessageQueuePut(vsync_queue, &dummy, 0, 0);
}

void OSWrappers::waitForVSync()
{
    uint32_t dummyGet;
    // Clear queue first
    osMessageQueueGet(vsync_queue, &dummyGet, 0, 0);
    // Wait for next VSYNC
    osMessageQueueGet(vsync_queue, &dummyGet, 0, osWaitForever);
}
```

**Flow:**
```
LTDC ISR → signalVSync() → Queue Put
                                ↓
                        GUI Task blocked
                                ↓
                        waitForVSync() → Queue Get
                                ↓
                        Render Next Frame
```

### 8.4 Task Priorities

```c
// main.c
const osThreadAttr_t defaultTask_attributes = {
    .priority = osPriorityNormal,  // Same priority
};

const osThreadAttr_t GUI_Task_attributes = {
    .priority = osPriorityNormal,  // Same priority
};
```

**Lưu ý:**
- Cả 2 tasks có cùng priority → time slicing
- GUI task sẽ block trên `waitForVSync()` → không waste CPU
- Default task chỉ delay 100ms → idle most of the time

### 8.5 Interrupt Priorities

```cpp
// TouchGFX/target/generated/TouchGFXGeneratedHAL.cpp
void TouchGFXGeneratedHAL::configureInterrupts()
{
    NVIC_SetPriority(DMA2D_IRQn, 9);  // Low priority
    NVIC_SetPriority(LTDC_IRQn, 9);   // Low priority
}
```

**Priority Levels (STM32):**
- 0-3: Very high (system critical)
- 4-7: High
- 8-11: Normal
- 12-15: Low

**Lý do priority 9:**
- Không block các interrupt quan trọng khác
- Đủ để đảm bảo VSYNC được xử lý đúng lúc

---

## 9. Hardware Abstraction Layer

### 9.1 LTDC (LCD TFT Display Controller)

LTDC là peripheral chính để điều khiển LCD:

```c
// Core/Src/main.c
static void MX_LTDC_Init(void)
{
    hltdc.Instance = LTDC;
    
    // Timing configuration
    hltdc.Init.HorizontalSync = 9;
    hltdc.Init.VerticalSync = 1;
    hltdc.Init.AccumulatedHBP = 29;
    hltdc.Init.AccumulatedVBP = 3;
    hltdc.Init.AccumulatedActiveW = 269;  // 240 + 29
    hltdc.Init.AccumulatedActiveH = 323;   // 320 + 3
    hltdc.Init.TotalWidth = 279;
    hltdc.Init.TotalHeigh = 327;
    
    // Layer configuration
    pLayerCfg.WindowX0 = 0;
    pLayerCfg.WindowX1 = 240;
    pLayerCfg.WindowY0 = 0;
    pLayerCfg.WindowY1 = 320;
    pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    pLayerCfg.FBStartAdress = 0;  // Sẽ được set bởi TouchGFX
    
    HAL_LTDC_Init(&hltdc);
    HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0);
}
```

**LTDC Features:**
- Hardware-accelerated display controller
- Double buffering support
- Line interrupt (VSYNC)
- Direct memory access từ SDRAM

### 9.2 DMA2D (2D DMA)

DMA2D được dùng cho graphics operations:

```c
static void MX_DMA2D_Init(void)
{
    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_M2M;  // Memory-to-Memory
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
    
    HAL_DMA2D_Init(&hdma2d);
}
```

**DMA2D Operations:**
- Copy rectangles (blit)
- Fill rectangles
- Alpha blending
- Color format conversion

TouchGFX tự động sử dụng DMA2D cho các operations này.

### 9.3 SDRAM Initialization

SDRAM cần initialization sequence đặc biệt:

```c
static void BSP_SDRAM_Initialization_Sequence(...)
{
    // Step 1: Clock Enable
    Command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);
    HAL_Delay(1);
    
    // Step 2: Precharge All
    Command->CommandMode = FMC_SDRAM_CMD_PALL;
    HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);
    
    // Step 3: Auto Refresh (4 cycles)
    Command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command->AutoRefreshNumber = 4;
    HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);
    
    // Step 4: Load Mode Register
    Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    Command->ModeRegisterDefinition = ...;
    HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);
    
    // Step 5: Set Refresh Rate
    HAL_SDRAM_ProgramRefreshRate(hsdram, REFRESH_COUNT);
}
```

**SDRAM Configuration:**
- Bank: FMC_SDRAM_BANK2
- Size: 8MB (4M × 16-bit)
- CAS Latency: 3
- Refresh Rate: 1386 cycles

### 9.4 I2C3 (Touch Controller)

```c
static void MX_I2C3_Init(void)
{
    hi2c3.Instance = I2C3;
    hi2c3.Init.ClockSpeed = 100000;  // 100kHz
    hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    
    HAL_I2C_Init(&hi2c3);
}
```

**I2C Communication:**
- Address: 0x82 (STMPE811)
- Speed: 100kHz (đủ cho touch sampling)
- Used for: Touch coordinate reading

### 9.5 SPI5 (LCD Control)

```c
static void MX_SPI5_Init(void)
{
    hspi5.Instance = SPI5;
    hspi5.Init.Mode = SPI_MODE_MASTER;
    hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    
    HAL_SPI_Init(&hspi5);
    
    // Check board revision
    // ...
}
```

**SPI Usage:**
- LCD command/data interface
- Chip Select: PC2
- Write/Read control: PD13

---

## 10. Performance Optimization

### 10.1 Frame Rate

Target frame rate: 60 FPS (16.67ms per frame)

**Bottlenecks:**
1. **Rendering**: Widget drawing
2. **DMA2D**: Graphics operations
3. **Memory Bandwidth**: SDRAM access

**Optimizations:**
- Double buffering để tránh flickering
- Partial updates (chỉ redraw dirty regions)
- DMA2D hardware acceleration
- Invalidation system để minimize redraws

### 10.2 Memory Usage

```
Stack Usage:
- defaultTask: 512 bytes
- GUI_Task: 32KB (TouchGFX needs large stack)

Heap Usage:
- FreeRTOS heap: Dynamic allocation
- FrontendHeap: UI objects

SDRAM Usage:
- Frame Buffer: 307KB (2 buffers)
- Animation Storage: ~76KB
- Free: ~7.6MB
```

### 10.3 CPU Usage

**Typical CPU Usage:**
- GUI Task: ~30-50% (rendering, event handling)
- Default Task: <1% (mostly idle)
- ISRs: <5% (VSYNC, DMA2D)

**Peak CPU Usage:**
- During animation: ~70-80%
- During game logic: ~10-20%

---

## 11. Kết Luận

### 11.1 Kiến Trúc Tổng Kết

Hệ thống sử dụng:
- **Layered Architecture**: Tách biệt application, framework, HAL, hardware
- **MVP Pattern**: Tách biệt logic và presentation
- **RTOS**: Multitasking với FreeRTOS
- **Hardware Acceleration**: LTDC, DMA2D cho graphics

### 11.2 Điểm Mạnh

1. **Modular Design**: Dễ maintain và extend
2. **Hardware Acceleration**: Hiệu năng tốt
3. **Double Buffering**: Smooth animation
4. **Multiple Input Methods**: Touch và GPIO buttons

### 11.3 Điểm Cần Cải Thiện

1. **Game Logic trong View**: Nên move sang Model hoặc separate GameEngine class
2. **Magic Numbers**: Nên define constants (grid size, tile size, etc.)
3. **Error Handling**: Cần thêm error handling cho hardware failures
4. **State Persistence**: Best score không được lưu persistent (flash)

### 11.4 Hướng Phát Triển

1. **Save/Load Game**: Lưu game state vào flash
2. **High Score Persistence**: Lưu best score vào EEPROM/Flash
3. **Sound Effects**: Thêm audio feedback
4. **More Game Modes**: Thêm các biến thể của 2048
5. **Undo Feature**: Cho phép undo move

---

**Tài liệu này cung cấp cái nhìn sâu về kiến trúc và implementation của game 2048 trên STM32F429. Hy vọng sẽ giúp bạn hiểu rõ cách hệ thống hoạt động và có thể mở rộng hoặc tối ưu hóa code.**

