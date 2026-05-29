#ifndef GRID_SYSTEM_HPP
#define GRID_SYSTEM_HPP

#include <glm/vec2.hpp>
#include <utility>

class GridSystem {
public:
    static constexpr int ROWS = 5;
    static constexpr int COLS = 9;
    static constexpr float CELL_WIDTH = 97.0f;
    static constexpr float CELL_HEIGHT = 125.0f;
    static constexpr float ORIGIN_X = -530.0f;
    static constexpr float ORIGIN_Y = -340.0f;

    static glm::vec2 CellToPosition(int row, int col) {
        return {
            ORIGIN_X + CELL_WIDTH * col + CELL_WIDTH / 2.0f,
            ORIGIN_Y + CELL_HEIGHT * row + CELL_HEIGHT / 2.0f
        };
    }

    static std::pair<int, int> PositionToCell(const glm::vec2& pos) {
        int col = static_cast<int>((pos.x - ORIGIN_X) / CELL_WIDTH);
        int row = static_cast<int>((pos.y - ORIGIN_Y) / CELL_HEIGHT);
        return {row, col};
    }

    static bool IsValidCell(int row, int col) {
        return row >= 0 && row < ROWS && col >= 0 && col < COLS;
    }
};

#endif // GRID_SYSTEM_HPP
