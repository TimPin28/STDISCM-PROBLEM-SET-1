#ifndef WALL_H
#define WALL_H

#include <SFML/Graphics.hpp>

class Wall {
public:
    sf::Vector2f start, end;

    Wall(float x1, float y1, float x2, float y2);

};

#endif // WALL_H
#pragma once
