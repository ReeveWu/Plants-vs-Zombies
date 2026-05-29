#ifndef PEASHOOTER_HPP
#define PEASHOOTER_HPP

#include "entities/Plant.hpp"

class Peashooter : public Plant {
public:
    Peashooter(int row, int col)
        : Plant(MakeFrames("peashooter", 24), 1, row, col, 300, 100) {}

    bool CanShoot() const override { return true; }
    int GetDamage() const override { return 20; }
};

#endif // PEASHOOTER_HPP
