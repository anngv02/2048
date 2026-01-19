#ifndef GAME_2048_ENGINE_HPP
#define GAME_2048_ENGINE_HPP

#include <stdint.h>

/**
 * @file Game2048Engine.hpp
 * @brief Template-based Game Engine for standard 2048 game logic
 * 
 * This engine handles the core 2048 algorithm:
 * - Slide tiles in 4 directions
 * - Merge pairs of same-value tiles (standard: newVal = oldVal * 2)
 * - Track score and best score
 * - Check game over condition
 * 
 * NOTE: This engine is for STANDARD 2048 only (3x3, 4x4, 5x5)
 * SuperMerging and LetterMerging have special logic and don't use this engine.
 * 
 * @tparam SIZE Grid size (3, 4, or 5)
 * 
 * Usage:
 *   Game2048Engine<4> engine;  // For 4x4 grid
 *   engine.moveLeft();
 *   if (engine.moved) { ... }
 */
template<int SIZE>
class Game2048Engine
{
public:
    // ==============================================================================
    // PUBLIC DATA
    // ==============================================================================
    
    /** @brief 2D grid of tile values (0 = empty) */
    uint16_t grid[SIZE][SIZE];
    
    /** @brief Grid state before last move (for change detection) */
    uint16_t gridBeforeMove[SIZE][SIZE];
    
    /** @brief Current score */
    uint32_t score;
    
    /** @brief Best score achieved */
    uint32_t bestScore;
    
    /** @brief Flag indicating if grid changed after last move */
    bool moved;

    // ==============================================================================
    // CONSTRUCTOR
    // ==============================================================================
    
    /**
     * @brief Constructor - initializes empty grid
     */
    Game2048Engine() : score(0), bestScore(0), moved(false)
    {
        reset();
    }

    /**
     * @brief Reset game to initial state (all tiles = 0)
     */
    void reset()
    {
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++)
                grid[r][c] = 0;
        score = 0;
        moved = false;
    }

    /**
     * @brief Set initial best score (loaded from storage)
     */
    void setBestScore(uint32_t best)
    {
        bestScore = best;
    }

    // ==============================================================================
    // GRID STATE MANAGEMENT
    // ==============================================================================
    
    /**
     * @brief Save current grid state (call before move)
     */
    void saveGridState()
    {
        for (int i = 0; i < SIZE; ++i)
            for (int j = 0; j < SIZE; ++j)
                gridBeforeMove[i][j] = grid[i][j];
    }

    /**
     * @brief Check if grid changed after move
     * @return true if any tile value changed
     */
    bool hasGridChanged() const
    {
        for (int i = 0; i < SIZE; ++i)
            for (int j = 0; j < SIZE; ++j)
                if (gridBeforeMove[i][j] != grid[i][j])
                    return true;
        return false;
    }

    // ==============================================================================
    // MOVE METHODS - Standard 2048 Algorithm
    // ==============================================================================
    
    /**
     * @brief Move all tiles LEFT
     * 
     * Algorithm:
     * 1. For each row, iterate columns left-to-right (skip col 0)
     * 2. Slide: Move tile as far left as possible
     * 3. Merge: If adjacent tile has same value AND not already merged, combine
     */
    void moveLeft()
    {
        moved = false;
        
        for (int row = 0; row < SIZE; ++row)
        {
            int merged[SIZE] = {0};  // Track merged tiles to prevent chain merge
            
            for (int col = 1; col < SIZE; ++col)
            {
                if (grid[row][col] == 0) continue;
                
                int currentCol = col;
                
                // SLIDE: Move tile left until blocked
                while (currentCol > 0 && grid[row][currentCol - 1] == 0)
                {
                    grid[row][currentCol - 1] = grid[row][currentCol];
                    grid[row][currentCol] = 0;
                    currentCol--;
                    moved = true;
                }
                
                // MERGE: Combine with adjacent tile if same value
                if (currentCol > 0 &&
                    grid[row][currentCol - 1] == grid[row][currentCol] &&
                    !merged[currentCol - 1])
                {
                    uint16_t newValue = grid[row][currentCol - 1] * 2;
                    grid[row][currentCol - 1] = newValue;
                    grid[row][currentCol] = 0;
                    merged[currentCol - 1] = 1;
                    score += newValue;
                    moved = true;
                }
            }
        }
        
        updateBestScore();
    }

    /**
     * @brief Move all tiles RIGHT
     */
    void moveRight()
    {
        moved = false;
        
        for (int row = 0; row < SIZE; ++row)
        {
            int merged[SIZE] = {0};
            
            for (int col = SIZE - 2; col >= 0; --col)
            {
                if (grid[row][col] == 0) continue;
                
                int currentCol = col;
                
                // SLIDE: Move tile right until blocked
                while (currentCol < SIZE - 1 && grid[row][currentCol + 1] == 0)
                {
                    grid[row][currentCol + 1] = grid[row][currentCol];
                    grid[row][currentCol] = 0;
                    currentCol++;
                    moved = true;
                }
                
                // MERGE
                if (currentCol < SIZE - 1 &&
                    grid[row][currentCol + 1] == grid[row][currentCol] &&
                    !merged[currentCol + 1])
                {
                    uint16_t newValue = grid[row][currentCol + 1] * 2;
                    grid[row][currentCol + 1] = newValue;
                    grid[row][currentCol] = 0;
                    merged[currentCol + 1] = 1;
                    score += newValue;
                    moved = true;
                }
            }
        }
        
        updateBestScore();
    }

    /**
     * @brief Move all tiles UP
     */
    void moveUp()
    {
        moved = false;
        
        for (int col = 0; col < SIZE; ++col)
        {
            int merged[SIZE] = {0};
            
            for (int row = 1; row < SIZE; ++row)
            {
                if (grid[row][col] == 0) continue;
                
                int currentRow = row;
                
                // SLIDE: Move tile up until blocked
                while (currentRow > 0 && grid[currentRow - 1][col] == 0)
                {
                    grid[currentRow - 1][col] = grid[currentRow][col];
                    grid[currentRow][col] = 0;
                    currentRow--;
                    moved = true;
                }
                
                // MERGE
                if (currentRow > 0 &&
                    grid[currentRow - 1][col] == grid[currentRow][col] &&
                    !merged[currentRow - 1])
                {
                    uint16_t newValue = grid[currentRow - 1][col] * 2;
                    grid[currentRow - 1][col] = newValue;
                    grid[currentRow][col] = 0;
                    merged[currentRow - 1] = 1;
                    score += newValue;
                    moved = true;
                }
            }
        }
        
        updateBestScore();
    }

    /**
     * @brief Move all tiles DOWN
     */
    void moveDown()
    {
        moved = false;
        
        for (int col = 0; col < SIZE; ++col)
        {
            int merged[SIZE] = {0};
            
            for (int row = SIZE - 2; row >= 0; --row)
            {
                if (grid[row][col] == 0) continue;
                
                int currentRow = row;
                
                // SLIDE: Move tile down until blocked
                while (currentRow < SIZE - 1 && grid[currentRow + 1][col] == 0)
                {
                    grid[currentRow + 1][col] = grid[currentRow][col];
                    grid[currentRow][col] = 0;
                    currentRow++;
                    moved = true;
                }
                
                // MERGE
                if (currentRow < SIZE - 1 &&
                    grid[currentRow + 1][col] == grid[currentRow][col] &&
                    !merged[currentRow + 1])
                {
                    uint16_t newValue = grid[currentRow + 1][col] * 2;
                    grid[currentRow + 1][col] = newValue;
                    grid[currentRow][col] = 0;
                    merged[currentRow + 1] = 1;
                    score += newValue;
                    moved = true;
                }
            }
        }
        
        updateBestScore();
    }

    // ==============================================================================
    // UTILITY METHODS
    // ==============================================================================

    /**
     * @brief Spawn a new tile at random empty position
     * @param randFunc Function pointer to random number generator
     * @return true if spawn succeeded, false if grid is full
     */
    bool spawnTile(uint32_t (*randFunc)())
    {
        // Find all empty cells
        struct Pos { int row, col; };
        Pos empties[SIZE * SIZE];
        int emptyCount = 0;

        for (int r = 0; r < SIZE; r++)
        {
            for (int c = 0; c < SIZE; c++)
            {
                if (grid[r][c] == 0)
                {
                    empties[emptyCount].row = r;
                    empties[emptyCount].col = c;
                    emptyCount++;
                }
            }
        }

        if (emptyCount == 0) return false;

        // Pick random empty cell
        int idx = randFunc() % emptyCount;
        int rr = empties[idx].row;
        int cc = empties[idx].col;

        // 10% chance for 4, 90% chance for 2
        grid[rr][cc] = (randFunc() % 10 == 0) ? 4 : 2;
        return true;
    }

    /**
     * @brief Get position of last spawned tile (for animation)
     * @param randFunc Same function used in spawnTile
     * @param outRow Output row position
     * @param outCol Output column position
     * @return true if found empty cell
     */
    bool getRandomEmptyCell(uint32_t (*randFunc)(), int& outRow, int& outCol)
    {
        struct Pos { int row, col; };
        Pos empties[SIZE * SIZE];
        int emptyCount = 0;

        for (int r = 0; r < SIZE; r++)
        {
            for (int c = 0; c < SIZE; c++)
            {
                if (grid[r][c] == 0)
                {
                    empties[emptyCount].row = r;
                    empties[emptyCount].col = c;
                    emptyCount++;
                }
            }
        }

        if (emptyCount == 0) return false;

        int idx = randFunc() % emptyCount;
        outRow = empties[idx].row;
        outCol = empties[idx].col;
        return true;
    }

    /**
     * @brief Check if game is over (no valid moves remaining)
     * @return true if game over
     */
    bool isGameOver() const
    {
        // Check for empty cells
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++)
                if (grid[r][c] == 0) 
                    return false;

        // Check for possible merges
        for (int r = 0; r < SIZE; r++)
        {
            for (int c = 0; c < SIZE; c++)
            {
                uint16_t val = grid[r][c];
                
                // Check right neighbor
                if (c < SIZE - 1 && grid[r][c + 1] == val) 
                    return false;
                
                // Check bottom neighbor
                if (r < SIZE - 1 && grid[r + 1][c] == val) 
                    return false;
            }
        }

        return true;
    }

    /**
     * @brief Get tile value at position
     */
    uint16_t getValue(int row, int col) const
    {
        if (row >= 0 && row < SIZE && col >= 0 && col < SIZE)
            return grid[row][col];
        return 0;
    }

    /**
     * @brief Set tile value at position
     */
    void setValue(int row, int col, uint16_t value)
    {
        if (row >= 0 && row < SIZE && col >= 0 && col < SIZE)
            grid[row][col] = value;
    }

    /**
     * @brief Get grid size
     */
    static constexpr int getSize() { return SIZE; }

private:
    /**
     * @brief Update best score if current score is higher
     */
    void updateBestScore()
    {
        if (score > bestScore)
            bestScore = score;
    }
};

#endif // GAME_2048_ENGINE_HPP
