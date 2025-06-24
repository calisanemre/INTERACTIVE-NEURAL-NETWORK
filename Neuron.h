#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "GUI.h"
#include <random>
class GUI;
class Layer;

// Represents a single artificial neuron in the network.
// Handles drawing, interaction, and basic neural operations.
class Neuron
{
private:
	sf::CircleShape shape; // Circle used to visually represent the neuron
	bool isActive = false; // Indicates if the neuron is selected
	bool wasPressed = false; // Used to debounce mouse clicks
	std::vector<float> weights; // List of weights for inputs
	float bias; // Bias term
	float output; // Output after activation
	float gradient; // Gradient used during backpropagation
	float activationPreReLU; // Pre-activation value (before ReLU or softmax)

public:
	Neuron(); //Constructor
	
	// Visual and interaction utilities
	sf::FloatRect getBounds();
	float getRadius();
	sf::Vector2f getPosition();
	bool checkActive();
	bool isSelected(sf::Event& event, GUI& window);
	void setPosition(float x, float y);
	void draw(sf::RenderWindow& window);
	void setActive(bool value);


	float activationFunc(float input); // ReLU
	void initializeWeights(int inputSize); // Random init using He initialization
	float run(const std::vector<float>& inputs, bool useReLU); // Forward computation
	
	// Accessors / Mutators
	float getBias() const;
	void setBias(float b);
	void updateWeights(int i, float newVal);
	float getOutput();
	void setGradient(float f);
	std::vector<float> getWeights();
	float getGradient();
	void setActivationPreReLU(float value);
	float getActivationPreReLU() const;
	void setOutput(float out);
};

