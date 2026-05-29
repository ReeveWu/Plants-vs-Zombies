#ifndef ICESHOOTER_HPP
#define ICESHOOTER_HPP

#include "entities/Plant.hpp"

class IceShooter : public Plant {
public:
    IceShooter(int row, int col)
        : Plant(MakeFrames("iceshooter", 15), 6, row, col, 300, 100) {}

    bool CanShoot() const override { return true; }
    bool IsIce() const override { return true; }
    int GetDamage() const override { return 20; }
};

#endif // ICESHOOTER_HPP
