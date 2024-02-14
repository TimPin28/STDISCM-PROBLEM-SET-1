#include "Simulation.h"

Simulation::Simulation(double width, double height, size_t threadCount)
    : width(width), height(height), pool(threadCount) {}

void Simulation::addParticle(const Particle& particle) {
    particles.push_back(particle);
}

void Simulation::addWall(const Wall& wall) {
    walls.push_back(wall);
}

void Simulation::simulate(double deltaTime) {
    for (auto& particle : particles) {
        // Single Threaded Version
        /*particle.updatePosition(deltaTime, width, height);
        checkCollisionWithWalls(particle);*/

        // Multi-threaded Version
        pool.enqueue([&particle, deltaTime, this] {
            particle.updatePosition(deltaTime, this->width, this->height);
            this->checkCollisionWithWalls(particle);
            });
    }
}

const std::vector<Particle>& Simulation::getParticles() const {
    return particles;
}

const std::vector<Wall>& Simulation::getWalls() const {
    return walls;
}

float Simulation::Min(float a, float b) {
    return (a < b) ? a : b;
}

float Simulation::Max(float a, float b) {
    return (a > b) ? a : b;
}

float Simulation::dot(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}

void Simulation::checkCollisionWithWalls(Particle& particle) {
    for (auto& wall : walls) {
        sf::Vector2f D = wall.end - wall.start; // Directional vector of the wall
        sf::Vector2f N = { D.y, -D.x }; // Normal vector to the wall

        // Normalize N
        float length = sqrt(N.x * N.x + N.y * N.y);
        N.x /= length;
        N.y /= length;

        // Predict next position of the particle
        sf::Vector2f nextPos(particle.x + particle.vx, particle.y + particle.vy);

        if (collisionDetected(particle, nextPos, wall)) {
            // Calculate the original speed
            float originalSpeed = sqrt(particle.vx * particle.vx + particle.vy * particle.vy);

            // Reflect velocity
            float dotProduct = particle.vx * N.x + particle.vy * N.y;
            particle.vx -= 2 * dotProduct * N.x;
            particle.vy -= 2 * dotProduct * N.y;

            // Normalize the reflected velocity vector
            float reflectedSpeed = sqrt(particle.vx * particle.vx + particle.vy * particle.vy);
            particle.vx = (particle.vx / reflectedSpeed) * originalSpeed;
            particle.vy = (particle.vy / reflectedSpeed) * originalSpeed;
        }
    }
}


bool Simulation::collisionDetected(const Particle& particle, const sf::Vector2f& nextPos, const Wall& wall) {

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