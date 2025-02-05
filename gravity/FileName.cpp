#include <SFML/Graphics.hpp>

using namespace std;
class Planet {
public:
	sf::Vector2f position;
	sf::Vector2f velocity;
	float mass;
	float radius;

	Planet(sf::Vector2f position, sf::Vector2f velocity, float mass, float radius) {
		this->position = position;
		this->velocity = velocity;
		this->mass = mass;
		this->radius = radius;
	}
	void applyGravity(Planet& other) {
		const float G = 100.f; // Scaled gravitational constant
		float dx = other.position.x - this->position.x;
		float dy = other.position.y - this->position.y;
		float distance = sqrt(dx * dx + dy * dy);

		if (distance < this->radius + other.radius) return; // Avoid extreme forces due to near-zero distance

		float force = (G * this->mass * other.mass) / (distance * distance);
		float acceleration = force / this->mass;

		// Normalize direction
		float angle = atan2(dy, dx);
		this->velocity.x += acceleration * cos(angle);
		this->velocity.y += acceleration * sin(angle);
	}
	void update(float dt) {
		this->position.x += this->velocity.x * dt;
		this->position.y += this->velocity.y * dt;
	}
};
int main()
{
	using namespace sf;
	RenderWindow window(VideoMode(1000, 500), "Gravity");
	window.setFramerateLimit(60);
	Planet planet1(sf::Vector2f(200.f, 250.f), sf::Vector2f(0.f, -2.f), 1000.f, 50.f);
	Planet planet3(sf::Vector2f(250.f, 250.f), sf::Vector2f(1.f, 2.f), 1000.f, 50.f);
	Planet planet2(sf::Vector2f(500.f, 250.f), sf::Vector2f(0.f, 2.f), 1000.f, 50.f);
	sf::CircleShape shape1(planet1.radius);
	sf::CircleShape shape2(planet2.radius);
	shape1.setFillColor(sf::Color::Blue);
	shape2.setFillColor(sf::Color::Red);
	shape1.setOrigin(planet1.radius, planet1.radius);
	shape2.setOrigin(planet2.radius, planet2.radius);	
	sf::CircleShape shape3(planet3.radius);
	shape3.setOrigin(planet3.radius, planet3.radius);
	shape3.setFillColor(sf::Color::Yellow);
	

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == event.Closed)
			{
				window.close();
			}
		}
		//Update
		float dt = 1.f / 60.f;
		planet1.applyGravity(planet2);
		planet2.applyGravity(planet1);
		planet3.applyGravity(planet1);
		planet3.applyGravity(planet2);
		planet1.applyGravity(planet3);
		planet2.applyGravity(planet3);
		planet1.update(dt);
		planet3.update(dt);
		planet2.update(dt);
		shape1.setPosition(planet1.position);
		shape2.setPosition(planet2.position);
		shape3.setPosition(planet3.position);
		//Draw
		window.clear();
		window.draw(shape1);
		window.draw(shape2);
		window.draw(shape3);
		window.display();
	}
}