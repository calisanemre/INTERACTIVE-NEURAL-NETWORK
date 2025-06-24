#pragma once
#include <SFML/Graphics.hpp>
#include "GUI.h"
#include "Neuron.h"
#include <cstdlib>
class GUI;
class Neuron;

// Represents a single layer in the neural network GUI
// Each layer contains a rectangle visual and a list of neurons
class Layer
{
private:
	sf::RectangleShape shape; // Visual representation of the layer box
	bool isActive = false; // Indicates if this layer is currently selected
	bool wasPressed = false; // Used to debounce mouse click events
	std::vector<Neuron*> neuronList; // List of neuron pointers inside the layer
	int neuronCount; // Number of neurons in this layer
public:
	Layer(); // Constructor
	bool isSelected(sf::Event& event, GUI& window); // Handles user interaction with the layer (selection)
	bool checkActive(); // Returns true if the layer is currently active
	sf::FloatRect getBounds(); // Gets the bounding box of the layer (optionally without outline)
	void setPosition(float x, float y); // Set the position of the layer on the screen
	void draw(sf::RenderWindow& window); // Draws the layer rectangle on the window
	sf::Vector2f getPosition(); // Get the position of the layer on the screen
	int getNeuronCount(); // Access neuron count
	std::vector<Neuron*>& getNeuronList(); // Access and modify the neuron list
	void setNeuronCount(int inc); // Modify neuron count
	void setActive(bool value);	// Set selection status
	void addNeuron(Neuron* neuron); // Add neuron to the model
};

