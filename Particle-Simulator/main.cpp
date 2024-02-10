#include <SFML/Graphics.hpp>
#include <vector>

class Particle {
public:
    double x, y; // Position
    double vx, vy; // Velocity
    double radius;

    Particle(double x, double y, double vx, double vy, double radius)
        : x(x), y(y), vx(vx), vy(vy), radius(radius) {}

    void updatePosition(double deltaTime, double simWidth, double simHeight) {
        // Update position based on velocity
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
    sf::RenderWindow window(sf::VideoMode(800, 600), "Particle Simulator");
    Simulation sim(800, 600);

    // Example: Add a particle
    sim.addParticle(Particle(400, 300, -2, -2, 10));

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