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

        double angleRad = angle * (M_PI / 180.0); // Convert angle from degrees to radians

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

class Wall {
public:
    sf::Vector2f start, end;

    Wall(float x1, float y1, float x2, float y2) : start(x1, y1), end(x2, y2) {}
};


class Simulation {
    std::vector<Particle> particles;
    std::vector<Wall> walls;
    double width, height; // Simulation area dimensions

public:
    Simulation(double width, double height) : width(width), height(height) {}

    void addParticle(const Particle& particle) {
        particles.push_back(particle);
    }

    void addWall(const Wall& wall) {
        walls.push_back(wall);
    }

    void checkCollisionWithWalls(Particle& particle) {
        for (auto& wall : walls) {
            // Example for a horizontal or vertical wall. Extend logic for any wall orientation.
            sf::Vector2f D = wall.end - wall.start; // Directional vector of the wall
            sf::Vector2f N = { D.y, -D.x }; // Normal vector to the wall

            // Normalize N
            float length = std::sqrt(N.x * N.x + N.y * N.y);
            N.x /= length;
            N.y /= length;

            // Predict next position of the particle
            sf::Vector2f nextPos = sf::Vector2f(static_cast<float>(particle.x + particle.vx), static_cast<float>(particle.y + particle.vy));

            // Simple collision detection (extend this to accurately detect collision)
            // Assume collision occurs

            // Reflect velocity
            float dotProduct = particle.vx * N.x + particle.vy * N.y;
            particle.vx -= 2 * dotProduct * N.x;
            particle.vy -= 2 * dotProduct * N.y;
        }
    }

    const std::vector<Particle>& getParticles() const {
        return particles;
    }

    void simulate(double deltaTime) {
        for (auto& particle : particles) {
            particle.updatePosition(deltaTime, width, height);
            checkCollisionWithWalls(particle);
            // Boundary checks and collision responses are now handled within updatePosition
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Particle Simulator");

    tgui::Gui gui(window); // Initialize TGUI Gui object for the window

    // Create and configure widgets for X and Y position, angle, and velocity input
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

    auto toggleCheckbox = tgui::CheckBox::create();
    toggleCheckbox->setPosition("10%", "40%");
    toggleCheckbox->setText("Toggle Edit Boxes");
    gui.add(toggleCheckbox);

    auto renderer = toggleCheckbox->getRenderer();
    renderer->setTextColor(sf::Color::White);

    auto wallX1EditBox = tgui::EditBox::create();
    wallX1EditBox->setPosition("75%", "5%");
    wallX1EditBox->setSize("18%", "6%");
    wallX1EditBox->setDefaultText("X Position (0-1280)");
    gui.add(wallX1EditBox);

    auto wallY1EditBox = tgui::EditBox::create();
    wallY1EditBox->setPosition("75%", "12%");
    wallY1EditBox->setSize("18%", "6%");
    wallY1EditBox->setDefaultText("Y Position (0-720)");
    gui.add(wallY1EditBox);

    auto wallX2EditBox = tgui::EditBox::create();
    wallX2EditBox->setPosition("75%", "19%");
    wallX2EditBox->setSize("18%", "6%");
    wallX2EditBox->setDefaultText("X Position (0-1280)");
    gui.add(wallX2EditBox);

    auto wallY2EditBox = tgui::EditBox::create();
    wallY2EditBox->setPosition("75%", "26%");
    wallY2EditBox->setSize("18%", "6%");
    wallY2EditBox->setDefaultText("Y Position (0-720)");
    gui.add(wallY2EditBox);

    auto addWallButton = tgui::Button::create("Add Particle");
    addWallButton->setPosition("75%", "33%"); // Adjust the percentage as needed based on your layout
    addWallButton->setSize("18%", "6%");
    gui.add(addWallButton);


    Simulation sim(1280, 720);

    toggleCheckbox->onChange([&](bool checked) {
        if (checked) {
            // Hide the edit boxes
            xPosEditBox->setVisible(false);
            yPosEditBox->setVisible(false);
            angleEditBox->setVisible(false);
            velocityEditBox->setVisible(false);
            addButton->setVisible(false);
        }
        else {
            // Show the edit boxes
            xPosEditBox->setVisible(true);
            yPosEditBox->setVisible(true);
            angleEditBox->setVisible(true);
            velocityEditBox->setVisible(true);
            addButton->setVisible(true);
        }
     });
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

    addWallButton->onPress([&]() {
        try {
            float x1 = std::stof(wallX1EditBox->getText().toStdString());
            float y1 = std::stof(wallY1EditBox->getText().toStdString());
            float x2 = std::stof(wallX2EditBox->getText().toStdString());
            float y2 = std::stof(wallY2EditBox->getText().toStdString());

            sim.addWall(Wall(x1, y1, x2, y2));

            // Optionally reset the wall input fields
            wallX1EditBox->setText("");
            wallY1EditBox->setText("");
            wallX2EditBox->setText("");
            wallY2EditBox->setText("");
        }
        catch (const std::exception& e) {
            std::cerr << "Error adding wall: " << e.what() << std::endl;
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