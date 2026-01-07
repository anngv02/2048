# 2048 Game on STM32F429I Discovery Board

## Description

This is a 2048 game developed for the STM32F429I Discovery board (REV D01). The game supports multiple game modes with a graphical interface built using the TouchGFX framework.

### Game Modes:
- **4x4 (Main Mode)**: Classic 4x4 grid of the 2048 game
- **3x3**: Smaller grid, more challenging
- **5x5**: Larger grid with more space
- **5x5 Super Merging**: Special merging mode with super merge tiles
- **5x5 Letter Merging**: Merging mode with letters

### Features:
- Touch control (swipe gestures)
- GPIO button control (UP, DOWN, LEFT, RIGHT, BACK)
- Score system and best score for each screen
- Animation when spawning new tiles
- Game Over screen when no valid moves remain

## System Architecture

### Hardware:
- **MCU**: STM32F429ZIT6 (ARM Cortex-M4F @ 180 MHz)
- **Display**: LCD TFT 2.4" ILI9341 (320 x 240 pixels, 16bpp)
- **Touch Controller**: STMPE811 (I2C)
- **Memory**: SDRAM for frame buffer
- **GPIO Buttons**: 
  - PB10: UP
  - PB12: DOWN
  - PA2: LEFT
  - PA6: RIGHT
  - PA0: BACK

### Software:
- **RTOS**: FreeRTOS (CMSIS-RTOS v2)
- **GUI Framework**: TouchGFX 4.25.0
- **HAL**: STM32 HAL Library
- **Graphics Acceleration**: 
  - LTDC (LCD TFT Display Controller) for display
  - DMA2D for graphics processing
- **Architecture**: MVP (Model-View-Presenter) pattern

### Directory Structure:
```
2048/
├── Core/                    # Core application code
│   ├── Inc/                 # Header files
│   └── Src/                 # Source files (main.c, etc.)
├── TouchGFX/                # TouchGFX GUI framework
│   ├── gui/                 # Game logic and UI screens
│   │   ├── mainscreen_screen/      # Main 4x4 screen
│   │   ├── screen3x3_screen/       # 3x3 screen
│   │   ├── screen5x5_screen/       # 5x5 screen
│   │   ├── screen5x5_supermerging_screen/  # Super merging mode
│   │   ├── screen5x5_lettermerging_screen/  # Letter merging mode
│   │   ├── gameoverscreen_screen/  # Game over screen
│   │   ├── containers/             # UI components (Tile, Score, etc.)
│   │   └── common/                  # Shared code (GameGlobal, etc.)
│   ├── target/              # HAL implementation for STM32
│   └── generated/           # Auto-generated code from TouchGFX Designer
├── Drivers/                 # STM32 HAL drivers
├── Middlewares/             # FreeRTOS and TouchGFX middleware
├── STM32CubeIDE/            # Project files for STM32CubeIDE
├── gcc/                     # GCC build files
├── EWARM/                   # IAR EWARM project files
├── MDK-ARM/                 # Keil MDK-ARM project files
└── STM32F429I_DISCO_REV_D01.ioc  # STM32CubeMX configuration file
```

## How to Run the Game

### Requirements:
1. **STM32CubeIDE** (default IDE) or:
   - IAR EWARM (version 8.50.9 or higher)
   - Keil MDK-ARM
2. **STM32CubeProgrammer** (to flash firmware)
3. **STM32CubeMX** (to edit configuration)
4. **TouchGFX Designer** (optional, to edit UI)

### Build and Flash:

#### Method 1: Using STM32CubeIDE (Recommended)
1. Open STM32CubeIDE
2. Import project from `STM32CubeIDE/` directory
3. Build project (Project → Build All)
4. Connect STM32F429I Discovery board via USB
5. Flash firmware (Run → Debug or Run → Run)

#### Method 2: Using TouchGFX Designer
1. Open TouchGFX Designer
2. Open file `TouchGFX/20481.touchgfx`
3. Build and flash directly from Designer (requires GCC and STM32CubeProgrammer)

## Display Configuration

- **Resolution**: 320 x 240 pixels
- **Color Depth**: 16 bits per pixel (RGB565)
- **Refresh Rate**: Can be measured using GPIO pin PE4 (FRAME_RATE)

## Performance Testing

The game supports performance measurement via GPIO pins:
- **PE2 (VSYNC_FREQ)**: VSYNC frequency
- **PE3 (RENDER_TIME)**: Render time
- **PE4 (FRAME_RATE)**: Frame rate
- **PE5 (MCU_ACTIVE)**: MCU active status

Connect these pins to an oscilloscope or logic analyzer to measure performance.

## How to Play

### Touch Control:
- **Swipe Up**: Move tiles upward
- **Swipe Down**: Move tiles downward
- **Swipe Left**: Move tiles to the left
- **Swipe Right**: Move tiles to the right

### GPIO Button Control:
- **PB10 (UP)**: Move up
- **PB12 (DOWN)**: Move down
- **PA2 (LEFT)**: Move left
- **PA6 (RIGHT)**: Move right
- **PA0 (BACK)**: Return to game mode selection screen

### Game Rules:
1. Tiles with the same value will merge when moved
2. After each successful move, a new tile (value 2 or 4) will appear
3. Goal: Create a tile with value 2048 (or higher)
4. Game ends when no valid moves remain

## Main Code Structure

### Game Logic:
- `MainScreenView.cpp`: Main 4x4 game logic
- `GameGlobal.cpp`: Global score management
- `Screen*.cpp` files: Logic for different game modes

### UI Components:
- `Tile.cpp`: Component to display a number tile
- `Score.cpp`: Component to display score
- `Best.cpp`: Component to display best score

### Hardware Abstraction:
- `TouchGFXHAL.cpp`: HAL implementation for TouchGFX
- `STM32TouchController.cpp`: Touch controller
- `main.c`: System initialization and FreeRTOS tasks

## Version

- **TouchGFX**: 4.25.0
- **STM32CubeMX**: 6.14.0
- **Firmware Package**: FW_F4 V1.28.1
- See `changelog.txt` for details of previous versions

## License

Copyright (c) 2020 STMicroelectronics. All rights reserved.

This software component is licensed by ST under Ultimate Liberty license SLA0044.
