#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>

class Particle {
public:
    double x, y; // Position
    double theta; // Initial angle in degrees (0 degrees is east, increases anticlockwise)
    double v; // Velocity in pixels per second
    double radius;

    Particle(double x, double y, double theta, double v, double radius)
        : x(x), y(y), theta(theta), v(v), radius(radius) {}

    void updatePosition(double deltaTime, double simWidth, double simHeight) {
        // Convert angle to radians
        double thetaRad = theta * 3.14159265358979323846 / 180.0;

        // Update position based on velocity and angle
        x += v * std::cos(thetaRad) * deltaTime;
        y += v * std::sin(thetaRad) * deltaTime;

        // Check for collisions with the simulation boundaries
        if (x - radius < 0 || x + radius > simWidth) { // Left or right wall collision
            // Reflect particle off the left or right wall
            theta = 180.0 - theta;

            // Ensure the angle is within the range [0, 360)
            if (theta < 0.0)
                theta += 360.0;

            x = std::max(radius, std::min(x, simWidth - radius)); // Correct position to stay within bounds
        }
        if (y - radius < 0 || y + radius > simHeight) { // Top or bottom wall collision
            // Reflect particle off the top or bottom wall
            theta = 360.0 - theta;

            // Ensure the angle is within the range [0, 360)
            if (theta >= 360.0)
                theta -= 360.0;

            y = std::max(radius, std::min(y, simHeight - radius)); // Correct position to stay within bounds
        }
    }
};

class Simulation {
    std::vector<Particle> particles;
    double width, height; // Simulation area dimensions

public:
    Simulation(double width, double height) : width(width), height(height) {}

    void addParticle(const Particle& particle) {
        particles.push_back(particle);
    }

    const std::vector<Particle>& getParticles() const {
        return particles;
    }

    void simulate(double deltaTime) {
        for (auto& particle : particles) {
            particle.updatePosition(deltaTime, width, height);
            // Boundary checks and collision responses are now handled within updatePosition
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Particle Simulator");
    Simulation sim(800, 600);
    //sim.addParticle(Particle(0, 0, 270, 10, 3));
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

       
        sim.simulate(0.01); // Advance the simulation

        // Input handling outside the event loop
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            double x, y, theta, v, radius;

            std::cout << "Enter x, y, theta, v, radius for the new particle: ";
            std::cin >> x >> y >> theta >> v >> radius;

            sim.addParticle(Particle(x, y, theta, v, radius));
        }

        window.clear();
        for (const auto& particle : sim.getParticles()) {
            sf::CircleShape shape(particle.radius);
            shape.setFillColor(sf::Color::Green);
            shape.setPosition(static_cast<float>(particle.x - particle.radius), static_cast<float>(particle.y - particle.radius));
            window.draw(shape);
        }
        window.display();
    }

    return 0;
}
