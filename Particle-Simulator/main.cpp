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

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");

	

	return 0;
}