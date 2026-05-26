#ifndef PLANT_GRID_HPP
#define PLANT_GRID_HPP

#include "GridSystem.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <vector>

class Plant;

class PlantGrid {
public:
    bool CanPlace(int row, int col, const std::vector<int>& activeLanes) const {
        return GridSystem::IsValidCell(row, col) && !m_Plants[row][col] &&
               std::find(activeLanes.begin(), activeLanes.end(), row) !=
                   activeLanes.end();
    }

    void Place(int row, int col, std::shared_ptr<Plant> plant) {
        m_Plants[row][col] = std::move(plant);
    }

    std::shared_ptr<Plant> Remove(int row, int col) {
        if (!GridSystem::IsValidCell(row, col)) return nullptr;
        auto plant = m_Plants[row][col];
        m_Plants[row][col] = nullptr;
        return plant;
    }

    std::shared_ptr<Plant> At(int row, int col) const {
        if (!GridSystem::IsValidCell(row, col)) return nullptr;
        return m_Plants[row][col];
    }

    template <typename Fn>
    void ForEachPlant(Fn&& fn) const {
        for (int row = 0; row < GridSystem::ROWS; ++row) {
            for (int col = 0; col < GridSystem::COLS; ++col) {
                if (m_Plants[row][col]) {
                    fn(row, col, m_Plants[row][col]);
                }
            }
        }
    }

    std::vector<std::shared_ptr<Plant>> Clear() {
        std::vector<std::shared_ptr<Plant>> removed;
        for (auto& row : m_Plants) {
            for (auto& plant : row) {
                if (plant) {
                    removed.push_back(plant);
                    plant = nullptr;
                }
            }
        }
        return removed;
    }

private:
    std::array<std::array<std::shared_ptr<Plant>, GridSystem::COLS>,
               GridSystem::ROWS>
        m_Plants{};
};

#endif // PLANT_GRID_HPP
