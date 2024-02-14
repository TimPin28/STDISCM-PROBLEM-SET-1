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
#include <thread>
#include <future>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "Particle.h" 
#include "Wall.h" 
#include "ThreadPool.h" 
#include "Simulation.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float Min(float a, float b) {
    return (a < b) ? a : b;
}

float Max(float a, float b) {
    return (a > b) ? a : b;
}

float dot(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Particle Simulator");

    ThreadPool pool(std::thread::hardware_concurrency()); // Use the number of concurrent threads supported by the hardware

    // Set the frame rate limit
    window.setFramerateLimit(60);
    
    sf::Clock clock; // Starts the clock for FPS calculation  
    sf::Clock fpsUpdateClock; // Clock to update the FPS counter every 0.5 seconds

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

    // Basic Particle Input 
    auto basicX1PosEditBox = tgui::EditBox::create();
    basicX1PosEditBox->setPosition("75%", "5%");
    basicX1PosEditBox->setSize("18%", "6%");
    basicX1PosEditBox->setDefaultText("X1 Coordinate");
    gui.add(basicX1PosEditBox);

    auto basicY1PosEditBox = tgui::EditBox::create();
    basicY1PosEditBox->setPosition("75%", "12%");
    basicY1PosEditBox->setSize("18%", "6%");
    basicY1PosEditBox->setDefaultText("Y1 Coordinate");
    gui.add(basicY1PosEditBox);

    auto basicAngleEditBox = tgui::EditBox::create();
    basicAngleEditBox->setPosition("75%", "19%");
    basicAngleEditBox->setSize("18%", "6%");
    basicAngleEditBox->setDefaultText("Angle Directon");
    gui.add(basicAngleEditBox);

    auto basicVelocityEditBox = tgui::EditBox::create();
    basicVelocityEditBox->setPosition("75%", "26%");
    basicVelocityEditBox->setSize("18%", "6%");
    basicVelocityEditBox->setDefaultText("Velocity");
    gui.add(basicVelocityEditBox);

    auto basicaddButton = tgui::Button::create("Add Particle");
    basicaddButton->setPosition("75%", "40%"); // Adjust the percentage as needed based on your layout
    basicaddButton->setSize("18%", "6%");
    gui.add(basicaddButton);

    // Wall Input Form 
    auto wallX1EditBox = tgui::EditBox::create();
    wallX1EditBox->setPosition("75%", "47%");
    wallX1EditBox->setSize("18%", "6%");
    wallX1EditBox->setDefaultText("X Position (0-1280)");
    gui.add(wallX1EditBox);

    auto wallY1EditBox = tgui::EditBox::create();
    wallY1EditBox->setPosition("75%", "54%");
    wallY1EditBox->setSize("18%", "6%");
    wallY1EditBox->setDefaultText("Y Position (0-720)");
    gui.add(wallY1EditBox);

    auto wallX2EditBox = tgui::EditBox::create();
    wallX2EditBox->setPosition("75%", "61%");
    wallX2EditBox->setSize("18%", "6%");
    wallX2EditBox->setDefaultText("X Position (0-1280)");
    gui.add(wallX2EditBox);

    auto wallY2EditBox = tgui::EditBox::create();
    wallY2EditBox->setPosition("75%", "68%");
    wallY2EditBox->setSize("18%", "6%");
    wallY2EditBox->setDefaultText("Y Position (0-720)");
    gui.add(wallY2EditBox);

    auto addWallButton = tgui::Button::create("Add Wall");
    addWallButton->setPosition("75%", "75%"); // Adjust the percentage as needed based on your layout
    addWallButton->setSize("18%", "6%");
    gui.add(addWallButton);

    size_t threadCount = std::thread::hardware_concurrency(); // Use the number of concurrent threads supported by the hardware

    Simulation sim(1280, 720, threadCount);

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

            basicX1PosEditBox->setVisible(false);
            basicY1PosEditBox->setVisible(false);
            basicAngleEditBox->setVisible(false);
            basicVelocityEditBox->setVisible(false);
            basicaddButton->setVisible(false);

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

            basicX1PosEditBox->setVisible(true);
            basicY1PosEditBox->setVisible(true);
            basicAngleEditBox->setVisible(true);
            basicVelocityEditBox->setVisible(true);
            basicaddButton->setVisible(true);

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

            float velocity = 20.0f; // constant velocity value
            float angle = 45.0f; // constant angle in degrees

            if (n <= 0) throw std::invalid_argument("Number of particles must be positive.");
            float xStep = (x2 - x1) / std::max(1, n - 1); // Calculate the x step between particles
            float yStep = (y2 - y1) / std::max(1, n - 1); // Calculate the y step between particles

            for (int i = 0; i < n; ++i) {
                float xPos = x1 + i * xStep; // Calculate the x position for each particle
                float yPos = y1 + i * yStep; // Calculate the y position for each particle

                // Add each particle to the simulation
                sim.addParticle(Particle(xPos, yPos, angle, velocity, 10)); // Assume radius is 10
            }

            // Clear the edit boxes after adding particles
            noParticles1->setText("");
            X1PosEditBox->setText("");
            Y1PosEditBox->setText("");
            X2PosEditBox->setText("");
            Y2PosEditBox->setText("");
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid input: " << e.what() << '\n';
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Input out of range: " << e.what() << '\n';
        }
        });

    // Attach an event handler to the "Add Particle" button for Form 2
    addButton2->onPress([&]() {
        try {
            int n = std::stoi(noParticles2->getText().toStdString()); // Number of particles
            float startTheta = std::stof(startAngleEditBox->getText().toStdString()); // Start angle in degrees
            float endTheta = std::stof(endAngleEditBox->getText().toStdString()); // End angle in degrees

            // constant velocity and a starting point for all particles
            float velocity = 20.0f; // velocity
            sf::Vector2f startPoint(640, 360); // start point

            if (n <= 0) throw std::invalid_argument("Number of particles must be positive.");

            float angularStep = (n > 1) ? (endTheta - startTheta) / (n - 1) : 0;

            if (startTheta == 0.0f && endTheta == 360.0f) {
                angularStep = (n > 1) ? (endTheta - startTheta) / (n) : 0;
            }

            for (int i = 0; i < n; ++i) {
                float angle = startTheta + i * angularStep; // Calculate the angle for each particle
                double angleRad = angle * (M_PI / 180.0); // Convert angle from degrees to radians              

                // Add each particle to the simulation
                sim.addParticle(Particle(startPoint.x, startPoint.y, angle, velocity, 10)); // Assume radius is 10
            }

            // Clear the edit boxes after adding particles
            noParticles2->setText("");
            startAngleEditBox->setText("");
            endAngleEditBox->setText("");
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid input: " << e.what() << '\n';        
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Input out of range: " << e.what() << '\n';
            
        }
        });

    // Attach an event handler to the "Add Particle" button for Form 3
    addButton3->onPress([&]() {
        try {
            int n = std::stoi(noParticles3->getText().toStdString()); // Number of particles
            float startVelocity = std::stof(startVelocityEditBox->getText().toStdString()); // Start velocity
            float endVelocity = std::stof(endVelocityEditBox->getText().toStdString()); // End velocity
            float angle = 90.0f; // constant angle in degrees
            sf::Vector2f startPoint(400, 300); // constant start point

            if (n <= 0) throw std::invalid_argument("Number of particles must be positive.");
            float velocityStep = (endVelocity - startVelocity) / std::max(1, n - 1); // Calculate the velocity step between particles

            for (int i = 0; i < n; ++i) {
                float velocity = startVelocity + i * velocityStep; // Calculate the velocity for each particle
                double angleRad = angle * (M_PI / 180.0); // Convert angle from degrees to radians
            
                // Add each particle to the simulation
                sim.addParticle(Particle(startPoint.x, startPoint.y, angle, velocity, 10)); // Assume radius is 10
            }

            // Clear the edit boxes after adding particles
            noParticles3->setText("");
            startVelocityEditBox->setText("");
            endVelocityEditBox->setText("");
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid input: " << e.what() << '\n';
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Input out of range: " << e.what() << '\n';
        }
        });

    // Attach an event handler to the "Add Particle" button for Basic Add Particle
    basicaddButton->onPress([&]() {
        try {
            float xPos = std::stof(basicX1PosEditBox->getText().toStdString()); // X coordinate
            float yPos = std::stof(basicY1PosEditBox->getText().toStdString()); // Y coordinate
            float angle = std::stof(basicAngleEditBox->getText().toStdString()); // Angle
            float velocity = std::stof(basicVelocityEditBox->getText().toStdString()); // Velocity
            
             // Add particle to the simulation
             sim.addParticle(Particle(xPos, yPos, angle, velocity, 10)); // Assume radius is 10
            
             // Clear the edit boxes after adding particles
             basicX1PosEditBox->setText("");
             basicY1PosEditBox->setText("");
             basicAngleEditBox->setText("");
             basicVelocityEditBox->setText("");
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid input: " << e.what() << '\n';
            
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Input out of range: " << e.what() << '\n';
            
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
        //compute framerate
        float currentTime = clock.restart().asSeconds();
        float fps = 1.0f/ (currentTime);

        if (fpsUpdateClock.getElapsedTime().asSeconds() >= 0.5f) {
            std::stringstream ss;
            ss.precision(0); // Set precision to zero
            ss << "FPS: " << std::fixed << fps; // Use std::fixed to avoid scientific notation
            fpsText.setString(ss.str());
            fpsUpdateClock.restart(); // Reset the fpsUpdateClock for the next 0.5-second interval
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            gui.handleEvent(event); // Pass events to the GUI

            if (event.type == sf::Event::Closed)
                window.close();
        }

        sim.simulate(1); // Advance the simulation
     
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