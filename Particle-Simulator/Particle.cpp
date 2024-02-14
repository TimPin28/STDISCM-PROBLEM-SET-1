#include "Particle.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor implementation
Particle::Particle(double x, double y, double angle, double velocity, double radius)
    : x(x), y(y), radius(radius) {
    double angleRad = angle * (M_PI / 180.0);
    vx = velocity * cos(angleRad);
    vy = -velocity * sin(angleRad); // Negative since SFML's y-axis increases downwards
}

// Method to update particle's position
void Particle::updatePosition(double deltaTime, double simWidth, double simHeight) {
    x += vx * deltaTime;
    y += vy * deltaTime;

    // Collision with simulation boundaries
    if (x - radius < 0 || x + radius > simWidth) {
        vx = -vx;
        x = (x - radius < 0) ? radius : simWidth - radius;
    }
    if (y - radius < 0 || y + radius > simHeight) {
        vy = -vy;
        y = (y - radius < 0) ? radius : simHeight - radius;
    }
}
