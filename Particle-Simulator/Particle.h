#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>
#include <cmath>

class Particle {
public:
    double x, y; // Position
    double vx, vy; // Velocity components
    double radius; // Radius of the particle

    // Constructor
    Particle(double x, double y, double angle, double velocity, double radius);

    // Method to update particle's position
    void updatePosition(double deltaTime, double simWidth, double simHeight);
};

#endif // PARTICLE_H
#pragma once
