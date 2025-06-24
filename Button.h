#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

// Button class for GUI interactions
class Button
{
private:
	sf::Font font; // Font used for button text
	sf::Text text; // Text displayed on the button
	sf::RectangleShape shape; // Rectangle representing the button shape
	bool isHeld = false; // Flag to detect button hold state

public:
	Button(sf::String txt, sf::String f = "assets/font.ttf"); // Constructor
	void setPosition(float x, float y); // Set position of button
	void draw(sf::RenderWindow& window); // Draw button on screen
	sf::FloatRect getBounds(); // Get button boundaries for interaction
	bool isPressed(sf::Event& event, sf::RenderWindow& window); // Check for mouse press event
};

