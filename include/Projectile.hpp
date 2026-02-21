#ifndef PROJECTILE_HPP
# define PROJECTILE_HPP

#include "AGameEntity.hpp"

class Projectile : public AGameEntity {
    private:
        int _colorPair;
    public:
        Projectile(float x, float y, float dx, float dy, int color);
        ~Projectile();

        void update(float dt, class Game &game) override;
        void render() const override;
};

#endif