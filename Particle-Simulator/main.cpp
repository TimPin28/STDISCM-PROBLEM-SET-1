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

        // Calculate velocity components based on angle and velocity
        // print cos(angle) and sin(angle)
        std::cout << "cos(angle): " << cos(angle) << std::endl;
        std::cout << "sin(angle): " << sin(angle) << std::endl;
        std::cout << "Velocity: " << velocity << std::endl;

        double angleRad = angle * (M_PI / 180.0);

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

    // Create and configure widgets for X and Y position input
    auto xPosEditBox = tgui::EditBox::create();
    xPosEditBox->setPosition("10%", "5%");
    xPosEditBox->setSize("18%", "6%");
    xPosEditBox->setDefaultText("X Position (0-1280)");
    gui.add(xPosEditBox);

    auto yPosEditBox = tgui::EditBox::create();
    yPosEditBox->setPosition("10%", "12%");
    yPosEditBox->setSize("18%", "6%");
    yPosEditBox->setDefaultText("Y Position (0-720)");
    gui.add(yPosEditBox);

    // Create and configure widgets for user input, e.g., for angle and velocity
    auto angleEditBox = tgui::EditBox::create();
    angleEditBox->setPosition("10%", "19%");
    angleEditBox->setSize("18%", "6%");
    angleEditBox->setDefaultText("Angle (0-360)");
    gui.add(angleEditBox);

    auto velocityEditBox = tgui::EditBox::create();
    velocityEditBox->setPosition("10%", "26%");
    velocityEditBox->setSize("18%", "6%");
    velocityEditBox->setDefaultText("Velocity (pixels/sec)");
    gui.add(velocityEditBox);

    auto addButton = tgui::Button::create("Add Particle");
    addButton->setPosition("10%", "33%"); // Adjust the percentage as needed based on your layout
    addButton->setSize("18%", "6%");
    gui.add(addButton);

    Simulation sim(1280, 720);

    //sim.addParticle(Particle(640, 360, 45, 100, 10)); // Add a particle at the center of the window

    // Attach an event handler to the "Add Particle" button
    addButton->onPress([&]() {
        try {
            std::string angleStr = angleEditBox->getText().toStdString();
            std::string velocityStr = velocityEditBox->getText().toStdString();
            std::string xPosStr = xPosEditBox->getText().toStdString();
            std::string yPosStr = yPosEditBox->getText().toStdString();

            // print velocityStr and angleStr
            std::cout << "Velocity: " << velocityStr << std::endl;

            double angle = std::stod(angleStr);
            double velocity = std::stod(velocityStr);
            double xPosition = std::stod(xPosStr);
            double yPosition = std::stod(yPosStr);

            // Add particle at the specified position
            sim.addParticle(Particle(xPosition, yPosition, angle, velocity, 10));

            // Reset the input values
            angleEditBox->setText("");
            velocityEditBox->setText("");
            xPosEditBox->setText("");
            yPosEditBox->setText("");
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid input. Please enter numerical values.\n";
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Input is out of range.\n";
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