#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>
#include "Wall.h"
#include <cmath>

class Particle {
public:
    double x, y; // Position
    double vx, vy; // Velocity components
    double radius; // Radius of the particle

    // Constructor
    Particle(double x, double y, double angle, double velocity, double radius);

    // Method to update particle's position
    void updatePosition(double deltaTime, double simWidth, double simHeight, const std::vector<Wall>& walls);

private:

    bool collisionDetected(const Particle& particle, const sf::Vector2f& nextPos, const Wall& wall);

    float Min(float a, float b);

    float Max(float a, float b);

    float dot(const sf::Vector2f& a, const sf::Vector2f& b);
};

#endif // PARTICLE_H
#pragma once
