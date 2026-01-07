# Game 2048 trên STM32F429I Discovery Board

## Mô tả

Đây là một game 2048 được phát triển cho board STM32F429I Discovery (REV D01). Game hỗ trợ nhiều chế độ chơi khác nhau với giao diện đồ họa được xây dựng bằng TouchGFX framework.

### Các chế độ chơi:
- **4x4 (Chế độ chính)**: Lưới 4x4 cổ điển của game 2048
- **3x3**: Lưới nhỏ hơn, thử thách hơn
- **5x5**: Lưới lớn hơn với nhiều không gian hơn
- **5x5 Super Merging**: Chế độ gộp đặc biệt với các ô siêu gộp
- **5x5 Letter Merging**: Chế độ gộp với các chữ cái

### Tính năng:
- Điều khiển bằng cảm ứng (swipe gestures)
- Điều khiển bằng GPIO buttons (UP, DOWN, LEFT, RIGHT, BACK)
- Hệ thống điểm số và best score cho từng màn 
- Animation khi spawn tile mới
- Màn hình Game Over khi không còn nước đi

## Kiến trúc hệ thống

### Phần cứng:
- **MCU**: STM32F429ZIT6 (ARM Cortex-M4F @ 180 MHz)
- **Display**: LCD TFT 2.4" ILI9341 (320 x 240 pixels, 16bpp)
- **Touch Controller**: STMPE811 (I2C)
- **Memory**: SDRAM cho frame buffer
- **GPIO Buttons**: 
  - PB10: UP
  - PB12: DOWN
  - PA2: LEFT
  - PA6: RIGHT
  - PA0: BACK

### Phần mềm:
- **RTOS**: FreeRTOS (CMSIS-RTOS v2)
- **GUI Framework**: TouchGFX 4.25.0
- **HAL**: STM32 HAL Library
- **Graphics Acceleration**: 
  - LTDC (LCD TFT Display Controller) cho hiển thị
  - DMA2D cho xử lý đồ họa
- **Kiến trúc**: MVP (Model-View-Presenter) pattern

### Cấu trúc thư mục:
```
2048/
├── Core/                    # Core application code
│   ├── Inc/                 # Header files
│   └── Src/                 # Source files (main.c, etc.)
├── TouchGFX/                # TouchGFX GUI framework
│   ├── gui/                 # Game logic và UI screens
│   │   ├── mainscreen_screen/      # Màn hình 4x4 chính
│   │   ├── screen3x3_screen/       # Màn hình 3x3
│   │   ├── screen5x5_screen/       # Màn hình 5x5
│   │   ├── screen5x5_supermerging_screen/  # Super merging mode
│   │   ├── screen5x5_lettermerging_screen/  # Letter merging mode
│   │   ├── gameoverscreen_screen/  # Màn hình game over
│   │   ├── containers/             # UI components (Tile, Score, etc.)
│   │   └── common/                  # Shared code (GameGlobal, etc.)
│   ├── target/              # HAL implementation cho STM32
│   └── generated/           # Code tự động sinh từ TouchGFX Designer
├── Drivers/                 # STM32 HAL drivers
├── Middlewares/             # FreeRTOS và TouchGFX middleware
├── STM32CubeIDE/            # Project files cho STM32CubeIDE
├── gcc/                     # GCC build files
├── EWARM/                   # IAR EWARM project files
├── MDK-ARM/                 # Keil MDK-ARM project files
└── STM32F429I_DISCO_REV_D01.ioc  # STM32CubeMX configuration file
```

## Cách chạy game

### Yêu cầu:
1. **STM32CubeIDE** (IDE mặc định) hoặc:
   - IAR EWARM (version 8.50.9 trở lên)
   - Keil MDK-ARM
2. **STM32CubeProgrammer** (để flash firmware)
3. **STM32CubeMX** (để chỉnh sửa cấu hình)
4. **TouchGFX Designer** (tùy chọn, để chỉnh sửa UI)

### Cách build và flash:

#### Phương pháp 1: Sử dụng STM32CubeIDE (Khuyến nghị)
1. Mở STM32CubeIDE
2. Import project từ thư mục `STM32CubeIDE/`
3. Build project (Project → Build All)
4. Kết nối board STM32F429I Discovery qua USB
5. Flash firmware (Run → Debug hoặc Run → Run)

#### Phương pháp 2: Sử dụng TouchGFX Designer
1. Mở TouchGFX Designer
2. Mở file `TouchGFX/20481.touchgfx`
3. Build và flash trực tiếp từ Designer (yêu cầu GCC và STM32CubeProgrammer)


## Cấu hình hiển thị

- **Độ phân giải**: 320 x 240 pixels
- **Độ sâu màu**: 16 bits per pixel (RGB565)
- **Refresh rate**: Có thể đo bằng GPIO pin PE4 (FRAME_RATE)

## Performance Testing

Game hỗ trợ đo hiệu năng qua các GPIO pins:
- **PE2 (VSYNC_FREQ)**: Tần số VSYNC
- **PE3 (RENDER_TIME)**: Thời gian render
- **PE4 (FRAME_RATE)**: Frame rate
- **PE5 (MCU_ACTIVE)**: Trạng thái hoạt động của MCU

Kết nối các pin này với oscilloscope hoặc logic analyzer để đo hiệu năng.

## Cách chơi

### Điều khiển bằng cảm ứng:
- **Vuốt lên**: Di chuyển các ô lên trên
- **Vuốt xuống**: Di chuyển các ô xuống dưới
- **Vuốt trái**: Di chuyển các ô sang trái
- **Vuốt phải**: Di chuyển các ô sang phải

### Điều khiển bằng GPIO buttons:
- **PB10 (UP)**: Di chuyển lên
- **PB12 (DOWN)**: Di chuyển xuống
- **PA2 (LEFT)**: Di chuyển trái
- **PA6 (RIGHT)**: Di chuyển phải
- **PA0 (BACK)**: Quay lại màn hình chọn chế độ

### Luật chơi:
1. Các ô có cùng giá trị sẽ gộp lại khi di chuyển
2. Mỗi lần di chuyển thành công, một ô mới (giá trị 2 hoặc 4) sẽ xuất hiện
3. Mục tiêu: Tạo ra ô có giá trị 2048 (hoặc cao hơn)
4. Game kết thúc khi không còn nước đi hợp lệ

## Cấu trúc code chính

### Game Logic:
- `MainScreenView.cpp`: Logic game 4x4 chính
- `GameGlobal.cpp`: Quản lý điểm số toàn cục
- Các file `Screen*.cpp`: Logic cho các chế độ khác nhau

### UI Components:
- `Tile.cpp`: Component hiển thị một ô số
- `Score.cpp`: Component hiển thị điểm
- `Best.cpp`: Component hiển thị điểm cao nhất

### Hardware Abstraction:
- `TouchGFXHAL.cpp`: HAL implementation cho TouchGFX
- `STM32TouchController.cpp`: Điều khiển touch
- `main.c`: Khởi tạo hệ thống và FreeRTOS tasks

## Phiên bản

- **TouchGFX**: 4.25.0
- **STM32CubeMX**: 6.14.0
- **Firmware Package**: FW_F4 V1.28.1
- Xem `changelog.txt` để biết chi tiết các phiên bản trước

## License

Copyright (c) 2020 STMicroelectronics. All rights reserved.

This software component is licensed by ST under Ultimate Liberty license SLA0044.
