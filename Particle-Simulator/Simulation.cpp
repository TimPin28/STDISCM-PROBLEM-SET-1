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
            particle.updatePosition(deltaTime, this->width, this->height, this->walls);
        });
    }
}

const std::vector<Particle>& Simulation::getParticles() const {
    return particles;
}

const std::vector<Wall>& Simulation::getWalls() const {
    return walls;
}