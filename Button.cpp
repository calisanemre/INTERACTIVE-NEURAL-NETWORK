#include "Button.h"
Button::Button(sf::String txt, const sf::String f) {
    shape.setSize(sf::Vector2f(180.f, 70.f)); // Set button size
    shape.setFillColor(sf::Color::Green); // Default color
    shape.setOutlineColor(sf::Color::Black); // Border color
    shape.setOutlineThickness(1.f); // Border thickness
    font.loadFromFile(f); // Load font from file
    text.setFont(font);
    text.setString(txt); // Set button label
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White); // Text color
}

void Button::setPosition(float x, float y) {
    shape.setPosition(x, y); // Place the button
    sf::FloatRect shapeBounds = shape.getGlobalBounds();
    sf::FloatRect textBounds = text.getGlobalBounds();
    float textX = shapeBounds.left + (shapeBounds.width - textBounds.width) / 2.f;
    float textY = shapeBounds.top + (shapeBounds.height - textBounds.height) / 3.f;
    text.setPosition(textX, textY);
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape); // Draw button shape
    window.draw(text); // Draw button text
}

// Return button clickable area
sf::FloatRect Button::getBounds() {
	return shape.getGlobalBounds(); 
}

bool Button::isPressed(sf::Event &event, sf::RenderWindow &window) {
    // Mouse pressed on the button
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && !isHeld) {
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        sf::Vector2f mouse_world = window.mapPixelToCoords(mouse_pos);
        if (this->getBounds().contains(mouse_world)) {
            isHeld = true; // Prevents continuous trigger
            shape.setFillColor(sf::Color::Red); // Feedback: press effect
            return true;
        }
    }

    // Mouse released, reset state
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        shape.setFillColor(sf::Color::Green); // Reset color
        isHeld = false; 
    }
    return false; // Not clicked
}
