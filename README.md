# 2048 Game on STM32F429I Discovery Board
# Members
Nguyễn Văn An - 20220013 \\
Nguyễn Thị Hà Chi - 20220016 \\
Văn Đức Cường - 20220021

# Work load
https://docs.google.com/document/d/1Rc6pQJn3TCWb4GFSWM3aUvRS78knzdguSiGfVOBGpjc/edit?fbclid=IwY2xjawPnuwJleHRuA2FlbQIxMABicmlkETFMYVR5NmR6aktueFNGV0Ric3J0YwZhcHBfaWQQMjIyMDM5MTc4ODIwMDg5MgABHhhcZjPp-nLuMoFxU31TWNCd5CQ9FvP_dBFLSqlJ4AyC39aU1sZZfHlC4a0P_aem_7runHD7D9ny46pr5oSFC4g&tab=t.0

# Video demo
https://drive.google.com/drive/folders/1EXR6k5SGpmYPlA_WsYHsWP5qKt1F-NfO?usp=sharing

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
- Analog Joystick control with Hysteresis and Diagonal Movement Fix
- Buzzer feedback for button presses and game over
- Score system and best score for each game mode
- Template-based Game Engine (`Game2048Engine`) for reusable game logic
- Animation when spawning new tiles
- Game Over screen when no valid moves remain

## System Architecture

### Hardware:

#### MCU & Display:
- **MCU**: STM32F429ZIT6 (ARM Cortex-M4F @ 180 MHz)
- **Display**: LCD TFT 2.4" ILI9341 (320 x 240 pixels, 16bpp)
- **Touch Controller**: STMPE811 (I2C)
- **Memory**: SDRAM for frame buffer

#### GPIO Buttons (Digital Input):
| Pin | Function | Mode | Description |
|-----|----------|------|-------------|
| PE2 | UP | Input, Pull-up, Active-LOW | Move tiles up |
| PE3 | DOWN | Input, Pull-up, Active-LOW | Move tiles down |
| PE4 | LEFT | Input, Pull-up, Active-LOW | Move tiles left |
| PE5 | RIGHT | Input, Pull-up, Active-LOW | Move tiles right |
| PE6 | JOY_BTN | Input, Pull-up, Active-LOW | Joystick button → BACK |
| PA0 | BACK | Input, No-Pull, Active-HIGH | Return to menu (User Button) |

#### Analog Joystick (ADC Input):
| Pin | ADC | Channel | Function |
|-----|-----|---------|----------|
| PA5 | ADC1 | CH5 | Joystick X-axis |
| PF6 | ADC3 | CH4 | Joystick Y-axis |

#### Buzzer:
| Pin | Mode | Description |
|-----|------|-------------|
| PC4 | Output, Active-HIGH | Audio feedback buzzer |

### Software:
- **RTOS**: FreeRTOS (CMSIS-RTOS v2)
- **GUI Framework**: TouchGFX 4.25.0
- **HAL**: STM32 HAL Library
- **Graphics Acceleration**: 
  - LTDC (LCD TFT Display Controller) for display
  - DMA2D for graphics processing
- **Architecture**: MVP (Model-View-Presenter) pattern

## MVP Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                           HARDWARE LAYER                            │
├─────────────────────────────────────────────────────────────────────┤
│  GPIO Buttons (PE2-PE6, PA0)  │  ADC Joystick (PA5, PF6)  │ Buzzer │
└───────────────────────────────┴───────────────────────────┴─────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────┐
│                              MODEL                                  │
│  (Model.hpp / Model.cpp)                                            │
├─────────────────────────────────────────────────────────────────────┤
│  • tick() - Called every frame (~16ms)                              │
│  • Polls GPIO buttons with software debounce (150ms)                │
│  • Polls ADC joystick with HYSTERESIS                               │
│  • Controls buzzer (non-blocking beep)                              │
│  • Notifies Presenter via ModelListener interface                   │
└─────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼ (onButtonUp, onButtonDown, etc.)
┌─────────────────────────────────────────────────────────────────────┐
│                         MODEL LISTENER                              │
│  (ModelListener.hpp)                                                │
├─────────────────────────────────────────────────────────────────────┤
│  • Interface for Presenter to receive events from Model             │
│  • Virtual functions: onButtonUp(), onButtonDown(), onButtonLeft(), │
│    onButtonRight(), onButtonBack()                                  │
│  • notifyGameOver() - Triggers 1-second buzzer beep                 │
└─────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────┐
│                           PRESENTER                                 │
│  (MainScreenPresenter.cpp, Screen3x3Presenter.cpp, etc.)            │
├─────────────────────────────────────────────────────────────────────┤
│  • Inherits from ModelListener                                      │
│  • Receives button events from Model                                │
│  • Forwards events to View (onMoveUp, onMoveDown, etc.)             │
└─────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────┐
│                              VIEW                                   │
│  (MainScreenView.cpp, Screen3x3View.cpp, etc.)                      │
├─────────────────────────────────────────────────────────────────────┤
│  • Handles UI rendering and touch gestures                          │
│  • Implements game logic (move tiles, merge, spawn, game over)      │
│  • Does NOT directly access GPIO/ADC (MVP pattern)                  │
│  • Calls presenter->notifyGameOver() when game ends                 │
└─────────────────────────────────────────────────────────────────────┘
```

## Joystick Hysteresis

To prevent buzzer spam when joystick values oscillate around thresholds:

```
ADC Value (0-4095)
       │
  4095 ┤
       │
  3000 ┼─────── MAX_THRESHOLD (Trigger UP/LEFT)
       │   ▲
  2500 ┼───┼─── RESET_HIGH (Reset after UP/LEFT)
       │   │
       │   │ Hysteresis Zone (no action)
       │   │
  1500 ┼───┼─── RESET_LOW (Reset after DOWN/RIGHT)
       │   ▼
  1000 ┼─────── MIN_THRESHOLD (Trigger DOWN/RIGHT)
       │
  2048 ┤─────── CENTER VALUE
       │
     0 ┤
```

### Joystick Mapping:
| ADC Value | Direction |
|-----------|-----------|
| Y > 3000 | UP |
| Y < 1000 | DOWN |
| X > 3000 | LEFT |
| X < 1000 | RIGHT |

### Diagonal Movement Handling:
When the joystick is moved diagonally, both X and Y axes exceed thresholds simultaneously. To prevent double buzzer beeps, the system uses **dominant axis selection**:
- Calculate displacement from center: `|X - 2048|` and `|Y - 2048|`
- Only trigger the direction with the **larger displacement**
- Example: If `X = 3500` (delta = 1452) and `Y = 3100` (delta = 1052), only LEFT is triggered

## Directory Structure

```
20481/
├── Core/                    # Core application code
│   ├── Inc/                 # Header files (main.h with GPIO defines)
│   └── Src/                 # Source files (main.c, ADC/GPIO init)
├── TouchGFX/                # TouchGFX GUI framework
│   ├── gui/                 # Game logic and UI screens
│   │   ├── model/                 # MVP Model (GPIO/ADC polling)
│   │   ├── mainscreen_screen/     # Main 4x4 screen
│   │   ├── screen3x3_screen/      # 3x3 screen
│   │   ├── screen5x5_screen/      # 5x5 screen
│   │   ├── screen5x5_supermerging_screen/  # Super merging mode
│   │   ├── screen5x5_lettermerging_screen/ # Letter merging mode
│   │   ├── chosing_mode_screen/   # Game mode selection
│   │   ├── selectedgamedesign_screen/  # Game size selection
│   │   ├── gameoverscreen_screen/ # Game over screen
│   │   ├── containers/            # UI components (Tile, Score, etc.)
│   │   └── common/                # Shared code
│   │       ├── Game2048Engine.hpp    # Template-based game engine (3x3, 4x4, 5x5)
│   │       └── GameGlobal.hpp        # Global game state (scores, game mode)
│   ├── target/              # HAL implementation for STM32
│   └── generated/           # Auto-generated code from TouchGFX Designer
├── Drivers/                 # STM32 HAL drivers
├── Middlewares/             # FreeRTOS and TouchGFX middleware
├── STM32CubeIDE/            # Project files for STM32CubeIDE
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

## How to Play

### Touch Control:
- **Swipe Up**: Move tiles upward
- **Swipe Down**: Move tiles downward
- **Swipe Left**: Move tiles to the left
- **Swipe Right**: Move tiles to the right

### GPIO Button Control:
| Button | Pin | Action |
|--------|-----|--------|
| UP | PE2 | Move tiles up |
| DOWN | PE3 | Move tiles down |
| LEFT | PE4 | Move tiles left |
| RIGHT | PE5 | Move tiles right |
| JOY_BTN | PE6 | Return to menu (same as BACK) |
| BACK | PA0 | Return to menu |

### Joystick Control:
| Direction | Action |
|-----------|--------|
| Push Up (Y > 3000) | Move tiles up |
| Push Down (Y < 1000) | Move tiles down |
| Push Left (X > 3000) | Move tiles left |
| Push Right (X < 1000) | Move tiles right |

### Audio Feedback:
- **Button press / Joystick move**: Short beep (100ms)
- **Game Over**: Long beep (1 second)

### Game Rules:
1. Tiles with the same value will merge when moved
2. After each successful move, a new tile (value 2 or 4) will appear
3. Goal: Create a tile with value 2048 (or higher)
4. Game ends when no valid moves remain

## Tile Movement Algorithm

### Overview

The 2048 game uses a **Slide and Merge** algorithm for tile movement. Each direction (UP, DOWN, LEFT, RIGHT) follows the same logic pattern but operates on different axes.

### Algorithm Steps (Example: Move Left)

```
Initial State:          After Slide:           After Merge:
┌───┬───┬───┬───┐      ┌───┬───┬───┬───┐      ┌───┬───┬───┬───┐
│ 2 │   │ 2 │ 4 │  →   │ 2 │ 2 │ 4 │   │  →   │ 4 │ 4 │   │   │
└───┴───┴───┴───┘      └───┴───┴───┴───┘      └───┴───┴───┴───┘
```

### Pseudocode

```
function moveLeft():
    for each row:
        merged[] = {false, false, false, false}  // Track merged tiles
        
        for col = 1 to 3:  // Skip first column
            if tile[row][col] == 0: continue
            
            // STEP 1: SLIDE - Move tile as far left as possible
            currentCol = col
            while currentCol > 0 AND tile[row][currentCol-1] == 0:
                tile[row][currentCol-1] = tile[row][currentCol]
                tile[row][currentCol] = 0
                currentCol--
            
            // STEP 2: MERGE - Merge with adjacent tile if same value
            if currentCol > 0 AND 
               tile[row][currentCol-1] == tile[row][currentCol] AND
               NOT merged[currentCol-1]:
                
                tile[row][currentCol-1] *= 2  // Double the value
                tile[row][currentCol] = 0     // Clear current tile
                merged[currentCol-1] = true   // Mark as merged
                score += tile[row][currentCol-1]
```

### Key Points

1. **Slide Phase**: Each tile slides as far as possible in the direction of movement until it hits another tile or the edge.

2. **Merge Phase**: If two adjacent tiles have the same value AND the target tile hasn't been merged in this move, they combine into one tile with double the value.

3. **Merge Flag**: The `merged[]` array prevents chain merging in a single move. Example:
   ```
   [2][2][4][0] → moveLeft → [4][4][0][0]  ✓ Correct
   NOT: [2][2][4][0] → [8][0][0][0]        ✗ Wrong (chain merge)
   ```

4. **Direction Variants**:
   | Direction | Iterate | Slide Direction | Check Neighbor |
   |-----------|---------|-----------------|----------------|
   | Left | col: 1→3 | currentCol-- | currentCol - 1 |
   | Right | col: 2→0 | currentCol++ | currentCol + 1 |
   | Up | row: 1→3 | currentRow-- | currentRow - 1 |
   | Down | row: 2→0 | currentRow++ | currentRow + 1 |

5. **Time Complexity**: O(n²) where n is the grid size (4 for standard 2048)

### Additional Logic

- **Spawn Tile**: After each valid move, a new tile (2 or 4) spawns in a random empty cell
- **Game Over Check**: Game ends when no empty cells AND no adjacent tiles with same value
- **Score Update**: Points are awarded when tiles merge (equal to the merged tile's new value)

### Game Engine Implementation

The game uses a **template-based engine** (`Game2048Engine<SIZE>`) to avoid code duplication:

- **Generic Algorithm**: Same slide & merge logic works for 3x3, 4x4, and 5x5 grids
- **Template Specialization**: `Game2048Engine<3>`, `Game2048Engine<4>`, `Game2048Engine<5>`
- **Features**:
  - Grid state management (save/restore)
  - Move detection (`moved` flag)
  - Score tracking
  - Game over detection
  - Spawn tile helper function

**Usage Example**:
```cpp
Game2048Engine<4> engine;  // 4x4 grid
engine.moveLeft();
if (engine.moved) {
    engine.spawnTile(myRand);
}
if (engine.isGameOver()) {
    // Game over logic
}
```

**Special Modes**: Super Merging and Letter Merging have unique merging rules, so they implement custom logic without using `Game2048Engine`.

## Main Code Structure

### MVP Components:
- `Model.cpp`: Hardware abstraction (GPIO polling, ADC reading, buzzer control, joystick hysteresis with diagonal fix)
- `ModelListener.hpp`: Interface for Presenter to receive events
- `*Presenter.cpp`: Event handlers, forwards to View
- `*View.cpp`: UI rendering and game state management

### Game Engine:
- **`Game2048Engine.hpp`**: Template-based generic game engine for standard 2048 logic
  - Template parameter: Grid size (3, 4, or 5)
  - Implements slide & merge algorithm
  - Used by: `MainScreenView` (4x4), `Screen3x3View`, `Screen5x5View`
  - **NOT used by**: Super Merging and Letter Merging modes (they have special logic)

### Game Logic Files:
- `MainScreenView.cpp`: 4x4 game - uses `Game2048Engine<4>`
- `Screen3x3View.cpp`: 3x3 game - uses `Game2048Engine<3>`
- `Screen5x5View.cpp`: 5x5 game - uses `Game2048Engine<5>`
- `Screen5x5_superMergingView.cpp`: Custom merging logic (no engine)
- `Screen5x5_letterMergingView.cpp`: Custom letter merging logic (no engine)
- `GameGlobal.cpp`: Global score management (best scores per game mode)

### UI Components:
- `Tile.cpp`: Component to display a number tile
- `Score.cpp`: Component to display score
- `Best.cpp`: Component to display best score

## Version

- **TouchGFX**: 4.25.0
- **STM32CubeMX**: 6.14.0
- **Firmware Package**: FW_F4 V1.28.1

## License

Copyright (c) 2020 STMicroelectronics. All rights reserved.

This software component is licensed by ST under Ultimate Liberty license SLA0044.
