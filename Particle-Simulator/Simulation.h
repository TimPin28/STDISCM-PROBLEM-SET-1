#ifndef SIMULATION_H
#define SIMULATION_H

#include "Particle.h" 
#include "Wall.h" 
#include "ThreadPool.h" 
#include <vector>
#include <SFML/Graphics.hpp> // For sf::Vector2f

class Simulation {
public:
    Simulation(double width, double height, size_t threadCount);

    void addParticle(const Particle& particle);
    void addWall(const Wall& wall);
    void simulate(double deltaTime);

    const std::vector<Particle>& getParticles() const;
    const std::vector<Wall>& getWalls() const;

private:
    std::vector<Particle> particles;
    std::vector<Wall> walls;
    double width, height;
    ThreadPool pool;
};

#endif // SIMULATION_H
