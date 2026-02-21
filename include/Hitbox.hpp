// Hitbox.hpp
#pragma once

struct Hitbox {
    int x, y, width, height;
};

bool collides(const Hitbox &a, const Hitbox &b);
