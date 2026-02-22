// Hitbox.hpp
#pragma once

struct Hitbox {
    float x, y, width, height;
};

bool collides(const Hitbox &a, const Hitbox &b);
