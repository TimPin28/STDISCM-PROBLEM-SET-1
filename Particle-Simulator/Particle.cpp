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
void Particle::updatePosition(double deltaTime, double simWidth, double simHeight, const std::vector<Wall>& walls) {
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

    for (auto& wall : walls) {
        sf::Vector2f D = wall.end - wall.start; // Directional vector of the wall
        sf::Vector2f N = { D.y, -D.x }; // Normal vector to the wall

        // Normalize N
        float length = sqrt(N.x * N.x + N.y * N.y);
        N.x /= length;
        N.y /= length;

        // Predict next position of the particle
        sf::Vector2f nextPos(x + vx, y + vy);

        if (collisionDetected(*this, nextPos, wall)) {
            // Calculate the original speed
            float originalSpeed = sqrt(vx * vx + vy * vy);

            // Reflect velocity
            float dotProduct = vx * N.x + vy * N.y;
            vx -= 2 * dotProduct * N.x;
            vy -= 2 * dotProduct * N.y;

            // Normalize the reflected velocity vector
            float reflectedSpeed = sqrt(vx * vx + vy * vy);
            vx = (vx / reflectedSpeed) * originalSpeed;
            vy = (vy / reflectedSpeed) * originalSpeed;
        }
    }
}

float Particle::Min(float a, float b) {
    return (a < b) ? a : b;
}

float Particle::Max(float a, float b) {
    return (a > b) ? a : b;
}

float Particle::dot(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}


bool Particle::collisionDetected(const Particle& particle, const sf::Vector2f& nextPos, const Wall& wall) {

    sf::Vector2f wallVector = wall.end - wall.start;
    sf::Vector2f particlePosition(nextPos.x, nextPos.y);
    sf::Vector2f wallStartToPoint = particlePosition - wall.start;

    float wallLengthSquared = dot(wallVector, wallVector);
    float t = Max(0.f, Min(1.f, dot(wallStartToPoint, wallVector) / wallLengthSquared));
    sf::Vector2f projection = wall.start + t * wallVector;

    sf::Vector2f distVector = particlePosition - projection;
    float distance = sqrt(distVector.x * distVector.x + distVector.y * distVector.y);

    return distance <= particle.radius;
}
