#include "Layer.h"
#include <iostream>
Layer::Layer() {
    // Initialize visual properties of the layer rectangle
	shape.setSize(sf::Vector2f(90.f, 408.f));
	shape.setFillColor(sf::Color::Blue);
    shape.setOutlineColor(sf::Color::Red);
    shape.setOutlineThickness(0.f); // Outline shown only if selected
}

sf::FloatRect Layer::getBounds() {
    // Temporarily disables outline to return correct hitbox
    if (isActive) {
        shape.setOutlineThickness(0.f);
        auto noOutline = shape.getGlobalBounds();
        shape.setOutlineThickness(5.f);
        return noOutline;
    }
    return shape.getGlobalBounds();
}

void Layer::setPosition(float x, float y) {
    shape.setPosition(x, y);
}

bool Layer::checkActive() {
    return isActive;
}

void Layer::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

bool Layer::isSelected(sf::Event& event, GUI& window) {
    // Handles mouse click interaction for selecting this layer
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && !wasPressed) {
        
        std::vector<sf::Vector2f> neuron_pos;
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        sf::Vector2f mouse_world = window.mapPixelToCoords(mouse_pos);
        
        // Prevent selection if clicking directly on a neuron
        for (int i = 0; i < neuronList.size(); i++) {
            if (neuronList[i]->getBounds().contains(mouse_world)) {
                return false;
            }
        }

        // Select layer if mouse is inside the rectangle bounds
        if (this->getBounds().contains(mouse_world)) {
            isActive = !isActive;
            wasPressed = true;
            if (isActive) {              
                this->setActive(isActive);
                return true;
            }
            else {
                this->setActive(isActive);
                return false;
            }       
        }
    }

    // Reset press tracking on mouse release
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        wasPressed = false;
    }   
}

sf::Vector2f Layer::getPosition() {
    return shape.getPosition();
}

void Layer::setNeuronCount(int inc) {
    neuronCount += inc;
}

int Layer::getNeuronCount() {
    return neuronCount;
}

std::vector<Neuron*> &Layer::getNeuronList() {
    return neuronList;
}

void Layer::setActive(bool value) {
    isActive = value;
    shape.setOutlineThickness(value ? 5.f : 0.f); // Toggle outline to indicate selection visually
}

void Layer::addNeuron(Neuron* neuron) {
    neuronList.push_back(neuron);
}