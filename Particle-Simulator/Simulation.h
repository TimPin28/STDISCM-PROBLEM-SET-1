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

    void checkCollisionWithWalls(Particle& particle);

    bool collisionDetected(const Particle& particle, const sf::Vector2f& nextPos, const Wall& wall);

    float Min(float a, float b);

    float Max(float a, float b);

    float dot(const sf::Vector2f& a, const sf::Vector2f& b);
};

#endif // SIMULATION_H
