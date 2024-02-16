# STDISCM-PROBLEM-SET-1
 
# Particle Simulator

## Introduction

This Particle Simulator is a graphical application designed to simulate the behavior of particles within a confined space. It uses SFML for rendering graphics and TGUI for the user interface, providing an interactive environment where users can add particles, define walls, and observe particle dynamics including collisions and reflections.

## Requirements

- C++17 compiler
- SFML (Simple and Fast Multimedia Library) 2.5 or newer
- TGUI (Texus' Graphical User Interface) 0.9 or newer

## Installation

To compile and run the Particle Simulator, follow these steps:

1. **Clone the Repository:** Clone this repository to your local machine using `git clone`, or download the source code as a ZIP file and extract it.

2. **Compile the Application:** Navigate to the directory containing the source code and compile the application.

3. **Run the Application:** Once compiled, you can run the application

## Usage

After launching the Particle Simulator, you will be presented with a graphical interface that allows you to interact with the simulation:


# Adding Particles
- Individual Particle Addition: Use the input fields to specify properties for individual particles (position, velocity, angle) and click "Add Particle".
- Batch Particle Addition: The application supports adding particles in batches through several forms:
  - Form 1: Specify a start and end position, and the application will distribute particles evenly along the line connecting these points.
  - Form 2: Define an angle range and the application will distribute particles evenly across the specified angular direction from a central point.
  - Form 3: Input a range of velocities, and particles will be added with velocities distributed within this range.

# Defining Walls
- Wall Input Form: Use the wall input form to specify the start and end points of a wall. Walls are added to the simulation space upon clicking "Add Wall" and affect particle trajectories through collisions.


# Simulation Control
- Particles move automatically and interact with walls and boundaries. You can dynamically add particles and walls during the simulation.

## Authors
* **Go, Eldrich**
* **Pinawin, Timothy**