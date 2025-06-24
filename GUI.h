#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Button.h"
#include "Layer.h"
#include "Neuron.h"
#include "Input.h"
#include <iostream>
#include <vector>
#include <array>

// Size limits
#define MAX_BUTTONS 5
#define MAX_LAYERS 5
#define MAX_NEURONS 12
#define PADDING 25

class Layer;
class Neuron;
class Input;

// GUI class that extends SFML's RenderWindow
class GUI : public sf::RenderWindow
{
private:
	std::vector<Button> buttonList; // List of buttons
	std::vector<Layer*> layerList; // List of layer pointers
	int layerCount;
	int buttonCount;
	Input* input; // Pointer to the input grid

public:
	
	GUI(int x, int y, sf::String name, int buttonCount=0, int layerCount = 0); // Constructor: initialize window and input grid
	void addButton(const Button& button); // Add a button to the button list (if limit not reached)
	void addLayer(int padding = PADDING); // Add new layer and position it
	void deleteLayer(Layer* layer); // Delete layer and its neurons
	void drawLayers(); // Draw all layers
	void repositionLayers(int padding = PADDING); // Position layers with spacing
	void addNeuron(Layer*); // Add a neuron to a layer
	void deleteNeuron(Layer* layer, Neuron* neuron); // Remove a neuron from a layer
	void drawNeurons(Layer* layer); // Draw neurons of a layer
	void repositionNeurons(Layer* layer); // Recalculate neuron positions inside a layer
	std::vector<Layer*>& getLayerList(); // Return reference to layer list
	std::vector <std::array<sf::Vertex, 2>> drawLines(); // Generate connection lines between layers
	void drawInput(); // Draw input grid
	Input* getInput(); // Return input grid pointer
};

