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

    const std::vector<Wall>& getWalls() const {
        return walls;
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

    // Check box to toggle visibility of input fields
    auto toggleCheckbox = tgui::CheckBox::create();
    toggleCheckbox->setPosition("10%", "1%");
    toggleCheckbox->setText("Toggle Edit Boxes");
    gui.add(toggleCheckbox);

    auto renderer = toggleCheckbox->getRenderer();
    renderer->setTextColor(sf::Color::White);

    // Widgets for input fields

    // Particle Input Form 1
    auto noParticles1 = tgui::EditBox::create();
    noParticles1->setPosition("10%", "5%");
    noParticles1->setSize("18%", "6%");
    noParticles1->setDefaultText("Number of Particles");
    gui.add(noParticles1);

    auto X1PosEditBox = tgui::EditBox::create();
    X1PosEditBox->setPosition("10%", "12%");
    X1PosEditBox->setSize("18%", "6%");
    X1PosEditBox->setDefaultText("X1 Coordinate");
    gui.add(X1PosEditBox);

    auto Y1PosEditBox = tgui::EditBox::create();
    Y1PosEditBox->setPosition("10%", "19%");
    Y1PosEditBox->setSize("18%", "6%");
    Y1PosEditBox->setDefaultText("Y1 Coordinate");
    gui.add(Y1PosEditBox);

    auto X2PosEditBox = tgui::EditBox::create();
    X2PosEditBox->setPosition("10%", "26%");
    X2PosEditBox->setSize("18%", "6%");
    X2PosEditBox->setDefaultText("X2 Coordinate");
    gui.add(X2PosEditBox);

    auto Y2PosEditBox = tgui::EditBox::create();
    Y2PosEditBox->setPosition("10%", "33%");
    Y2PosEditBox->setSize("18%", "6%");
    Y2PosEditBox->setDefaultText("Y2 Coordinate");
    gui.add(Y2PosEditBox);

    auto addButton1 = tgui::Button::create("Add Particle");
    addButton1->setPosition("10%", "40%"); // Adjust the percentage as needed based on your layout
    addButton1->setSize("18%", "6%");
    gui.add(addButton1);

    // Wall Input Form 
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

    //Checkbox event handler
    toggleCheckbox->onChange([&](bool checked) {
        if (checked) {
            // Hide the input fields
            noParticles1->setVisible(false);
            X1PosEditBox->setVisible(false);
            Y1PosEditBox->setVisible(false);
            X2PosEditBox->setVisible(false);
            Y2PosEditBox->setVisible(false);

            wallX1EditBox->setVisible(false);
            wallY1EditBox->setVisible(false);
            wallX2EditBox->setVisible(false);
            wallY2EditBox->setVisible(false);
            addWallButton->setVisible(false);
        }
        else {
            // Show the input boxes
            noParticles1->setVisible(true);
            X1PosEditBox->setVisible(true);
            Y1PosEditBox->setVisible(true);
            X2PosEditBox->setVisible(true);
            Y2PosEditBox->setVisible(true);

            wallX1EditBox->setVisible(true);
            wallY1EditBox->setVisible(true);
            wallX2EditBox->setVisible(true);
            wallY2EditBox->setVisible(true);
            addWallButton->setVisible(true);
        }
     });
    //sim.addParticle(Particle(640, 360, 45, 100, 10)); // Add a particle at the center of the window

    // Attach an event handler to the "Add Particle" button for Form 1
    addButton1->onPress([&]() {
        try {
            std::string noParticlesStr = noParticles1->getText().toStdString(); 
            std::string x1Str = X1PosEditBox->getText().toStdString();
            std::string y1Str = Y1PosEditBox->getText().toStdString();
            std::string x2Str = X2PosEditBox->getText().toStdString();
            std::string y2Str = Y2PosEditBox->getText().toStdString();

            int noParticles = std::stoi(noParticlesStr);
            float x1 = std::stoi(x1Str);
            float y1 = std::stoi(y1Str);
            float x2 = std::stoi(x2Str);
            float y2 = std::stoi(y2Str);

            float velocity = 5;
            float angle = 45;

            // Add particle at the specified position
            sim.addParticle(Particle(xPosition, yPosition, angle, velocity, 10));
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid input. Please enter numerical values.\n";
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Input is out of range.\n";
        }
        });

    // Attach an event handler to the "Add Wall" button
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
        // Draw walls
        for (const auto& wall : sim.getWalls()) {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = sf::Vector2f(wall.start.x, wall.start.y);
            line[0].color = sf::Color::White;
            line[1].position = sf::Vector2f(wall.end.x, wall.end.y);
            line[1].color = sf::Color::White;
            window.draw(line);
        }

        gui.draw(); // Draw the GUI
        window.display();
    }

    return 0;
}