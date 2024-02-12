#include <SFML/Graphics.hpp>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Particle {
public:
    double x, y; // Position
    double vx, vy; // Velocity components calculated from angle and velocity
    double radius;

    // Constructor now takes angle (in degrees) and velocity, along with position and radius
    Particle(double x, double y, double angle, double velocity, double radius)
        : x(x), y(y), radius(radius) {
        // Convert angle to radians for trigonometry functions
        double angleRad = angle * (M_PI / 180.0);
        // Calculate velocity components based on angle and velocity
        vx = velocity * cos(angleRad);
        vy = -velocity * sin(angleRad); // Negative since SFML's y-axis increases downwards
    }

    void updatePosition(double deltaTime, double simWidth, double simHeight) {
        x += vx * deltaTime;
        y += vy * deltaTime;

        // Check for collisions with the simulation boundaries
        if (x - radius < 0 || x + radius > simWidth) { // Left or right wall collision
            vx = -vx; // Invert velocity in x-direction
            x = (x - radius < 0) ? radius : simWidth - radius; // Correct position to stay within bounds
        }
        if (y - radius < 0 || y + radius > simHeight) { // Top or bottom wall collision
            vy = -vy; // Invert velocity in y-direction
            y = (y - radius < 0) ? radius : simHeight - radius; // Correct position to stay within bounds
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
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Particle Simulator");
    Simulation sim(1280, 720);

    // Example:
    sim.addParticle(Particle(400, 300, 5, 10, 10)); // 5 degrees, 10 pixels/second velocity


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sim.simulate(0.01); // Advance the simulation

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