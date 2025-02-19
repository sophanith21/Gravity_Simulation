#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <iomanip>


class GravitySource
{
	sf::Vector2f pos;
	float strength;
	sf::CircleShape s;
	bool drag;
	bool outlineDrag;
	bool press;
	sf::CircleShape resize;
	float oldDistance;
	float newDistance;


public:
	GravitySource(float pos_x, float pos_y)
	{
		pos.x = pos_x;
		pos.y = pos_y;
		strength = 0;

		s.setPosition(pos);
		s.setFillColor(sf::Color::White);
		s.setRadius(20);
		s.setOrigin(s.getRadius(), s.getRadius());

		resize.setPosition(pos);
		resize.setFillColor(sf::Color::Transparent);
		resize.setOutlineColor(sf::Color::White);
		resize.setOutlineThickness(-3);
		resize.setRadius(s.getRadius() * 2);
		resize.setOrigin(resize.getRadius(), resize.getRadius());

		outlineDrag = false;
		drag = false;
		press = false;
		oldDistance = newDistance = 0;
	}

	void render(sf::RenderWindow& wind)
	{
		wind.draw(s);
	}

	void renderOutline(sf::RenderWindow& wind)
	{
		wind.draw(resize);
	}

	void setOutlineDrag(bool outlineDrag)
	{
		this->outlineDrag = outlineDrag;
	}

	bool isOutlineDrag()
	{
		return outlineDrag;
	}

	void setPressed(bool press)
	{
		this->press = press;
	}

	bool isPressed()
	{
		return press;
	}

	sf::Vector2f get_pos()
	{
		return pos;
	}
	
	float get_strength()
	{
		return strength;
	}

	void set_strength()
	{
		float k = 2.0f; // Adjust this value as needed
		strength = k * (s.getRadius() * s.getRadius());
	}
	
	void set_pos()
	{
		this->pos = s.getPosition();
	}

	sf::CircleShape* getShape()
	{
		return &s;
	}

	sf::CircleShape* getOutline()
	{
		return &resize;
	}

	void setDrag(bool isDragged)
	{
		this->drag = isDragged;
	}
	bool isDragged()
	{
		return drag;
	}

	void updateOldDistance(float distance)
	{
		oldDistance = distance;
	}
	float getOldDistance()
	{
		return oldDistance;
	}

	void updateNewDistance(float distance)
	{
		newDistance = distance;
	}
	float getNewDistance()
	{
		return newDistance;
	}

	void updateRadius(float distance)
	{

		float minRadius = 5.0f;  // Prevents the ring from disappearing
		float maxRadius = 300.0f; // Optional: Prevents excessive size

		float newRadius = s.getRadius() + distance;
		newRadius = std::max(minRadius, std::min(maxRadius, newRadius)); // Keep radius within bounds

		s.setRadius(newRadius);
		s.setOrigin(newRadius, newRadius); // Adjust origin to stay centered

		resize.setRadius(newRadius*2);
		resize.setOrigin(newRadius*2, newRadius*2); // Keep resize handle centered
	}

	
};

class Particle
{
	sf::Vector2f pos;
	sf::Vector2f velocity;
	sf::CircleShape s;

public:
	Particle(float pos_x, float pos_y, float vel_x, float vel_y)
	{
		pos.x = pos_x;
		pos.y = pos_y;
		velocity.x = vel_x;
		velocity.y = vel_y;
		s.setPosition(pos);
		s.setFillColor(sf::Color::White);
		s.setRadius(2);
	}

	void render(sf::RenderWindow& wind)
	{
		s.setPosition(pos);
		wind.draw(s);
	}

	void set_color(sf::Color col)
	{
		s.setFillColor(col);
	}
	void update_physic(GravitySource& s)
	{
		float distance_x = s.get_pos().x - pos.x;
		float distance_y = s.get_pos().y - pos.y;

		double distance = sqrt(pow(distance_x, 2) + pow(distance_y, 2));
		
		float inverse_distance = 1.f / distance;

		float normalized_x = inverse_distance * distance_x;
		float normalized_y = inverse_distance * distance_y;

		float inverse_square_dropoff = inverse_distance * inverse_distance;

		float acceleration_x = normalized_x * s.get_strength() * inverse_square_dropoff;
		float acceleration_y = normalized_y * s.get_strength() * inverse_square_dropoff;

		velocity.x += acceleration_x;
		velocity.y += acceleration_y;

		pos.x += velocity.x;
		pos.y += velocity.y;
	}

	bool isOut(sf::RenderWindow &window)
	{
		if (pos.x > window.getSize().x || pos.x <0)
		{
			return true;
		}
		if (pos.y > window.getSize().y || pos.y < 0)
		{
			return true;
		}
		return false;
	}
};

sf::Color map_val_to_color(float value)
{
	if (value < 0.0f) value = 0;
	if (value > 1.0f) value = 1;
	
	int r = 0, g = 0, b = 0;
	if (value < 0.5f)
	{
		b = 255 * (1.0f - 2 * value);
		g = 255 *  2 * value;
	}
	else
	{
		g = 255 * (1.0f - 2 * value);
		r = 255 * 2 * value;
	}
	return sf::Color(r, g, b);
}

int main()
{
	using namespace sf;
	RenderWindow window(VideoMode(1600, 1000), "Gravity");
	window.setFramerateLimit(60);

	bool pause = false;

	bool isSpawn = false;

	

	std::vector <GravitySource> sources;
	sources.push_back(GravitySource(1000, 500));
	sources.push_back(GravitySource(700, 500));

	int num_particle = 2000;

	std::vector <Particle> particles;

	int i = 0;
	particles.push_back(Particle(600.f, 700.f, 4.f, 0.2f + (0.1 / num_particle)));
	particles[i].set_color(map_val_to_color((float)i / (float)num_particle));
	Font arial;
	if (!arial.loadFromFile("Arial.ttf"))
	{
		std::cerr << "Failed to load font!" << std::endl;
		return -1;
	}

	sf::Clock clock;
	int frameCount = 0;
	float elapsedTime = 0.0f;
	Text fps;
	fps.setFont(arial);
	fps.setCharacterSize(23);
	fps.setPosition(0, 100);
	
	

	Text dis;
	dis.setFont(arial);
	dis.setCharacterSize(23);

	std::string temp_text = "The system has been paused";
	Text pause_dis;
	pause_dis.setString(temp_text);
	pause_dis.setFillColor(Color::Red);
	pause_dis.setFont(arial);
	pause_dis.setCharacterSize(22);
	pause_dis.setPosition(window.getSize().x / 2 - 100, window.getSize().y - 100);
	std::string userInput;

	Text menu;
	menu.setString("Hold or tap left click to spawn a particle (s)\nPress Space to Pause\nType Numbers and Press Enter to change the strength of gravity\nPress N to enable \"Spawn Particle\"\nClick on the white Circle to resize (Drag the appeared Circle Ring to resize)");
	menu.setFont(arial);
	menu.setCharacterSize(22);
	menu.setPosition(0, 300);
	bool isMenu = false;
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == event.Closed)
			{
				window.close();
			}
			frameCount++;
			elapsedTime += clock.restart().asSeconds();
			if (elapsedTime >= 1.0f)
			{
				fps.setString("FPS: " + std::to_string(frameCount));
				frameCount = 0;
				elapsedTime = 0.0f;
			}
		
			if (event.type == event.KeyPressed)
			{
				if (event.key.code == Keyboard::Escape)
				{
					window.close();
				}

				if (event.key.code == Keyboard::Space)
				{
					if (pause)
					{
						pause = false;
					}
					else
					{
						pause = true;
					}
				}

				if (event.key.code == Keyboard::M)
				{
					if (isMenu)
					{
						isMenu = false;
					}
					else
					{
						isMenu = true;
					}
				}

				if (event.key.code == Keyboard::N)
				{
					if (isSpawn)
					{
						isSpawn = false;
					}
					else
					{
						isSpawn = true;
					}
				}
			}
			float radius;
			Vector2f mouseCoordinate;
			Vector2f distance_xy;
			float distance;
			float innerRadius;
			if (!isSpawn)
			{
				for (int i = 0; i < sources.size(); i++)
				{
					radius = sources[i].getOutline()->getRadius();
					mouseCoordinate = window.mapPixelToCoords(Mouse::getPosition(window));
					distance_xy = Vector2f(sources[i].getOutline()->getPosition().x - mouseCoordinate.x, sources[i].getOutline()->getPosition().y - mouseCoordinate.y);
					distance = sqrt(pow(distance_xy.x, 2) + pow(distance_xy.y, 2));
					innerRadius = radius - std::abs(sources[i].getOutline()->getOutlineThickness());

					if (!sources[i].isOutlineDrag())
					{
						sources[i].updateOldDistance(distance);
					}

					if (distance <= (radius + 10) && distance >= (innerRadius - 10) && sources[i].isPressed() && event.mouseButton.button == Mouse::Left)
					{
						sources[i].setOutlineDrag(true);
					}

					if (sources[i].isOutlineDrag())
					{
						sources[i].updateNewDistance(distance);
					}
				}

			}
			
			if (event.type == event.MouseButtonPressed && !isSpawn)
			{
				
				if (event.mouseButton.button == Mouse::Left)
				{
					for (int i = 0; i < sources.size(); i++)
					{
						if (sources[i].getShape()->getGlobalBounds().contains(Vector2f(Mouse::getPosition(window))))
						{
							if (sources[i].isPressed())
							{
								sources[i].setPressed(false);
							}
							else
							{
								sources[i].setPressed(true);
							}
							sources[i].setDrag(true);
							break;
						}
					}
				}
				
			}
			if(event.type == event.MouseButtonReleased && !isSpawn)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					for (int i = 0; i < sources.size(); i++)
					{
						
						sources[i].setDrag(false);
						sources[i].setOutlineDrag(false);
					}
					
				}
			}
			
		}
		//Update
		if (!pause)
		{
			
			if (Mouse::isButtonPressed(Mouse::Left) && isSpawn)
			{
				i++;
				particles.push_back(Particle(Mouse::getPosition(window).x, Mouse::getPosition(window).y, 5.f, 0.2f + (0.1 / num_particle) * i));
				particles[i].set_color(map_val_to_color((float)i / (float)num_particle));
			}
			for (int k = 0; k < particles.size(); k++)
			{
				if ((particles[k].isOut(window) && !particles.empty()) || i > 2000)
				{
					particles.erase(particles.begin() + k);
					i--;
				}
			}
			for (int i = 0; i < particles.size(); i++)
			{
				particles[i].update_physic(sources[0]);
				particles[i].update_physic(sources[1]);
			}
		}
		if (pause)
		{
			window.clear();
			window.draw(pause_dis);
		}
		else
		{
			window.clear();
		}

		if (!isSpawn)
		{
			for (int i = 0; i < sources.size(); i++)
			{
				if (sources[i].isDragged())
				{
					sources[i].getShape()->setPosition((Vector2f)Mouse::getPosition(window));
					sources[i].getOutline()->setPosition((Vector2f)Mouse::getPosition(window));
					sources[i].set_pos();
				}
				if (sources[i].isPressed() && !sources[i].isDragged())
				{
					if (sources[i].isOutlineDrag())
					{
						sources[i].getOutline()->setOutlineColor(Color::Yellow);
						sources[i].updateRadius(sources[i].getNewDistance() - sources[i].getOldDistance());
						sources[i].updateOldDistance(sources[i].getNewDistance());
						
					}
					else
					{
						sources[i].getOutline()->setOutlineColor(Color::White);
					}
					sources[i].renderOutline(window);
				}

				
			}
		}

		sources[0].set_strength();
		sources[1].set_strength();
		dis.setString("Gravity Stregnth [1]: " + std::to_string(sources[0].get_strength()) + "\n" +
			"Gravity Strength [2]: " + std::to_string(sources[1].get_strength()) + "\n" +
			"Particles: " + std::to_string(particles.size()) + "\n" +
			"Press m for more info");
			/*"New Distance: " + std::to_string(sources[0].getNewDistance()) + "\n" +
			"Old Distaace: " + std::to_string(sources[0].getOldDistance()) + "\n" + 
			"Distnace: " + std::to_string(sources[0].getNewDistance() - sources[0].getOldDistance()));*/

		//Draw
		
		for (size_t i = 0; i < particles.size(); i++)
		{
			particles[i].render(window);
		}
		
		
		if (isMenu)
		{
			window.draw(menu);
		}
		window.draw(dis);

		sources[0].render(window);
		sources[1].render(window);

		for (size_t i = 0; i < particles.size(); i++)
		{
			particles[i].render(window);
		}
		if (isMenu)
		{
			window.draw(menu);
		}
		window.draw(fps);

		
		window.display();
	}
}
