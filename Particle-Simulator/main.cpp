#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widget.hpp>
#include <TGUI/String.hpp>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <sstream>
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

bool collisionDetected(const Particle& particle, const sf::Vector2f& nextPos, const Wall& wall) {
    // Convert line segments to a general form Ax + By = C
    float A1 = nextPos.y - static_cast<float>(particle.y);
    float B1 = static_cast<float>(particle.x) - nextPos.x;
    float C1 = A1 * static_cast<float>(particle.x) + B1 * static_cast<float>(particle.y);

    float A2 = wall.end.y - wall.start.y;
    float B2 = wall.start.x - wall.end.x;
    float C2 = A2 * wall.start.x + B2 * wall.start.y;

    // Calculate the intersection point
    float det = A1 * B2 - A2 * B1;
    if (fabs(det) < 1e-9) { // Lines are parallel
        return false;
    }
    else {
        float x = (B2 * C1 - B1 * C2) / det;
        float y = (A1 * C2 - A2 * C1) / det;

        // Check if the intersection point is within both line segments
        bool onParticlePath = std::min(static_cast<float>(particle.x), nextPos.x) <= x && x <= std::max(static_cast<float>(particle.x), nextPos.x) &&
            std::min(static_cast<float>(particle.y), nextPos.y) <= y && y <= std::max(static_cast<float>(particle.y), nextPos.y);
        bool onWall = std::min(wall.start.x, wall.end.x) <= x && x <= std::max(wall.start.x, wall.end.x) &&
            std::min(wall.start.y, wall.end.y) <= y && y <= std::max(wall.start.y, wall.end.y);

        return onParticlePath && onWall;
    }
}

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
            sf::Vector2f D = wall.end - wall.start; // Directional vector of the wall
            sf::Vector2f N = { D.y, -D.x }; // Normal vector to the wall, assuming wall.end and wall.start are sf::Vector2f

            // Normalize N
            float length = std::sqrt(N.x * N.x + N.y * N.y);
            N.x /= length;
            N.y /= length;

            // Predict next position of the particle
            sf::Vector2f nextPos(particle.x + particle.vx, particle.y + particle.vy);

            // Implement actual collision detection here
            // If a collision is detected:
            if (collisionDetected(particle, nextPos, wall)) {
                // Step 1: Calculate the original speed
                float originalSpeed = std::sqrt(particle.vx * particle.vx + particle.vy * particle.vy);

                // Step 2: Reflect velocity
                float dotProduct = particle.vx * N.x + particle.vy * N.y;
                particle.vx -= 2 * dotProduct * N.x;
                particle.vy -= 2 * dotProduct * N.y;

                // Step 3: Normalize the reflected velocity vector
                float reflectedSpeed = std::sqrt(particle.vx * particle.vx + particle.vy * particle.vy);
                particle.vx = (particle.vx / reflectedSpeed) * originalSpeed;
                particle.vy = (particle.vy / reflectedSpeed) * originalSpeed;
            }

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

    // Set the frame rate limit
    window.setFramerateLimit(60);
    
    sf::Clock clock; // Starts the clock for FPS calculation
    sf::Clock displayClock; // Separate clock for controlling display update rate
    unsigned int frames = 0;
    unsigned int fps = 0;

    sf::Font font;
    if (!font.loadFromFile("OpenSans-Regular.ttf")) {
        std::cerr << "Could not load font\n";
        return -1;
    }

    sf::Text fpsText("", font, 20);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(5.f, 5.f); // Position the FPS counter in the top-left corner

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

    auto addButton1 = tgui::Button::create("Add Batch Particle 1");
    addButton1->setPosition("10%", "40%"); // Adjust the percentage as needed based on your layout
    addButton1->setSize("18%", "6%");
    gui.add(addButton1);

    // Particle Input Form 2
    auto noParticles2 = tgui::EditBox::create();
    noParticles2->setPosition("30%", "5%");
    noParticles2->setSize("18%", "6%");
    noParticles2->setDefaultText("Number of Particles");
    gui.add(noParticles2);

    auto startAngleEditBox = tgui::EditBox::create();
    startAngleEditBox->setPosition("30%", "12%");
    startAngleEditBox->setSize("18%", "6%");
    startAngleEditBox->setDefaultText("Start Angle");
    gui.add(startAngleEditBox);

    auto endAngleEditBox = tgui::EditBox::create();
    endAngleEditBox->setPosition("30%", "19%");
    endAngleEditBox->setSize("18%", "6%");
    endAngleEditBox->setDefaultText("End Angle");
    gui.add(endAngleEditBox);

    auto addButton2 = tgui::Button::create("Add Batch Particle 2");
    addButton2->setPosition("30%", "40%"); // Adjust the percentage as needed based on your layout
    addButton2->setSize("18%", "6%");
    gui.add(addButton2);

    // Particle Input Form 3
    auto noParticles3 = tgui::EditBox::create();
    noParticles3->setPosition("50%", "5%");
    noParticles3->setSize("18%", "6%");
    noParticles3->setDefaultText("Number of Particles");
    gui.add(noParticles3);

    auto startVelocityEditBox = tgui::EditBox::create();
    startVelocityEditBox->setPosition("50%", "12%");
    startVelocityEditBox->setSize("18%", "6%");
    startVelocityEditBox->setDefaultText("Start Velocity");
    gui.add(startVelocityEditBox);

    auto endVelocityEditBox = tgui::EditBox::create();
    endVelocityEditBox->setPosition("50%", "19%");
    endVelocityEditBox->setSize("18%", "6%");
    endVelocityEditBox->setDefaultText("End Velocity");
    gui.add(endVelocityEditBox);

    auto addButton3 = tgui::Button::create("Add Batch Particle 3");
    addButton3->setPosition("50%", "40%"); // Adjust the percentage as needed based on your layout
    addButton3->setSize("18%", "6%");
    gui.add(addButton3);

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

    auto addWallButton = tgui::Button::create("Add Wall");
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
            addButton1->setVisible(false);

            noParticles2->setVisible(false);
            startAngleEditBox->setVisible(false);
            endAngleEditBox->setVisible(false);
            addButton2->setVisible(false);

            noParticles3->setVisible(false);
            startVelocityEditBox->setVisible(false);
            endVelocityEditBox->setVisible(false);
            addButton3->setVisible(false);

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
            addButton1->setVisible(true);

            noParticles2->setVisible(true);
            startAngleEditBox->setVisible(true);
            endAngleEditBox->setVisible(true);
            addButton2->setVisible(true);

            noParticles3->setVisible(true);
            startVelocityEditBox->setVisible(true);
            endVelocityEditBox->setVisible(true);
            addButton3->setVisible(true);

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
            int n = std::stoi(noParticles1->getText().toStdString()); // Number of particles
            float x1 = std::stof(X1PosEditBox->getText().toStdString()); // Start X coordinate
            float y1 = std::stof(Y1PosEditBox->getText().toStdString()); // Start Y coordinate
            float x2 = std::stof(X2PosEditBox->getText().toStdString()); // End X coordinate
            float y2 = std::stof(Y2PosEditBox->getText().toStdString()); // End Y coordinate

            float velocity = 100.0f; // Assuming a constant velocity value
            float angle = 0.0f; // Assuming a constant angle in degrees

            if (n <= 0) throw std::invalid_argument("Number of particles must be positive.");
            float xStep = (x2 - x1) / std::max(1, n - 1); // Calculate the x step between particles
            float yStep = (y2 - y1) / std::max(1, n - 1); // Calculate the y step between particles

            for (int i = 0; i < n; ++i) {
                float xPos = x1 + i * xStep; // Calculate the x position for each particle
                float yPos = y1 + i * yStep; // Calculate the y position for each particle

                // Add each particle to the simulation
                sim.addParticle(Particle(xPos, yPos, angle, velocity, 10)); // Assume radius is 10
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid input: " << e.what() << '\n';
            // Optionally, display an error message in the GUI
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Input out of range: " << e.what() << '\n';
            // Optionally, display an error message in the GUI
        }
        });


    // Attach an event handler to the "Add Particle" button for Form 2
    addButton2->onPress([&]() {
        // Ensure you've defined or have access to sim, which is your Simulation instance
        try {
            int n = std::stoi(noParticles2->getText().toStdString()); // Number of particles
            float startTheta = std::stof(startAngleEditBox->getText().toStdString()); // Start angle in degrees
            float endTheta = std::stof(endAngleEditBox->getText().toStdString()); // End angle in degrees

            // Assuming constant velocity and a starting point for all particles
            float velocity = 100.0f; // Example velocity
            sf::Vector2f startPoint(640, 360); // Example start point, adjust as needed

            if (n <= 0) throw std::invalid_argument("Number of particles must be positive.");
            float angularStep = (n > 1) ? (endTheta - startTheta) / (n - 1) : 0;

            for (int i = 0; i < n; ++i) {
                float angle = startTheta + i * angularStep; // Calculate the angle for each particle
                double angleRad = angle * (M_PI / 180.0); // Convert angle from degrees to radians

                // Calculate velocity components based on angle
                double vx = velocity * std::cos(angleRad);
                double vy = -velocity * std::sin(angleRad); // SFML's y-axis increases downwards

                // Add each particle to the simulation
                sim.addParticle(Particle(startPoint.x, startPoint.y, angle, velocity, 10)); // Assume radius is 10
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid input: " << e.what() << '\n';
            // Optionally, display an error message in the GUI
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Input out of range: " << e.what() << '\n';
            // Optionally, display an error message in the GUI
        }
        });


    // Attach an event handler to the "Add Particle" button for Form 3
    addButton3->onPress([&]() {
        try {
            std::string noParticlesStr = noParticles3->getText().toStdString();
            std::string startVelocityStr = startVelocityEditBox->getText().toStdString();
            std::string endVelocityStr = endVelocityEditBox->getText().toStdString();

            int noParticles = std::stoi(noParticlesStr);
            float startVelocity = std::stoi(startVelocityStr);
            float endVelocity = std::stoi(endVelocityStr);

            float xPosition = 400;
            float yPosition = 400;
            float angle = 225;

            // Add particle at the specified position
            //sim.addParticle(Particle(xPosition, yPosition, angle, velocity, 10));
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

            // Check if the coordinates are within the simulation boundaries
            if (x1 < 0 || x1 > 1280 || y1 < 0 || y1 > 720 ||
                x2 < 0 || x2 > 1280 || y2 < 0 || y2 > 720) {
                throw std::invalid_argument("Wall coordinates are out of bounds.");
            }

            // Check if the wall endpoints are distinct
            if (x1 == x2 && y1 == y2) {
                throw std::invalid_argument("Wall start and end points cannot be the same.");
            }

            sim.addWall(Wall(x1, y1, x2, y2));

            // Reset the wall input fields
            wallX1EditBox->setText("");
            wallY1EditBox->setText("");
            wallX2EditBox->setText("");
            wallY2EditBox->setText("");
        }

        catch (const std::invalid_argument& e) {
            std::cerr << "Error adding wall: " << e.what() << std::endl;
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Input is out of range: " << e.what() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "An error occurred: " << e.what() << std::endl;
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

        frames++; // Increment frame count for this second

        // Calculate FPS
        if (displayClock.getElapsedTime().asSeconds() >= 0.5f) {
            fps = static_cast<unsigned int>(frames / displayClock.getElapsedTime().asSeconds());
            frames = 0;
            displayClock.restart();

            // Update the FPS text
            std::stringstream ss;
            ss << "FPS: " << fps;
            fpsText.setString(ss.str());
        }

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

        window.draw(fpsText); // Draw the FPS counter on the window
        gui.draw(); // Draw the GUI
        window.display();
        
    }

    return 0;
}