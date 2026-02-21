#include "Hitbox.hpp"

bool collides(const Hitbox &a, const Hitbox &b) {
    return a.x < b.x + b.width  && a.x + a.width  > b.x
        && a.y < b.y + b.height && a.y + a.height > b.y;
}