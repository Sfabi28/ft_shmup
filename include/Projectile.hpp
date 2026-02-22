#pragma once

#include "AGameEntity.hpp"

class Projectile : public AGameEntity {
    public:
        Projectile(float x, float y, float dx, float dy, int color, Team team);
        ~Projectile();

        void update(float dt, class Game &game) override;
};
