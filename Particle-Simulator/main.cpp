#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widget.hpp>
#include <TGUI/String.hpp>
#include <iostream>
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

    tgui::Gui gui(window); // Initialize TGUI Gui object for the window

    // Create and configure widgets for user input, e.g., for angle and velocity
    auto angleEditBox = tgui::EditBox::create();
    angleEditBox->setPosition("10%", "5%");
    angleEditBox->setSize("18%", "6%");
    angleEditBox->setDefaultText("Angle (0-360)");
    gui.add(angleEditBox);

    auto velocityEditBox = tgui::EditBox::create();
    velocityEditBox->setPosition("10%", "12%");
    velocityEditBox->setSize("18%", "6%");
    velocityEditBox->setDefaultText("Velocity (pixels/sec)");
    gui.add(velocityEditBox);

    auto addButton = tgui::Button::create("Add Particle");
    addButton->setPosition("10%", "19%");
    addButton->setSize("18%", "6%");
    gui.add(addButton);

    Simulation sim(1280, 720);

    // Attach an event handler to the "Add Particle" button
    addButton->onPress([&]() {
        try {
            // Convert tgui::String to std::string directly
            std::string angleStr = angleEditBox->getText().toStdString();
            std::string velocityStr = velocityEditBox->getText().toStdString();

            // Then convert std::string to double
            double angle = std::stod(angleStr);
            double velocity = std::stod(velocityStr);

            // Convert angle to radians and calculate velocity components
            double angleRad = angle * (M_PI / 180.0);
            double vx = velocity * cos(angleRad);
            double vy = -velocity * sin(angleRad); // Assuming SFML's coordinate system

            // Add particle at a fixed position for demonstration
            sim.addParticle(Particle(640, 360, vx, vy, 10)); // Center of the window
        }
        catch (const std::invalid_argument& e) {
            // Handle the case where the conversion from string to double fails
            std::cerr << "Invalid input for angle or velocity. Please enter numerical values.\n";
        }
        catch (const std::out_of_range& e) {
            // Handle the case where the input numbers are too large
            std::cerr << "Input for angle or velocity is out of range.\n";
        }
        });


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            gui.handleEvent(event); // Pass events to the GUI

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
        gui.draw(); // Draw the GUI
        window.display();
    }

    return 0;
}